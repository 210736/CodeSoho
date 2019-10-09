//-----------------------------------------------------------------------------
// author Lyp
// date 2004-08-04
// last 2004-08-04
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
class Broadcast
{
public:
	BOOL	Init(INT nPort, LPCSTR szLocalIP=NULL);
	VOID	Destroy();

	//-------------------------------------------------------------------------
	// 广播数据
	//-------------------------------------------------------------------------
	VOID	Broad(LPBYTE pMsg, DWORD dwMsgSize);

	//-------------------------------------------------------------------------
	// 接收广播
	//-------------------------------------------------------------------------
	LPBYTE	RecvBroadcast(DWORD& dwMsgSize);

	//-------------------------------------------------------------------------
	// 归还收取的广播数据
	//-------------------------------------------------------------------------
	VOID	FreeRecved(LPBYTE pMsg);


	Broadcast();
	~Broadcast();


private:	
	MsgQueueTS*		m_pSendMsgQueue;				// 发送队列
	MsgQueueTS*		m_pRecvMsgQueue;				// 接收队列

	ThreadMgr*		m_pThreadMgr;					// 线程管理器

	DWORD			m_dwTDBroadcastID;				// 广播线程的线程池ID
	DWORD			m_dwTDRecvBroadcastID;			// 接收广播线程的线程池ID
	BOOL VOLATILE 	m_bCloseBroadcast;				// 广播线程结束标志
	BOOL VOLATILE 	m_bCloseRecvBroadcast;			// 接收广播线程结束标志

	INT VOLATILE 	m_nPort;						// 绑定端口
	TCHAR VOLATILE 	m_szLocalIP[LONG_STRING];		// 绑定地址

	//-------------------------------------------------------------------------
	// 开始广播/接收广播线程
	//-------------------------------------------------------------------------
	BOOL	StartBroadcast();
	BOOL	StartRecvBroadcast();

	//-------------------------------------------------------------------------
	// 结束广播/接收广播线程
	//-------------------------------------------------------------------------
	BOOL	CloseBroadcast();
	BOOL	CloseRecvBroadcast();

	//-------------------------------------------------------------------------
	// 线程函数
	//-------------------------------------------------------------------------
	DWORD	TDBroadcast();							// 广播线程
	DWORD	TDRecvBroadcast();						// 接收广播线程

	//-------------------------------------------------------------------------
	// 关闭线程函数
	//-------------------------------------------------------------------------
	DWORD	CloseTDBroadcast()		{ InterlockedExchangeEx((LPLONG)&m_bCloseBroadcast, TRUE); return 0; }
	DWORD	CloseTDRecvBroadcast()	{ InterlockedExchangeEx((LPLONG)&m_bCloseRecvBroadcast, TRUE); return 0; }
};

} // namespace ECore {
