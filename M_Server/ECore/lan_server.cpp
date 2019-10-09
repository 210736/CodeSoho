//-----------------------------------------------------------------------------
// author Lyp
// date 2007-11-27
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "lan_server.h"

#include "msg_queue_ts.h"
#include "mem_cache.h"
#include "callback.h"
#include "executor_fun.h"
#include "thread_mgr.h"

namespace ECore {
namespace Lan {
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Server::Server()
{
	m_pThreadMgr	=	new ThreadMgr;
}

Server::~Server()
{
	SAFE_DEL(m_pThreadMgr);
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOL Server::Init(LogHandler* pLogHandler, INT nPort)
{
	m_pLogHandler = pLogHandler;

	m_mapClient.Clear();
	m_listDisClient.Clear();
	m_bCloseAccept = 0;
	m_bCloseDisconnect = 0;
	m_dwAcceptID = INVALID;
	m_dwDisconnectID = INVALID;

	m_dwMsgSend = 0;
	m_dwMsgRecv = 0;
	m_dwBytesSend = 0;
	m_dwBytesRecv = 0;

	// 建立网络连接
	m_Address.sin_addr.s_addr	=	htonl(INADDR_ANY);
	m_Address.sin_family		=	AF_INET;
	m_Address.sin_port			=	htons(nPort);

	m_Sock = socket(AF_INET, SOCK_STREAM, 0);

	DWORD dwCmd = 1;
	INT nResult = ioctlsocket( m_Sock, FIONBIO, &dwCmd );	// 非阻塞
	if( SOCKET_ERROR == nResult )
	{
		ERR(_T("set nonblocking mode failed in init server"));
		closesocket(m_Sock);
		return FALSE;
	}
	
	linger lin;
	lin.l_onoff = 1;
	lin.l_linger = 0;
	setsockopt(m_Sock, SOL_SOCKET, SO_LINGER, (char*)&lin, sizeof(lin));	// 软关闭

	// reuse造成多次绑定成功，可能不能找到正确可用的端口
	// 如果两台服务器开在同一台机器会使用同一个端口，造成混乱
	DWORD dwReUseAddr = TRUE;	
	setsockopt( m_Sock, SOL_SOCKET, SO_REUSEADDR, (char*)&dwReUseAddr, sizeof(dwReUseAddr) );

	// 关闭Nagle算法
	BOOL bNoDelay = TRUE;
	setsockopt(m_Sock, IPPROTO_TCP, TCP_NODELAY, (char*)&bNoDelay, sizeof(bNoDelay));

	INT nAddresslen = sizeof(m_Address);
	if( bind(m_Sock, (sockaddr*)&m_Address, nAddresslen) )
	{
		ERR(_T("bind error"));
		closesocket(m_Sock);
		return FALSE;
	}
	else
	{
		listen(m_Sock, 5);	// 侦听

		CallbackBase* pAcceptExec	= new CallbackP0<Server>(this, &Server::DoAccept);
		CallbackBase* pAcceptEnd	= new CallbackP0<Server>(this, &Server::CloseAccept);
		m_dwAcceptID = m_pThreadMgr->CreateThread(new ExecutorFun(pAcceptExec, pAcceptEnd), TRUE);
		ASSERT(VALID(m_dwAcceptID));

		CallbackBase* pDisconnectExec	= new CallbackP0<Server>(this, &Server::DoDisconnect);
		CallbackBase* pDisconnectEnd	= new CallbackP0<Server>(this, &Server::CloseDisconnect);
		m_dwDisconnectID = m_pThreadMgr->CreateThread(new ExecutorFun(pDisconnectExec, pDisconnectEnd), TRUE);
		ASSERT(VALID(m_dwDisconnectID));
	}

	return TRUE;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOL Server::Destroy()
{
	m_pThreadMgr->StopThread(m_dwAcceptID);

	std::list<Client*> list;
	m_mapClient.ExportAllValue(list);

	if( VALID(m_pLogHandler) )
		m_pLogHandler->m_bAutoClearClient = TRUE;	// 将自动清除临时设置为TRUE

	std::list<Client*>::iterator it;
	for(it = list.begin(); it != list.end(); ++it)
		this->DisconnetClient((*it));	// 关闭所有的客户端

	m_pThreadMgr->StopThread(m_dwDisconnectID);
	m_pThreadMgr->ShutDown();

	closesocket(m_Sock);
	return TRUE;
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOL Server::Kick(DWORD dwClientID)
{
	Client* pClient = m_mapClient.Peek(dwClientID);
	if( !VALID(pClient) ) 
		return FALSE;

	this->DisconnetClient(pClient);
	return TRUE;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOL Server::GetClientAddress(sockaddr_in* pAddr, DWORD dwClientID)
{
	Client* pClient = m_mapClient.Peek(dwClientID);
	if( !VALID(pClient) ) 
		return FALSE;

	INT nLen = (INT)sizeof(sockaddr_in);
	if( 0 == getpeername(pClient->Sock, (sockaddr*)pAddr, &nLen) )
		return TRUE;
	else
		return FALSE;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
DWORD Server::DoAccept()
{
	fd_set fdread;
	timeval block_time;
	block_time.tv_sec = 0;
	block_time.tv_usec = NET_BLOCK_TIME;
	INT nAddlen = (INT)sizeof(m_Address);
	
	while( !m_bCloseAccept )
	{
		FD_ZERO(&fdread);
		FD_SET(m_Sock, &fdread);

		if( 1 == select(0, &fdread, NULL, NULL, &block_time) )	// 侦听阻塞
		{
			Client* pClient = new Client;
			if( NULL == pClient )
				continue;

			pClient->Sock	= accept(m_Sock, (sockaddr*)&(m_Address), &nAddlen);
			if( INVALID_SOCKET == pClient->Sock )
			{	
				SAFE_DEL(pClient);
				continue;
			}

			pClient->pSendQueue = new MsgQueueTS(TRUE, TRUE);
			pClient->pRecvQueue = new MsgQueueTS(FALSE, FALSE);
			if( NULL == pClient->pSendQueue || NULL == pClient->pRecvQueue )
			{
				SAFE_DEL(pClient->pSendQueue);
				SAFE_DEL(pClient->pRecvQueue);
				closesocket(pClient->Sock);
				SAFE_DEL(pClient);
				continue;
			}

			pClient->dwConnectTime = timeGetTime();

			CallbackBase* pExec1 = new CallbackP1<Server, Client*>(this, &Server::DoRecv, pClient);
			pClient->dwRecvID = m_pThreadMgr->CreateThread(new ExecutorFun(pExec1, NULL), TRUE);

			CallbackBase* pExec2 = new CallbackP1<Server, Client*>(this, &Server::DoSend, pClient);
			pClient->dwSendID = m_pThreadMgr->CreateThread(new ExecutorFun(pExec2, NULL), TRUE);
		}
	}
	
	return 0;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
DWORD Server::DoDisconnect()
{
	while( !m_bCloseDisconnect || m_listDisClient.Size() > 0 )
	{
		Client* pClient = m_listDisClient.PopFront();
		if( !VALID(pClient) )
		{
			Sleep(SLEEP_TIME);
			continue;
		}

		m_pThreadMgr->StopThread(pClient->dwRecvID);
		m_pThreadMgr->StopThread(pClient->dwSendID);
		SAFE_DEL(pClient->pRecvQueue);
		SAFE_DEL(pClient->pSendQueue);
		closesocket(pClient->Sock);
		SAFE_DEL(pClient);
	}

	return 0;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
DWORD Server::DoSend(Client* pClient)
{
	fd_set fdwrite;
	timeval block_time;
	block_time.tv_sec = 0;
	block_time.tv_usec = NET_BLOCK_TIME;
	
	while( !pClient->bClose )
	{
		for(;;)
		{
			DWORD dwMsgSize = 0;
			LPBYTE pMsg = pClient->pSendQueue->GetMsg(dwMsgSize);
			if( NULL == pMsg ) 
				break;

			LPBYTE pTempPtr = pMsg;
			while( dwMsgSize > 0 && !pClient->bClose )
			{
				INT nReturn = send(pClient->Sock, (char*)pTempPtr, dwMsgSize, 0);
				if( SOCKET_ERROR == nReturn )
				{
					switch(WSAGetLastError())
					{
					case WSAEWOULDBLOCK:
					case WSAENOBUFS:
						FD_ZERO(&fdwrite);
						FD_SET(pClient->Sock, &fdwrite);
						select(0, NULL, &fdwrite, NULL, &block_time);
						break;

					default:	// 网络错误
						pClient->pSendQueue->FreeMsg(pMsg);
						goto __thread_send_end;
						break;
					}
				}
				else
				{
					pTempPtr += nReturn;
					dwMsgSize -= nReturn;
					m_dwBytesSend += nReturn;
				}
			}

			pClient->pSendQueue->FreeMsg(pMsg);
			++m_dwMsgSend;
		}

		::WaitForSingleObject(pClient->pSendQueue->GetEvent(), 200);
	}

__thread_send_end:
	this->DisconnetClient(pClient);
	return 0;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
DWORD Server::DoRecv(Client* pClient)
{
	fd_set fdread;
	timeval block_time;
	block_time.tv_sec = 0;
	block_time.tv_usec = NET_BLOCK_TIME;
	INT nResult = 0;

	while( !pClient->bClose )
	{
		DWORD dwMsgSize = 0;
		DWORD dwTempSize = sizeof(DWORD);
		CHAR* pTempPtr = (CHAR*)&dwMsgSize;

		while( dwTempSize > 0 )
		{
			// 先取得数据的大小
			FD_ZERO(&fdread);
			FD_SET(pClient->Sock, &fdread);
			if( 1 == select(0, &fdread, NULL, NULL, &block_time) )
			{
				nResult = recv(pClient->Sock, pTempPtr, dwTempSize, 0);
				if( 0 == nResult || SOCKET_ERROR == nResult )
					goto __thread_recv_end;

				dwTempSize -= nResult;
				pTempPtr += nResult;
				m_dwBytesRecv += nResult;
			}

			if( pClient->bClose )
				goto __thread_recv_end;

			if( 0 == pClient->nRecvNum && timeGetTime() - pClient->dwConnectTime > 1000 * 60 * 3 )
				goto __thread_recv_end;	// 3分钟没有发送第一个验证包
		}

		if( SOCKET_ERROR != nResult )	// 接受实际信息
		{
			char* pTemp = (char*)MCALLOC(dwMsgSize);
			dwTempSize = dwMsgSize;
			pTempPtr = pTemp;
			while( dwTempSize > 0 )
			{
				FD_ZERO(&fdread);
				FD_SET(pClient->Sock, &fdread);
				if( 1 == select(0, &fdread, NULL, NULL, &block_time) )
				{
					nResult = recv(pClient->Sock, pTempPtr, dwTempSize, 0);
					if( 0 == nResult || nResult == SOCKET_ERROR )	
					{
						MCFREE(pTemp);
						goto __thread_recv_end;
					}

					dwTempSize -= nResult;
					pTempPtr += nResult;
					m_dwBytesRecv += nResult;
				}

				if( 0 == pClient->nRecvNum && timeGetTime() - pClient->dwConnectTime > 1000 * 60 * 3 )
					goto __thread_recv_end; // 3分钟没有发送第一个验证包

				if( pClient->bClose )
					goto __thread_recv_end;
			}

			pClient->pRecvQueue->AddMsg(pTemp, dwMsgSize);
			++pClient->nRecvNum;
			if( 1 == pClient->nRecvNum )
			{
				DWORD dwClientID = m_pLogHandler->Logon((LPBYTE)pTemp, dwMsgSize);	// 第一个包，登陆
				if( INVALID == dwClientID )
				{
					MCFREE(pTemp);
					goto __thread_recv_end;
				}

				pClient->dwID = dwClientID;
				m_mapClient.Add(dwClientID, pClient);
			}

			++m_dwMsgRecv;
			MCFREE(pTemp);
		}
	}

__thread_recv_end:
	this->DisconnetClient(pClient);
	return 0;
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
VOID Server::DisconnetClient(Client* pClient)
{
	m_DisconnectLock.Lock();
	if( pClient->bClose )
	{
		m_DisconnectLock.Unlock();
		return;
	}

	if( VALID(pClient->dwID) )	// 登陆成功，才调用上层登出
		m_pLogHandler->Logoff(pClient->dwID);

	shutdown(pClient->Sock, SD_SEND);
	InterlockedExchangeEx((LPLONG)&pClient->bClose, TRUE);
	
	if( m_pLogHandler->m_bAutoClearClient )
	{
		m_mapClient.Erase(pClient->dwID);
		m_listDisClient.PushBack(pClient);	// 交给线程释放
	}

	m_DisconnectLock.Unlock();
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
DWORD Server::CloseAccept()
{
	InterlockedExchangeEx((LPLONG)&m_bCloseAccept, TRUE); 
	return 0;
}


//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
DWORD Server::CloseDisconnect()
{
	InterlockedExchangeEx((LPLONG)&m_bCloseDisconnect, TRUE);
	return 0;
}


//-----------------------------------------------------------------------------
// 手动清除客户端
//-----------------------------------------------------------------------------
BOOL Server::ClearClient(DWORD dwClientID)
{
	Client* pClient = m_mapClient.Peek(dwClientID);
	if( !VALID(pClient) ) 
		return FALSE;

	m_mapClient.Erase(dwClientID);
	m_listDisClient.PushBack(pClient);	// 交给线程释放
	return TRUE;
}


}
}