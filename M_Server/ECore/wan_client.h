//--------------------------------------------------------------------------------
// author Lyp
// date 2003-03-17
// last 2009-12-16
//--------------------------------------------------------------------------------
#pragma once

namespace ECore {
namespace Wan {
//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------
class ECORE_API Client
{
public:
	BOOL	Init();
	VOID	Destroy();

	//-----------------------------------------------------------------------------
	// 网络基本操作
	//-----------------------------------------------------------------------------
	BOOL	TryConnect(LPCSTR szIP, INT nPort);	// 尝试连接
	BOOL	IsTryingConnect();					// 是否正尝试连接
	VOID	Disconnect(); // 断开连接
	INLINE BOOL	IsConnected() { return m_bConnected; }

	INLINE BOOL		Send(LPVOID pMsg, DWORD dwMsgSize);
	INLINE LPBYTE	Recv(DWORD& dwMsgSize);
	INLINE VOID		FreeRecved(LPBYTE pMsg);
	
	INLINE HANDLE GetRecvEvent() { return m_RecvMsgQueue.GetEvent(); } // 有包收到，将被激活
	//------------------------------------------------------------------------------
	// 信息统计
	//------------------------------------------------------------------------------
	INLINE INT		GetUnsend(); // 未发送的包数量，可以以此来判断网络负荷
	INLINE INT		GetUnreceive();	// 未被上层收的包数量
	INLINE DWORD	GetSend() { return m_dwSendPackNum; }	// 总发包数
	INLINE DWORD	GetRecved() { return m_dwRecvPackNum; }	// 总收包数
	INLINE DWORD	GetSendBPS() { return m_dwSendBPS; }	// 秒流量
	INLINE DWORD	GetRecvBPS() { return m_dwRecvBPS; }	// 秒流量
	INLINE LPCSTR	GetServerIP() { return m_strIP.c_str(); }	// 得到服务器IP


	Client();
	~Client();

private:
	TObj<Filter>		m_pFilter;
	ThreadMgr*			m_pThreadMgr;

	MsgQueueTS			m_SendMsgQueue;	// 发送队列
	MsgQueueTS			m_RecvMsgQueue;	// 接收队列
	
	//-------------------------------------------------------------------------------
	// 网络
	//-------------------------------------------------------------------------------
	std::string			m_strIP;			// 服务器IP地址
	INT					m_nPort;			// 服务器端口
	SOCKET				m_Sock;				// 套接字
	BOOL VOLATILE		m_bConnected;		// 是否连接上

	//--------------------------------------------------------------------------------
	// 线程相关
	//--------------------------------------------------------------------------------
	DWORD VOLATILE 		m_dwTDConnectID;	// 连接线程的线程池ID
	BOOL VOLATILE 		m_bCloseConnect;	// 结束连接线程
	BOOL VOLATILE 		m_bCloseSend;		// 结束发送线程
	BOOL VOLATILE 		m_bCloseReceive;	// 结束接收线程

	//--------------------------------------------------------------------------------
	// 统计
	//--------------------------------------------------------------------------------
	DWORD VOLATILE 		m_dwLastStatInTime;	// 上次In统计时间
	DWORD VOLATILE 		m_dwLastStatOutTime;// 上次Out统计时间
	DWORD VOLATILE 		m_dwSendBPS;
	DWORD VOLATILE 		m_dwRecvBPS;
	DWORD VOLATILE 		m_dwByteOutCounter;
	DWORD VOLATILE 		m_dwByteInCounter;
	DWORD VOLATILE 		m_dwSendPackNum;	// 总发包数(包含发送失败和还未发出去的)
	DWORD VOLATILE 		m_dwRecvPackNum;	// 总收包数(包含还未处理的)


	//-------------------------------------------------------------------------------
	//
	//-------------------------------------------------------------------------------
	VOID	Clear();
	BOOL	CloseSocket(SOCKET& socket, INT nRetryTimes);
	DWORD	TDConnect();
	DWORD	TDSend();
	DWORD	TDReceive();
	INLINE DWORD	CloseTDConnect()	{ InterlockedExchangeEx((LPLONG)&m_bCloseConnect,	TRUE); return 0; }
	INLINE DWORD	CloseTDSend()		{ InterlockedExchangeEx((LPLONG)&m_bCloseSend,		TRUE); return 0; }
	INLINE DWORD	CloseTDReceive()	{ InterlockedExchangeEx((LPLONG)&m_bCloseReceive,	TRUE); return 0; }
};


//-----------------------------------------------------------------------------
// 发送
//-----------------------------------------------------------------------------
INLINE BOOL Client::Send(LPVOID pMsg, DWORD dwMsgSize)
{
	ASSERT(dwMsgSize < 0xFFFFFF);

	// 将发送信息添加到发送信息缓冲
	if( FALSE == m_bConnected )
		return FALSE;

	return m_SendMsgQueue.AddMsg((LPBYTE)pMsg, dwMsgSize);
}


//-----------------------------------------------------------------------------
// 接收
//-----------------------------------------------------------------------------
INLINE LPBYTE Client::Recv(DWORD& dwMsgSize)
{
	// 从接受缓冲接收消息
	return m_RecvMsgQueue.GetMsg(dwMsgSize);
}


//-----------------------------------------------------------------------------
// 释放已经收到的消息
//-----------------------------------------------------------------------------
INLINE VOID Client::FreeRecved(LPBYTE pMsg) 
{
	m_RecvMsgQueue.FreeMsg(pMsg);
}


//-----------------------------------------------------------------------------
// 得到未发送包的数量，可以以此来判断网络负荷
//-----------------------------------------------------------------------------
INLINE INT	Client::GetUnsend()
{
	if( !m_bConnected )
		return 0;

	return m_SendMsgQueue.GetMsgNum();
}


//-----------------------------------------------------------------------------
// 得到已接受包的数量，可以以此来判断网络负荷
//-----------------------------------------------------------------------------
INLINE INT	Client::GetUnreceive()
{
	if( !m_bConnected )
		return 0;

	return m_RecvMsgQueue.GetMsgNum();
}

}
}
