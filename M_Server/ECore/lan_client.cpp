//-----------------------------------------------------------------------------
// author Lyp
// date 2003-03-17
// last 2010-01-13
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "lan_client.h"

#include "msg_queue_ts.h"
#include "mem_cache.h"
#include "callback.h"
#include "executor_fun.h"
#include "thread_mgr.h"

#define ONE_SECOND	1000
#define TEMP_BUFF	512*1024


namespace ECore {
namespace Lan {

//-----------------------------------------------------------------------------
// 构造和析构
//-----------------------------------------------------------------------------
Client::Client() : m_SendMsgQueue(TRUE, TRUE), m_RecvMsgQueue(TRUE, FALSE)
{
	Clear();
	m_bBigEndian = 0;	// 默认Little Endian
	m_pThreadMgr = new ThreadMgr;
}

Client::~Client()
{
	this->Destroy();
	SAFE_DEL(m_pThreadMgr);
}


//-----------------------------------------------------------------------------
// 清除
//-----------------------------------------------------------------------------
VOID Client::Clear()
{
	m_strIP.clear();
	m_nPort			= 0;
	m_Sock			= 0;
	m_bCloseConnect = 0;
	m_bCloseSend	= 0;
	m_bCloseReceive	= 0;
	m_bConnected	= 0;

	m_dwConnectID	= INVALID;

	m_SendMsgQueue.Clear();
	m_RecvMsgQueue.Clear();
}



//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL Client::Init(EClientMode eMode)
{
	this->Clear();

	m_eMode = eMode;
	
	if( ECM_Package == m_eMode )	// 根据发送模式来决定是否在包前面加入大小信息
		m_SendMsgQueue.SetAddSizeAhead(TRUE);
	else
		m_SendMsgQueue.SetAddSizeAhead(FALSE);

	// 创建socket
	m_Sock = socket(AF_INET, SOCK_STREAM, 0);
	if( INVALID_SOCKET == m_Sock )
		return FALSE;

	// 使用非阻塞方式,linger行为
	DWORD dwCmd = 1;
	INT nResult = ioctlsocket(m_Sock, FIONBIO, &dwCmd);
	if( SOCKET_ERROR == nResult )
	{
		closesocket(m_Sock);
		m_Sock = 0;
		return FALSE;
	}

	// 关闭Nagle算法
	BOOL bNoDelay = TRUE;
	setsockopt(m_Sock, IPPROTO_TCP, TCP_NODELAY, (char*)&bNoDelay, sizeof(bNoDelay));

	// 设置端口重用
	DWORD dwReUseAddr = TRUE;
	setsockopt(m_Sock, SOL_SOCKET, SO_REUSEADDR, (char*)&dwReUseAddr, sizeof(dwReUseAddr));

	// 指定硬关闭方式
	linger lin;
	lin.l_linger = 0;
	lin.l_onoff = 1;
	setsockopt(m_Sock, SOL_SOCKET, SO_LINGER, (char*)&lin, sizeof(lin));

	// 每秒流量统计
	m_dwLastStatOutTime = timeGetTime();
	m_dwLastStatInTime = timeGetTime();
	m_dwSendBPS = 0;
	m_dwRecvBPS = 0;
	m_dwByteOutCounter = 0;
	m_dwByteInCounter = 0;

	m_dwSendPackNum = 0;
	m_dwRecvPackNum = 0;

	return TRUE;
}


//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
void Client::Destroy()
{
	// 关闭线程池
	m_pThreadMgr->ShutDown();

	// 重置连接线程ID
	m_dwConnectID = INVALID;

	// 完美关闭socket
	if( m_bConnected )
	{
		CloseSocket(m_Sock, 20);	// 重试 20 次
	}
	else
	{
		// m_bConnected尚未为真之前极短时间内可能已经连上
		// 所以先shutdown一下，并无副作用
		shutdown(m_Sock, SD_SEND);
		closesocket(m_Sock);
		m_Sock = 0;
	}

	// 关闭缓冲
	InterlockedExchangeEx((LONG*)&m_bConnected, FALSE);
	
	m_SendMsgQueue.Clear();
	m_RecvMsgQueue.Clear();
}



//-----------------------------------------------------------------------------
// 完美关闭socket
//-----------------------------------------------------------------------------
BOOL Client::CloseSocket(SOCKET& socket, INT nRetryTimes)
{
	if( 0 == socket )
		return TRUE;

	shutdown(socket, SD_SEND);

	INT nRetry = 0;
	fd_set fdread;
	timeval block_time;	// 设置阻塞时间
	block_time.tv_sec = 0;
	block_time.tv_usec = NET_BLOCK_TIME;	// 150 ms 

	while( nRetry++ < nRetryTimes )
	{
		FD_ZERO(&fdread);
		FD_SET(socket, &fdread);
		if( 1 == select(0, &fdread, NULL, NULL, &block_time) )
		{
			CHAR chTemp;
			INT nResult = recv(socket, &chTemp, 1, 0);
			if( 0 == nResult )	// receive FIN, ok
			{
				closesocket(socket);
				socket = 0;
				return TRUE;
			}

			if( SOCKET_ERROR == nResult )	// network down
				break;
		}
	}


	// a hard shutdown
	closesocket(socket);
	socket = 0;
	return FALSE;
}



//-----------------------------------------------------------------------------
// 设置包模式下SIZE头的字节序
//-----------------------------------------------------------------------------
BOOL Client::SetEndian(BOOL bBigEndian)
{
	if( ECM_Package != m_eMode )
		return FALSE;

	m_bBigEndian = bBigEndian;
	return TRUE;
}



//-----------------------------------------------------------------------------
// 尝试连接指定地址
//-----------------------------------------------------------------------------
VOID Client::TryConnect(LPCSTR szIP, INT nPort)
{
	this->Disconnect();

	m_strIP = szIP;
	m_nPort = nPort;

	// 启动连接线程
	CallbackBase* pExec	= new CallbackP0<Client>(this, &Client::Connect);
	CallbackBase* pEnd	= new CallbackP0<Client>(this, &Client::CloseConnect);
	m_dwConnectID = m_pThreadMgr->CreateThread(new ExecutorFun(pExec, pEnd), TRUE);

	ASSERT(VALID(m_dwConnectID));
}


//-----------------------------------------------------------------------------
//! 返回是否正在尝试连接
//! 假如所有设置正确，但是远程主机并没有启动服务器，那么就会一直尝试连接
//! 但是如果网络发生错误，就有可能停止尝试连接
//-----------------------------------------------------------------------------
BOOL Client::IsTryingConnect()
{
	if( !VALID(m_dwConnectID) )
		return FALSE;
	if( !m_pThreadMgr->IsThreadActive(m_dwConnectID) )
		return FALSE;

	return TRUE;
}


//-----------------------------------------------------------------------------
// 正常断开连接
//-----------------------------------------------------------------------------
VOID Client::Disconnect()
{
	// 如果我们已经连接上了，那么也会断开
	this->Destroy();
	this->Init(m_eMode);
}





//-----------------------------------------------------------------------------
// 连接线程
//-----------------------------------------------------------------------------
DWORD Client::Connect()
{
	sockaddr_in address;
	ZeroMemory(&address, sizeof(address));
	address.sin_addr.s_addr = inet_addr(m_strIP.c_str());
	address.sin_family = AF_INET;
	address.sin_port = htons(m_nPort);

	// 检查IP是否合法
	if( INADDR_NONE == address.sin_addr.s_addr )
		goto __connected_terminated;
	
	fd_set fdwrite;
	timeval block_time;
	block_time.tv_sec = 0;
	block_time.tv_usec = NET_BLOCK_TIME;

	INT nResult = connect(m_Sock, (sockaddr*)&(address), sizeof(address));	// 尝试连接
	if( SOCKET_ERROR == nResult )
	{	
		if( WSAEWOULDBLOCK == WSAGetLastError() )
		{
			for(INT n = 0; n < 64; ++n) // 重试
			{
				FD_ZERO(&fdwrite);
				FD_SET(m_Sock, &fdwrite);

				if( 1 == select(0, NULL, &fdwrite, NULL, &block_time) )
					goto __connected;

				if( m_bCloseConnect )
					goto __connected_terminated;
			}
		}

		// 连接尝试失败
		goto __connected_terminated;
	}
	
__connected:

	// 初始化信息统计
	m_dwSendPackNum = 0;
	m_dwRecvPackNum = 0;

	// 启动发送线程
	CallbackBase* pSendExec	= new CallbackP0<Client>(this, &Client::Send);
	CallbackBase* pSendEnd	= new CallbackP0<Client>(this, &Client::CloseSend);
	m_pThreadMgr->CreateThread(new ExecutorFun(pSendExec, pSendEnd), TRUE);

	// 启动接收线程
	switch( m_eMode )
	{
	case ECM_Package:
		{
			CallbackBase* pRecvExec	= new CallbackP0<Client>(this, &Client::Receive);
			CallbackBase* pRecvEnd	= new CallbackP0<Client>(this, &Client::CloseReceive);
			m_pThreadMgr->CreateThread(new ExecutorFun(pRecvExec, pRecvEnd), TRUE);
		}
		break;
	case ECM_Stream:
	case ECM_Text:
		{
			CallbackBase* pRecvExec	= new CallbackP0<Client>(this, &Client::StreamReceive);
			CallbackBase* pRecvEnd	= new CallbackP0<Client>(this, &Client::CloseReceive);
			m_pThreadMgr->CreateThread(new ExecutorFun(pRecvExec, pRecvEnd), TRUE);
		}
		break;
	}

	// 连接成功，线程停止
	InterlockedExchangeEx((LONG*)&m_bConnected, TRUE);
	return 0;

__connected_terminated:
		
	// 连接失败
	InterlockedExchangeEx((LONG*)&m_bConnected, FALSE);
	return 0;
}

//-----------------------------------------------------------------------------
// 发送线程
//-----------------------------------------------------------------------------
DWORD Client::Send()
{
	fd_set fdwrite;
	timeval block_time;	// 设置发送阻塞时间
	block_time.tv_sec = 0;
	block_time.tv_usec = NET_BLOCK_TIME;	// 150ms

	LPBYTE	pMsg = NULL, pTempPtr = NULL;
	DWORD	dwMsgSize = 0;
	INT		nReturn = 0;

	while( FALSE == m_bCloseSend )
	{
		for(;;)	// 注意内部的break
		{
			// 流量统计
			DWORD dwTime = timeGetTime();
			if( dwTime - m_dwLastStatOutTime >= ONE_SECOND ) 
			{
				m_dwLastStatOutTime = dwTime;
				m_dwSendBPS = m_dwByteOutCounter;
				m_dwByteOutCounter = 0;
			}

			pTempPtr = pMsg = m_SendMsgQueue.GetMsg(dwMsgSize);
			if( NULL == pMsg )
				break;	// 发完为止

			if( m_bBigEndian )	// 处理字节序
				*(u_long*)pMsg = htonl(*(u_long*)pMsg);

			// send 可能不能一次将数据发完
			while( dwMsgSize > 0 && FALSE == m_bCloseSend )
			{
				nReturn = send(m_Sock, (char*)pTempPtr, dwMsgSize, 0);
				if( SOCKET_ERROR == nReturn )
				{
					switch(WSAGetLastError())
					{
					case WSAEWOULDBLOCK:
					case WSAENOBUFS:
						// no buffer space is available within the transport 
						// system to hold the data to be transmitted
						// 等待数据可以发送的时机
						FD_ZERO(&fdwrite);
						FD_SET(m_Sock, &fdwrite);
						select(0, NULL, &fdwrite, NULL, &block_time);
						break;

					default:	// 故障
						shutdown(m_Sock, SD_SEND);
						m_SendMsgQueue.FreeMsg(pMsg);
						goto __thread_send_end;
					}
				}
				else
				{
					pTempPtr += nReturn;
					dwMsgSize -= nReturn;
					m_dwByteOutCounter += nReturn;	// 流量统计
				}
			}

			m_SendMsgQueue.FreeMsg(pMsg);
			pTempPtr = pMsg = NULL;
		}

		::WaitForSingleObject(m_SendMsgQueue.GetEvent(), 120);
	}


__thread_send_end:
	InterlockedExchangeEx((LONG*)&m_bConnected, FALSE);
	return 0;
}



//-----------------------------------------------------------------------------
// 接收线程
//-----------------------------------------------------------------------------
DWORD Client::Receive()
{
	fd_set fdread;
	timeval block_time;	// 设置阻塞时间
	block_time.tv_sec = 0;
	block_time.tv_usec = NET_BLOCK_TIME;

	INT nResult = 0;
	DWORD dwMsgSize = 0;
	DWORD dwTempSize = 0;
	CHAR* pTempPtr = NULL;

	// 循环获得数据
	while( FALSE == m_bCloseReceive )
	{
		// 先接受DWORD型的信息长度信息
		dwTempSize = sizeof(DWORD);
		pTempPtr = (char*)&dwMsgSize;

		// 流量统计
		DWORD dwTime = timeGetTime();
		if( dwTime - m_dwLastStatInTime >= ONE_SECOND )
		{
			m_dwLastStatInTime = dwTime;
			m_dwRecvBPS = m_dwByteInCounter;
			m_dwByteInCounter = 0;
		}

		while( dwTempSize > 0 )
		{
			// 先取得数据的大小
			FD_ZERO(&fdread);
			FD_SET(m_Sock, &fdread);
			if( 1 == select(0, &fdread, NULL, NULL, &block_time) )
			{
				nResult = recv(m_Sock, pTempPtr, dwTempSize, 0);
				if( 0 == nResult )	// FIN
					goto __thread_receive_end;

				if( SOCKET_ERROR == nResult )
				{
					// 网络故障
					shutdown(m_Sock, SD_SEND);
					goto __thread_receive_end;
				}
				else
				{
					dwTempSize -= nResult;
					pTempPtr += nResult;

					// 流量统计
					m_dwByteInCounter += nResult;
				}
			}
			
			if( m_bCloseReceive )
				goto __thread_receive_end;
		}

		if( m_bBigEndian )	// 处理字节序
			dwMsgSize = ntohl(dwMsgSize);

		// ASSERT(dwMsgSize < 40960);	// 异常的数据
		if( SOCKET_ERROR != nResult )
		{
			char* pTemp = (char*)MCALLOC(dwMsgSize);
			dwTempSize = dwMsgSize;
			pTempPtr = pTemp;
			while( dwTempSize > 0 )
			{
				FD_ZERO(&fdread);
				FD_SET(m_Sock, &fdread);
				if( 1 == select(0, &fdread, NULL, NULL, &block_time) )
				{
					nResult = recv(m_Sock, pTempPtr, dwTempSize, 0);
					if( 0 == nResult )	// FIN
					{
						MCFREE(pTemp);
						goto __thread_receive_end;
					}

					if( nResult == SOCKET_ERROR )
					{
						// 网络故障
						shutdown(m_Sock, SD_SEND);
						MCFREE(pTemp);
						goto __thread_receive_end;
					}
					else
					{
						dwTempSize -= nResult;
						pTempPtr += nResult;

						// 流量统计
						m_dwByteInCounter += nResult;
					}
					
					if( m_bCloseReceive )
					{
						MCFREE(pTemp);
						goto __thread_receive_end;
					}
				}
			}
			
			// 初始化信息统计
			++m_dwRecvPackNum; 
			m_RecvMsgQueue.AddMsg(pTemp, dwMsgSize);
			MCFREE(pTemp);
		}
	}

__thread_receive_end:
	InterlockedExchangeEx((LONG*)&m_bConnected, FALSE);
	_endthreadex(0);
	return 0;
}



//-----------------------------------------------------------------------------
// 流式接收线程函数
//-----------------------------------------------------------------------------
DWORD Client::StreamReceive()
{
	fd_set fdread;
	timeval block_time;	// 设置阻塞时间
	block_time.tv_sec = 0;
	block_time.tv_usec = NET_BLOCK_TIME;

	CONST INT RECV_BUFF_SIZE = 2048;	// 2KB recv buff
	INT nResult = 0;
	LPBYTE pBuffer = (LPBYTE)MCALLOC(RECV_BUFF_SIZE);

	// 循环获得数据
	while( FALSE == m_bCloseReceive )
	{
		DWORD dwTime = timeGetTime();
		if( dwTime - m_dwLastStatInTime >= ONE_SECOND )	// 流量统计
		{
			m_dwLastStatInTime = dwTime;
			m_dwRecvBPS = m_dwByteInCounter;
			m_dwByteInCounter = 0;
		}

		if( ECM_Stream == m_eMode )
		{
			FD_ZERO(&fdread);
			FD_SET(m_Sock, &fdread);
			if( 1 == select(0, &fdread, NULL, NULL, &block_time) )
			{
				nResult = recv(m_Sock, (CHAR*)pBuffer, RECV_BUFF_SIZE, 0);
				if( 0 == nResult )	// FIN
					goto __thread_stream_receive_end;

				if( nResult == SOCKET_ERROR )	// 网络故障
				{
					shutdown(m_Sock, SD_SEND);
					goto __thread_stream_receive_end;
				}
				else
				{
					m_dwByteInCounter += nResult;	// 流量统计
					++m_dwRecvPackNum;	// 流量统计
					m_RecvMsgQueue.AddMsg(pBuffer, nResult);	// 保存数据
				}
			}
		}
		else if( ECM_Text == m_eMode )	// 文本模式按字节收
		{
			CHAR* pTempPtr = (CHAR*)pBuffer;
			DWORD dwMsgSize = 0;
			while( FALSE == m_bCloseReceive )
			{
				FD_ZERO(&fdread);
				FD_SET(m_Sock, &fdread);
				if( 1 == select(0, &fdread, NULL, NULL, &block_time) )
				{
					nResult = recv(m_Sock, pTempPtr, 1, 0);	// 收1个字节
					if( 0 == nResult )	// FIN
						goto __thread_stream_receive_end;

					if( nResult == SOCKET_ERROR )	// 网络故障
					{
						shutdown(m_Sock, SD_SEND);
						goto __thread_stream_receive_end;
					}
					else
					{
						dwMsgSize += nResult;
						if( *pTempPtr == '\n' || dwMsgSize >= RECV_BUFF_SIZE )
						{
							//初始化信息统计
							m_dwRecvPackNum++; 
							m_RecvMsgQueue.AddMsg(pBuffer, dwMsgSize);
							break;
						}

						// 流量统计
						m_dwByteInCounter += nResult;
						pTempPtr += nResult;
					}
				}
				else
				{
					if( dwMsgSize == 0 )
						break;
				}
			}
		}
	}

__thread_stream_receive_end:
	MCFREE(pBuffer);
	InterlockedExchangeEx((LONG*)&m_bConnected, FALSE);
	return 0;
}


//-----------------------------------------------------------------------------
// 文本模式和流模式之间切换，适合HTTP传输
//-----------------------------------------------------------------------------
BOOL Client::SwitchMode(EClientMode eMode)
{
	if( ECM_Package == m_eMode || ECM_Package == eMode )
		return FALSE;

	m_eMode = eMode;
	return TRUE;

}

INT32 Client::Connect(LPCSTR strIp, INT32 nPort)
{
	m_strIP = strIp;
	m_nPort = nPort;

	sockaddr_in address;
	ZeroMemory(&address, sizeof(address));
	address.sin_addr.s_addr = inet_addr(m_strIP.c_str());
	address.sin_family = AF_INET;
	address.sin_port = htons(m_nPort);

	// 检查IP是否合法
	if( INADDR_NONE == address.sin_addr.s_addr )
		goto __connected_terminated;

	fd_set fdwrite;
	timeval block_time;
	block_time.tv_sec = 0;
	block_time.tv_usec = NET_BLOCK_TIME;

	INT nResult = connect(m_Sock, (sockaddr*)&(address), sizeof(address));	// 尝试连接
	if( SOCKET_ERROR == nResult )
	{	
		if( WSAEWOULDBLOCK == WSAGetLastError() )
		{
			for(INT n = 0; n < 64; ++n) // 重试
			{
				FD_ZERO(&fdwrite);
				FD_SET(m_Sock, &fdwrite);

				if( 1 == select(0, NULL, &fdwrite, NULL, &block_time) )
					goto __connected;

				if( m_bCloseConnect )
					goto __connected_terminated;
			}
		}

		// 连接尝试失败
		goto __connected_terminated;
	}

__connected:

	// 初始化信息统计
	m_dwSendPackNum = 0;
	m_dwRecvPackNum = 0;

	// 启动发送线程
	CallbackBase* pSendExec	= new CallbackP0<Client>(this, &Client::Send);
	CallbackBase* pSendEnd	= new CallbackP0<Client>(this, &Client::CloseSend);
	m_pThreadMgr->CreateThread(new ExecutorFun(pSendExec, pSendEnd), TRUE);

	// 启动接收线程
	switch( m_eMode )
	{
	case ECM_Package:
		{
			CallbackBase* pRecvExec	= new CallbackP0<Client>(this, &Client::Receive);
			CallbackBase* pRecvEnd	= new CallbackP0<Client>(this, &Client::CloseReceive);
			m_pThreadMgr->CreateThread(new ExecutorFun(pRecvExec, pRecvEnd), TRUE);
		}
		break;
	case ECM_Stream:
	case ECM_Text:
		{
			CallbackBase* pRecvExec	= new CallbackP0<Client>(this, &Client::StreamReceive);
			CallbackBase* pRecvEnd	= new CallbackP0<Client>(this, &Client::CloseReceive);
			m_pThreadMgr->CreateThread(new ExecutorFun(pRecvExec, pRecvEnd), TRUE);
		}
		break;
	}

	// 连接成功，线程停止
	InterlockedExchangeEx((LONG*)&m_bConnected, TRUE);
	return 1;

__connected_terminated:

	// 连接失败
	InterlockedExchangeEx((LONG*)&m_bConnected, FALSE);
	return 0;

}

}
}