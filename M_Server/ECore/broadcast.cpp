//-----------------------------------------------------------------------------
// author Lyp
// date 2004-08-04
// last 2004-08-04
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "broadcast.h"

#include "msg_queue_ts.h"
#include "callback.h"
#include "executor_fun.h"
#include "thread_mgr.h"

namespace ECore {

//-----------------------------------------------------------------------------
// 构造函数
//-----------------------------------------------------------------------------
Broadcast::Broadcast()
{
	m_pThreadMgr = new ThreadMgr;			// 初始化线程管理器

	m_dwTDBroadcastID		=	INVALID;
	m_dwTDRecvBroadcastID	=	INVALID;
	m_bCloseBroadcast		=	FALSE;
	m_bCloseRecvBroadcast	=	FALSE;

	m_pSendMsgQueue = NULL;
	m_pRecvMsgQueue = NULL;
}

//-----------------------------------------------------------------------------
// 析构函数
//-----------------------------------------------------------------------------
Broadcast::~Broadcast()
{
	Destroy();

	SAFE_DEL(m_pThreadMgr);		// 删除线程管理器
}

//-----------------------------------------------------------------------------
// 初始化
// szLocalIP绑定到本地什么IP，可以为空
//-----------------------------------------------------------------------------
BOOL Broadcast::Init(INT nPort, LPCSTR szLocalIP)
{
	this->Destroy();

	// 初始化IP和端口
	m_nPort = nPort;
	if( szLocalIP )
		strcpy((char*)m_szLocalIP, szLocalIP);

	// 初始化各种数据
	m_dwTDBroadcastID		=	INVALID;
	m_dwTDRecvBroadcastID	=	INVALID;
	m_bCloseBroadcast		=	FALSE;
	m_bCloseRecvBroadcast	=	FALSE;

	// 申请发送接受缓冲
	m_pSendMsgQueue = new MsgQueueTS(TRUE, FALSE);
	m_pRecvMsgQueue = new MsgQueueTS(FALSE, FALSE);
	if( NULL == m_pSendMsgQueue || NULL == m_pRecvMsgQueue )
	{
		SAFE_DEL(m_pSendMsgQueue);
		SAFE_DEL(m_pRecvMsgQueue);
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
VOID Broadcast::Destroy()
{
	// 关闭线程
	m_pThreadMgr->ShutDown();

	SAFE_DEL(m_pSendMsgQueue);
	SAFE_DEL(m_pRecvMsgQueue);

	m_bCloseBroadcast		=	FALSE;
	m_bCloseRecvBroadcast	=	FALSE;
	m_dwTDBroadcastID		=	INVALID;
	m_dwTDRecvBroadcastID	=	INVALID;
}


//-----------------------------------------------------------------------------
// 开始广播
//-----------------------------------------------------------------------------
BOOL Broadcast::StartBroadcast()
{
	// 启动广播线程
	CallbackBase* pExec = new CallbackP0<Broadcast>(this, &Broadcast::TDBroadcast);
	CallbackBase* pEnd	= new CallbackP0<Broadcast>(this, &Broadcast::CloseTDBroadcast);

	m_dwTDBroadcastID = m_pThreadMgr->CreateThread(new ExecutorFun(pExec, pEnd), TRUE);

	return (VALID(m_dwTDBroadcastID));
}


//-----------------------------------------------------------------------------
// 开始收听广播
//-----------------------------------------------------------------------------
BOOL Broadcast::StartRecvBroadcast()
{
	// 启动广播线程
	CallbackBase* pExec = new CallbackP0<Broadcast>(this, &Broadcast::TDRecvBroadcast);
	CallbackBase* pEnd	= new CallbackP0<Broadcast>(this, &Broadcast::CloseTDRecvBroadcast);

	m_dwTDRecvBroadcastID = m_pThreadMgr->CreateThread(new ExecutorFun(pExec, pEnd), TRUE);

	return (VALID(m_dwTDBroadcastID));
}

//-----------------------------------------------------------------------------
// 结束广播线程
//-----------------------------------------------------------------------------
BOOL Broadcast::CloseBroadcast()
{
	if( VALID(m_dwTDBroadcastID) )
	{
		m_pThreadMgr->StopThread(m_dwTDBroadcastID);

		InterlockedExchangeEx((LPLONG)&m_bCloseBroadcast, FALSE);
		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 结束接收广播线程
//-----------------------------------------------------------------------------
BOOL Broadcast::CloseRecvBroadcast()
{
	if( VALID(m_dwTDRecvBroadcastID) )
	{
		m_pThreadMgr->StopThread(m_dwTDRecvBroadcastID);

		InterlockedExchangeEx((LPLONG)&m_bCloseRecvBroadcast, FALSE);
		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 广播
//-----------------------------------------------------------------------------
VOID Broadcast::Broad(LPBYTE pMsg, DWORD dwMsgSize)
{
	ASSERT(NULL != pMsg);
	ASSERT(dwMsgSize > 0 && dwMsgSize < 1024);

	m_pSendMsgQueue->AddMsg(pMsg, dwMsgSize);
}


//-----------------------------------------------------------------------------
// 得到接收到的广播信息，没有就返回NULL
//-----------------------------------------------------------------------------
LPBYTE Broadcast::RecvBroadcast(DWORD& dwMsgSize)
{
	return m_pRecvMsgQueue->GetMsg(dwMsgSize);
}


//-----------------------------------------------------------------------------
// 归还收到的消息内存
//-----------------------------------------------------------------------------
VOID Broadcast::FreeRecved(LPBYTE pMsg)
{
	m_pRecvMsgQueue->FreeMsg(pMsg);
}


//-----------------------------------------------------------------------------
// 广播线程
//-----------------------------------------------------------------------------
DWORD Broadcast::TDBroadcast()
{
	SOCKET sock = socket(PF_INET, SOCK_DGRAM, 0);
	if( INVALID_SOCKET == sock )
	{
		ERR(_T("couldn't create socket in thread broadcast"));
		return 0;
	}

	// 设置重用端口
	DWORD dwReUseAddr = TRUE;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, 
		(char*)&dwReUseAddr, sizeof(dwReUseAddr));

	// 绑定
	sockaddr_in	add;
	ZeroMemory(&add, sizeof(add));
	add.sin_family = PF_INET;
	add.sin_addr.s_addr = htonl(INADDR_ANY);
	add.sin_port = htons((unsigned short)m_nPort);

	if( bind(sock, (struct sockaddr*)&add, sizeof(add)) != 0 )
	{
		// 可能本地已经创建了一个客户端
		ERR(_T("bind error in thread broadcast"));
		closesocket(sock);
		return 0;
	}

	// 设置广播属性
	BOOL opt = TRUE;
	INT nResult = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof(BOOL));
	if( SOCKET_ERROR == nResult )
	{
		ERR(_T("setsockopt failed"));
		closesocket(sock);
		return 0;
	}
	
	// 发送目标地址
	sockaddr_in dest;
	ZeroMemory(&dest, sizeof(dest));
	dest.sin_family = PF_INET;
	dest.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	dest.sin_port = htons((unsigned short)m_nPort);

	while( !m_bCloseBroadcast )
	{
		while(1)
		{
			DWORD dwSize = 0;
			LPBYTE pMsg = m_pSendMsgQueue->GetMsg(dwSize);
			if( NULL == pMsg )
				break;

			INT nSent = sendto(sock, (const char*)pMsg, dwSize, 
				0, (sockaddr*)&dest, sizeof(dest));

			if( nSent != (INT)dwSize )
				ERR(_T("send udp broadcast message failed"));
		}

		::WaitForSingleObject(m_pSendMsgQueue->GetEvent(), 120);
	}

	closesocket(sock);
	return 0;
}

//-----------------------------------------------------------------------------
// 接收广播线程
//-----------------------------------------------------------------------------
DWORD Broadcast::TDRecvBroadcast()
{
	SOCKET sock = socket(PF_INET, SOCK_DGRAM, 0);
	if( INVALID_SOCKET == sock )
	{
		ERR(_T("create socket failed in thread recv broadcast"));
		return 0;
	}

	// 使用非阻塞方式
	DWORD dwCmd = 1;
	INT nResult = ioctlsocket(sock, FIONBIO, &dwCmd);
	if( SOCKET_ERROR == nResult )
	{
		ERR(_T("set nonblocking mode failed in thread recv broadcast"));
		closesocket(sock);
		return 0;
	}

	// 重用端口
	DWORD dwReUseAddr = TRUE;
	setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, 
		(char*)&dwReUseAddr, sizeof(dwReUseAddr) );

	// 绑定
	sockaddr_in	add;
	ZeroMemory(&add, sizeof(add));
	add.sin_family = PF_INET;
	add.sin_addr.s_addr = htonl(INADDR_ANY);
	add.sin_port = htons((unsigned short)m_nPort);
	if( SOCKET_ERROR == bind(sock, (struct sockaddr*)&add, sizeof(add)) )
	{
		ERR(_T("bind error in thread recv broadcast"));
		closesocket(sock);
		return 0;
	}

	// 设置阻塞时间
	timeval block_time;
	block_time.tv_sec = 0;
	block_time.tv_usec = NET_BLOCK_TIME;

	sockaddr_in	source;
	ZeroMemory(&source, sizeof(source));

	while( !m_bCloseRecvBroadcast )
	{
		fd_set fdread;
		FD_ZERO(&fdread);
		FD_SET(sock, &fdread);
		nResult = select(0, &fdread, NULL, NULL, &block_time);

		if( 1 == nResult )
		{
			BYTE msg[1024];
			INT nLen = sizeof(sockaddr_in);

			nResult = recvfrom(sock, (char*)msg, sizeof(msg)-4, 0, 
				(sockaddr*)&source, &nLen);

			if( SOCKET_ERROR == nResult || 0 == nResult )
			{
				ERR(_T("recvfrom failed in thread recv broadcast"));
			}
			else
			{
				// 将原地址放入紧接着消息的4字节
				*(DWORD*)&msg[nResult] = source.sin_addr.S_un.S_addr; 
				m_pRecvMsgQueue->AddMsg(&msg, nResult+4);
			}
		}
	}

	closesocket(sock);
	return 0;
}



} // namespace ECore {
