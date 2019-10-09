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

#define MEM_POOL_NUM	32	// �ڴ�ظ����������Ҫ�ģ���Ϊ�ܶ�ط���& 0x1F������ȡģ
#define MAX_RECV_SIZE	(512-sizeof(tagBag))
//-----------------------------------------------------------------------------
// �����������Ĵ���״̬
//-----------------------------------------------------------------------------
enum EState
{
	ES_Null = 0,
	ES_Accept,			// ����
	ES_Recv,			// ��������
	ES_Send,			// ����
	ES_End
};


enum EDisconnectReason
{
	EDR_Start = 0,
	EDR_Normal = 0,						// �������
	EDR_FIN = EDR_Normal,				// �յ�FIN,�ͻ�����������
	EDR_Kick,							// �ϲ��ߵ�
	EDR_FailedAuth,						// δͨ����֤
	EDR_AuthTimeOut,					// ��֤��ʱ

	EDR_Abnormal,						// ������
	EDR_CPortError = EDR_Abnormal,		// ��ɶ˿ڷ���ʧ��
	EDR_CPortBindError,					// ��ɶ˿ڰ�ʧ��
	EDR_TooManyRecv,					// �ȴ��ϲ㴦����հ�����������
	EDR_TooManySend,					// �ȴ��ײ㷢�͵İ�����������
	EDR_TooBigRecv,						// �յ��������ô�С�İ�
	EDR_TooBigSend,						// ���ʹ������ô�С�İ�
	EDR_WSASendError,					// API ���ش���
	EDR_WSARecvError,					// API ���ش���
	EDR_TooManySendCast,				// ̫��û�н��յİ�
	EDR_TooMuchSendCastSize,			// ̫��û�н��յ��ֽ�
	EDR_End,
};


ECORE_API LPCTSTR g_szDisconnectReason[];



//-----------------------------------------------------------------------------
// ���������
//-----------------------------------------------------------------------------
struct tagBag
{
	WSAOVERLAPPED			ov;							// �ص�IO�ṹ
	EState					eState;						// �����״̬
	MemCache<AtomMutex>*	pPool;						// ������ڴ��
	tagBag*					pNext;						// ָ����һ��unit
	DWORD					dwReady;					// �Ѿ��ж����ֽڴ������
	union
	{
		DWORD				dwSize;						// ��Ч���ݴ�С,����ʵ���ڴ�ռ��С
		CHAR				pBuffer[sizeof(DWORD)];
	};
		
	CHAR					pMsg[4];
};


struct tagRaw
{
	MemCache<AtomMutex>*	pPool;					// ������ڴ��
	DWORD					dwSize;					// ��Ч���ݴ�С,����ʵ���ڴ�ռ��С
	LONG VOLATILE			lCounter;				// Ϊ�ಥ׼�������ü���
	CHAR					pMsg[8];
};


//-----------------------------------------------------------------------------
// ����ڵ����ͻ��˵Ķ���
//-----------------------------------------------------------------------------
struct tagClient
{
	BOOL VOLATILE			bShutDown;		// �Ƿ��Ѿ���shutdown
	BOOL VOLATILE			bCanClear;		// �Ƿ����ʩ��
	LONG VOLATILE			lSendCast;		// �Ѿ�������send,��δ�յ�֪ͨ��
	LONG VOLATILE			lRecvCast;		// �Ѿ�������recv,��δ�յ�֪ͨ��
	LONG VOLATILE			lSendCastSize;	// �Ѿ������Ĵ�С,��δ�յ�֪ͨ��
	
	DWORD					dwClientID;		// �ͻ���ID,�ϲ�ȷ�ϵ�һ�����Ժ����Ч
	DWORD					dwRecvSerial;	// �������к�

	RingQueue<tagRaw*>*		pSendQueue;		// ��������,��ƴ��ѹ��
	RingQueue<tagBag*>* 	pRecvQueue;		// �հ�����,���ϲ���ȡ

	SOCKET					sock;			// �ͻ��˶�Ӧ��socket
	EDisconnectReason		eReason;		// �ǳ�ԭ��

	union
	{
		DWORD				dwConnectTime;	// ����ȷ���߳���ʱ��δ��֤�ͻ���
		DWORD				dwDebugMsg;
	};

};



//------------------------------------------------------------------------------
// ����ʵ��bag��С
//------------------------------------------------------------------------------
#define BAG_SIZE(size) (sizeof(tagBag)+(size))

//------------------------------------------------------------------------------
// LZO����ʹ����ѹ�����󣬼����padding���Ա�֤����Խ��
//------------------------------------------------------------------------------
#define LZO_ADD(size) (((size)/64) + 16 + 3)

//------------------------------------------------------------------------------
// ����ʵ������unit�Ĵ�С
//------------------------------------------------------------------------------
#define RAWBAG_SIZE(size) (sizeof(tagRaw)+(size))


//-----------------------------------------------------------------------------
// ��½������������Ϊ�ͻ��˵�½ʱ �ϲ���²��֮����Ϣ�������ֶ�
//-----------------------------------------------------------------------------
struct tagLoginParam
{
	HANDLE	hHandle;	// ÿ�ͻ���handle
	DWORD	dwAddress;	// Զ�˵�ַ
	DWORD	dwPort;		// Զ�˶˿�
};

#define PARAM_SIZE	(((sizeof(sockaddr_in)+16)*2)+sizeof(tagLoginParam))
#define PARAM_INDEX ((sizeof(sockaddr_in)+16)*2)


//-----------------------------------------------------------------------------
// ��������
//-----------------------------------------------------------------------------
struct LogHandler
{  
	virtual DWORD Logon(LPVOID pLogonMsg, DWORD dwSize, tagLoginParam*)=0;
	virtual VOID  PreLogoff(DWORD dwClientID, EDisconnectReason eReason, DWORD dwDebugMsg=0)=0;
};


struct tagConfig
{
	LogHandler*	pLogHandle;		// ����ǳ�����

	INT	nPort;					// �˿�
	INT nMaxLoad;				// ��������������
	INT nPerCpuThread;			// ��CPU�����߳�
	INT nAcceptEx;				// acceptex����
	DWORD dwPoolSize;			// ÿ���ڴ�صĴ�С
	DWORD dwAuthTime;			// ���֤�ȴ�ʱ��
	DWORD dwMaxTickSend;		// ���ͻ���ÿ������������ֽ���

	LONG lMaxSendCastSize;		// ���ͻ�����󷢰�δ�յ��ۻ���С
	LONG lMaxSendCast;			// ���ͻ�����󷢰�δ�յ��ۻ���


	LONG lMaxSendBag;			// ÿ�ͻ���ÿ����������Ͱ��ۻ�
	LONG lMaxRecvBag;			// ÿ�ͻ���ÿ�����������հ��ۻ�

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




