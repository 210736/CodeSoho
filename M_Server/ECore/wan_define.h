//-----------------------------------------------------------------------------
// author Lyp
// date 2004-07-02
// last 2009-04-03
//-----------------------------------------------------------------------------
#pragma once

#include "mem_cache.h"
#include "wan_queue.h"
#include "wan_list.h"

namespace ECore {
namespace Wan {

#define MEM_POOL_NUM	32	// 内存池个数，这个不要改，因为很多地方用& 0x1F来代替取模
#define MAX_RECV_SIZE	(512-sizeof(tagBag))
//-----------------------------------------------------------------------------
// 网络包里包含的处理状态
//-----------------------------------------------------------------------------
enum EState
{
	ES_Null = 0,
	ES_Accept,			// 连接
	ES_Recv,			// 接收数据
	ES_Send,			// 发送
	ES_End
};


enum EDisconnectReason
{
	EDR_Start = 0,
	EDR_Normal = 0,						// 正常情况
	EDR_FIN = EDR_Normal,				// 收到FIN,客户端正常下线
	EDR_Kick,							// 上层踢掉
	EDR_FailedAuth,						// 未通过认证
	EDR_AuthTimeOut,					// 认证超时

	EDR_Abnormal,						// 非正常
	EDR_CPortError = EDR_Abnormal,		// 完成端口返回失败
	EDR_CPortBindError,					// 完成端口绑定失败
	EDR_TooManyRecv,					// 等待上层处理的收包大于配置数
	EDR_TooManySend,					// 等待底层发送的包大于配置数
	EDR_TooBigRecv,						// 收到大于配置大小的包
	EDR_TooBigSend,						// 发送大于配置大小的包
	EDR_WSASendError,					// API 返回错误
	EDR_WSARecvError,					// API 返回错误
	EDR_TooManySendCast,				// 太多没有接收的包
	EDR_TooMuchSendCastSize,			// 太多没有接收的字节
	EDR_End,
};


ECORE_API LPCTSTR g_szDisconnectReason[];



//-----------------------------------------------------------------------------
// 网络包定义
//-----------------------------------------------------------------------------
struct tagBag
{
	WSAOVERLAPPED			ov;							// 重叠IO结构
	EState					eState;						// 网络包状态
	MemCache<AtomMutex>*	pPool;						// 分配的内存池
	tagBag*					pNext;						// 指向下一个unit
	DWORD					dwReady;					// 已经有多少字节传输完毕
	union
	{
		DWORD				dwSize;						// 有效内容大小,并非实际内存空间大小
		CHAR				pBuffer[sizeof(DWORD)];
	};
		
	CHAR					pMsg[4];
};


struct tagRaw
{
	MemCache<AtomMutex>*	pPool;					// 分配的内存池
	DWORD					dwSize;					// 有效内容大小,并非实际内存空间大小
	LONG VOLATILE			lCounter;				// 为多播准备的引用计数
	CHAR					pMsg[8];
};


//-----------------------------------------------------------------------------
// 针对于单个客户端的定义
//-----------------------------------------------------------------------------
struct tagClient
{
	BOOL VOLATILE			bShutDown;		// 是否已经被shutdown
	BOOL VOLATILE			bCanClear;		// 是否可以施放
	LONG VOLATILE			lSendCast;		// 已经发出的send,尚未收到通知的
	LONG VOLATILE			lRecvCast;		// 已经发出的recv,尚未收到通知的
	LONG VOLATILE			lSendCastSize;	// 已经发出的大小,尚未收到通知的
	
	DWORD					dwClientID;		// 客户端ID,上层确认第一个包以后才有效
	DWORD					dwRecvSerial;	// 接收序列号

	RingQueue<tagRaw*>*		pSendQueue;		// 发包队列,待拼合压缩
	RingQueue<tagBag*>* 	pRecvQueue;		// 收包队列,待上层收取

	SOCKET					sock;			// 客户端对应的socket
	EDisconnectReason		eReason;		// 登出原因

	union
	{
		DWORD				dwConnectTime;	// 用来确定踢出长时间未验证客户端
		DWORD				dwDebugMsg;
	};

};



//------------------------------------------------------------------------------
// 计算实际bag大小
//------------------------------------------------------------------------------
#define BAG_SIZE(size) (sizeof(tagBag)+(size))

//------------------------------------------------------------------------------
// LZO可能使数据压缩后变大，加入该padding可以保证不会越界
//------------------------------------------------------------------------------
#define LZO_ADD(size) (((size)/64) + 16 + 3)

//------------------------------------------------------------------------------
// 计算实际申请unit的大小
//------------------------------------------------------------------------------
#define RAWBAG_SIZE(size) (sizeof(tagRaw)+(size))


//-----------------------------------------------------------------------------
// 登陆参数，可以作为客户端登陆时 上层和下层的之间信息交换的手段
//-----------------------------------------------------------------------------
struct tagLoginParam
{
	HANDLE	hHandle;	// 每客户端handle
	DWORD	dwAddress;	// 远端地址
	DWORD	dwPort;		// 远端端口
};

#define PARAM_SIZE	(((sizeof(sockaddr_in)+16)*2)+sizeof(tagLoginParam))
#define PARAM_INDEX ((sizeof(sockaddr_in)+16)*2)


//-----------------------------------------------------------------------------
// 启动参数
//-----------------------------------------------------------------------------
struct LogHandler
{  
	virtual DWORD Logon(LPVOID pLogonMsg, DWORD dwSize, tagLoginParam*)=0;
	virtual VOID  PreLogoff(DWORD dwClientID, EDisconnectReason eReason, DWORD dwDebugMsg=0)=0;
};


struct tagConfig
{
	LogHandler*	pLogHandle;		// 登入登出处理

	INT	nPort;					// 端口
	INT nMaxLoad;				// 服务器人数上限
	INT nPerCpuThread;			// 单CPU工作线程
	INT nAcceptEx;				// acceptex数量
	DWORD dwPoolSize;			// 每个内存池的大小
	DWORD dwAuthTime;			// 最长认证等待时间
	DWORD dwMaxTickSend;		// 单客户端每心跳发送最大字节数

	LONG lMaxSendCastSize;		// 单客户端最大发包未收到累积大小
	LONG lMaxSendCast;			// 单客户端最大发包未收到累积数


	LONG lMaxSendBag;			// 每客户端每次心跳最大发送包累积
	LONG lMaxRecvBag;			// 每客户端每次心跳最大接收包累积

	tagConfig()
	{
		pLogHandle = NULL;
		nPort = 6008; 
		nMaxLoad = 8000; 
		nPerCpuThread = 2;
		nAcceptEx = 32; 
		
		dwPoolSize = 64*1024*1024;
		dwAuthTime = 32*1000;
		dwMaxTickSend = 4*1024*1024;

		lMaxSendCastSize = 2*1024*1024;
		lMaxSendCast = 512;

		lMaxSendBag = 2048;
		lMaxRecvBag = 128;
	}
};

}
}




