//-----------------------------------------------------------------------------
// author Lyp
// date 2003-03-17
// last 2008-04-17
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "wan_client.h"

#include "msg_queue_ts.h"
#include "filter.h"
#include "mem_cache.h"
#include "callback.h"
#include "executor_fun.h"
#include "thread_mgr.h"

#define	ONE_SECOND	1000
#define TEMP_BUFF	512*1024


namespace ECore {
namespace Wan {
//-----------------------------------------------------------------------------
// 构造析构函数
//-----------------------------------------------------------------------------
Client::Client():m_SendMsgQueue(TRUE,TRUE), m_RecvMsgQueue(TRUE, FALSE)
{
	Clear();
	m_pThreadMgr = new ThreadMgr;
	m_pFilter->Init(EF_Lzo1X);
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
	m_SendMsgQueue.Clear();
	m_RecvMsgQueue.Clear();

	m_strIP.clear();

	m_nPort = 0;
	m_Sock = 0;
	m_dwTDConnectID = INVALID;

	InterlockedExchangeEx((LPLONG)&m_bCloseConnect,	FALSE);
	InterlockedExchangeEx((LPLONG)&m_bCloseSend,	FALSE);
	InterlockedExchangeEx((LPLONG)&m_bCloseReceive,	FALSE);
	InterlockedExchangeEx((LPLONG)&m_bConnected,	FALSE);
}


//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL Client::Init()
{
	this->Clear();
	
	// 创建socket
	m_Sock = socket(AF_INET, SOCK_STREAM, 0);
	if( INVALID_SOCKET == m_Sock )
		return FALSE;

	// 使用非阻塞方式,linger行为
	DWORD dwCmd = 1;
	ioctlsocket(m_Sock, FIONBIO, &dwCmd);

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
VOID Client::Destroy()
{
	// 关闭线程池
	m_pThreadMgr->ShutDown();

	// 重置连接线程ID
	m_dwTDConnectID = INVALID;

	// 完美关闭socket
	if( m_bConnected )
	{
		CloseSocket(m_Sock, 10);	// 重试 10 次
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
	
	// a hard shutdown
	closesocket(socket);
	socket = 0;
	return FALSE;
}



//-----------------------------------------------------------------------------
// 尝试连接指定地址
//-----------------------------------------------------------------------------
BOOL Client::TryConnect(LPCSTR szIP, INT nPort)
{
	this->Disconnect();
	
	m_strIP = szIP;
	m_nPort = nPort;

	// 启动连接线程
	CallbackBase* pExec	= new CallbackP0<Client>(this, &Client::TDConnect);
	CallbackBase* pEnd	= new CallbackP0<Client>(this, &Client::CloseTDConnect);
	m_dwTDConnectID = m_pThreadMgr->CreateThread(new ExecutorFun(pExec, pEnd), TRUE);

	return VALID(m_dwTDConnectID);
}


//-----------------------------------------------------------------------------
// 是否正在尝试连接
//-----------------------------------------------------------------------------
BOOL Client::IsTryingConnect()
{
	if( !VALID(m_dwTDConnectID) )
		return FALSE;

	if( !m_pThreadMgr->IsThreadActive(m_dwTDConnectID) )
		return FALSE;

	return TRUE;
}

//-----------------------------------------------------------------------------
// 断开连接
//-----------------------------------------------------------------------------
VOID Client::Disconnect()
{
	this->Destroy();
	this->Init();
}




//-----------------------------------------------------------------------------
// 连接线程
//-----------------------------------------------------------------------------
DWORD Client::TDConnect()
{
	INT nResult = 0;
	sockaddr_in address;
	ZeroMemory(&address, sizeof(address));
	address.sin_addr.s_addr = inet_addr(m_strIP.c_str());
	address.sin_family = AF_INET;
	address.sin_port = htons(m_nPort);

	// 检查IP是否合法
	if( INADDR_NONE == address.sin_addr.s_addr )
		goto __connected_terminated;
	
	// 尝试连接
	fd_set fdwrite;
	timeval block_time;
	block_time.tv_sec = 0;
	block_time.tv_usec = NET_BLOCK_TIME;
	
	// For connection-oriented, nonblocking sockets, it is often not possible 
	// to complete the connection immediately. In such a case, "connect"
	// returns the error WSAEWOULDBLOCK. However, the operation proceeds.
	nResult = connect(m_Sock, (sockaddr*)&(address), sizeof(address));

	if( SOCKET_ERROR == nResult )
	{	
		if( WSAEWOULDBLOCK == WSAGetLastError() )
		{
			for(INT n = 0; n < 64; ++n) // 不停重试,外部可以通过调用StopTryingConnect停止重试
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
	CallbackBase* pSendExec	= new CallbackP0<Client>(this, &Client::TDSend);
	CallbackBase* pSendEnd	= new CallbackP0<Client>(this, &Client::CloseTDSend);
	m_pThreadMgr->CreateThread(new ExecutorFun(pSendExec, pSendEnd), TRUE);

	// 启动接收线程
	CallbackBase* pRecvExec	= new CallbackP0<Client>(this, &Client::TDReceive);
	CallbackBase* pRecvEnd	= new CallbackP0<Client>(this, &Client::CloseTDReceive);
	m_pThreadMgr->CreateThread(new ExecutorFun(pRecvExec, pRecvEnd), TRUE);

	// 连接成功，线程停止
	InterlockedExchangeEx((LONG*)&m_bConnected, TRUE);
	return 0;

__connected_terminated:
		
	// 连接失败
	InterlockedExchangeEx((LONG*)&m_bConnected, FALSE);
	return 0;
}



//-----------------------------------------------------------------------------
// 发送线程函数
//-----------------------------------------------------------------------------
DWORD Client::TDSend()
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

			// 加密
			DWORD dwSerial = (m_dwSendPackNum++) % 0x1000;
			for(DWORD dw=0; dw<dwMsgSize-sizeof(DWORD); ++dw)
				pMsg[dw+sizeof(DWORD)] ^= g_SqrtTable[dwSerial+dw];

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
DWORD Client::TDReceive()
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

		ASSERT(dwMsgSize < 40960);	// 异常的数据
		if( SOCKET_ERROR != nResult )
		{
			CHAR* pBufferIn = (CHAR*)MCALLOC(dwMsgSize+dwMsgSize/64+64);
			dwTempSize = sizeof(DWORD) + dwMsgSize;
			pTempPtr = pBufferIn;
			while( dwTempSize > 0 )
			{
				FD_ZERO(&fdread);
				FD_SET(m_Sock, &fdread);
				if( 1 == select(0, &fdread, NULL, NULL, &block_time) )
				{
					nResult = recv(m_Sock, pTempPtr, dwTempSize, 0);
					if( 0 == nResult )	// FIN
					{
						MCFREE(pBufferIn);
						goto __thread_receive_end;
					}

					if( nResult == SOCKET_ERROR )
					{
						// 网络故障
						shutdown(m_Sock, SD_SEND);
						MCFREE(pBufferIn);
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
						MCFREE(pBufferIn);
						goto __thread_receive_end;
					}
				}
			}
			
			// 初始化信息统计
			++m_dwRecvPackNum;

			CHAR* pBufferOut = (CHAR*)MCALLOC(*(DWORD*)pBufferIn+sizeof(DWORD));

			// 反过滤（解压）文件
			tagFiltParam param;
			param.bCompress = FALSE;
			param.dwInBuffer = dwMsgSize+dwMsgSize/64+64;
			param.stInData = dwMsgSize;
			param.dwOutBuffer = *(DWORD*)pBufferIn;
			param.stOutData = 0;
			param.eFilter = EF_Lzo1X;
			param.pMemIn = pBufferIn + sizeof(DWORD);
			param.pMemOut = pBufferOut;

			// 进行过滤文件信息
			m_pFilter->Filt(&param);

			m_RecvMsgQueue.AddMsg(pBufferOut, (DWORD)param.stOutData);
			MCFREE(pBufferIn);
			MCFREE(pBufferOut);
		}
	}

__thread_receive_end:
	InterlockedExchangeEx((LONG*)&m_bConnected, FALSE);
	return 0;
}

}
}