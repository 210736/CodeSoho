//-----------------------------------------------------------------------------
// author Lyp
// date 2007-11-27
//-----------------------------------------------------------------------------
#pragma once
namespace ECore {
namespace Lan {
	
struct LogHandler
{  
	virtual DWORD Logon(LPBYTE pByte, DWORD dwSize)=0;
	virtual VOID Logoff(DWORD dwClientID)=0;


	// m_bAutoClearClient ： 如果客户端断连，是否自动清除客户端信息，包含未处理的网络消息
	// 如果设置为FALSE. 当有客户端断连时，外部必须手动调用ClearClient()函数
	BOOL m_bAutoClearClient;	
	
	LogHandler() { m_bAutoClearClient = TRUE; }
};


//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------
class ECORE_API Server
{
public:
	BOOL	Init(LogHandler* pLogHandler, INT nPort);
	BOOL	Destroy();

	//-----------------------------------------------------------------------------
	// 基本操作
	//-----------------------------------------------------------------------------
	INLINE INT		Send(DWORD dwClientID, LPVOID pMsg, DWORD dwSize);
	INLINE LPBYTE	Recv(DWORD dwClientID, DWORD& dwSize, INT& nUnRecved);
	INLINE VOID		FreeRecved(DWORD dwClientID, LPBYTE pMsg) ;

	BOOL	Kick(DWORD dwClientID);
	BOOL	ClearClient(DWORD dwClientID);
	BOOL	GetClientAddress(sockaddr_in* pAddr, DWORD dwClientID);

	//-----------------------------------------------------------------------------
	// 信息统计
	//-----------------------------------------------------------------------------
	INLINE INT		GetPort()					{ return (INT)ntohs(m_Address.sin_port); }
	INLINE INT		GetClientNum()				{ return m_mapClient.Size(); }  
	INLINE BOOL		IsClientExist(DWORD dwID)	{ return m_mapClient.IsExist(dwID); }
	INLINE DWORD	GetMsgSend()				{ return m_dwMsgSend; }
	INLINE DWORD	GetMsgRecv()				{ return m_dwMsgRecv; }
	INLINE DWORD	GetBytesSend()				{ return m_dwBytesSend; }
	INLINE DWORD	GetBytesRecv()				{ return m_dwBytesRecv; }


	Server();
	~Server();


private:
	struct Client
	{
		SOCKET			Sock;				// 套接字接口
		DWORD			dwID;				// 客户端ID
		BOOL VOLATILE 	bClose;				// 是否已经终止
		DWORD			dwSendID;			// 发送线程ID
		DWORD			dwRecvID;			// 接收线程ID
		MsgQueueTS*		pSendQueue;			// 发送队列
		MsgQueueTS*		pRecvQueue;			// 接收队列
		DWORD			dwConnectTime;		// 连接时间
		INT				nRecvNum;			// 收包总量

		Client() { ZeroMemory(this, sizeof(*this)); dwID = INVALID; }
	};

	ThreadMgr*					m_pThreadMgr;			// 线程管理器
	LogHandler*					m_pLogHandler;			// 登入登出处理接口

	Mutex						m_DisconnectLock;		// 断开连接时的锁定

	MapTS<DWORD, Client*>		m_mapClient;			// 记录用户nCDIndex和ID情况
	ListTS<Client*>				m_listDisClient;		// 将要删除的客户端队列

	DWORD						m_dwAcceptID;			// 接收连接线程的线程池ID
	DWORD						m_dwDisconnectID;		// 关闭连接线程的线程池ID
	BOOL VOLATILE 				m_bCloseAccept;			// 是否需要关闭Accept线程
	BOOL VOLATILE 				m_bCloseDisconnect;		// 是否需要关闭断连线程
	sockaddr_in					m_Address;				// server address
	SOCKET						m_Sock;					// 用来Accept的socket

	DWORD VOLATILE 				m_dwMsgSend;			// 发包统计
	DWORD VOLATILE 				m_dwMsgRecv;			// 收包统计
	DWORD VOLATILE 				m_dwBytesSend;			// 发包字节数统计
	DWORD VOLATILE 				m_dwBytesRecv;			// 收包字节数统计

	DWORD	DoAccept();
	DWORD	DoDisconnect();
	DWORD	DoSend(Client*);
	DWORD	DoRecv(Client*);
	VOID	DisconnetClient(Client* pClient);
	DWORD	CloseAccept();
	DWORD	CloseDisconnect();
	
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
INLINE INT Server::Send(DWORD dwClientID, LPVOID pMsg, DWORD dwSize)
{
	Client* pClient = m_mapClient.Peek(dwClientID);
	if( !VALID(pClient) || pClient->bClose )
		return INVALID;

	pClient->pSendQueue->AddMsg(pMsg, dwSize);
	return pClient->pSendQueue->GetMsgNum();
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
INLINE LPBYTE Server::Recv(DWORD dwClientID, DWORD& dwSize, INT& nUnRecved)
{
	Client* pClient = m_mapClient.Peek(dwClientID);
	if( !VALID(pClient) ) 
		return NULL;

	nUnRecved = pClient->pRecvQueue->GetMsgNum();
	return pClient->pRecvQueue->GetMsg(dwSize);
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
INLINE VOID Server::FreeRecved(DWORD dwClientID, LPBYTE pMsg) 
{
	Client* pClient = m_mapClient.Peek(dwClientID);
	if( !VALID(pClient) ) 
		return;

	pClient->pRecvQueue->FreeMsg(pMsg);
}

}
}