//-----------------------------------------------------------------------------
// author Lyp
// date 2000-01-17
// last 2010-01-13
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {
namespace Lan {

// 支持文本模式和流模式，且能同一连接下动态切换，适合HTTP传输
enum EClientMode
{
	ECM_Package = 0,
	ECM_Stream = 1,
	ECM_Text = 2
};

//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------
class ECORE_API Client
{
public:
	//-----------------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------------
	BOOL	Init(EClientMode eMode=ECM_Package);
	VOID	Destroy();

	//-----------------------------------------------------------------------------
	// 网络基本操作
	//-----------------------------------------------------------------------------
	VOID	TryConnect(LPCSTR szIP, INT nPort=0);	// 尝试连接指定地址
	BOOL	IsTryingConnect(); // 是否正在尝试连接
	
	INLINE BOOL	IsConnected()			{ return m_bConnected; }
	VOID	Disconnect();


	//------------------------------------------------------------------------------
	// 发送，如果返回FALSE,代表没有连接或没有内存空间存放所给的msg
	//------------------------------------------------------------------------------
	INLINE BOOL	Send(LPVOID pMsg, DWORD dwMsgSize);

	//------------------------------------------------------------------------------
	// 接收，如果返回NULL表示没有连接或未收到数据
	//------------------------------------------------------------------------------
	INLINE LPBYTE	Recv(DWORD& dwMsgSize) { return m_RecvMsgQueue.GetMsg(dwMsgSize); }

	//------------------------------------------------------------------------------
	// 释放接收的内存，对应于Recv(),因为内存是内部分配的，所以应该交还内部来释放
	//------------------------------------------------------------------------------
	INLINE VOID	FreeRecved(LPBYTE pMsg) { m_RecvMsgQueue.FreeMsg(pMsg); }

	//-----------------------------------------------------------------------------
	// 信息统计
	//-----------------------------------------------------------------------------
	INLINE INT		GetUnsend();									// 得到未发出的包数量
	INLINE INT		GetUnreceive();									// 得到未被上层接收的包数量
	INLINE DWORD	GetSend()		{ return m_dwSendPackNum; }		// 获取总发包数(含未发出的或发送失败的)
	INLINE DWORD	GetRecved()		{ return m_dwRecvPackNum; }		// 获取总收包数
	INLINE DWORD	GetSendBPS()	{ return m_dwSendBPS; }			// 得到每秒流出量
	INLINE DWORD	GetRecvBPS()	{ return m_dwRecvBPS; }			// 得到每秒流入量

	INLINE LPCSTR	GetServerIP()	{ return m_strIP.c_str(); }		// 得到服务器IP

	BOOL	SwitchMode(EClientMode eMode);		// 文本和流模式之间实时切换，适合HTTP传输
	BOOL	SetEndian(BOOL bBigEndian);			// 包模式下SIZE头的字节序
	

	INT32  Connect(LPCSTR strIp, INT nport);


	Client();
	~Client();

private:
	//-------------------------------------------------------------------------------
	// 内部方法
	//-------------------------------------------------------------------------------
	VOID		Clear();
	BOOL		CloseSocket(SOCKET& socket, INT nRetryTimes);


private:
	//-------------------------------------------------------------------------------
	// 线程管理器
	//-------------------------------------------------------------------------------
	ThreadMgr*				m_pThreadMgr;

	//-------------------------------------------------------------------------------
	// 网络消息相关
	//-------------------------------------------------------------------------------
	MsgQueueTS				m_SendMsgQueue;			// 发送队列
	MsgQueueTS				m_RecvMsgQueue;			// 接收队列
	
	//-------------------------------------------------------------------------------
	// 配置
	//-------------------------------------------------------------------------------
	std::string				m_strIP;					// 服务器IP地址
	INT						m_nPort;					// 服务器端口
	SOCKET					m_Sock;	
	EClientMode				m_eMode;					// 传输模式
	BOOL					m_bBigEndian;				// 包头字节序

	//--------------------------------------------------------------------------------
	// 连接状态
	//--------------------------------------------------------------------------------
	BOOL VOLATILE 			m_bConnected;

	//--------------------------------------------------------------------------------
	// 连接线程的线程ID
	//--------------------------------------------------------------------------------
	DWORD VOLATILE 			m_dwConnectID;

	//--------------------------------------------------------------------------------
	// 线程相关
	//--------------------------------------------------------------------------------
	BOOL VOLATILE 			m_bCloseConnect;			
	BOOL VOLATILE			m_bCloseSend;				
	BOOL VOLATILE 			m_bCloseReceive;			

	//--------------------------------------------------------------------------------
	// 统计
	//--------------------------------------------------------------------------------
	DWORD VOLATILE 			m_dwLastStatInTime;			// In统计开始时间
	DWORD VOLATILE 			m_dwLastStatOutTime;		// Out统计开始时间
	DWORD VOLATILE	 		m_dwSendBPS;
	DWORD VOLATILE			m_dwRecvBPS;
	DWORD VOLATILE			m_dwByteOutCounter;
	DWORD VOLATILE 			m_dwByteInCounter;
	DWORD VOLATILE			m_dwSendPackNum;
	DWORD VOLATILE 			m_dwRecvPackNum;

	//------------------------------------------------------------------------------
	// 内部线程
	//------------------------------------------------------------------------------
	DWORD Connect();		// 连接线程函数
	DWORD Send();			// 发送线程函数
	DWORD Receive();		// 接收线程函数
	DWORD StreamReceive();	// 流接收线程函数

	//-------------------------------------------------------------------------------
	// 线程关闭方法
	//-------------------------------------------------------------------------------
	INLINE DWORD CloseConnect() { InterlockedExchangeEx((LPLONG)&m_bCloseConnect,	TRUE); return 0; }
	INLINE DWORD CloseSend()	 { InterlockedExchangeEx((LPLONG)&m_bCloseSend,		TRUE); return 0; }
	INLINE DWORD CloseReceive() { InterlockedExchangeEx((LPLONG)&m_bCloseReceive,	TRUE); return 0; }

};

//-----------------------------------------------------------------------------
// 发送
//-----------------------------------------------------------------------------
INLINE BOOL Client::Send(LPVOID pMsg, DWORD dwMsgSize)
{
	ASSERT(pMsg);
	ASSERT(dwMsgSize > 0 && dwMsgSize < 0xFFFFFF);	// 数据过分长

	// 将发送信息添加到发送信息缓冲
	if( FALSE == m_bConnected )
		return FALSE;

	// 增加发送消息个数
	++m_dwSendPackNum;
	return m_SendMsgQueue.AddMsg((LPBYTE)pMsg, dwMsgSize);
}


//-----------------------------------------------------------------------------
// 得到未发送包的数量，可以以此来判断网络负荷
//-----------------------------------------------------------------------------
INLINE INT Client::GetUnsend()
{
	if( !m_bConnected )
		return 0;

	return m_SendMsgQueue.GetMsgNum();
}


//-----------------------------------------------------------------------------
// 得到已接受包的数量，可以以此来判断网络负荷
//-----------------------------------------------------------------------------
INLINE INT Client::GetUnreceive()
{
	if( !m_bConnected )
		return 0;

	return m_RecvMsgQueue.GetMsgNum();
}


}
}
