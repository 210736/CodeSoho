//-----------------------------------------------------------------------------
// author Lyp
// date 2004-04-03
// last 2009-06-15
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "wan_server.h"

#include "filter.h"
#include "callback.h"
#include "executor_fun.h"
#include "thread_mgr.h"

#pragma intrinsic(memcpy)

namespace ECore {
namespace Wan {
//-----------------------------------------------------------------------------
//! construction
//-----------------------------------------------------------------------------
Server::Server()
{
	m_Client			= 0;
	m_hPort				= 0;
	m_lpfnAcceptEx		= 0;
	m_lpfnGetAESockAddr	= 0;
	m_bCloseThread		= 0;
	m_Pool				= 0;
	m_nAcceptEx			= 0;
	m_n64SendSize		= 0;
	m_n64RealSendSize	= 0;
	
	m_nSendCast			= 0;
	m_nSendCastSize		= 0;
	m_nRecvCast			= 0;
	m_lGetRawNum		= 0;

	for(INT n=EDR_Start; n<EDR_End; ++n)
		m_lKickNum[n] = 0;

	// 线程事件
	m_hEventAccepEx	= ::CreateEvent(NULL, FALSE, TRUE, NULL);		// 自动激活
	m_hEventDestroy	= ::CreateEvent(NULL, FALSE, TRUE, NULL);		// 自动激活

	// 压缩器
	m_pThreadMgr = new ThreadMgr;
	m_pFilter = new Filter;
	m_pFilter->Init(EF_Lzo1X);
}


//-----------------------------------------------------------------------------
//! destruction
//-----------------------------------------------------------------------------
Server::~Server()
{
	// 销毁压缩器
	SAFE_DEL(m_pThreadMgr);
	SAFE_DEL(m_pFilter);

	// 关闭线程事件对象
	CloseHandle(m_hEventAccepEx);

	for(size_t n=0; n<m_vecEventSend.size(); ++n)
		CloseHandle(m_vecEventSend[n]);

	CloseHandle(m_hEventDestroy);
	SAFE_DEL(m_pThreadMgr);
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOL Server::Init(tagConfig& param)
{
	memcpy(&m_Config, &param, sizeof(m_Config));
	
	// 根据配置信息中的客户端连接上线，生成客户端数组
	m_Client = new tagClient[m_Config.nMaxLoad];
	if( !m_Client ) 
		return FALSE;

	// 初始化里面的每一个客户端，并放入空闲列表里面
	ZeroMemory(m_Client, sizeof(tagClient)*m_Config.nMaxLoad);
	for(INT n = 0; n < m_Config.nMaxLoad; ++n)
	{
		m_Client[n].pSendQueue = new RingQueue<tagRaw*>(m_Config.lMaxSendBag);
		m_Client[n].pRecvQueue = new RingQueue<tagBag*>(m_Config.lMaxRecvBag);
		m_listFreeClient.PushBack(&m_Client[n]);
	}

	// 初始化监听socket，并进行相关配置
	m_Sock = socket(AF_INET, SOCK_STREAM, 0);

	// reuse造成多次绑定成功，可能不能找到正确可用的端口
	// 如果两台服务器开在同一台机器会使用同一个端口，造成混乱
	DWORD dwReUseAddr = TRUE;	
	setsockopt(m_Sock, SOL_SOCKET, SO_REUSEADDR, (char*)&dwReUseAddr, sizeof(dwReUseAddr) );

	// 关闭Nagle算法
	BOOL bNoDelay = TRUE;
	setsockopt(m_Sock, IPPROTO_TCP, TCP_NODELAY, (char*)&bNoDelay, sizeof(bNoDelay));

	// 绑定端口
	sockaddr_in address;
    ZeroMemory(&address, sizeof(address));
    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port        = htons(m_Config.nPort);

    while( SOCKET_ERROR == bind(m_Sock, (sockaddr*)&address, sizeof(address)) )
	{
		if( WSAEADDRINUSE == WSAGetLastError() )
		{
			++m_Config.nPort;	// 依次尝试后面的端口
			address.sin_port = htons(m_Config.nPort);
		}
		else
		{
			closesocket(m_Sock);
			ERR(_T("bind() failed"));
			return FALSE;
		}
	}

	// 生成完成端口，并将监听socket绑定到该完成端口
    m_hPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    listen(m_Sock, SOMAXCONN);
	CreateIoCompletionPort((HANDLE)m_Sock, m_hPort, (ULONG_PTR)0, 0);

	// 获得AcceptEx\GetAcceptExSockAddrs的函数指针
	GUID Guid = WSAID_ACCEPTEX, Guid2 = WSAID_GETACCEPTEXSOCKADDRS;
	DWORD dwBytes = 0;
	WSAIoctl(m_Sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &Guid,
		sizeof(GUID), &m_lpfnAcceptEx, sizeof(m_lpfnAcceptEx), &dwBytes, 0, 0);
	WSAIoctl(m_Sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &Guid2,
		sizeof(GUID), &m_lpfnGetAESockAddr,sizeof(m_lpfnGetAESockAddr), &dwBytes, 0, 0);


	// 创建内存池以及GC线程
	if( FALSE == CreateMemCache() )
		return FALSE;

	// 按配置文件指定的数目建立工作线程
	SYSTEM_INFO  sysinfo;
	GetSystemInfo(&sysinfo);
	INT nWorkerThread = (INT)sysinfo.dwNumberOfProcessors * m_Config.nPerCpuThread;
	
	if( FALSE == CreatTDWorker(nWorkerThread) )	// 创建工作线程以及内存池
		return FALSE;
	
	if( FALSE == CreateTDSetupAccepEx() )		// 创建接收连接线程
		return FALSE;

	if( FALSE == CreateTDSend((INT)sysinfo.dwNumberOfProcessors) )				// 创建发送线程
		return FALSE;
	
	if( FALSE == CreateTDUnauthHeartbeat() )	// 创建踢出未认证客户端线程
		return FALSE;

	if( FALSE == CreateTDDestroy() )			// 创建资源回收线程
		return FALSE;

	return TRUE;
}


//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
BOOL Server::Destroy()
{
	// 关闭完成端口
	SAFE_CLOSE_HANDLE(m_hPort);

	// 关闭线程管理器
	m_pThreadMgr->ShutDown();

	// 销毁各个客户端的内容
	if( m_Client )
	{
		for(INT n = 0; n < m_Config.nMaxLoad; ++n)	// 设置 index
		{
			if( !m_Client[n].bShutDown )
			{
				closesocket(m_Client[n].sock);

				SAFE_DEL(m_Client[n].pRecvQueue);
				SAFE_DEL(m_Client[n].pSendQueue);
			}
		}
	}

	// 销毁预分配内存
	SAFE_DEL_ARRAY(m_Pool);
	SAFE_DEL_ARRAY(m_Client);
	return TRUE;
}


//-----------------------------------------------------------------------------
// 创建内存池以及GC线程
//-----------------------------------------------------------------------------
BOOL Server::CreateMemCache()
{
	// 初始化内存池
	m_Pool = new MemCache<AtomMutex>[MEM_POOL_NUM];
	for(INT n = 0; n < MEM_POOL_NUM; ++n)
		m_Pool[n].SetMaxSize(m_Config.dwPoolSize);


	// 创建GC线程
	CallbackBase* pExec	= new CallbackP0<Server>(this, &Server::TDGC);
	CallbackBase* pEnd	= new CallbackP0<Server>(this, &Server::CloseThread);
	DWORD dwID = m_pThreadMgr->CreateThread(new ExecutorFun(pExec, pEnd), TRUE);

	if( !VALID(dwID) )
	{
		ERR(_T("[s]create gc thread failed"));
		return FALSE;
	}

	return TRUE;
}


//-----------------------------------------------------------------------------
// 创建接收连接线程
//-----------------------------------------------------------------------------
BOOL Server::CreateTDSetupAccepEx()
{
	CallbackBase* pExec	= new CallbackP0<Server>(this, &Server::TDSetupAcceptEx);
	CallbackBase* pEnd	= new CallbackP0<Server>(this, &Server::CloseThread);
	DWORD dwID = m_pThreadMgr->CreateThread(new ExecutorFun(pExec, pEnd), TRUE);

	if( !VALID(dwID) )
	{
		ERR(_T("[s]create setup acceptex thread failed"));
		return FALSE;
	}

	// 激活event
	::SetEvent(m_hEventAccepEx);

	return TRUE;
}


//-----------------------------------------------------------------------------
// 创建工作线程
//-----------------------------------------------------------------------------
BOOL Server::CreatTDWorker(INT nThreadNum)
{
	for(INT n = 0; n < nThreadNum; ++n)
	{
		// 创建工作线程
		CallbackBase* pExec	= new CallbackP1<Server, INT>(this, &Server::TDWorker, n);
		CallbackBase* pEnd	= new CallbackP0<Server>(this, &Server::CloseThread);
		DWORD dwID = m_pThreadMgr->CreateThread(new ExecutorFun(pExec, pEnd), TRUE);

		if( !VALID(dwID) )
		{
			ERR(_T("[s]create worker thread failed"));
			return FALSE;
		}
	}

	return TRUE;
}


//-----------------------------------------------------------------------------
// 创建发送线程
//-----------------------------------------------------------------------------
BOOL Server::CreateTDSend(INT nThreadNum)
{
	
	for(INT n = 0; n < nThreadNum; ++n)
	{
		m_vecEventSend.push_back(::CreateEvent(NULL, FALSE, TRUE, NULL));		// 自动激活

		CallbackBase* pExec	= new CallbackP2<Server, INT, INT>(this, &Server::TDSend, n, nThreadNum);
		CallbackBase* pEnd	= new CallbackP0<Server>(this, &Server::CloseThread);
		DWORD dwID = m_pThreadMgr->CreateThread(new ExecutorFun(pExec, pEnd), TRUE);

		if( !VALID(dwID) )
		{
			ERR(_T("create send thread failed"));
			return FALSE;
		}
	}

	return TRUE;
}


//-----------------------------------------------------------------------------
// 创建踢出未认证客户端线程
//-----------------------------------------------------------------------------
BOOL Server::CreateTDUnauthHeartbeat()
{
	CallbackBase* pExec	= new CallbackP1<Server, DWORD>(this, &Server::TDUnauthHeartbeat, m_Config.dwAuthTime);
	CallbackBase* pEnd	= new CallbackP0<Server>(this, &Server::CloseThread);
	DWORD dwID = m_pThreadMgr->CreateThread(new ExecutorFun(pExec, pEnd), TRUE);

	if( !VALID(dwID) )
	{
		ERR(_T("[s]create heart beat to unauth client thread failed"));
		return FALSE;
	}

	return TRUE;
}


//-----------------------------------------------------------------------------
// 创建资源回收线程
//-----------------------------------------------------------------------------
BOOL Server::CreateTDDestroy()
{
	CallbackBase* pExec	= new CallbackP0<Server>(this, &Server::TDDestroy);
	CallbackBase* pEnd	= new CallbackP0<Server>(this, &Server::CloseThread);
	DWORD dwID = m_pThreadMgr->CreateThread(new ExecutorFun(pExec, pEnd), TRUE);

	if( !VALID(dwID) )
	{
		ERR(_T("[s]create destroy client thread failed"));
		return FALSE;
	}

	return TRUE;
}


//-----------------------------------------------------------------------------
// GC线程函数,不断维持内存池的平衡
//-----------------------------------------------------------------------------
DWORD Server::TDGC()
{
	while( !m_bCloseThread )
	{
		// 轮询内存池
		for(INT n = 0; n < MEM_POOL_NUM; ++n)
		{
			if( m_Pool[n].GetSize() > m_Config.dwPoolSize / 2 )
			{
				m_Pool[n].TryGC( m_Config.dwPoolSize / 64 );
				Sleep(SLEEP_TIME);	// 每次GC都SLEEP
			}
		}

		Sleep(SLEEP_TIME);
	}
	return 0;
}



//-----------------------------------------------------------------------------
// 接收连接线程函数
//-----------------------------------------------------------------------------
DWORD Server::TDSetupAcceptEx()
{
	tagClient* pClient = NULL;
	tagBag *pBag = NULL;
	BOOL bResult = FALSE;
	DWORD dwBytes = 0;
	INT	nFakeRandom = (INT)timeGetTime() & 0x1F;

	while( !m_bCloseThread )
	{
		while( m_nAcceptEx < m_Config.nAcceptEx )
		{
			// 得到一个空闲客户端
			pClient = m_listFreeClient.PopFront();
			if( !VALID(pClient) ) 
			{
				WaitForSingleObject(m_hEventAccepEx, SLEEP_TIME);
				continue;
			}

			// 初始化该客户端
			pClient->bShutDown = FALSE;
			pClient->bCanClear = FALSE;
			pClient->lSendCast = 0;
			pClient->lRecvCast = 1;
			pClient->lSendCastSize = 0;

			pClient->dwClientID = INVALID;
			pClient->dwConnectTime = INVALID;
			pClient->dwRecvSerial = 0;

			pClient->sock = socket(AF_INET, SOCK_STREAM, 0);
			pClient->eReason = EDR_Normal;
		
			// 创建一个包，用于接收连接请求
			for( pBag=0;!pBag; ) 
			{
				pBag = (tagBag*)m_Pool[nFakeRandom].TryAlloc(BAG_SIZE(PARAM_SIZE));
				if( !pBag && ++nFakeRandom >= MEM_POOL_NUM )
					nFakeRandom = 0;
			} 

			pBag->pPool	= &m_Pool[nFakeRandom];
			pBag->ov.hEvent	= NULL;
			pBag->eState	= ES_Accept;
			pBag->pNext		= NULL;
			((tagLoginParam*)(&pBag->pBuffer[PARAM_INDEX]))->hHandle = (HANDLE)pClient;
			
			// 如果接收连接不成功，则归还该客户端的资源
			bResult = m_lpfnAcceptEx(m_Sock, pClient->sock, pBag->pBuffer, 0, 
				sizeof(sockaddr_in)+16, sizeof(sockaddr_in)+16, &dwBytes, (LPOVERLAPPED)pBag);
			if( !bResult && ERROR_IO_PENDING != WSAGetLastError() )
			{
				closesocket(pClient->sock);
				DestroyClient(pClient);
				pBag->pPool->Free(pBag);
				continue;
			}

			InterlockedIncrementEx((LPLONG)&m_nAcceptEx);
		}

		WaitForSingleObject(m_hEventAccepEx, SLEEP_TIME);
	}

	shutdown(m_Sock, SD_BOTH);
	closesocket(m_Sock);
	return 0;
}


//-----------------------------------------------------------------------------
// 工作线程函数，接受完成端口的返回通知并作相应的后续操作
//-----------------------------------------------------------------------------
DWORD Server::TDWorker(INT nThreadIndex)
{
	DWORD dwFlags = 0;
	DWORD dwBytes = 0;
	BOOL bTRUE = TRUE;
	INT nResult = 0;

	sockaddr_in *pRemoteAddr, *pLocalAddr;
	DWORD dwRemoteAddrSize = 0, dwLocalAddrSize = 0;
	
	WSAOVERLAPPED*	pOverlap = NULL;

	tagClient* pClient = NULL;
	tagBag *pBag = NULL, *pGot = NULL, *pNew = NULL;

	WSABUF wbuf;
	DWORD dwBytesNeed = 0;
	DWORD dwBytesComp = 0;

	INT	nFakeRandom = nThreadIndex & 0x1F;

	while( !m_bCloseThread )
	{
		// 得到完成端口的完成通知
		INT nResult = GetQueuedCompletionStatus(m_hPort, &dwBytesComp, (PULONG_PTR)&pClient, &pOverlap, SLEEP_TIME);
		if( !pOverlap ) 
			continue;

		// 从完成通知中得到Unit
		pGot = CONTAINING_RECORD(pOverlap, tagBag, ov);

		// 如果该Unit是接收时给的，那么对应的client指针式从buff里得到的
		if( ES_Accept == pGot->eState )
		{
			pClient = (tagClient*)(((tagLoginParam*)(&pGot->pBuffer[PARAM_INDEX]))->hHandle);
			InterlockedDecrementEx((LONG*)&m_nAcceptEx);	// 再创建AcceptEx
			SetEvent(m_hEventAccepEx);
		}

		// 完成端口失败，则执行客户端的预销毁步骤
		if( !nResult )
		{
			DisconnectClient(pClient, pGot, EDR_CPortError); 
			continue;
		}

		// 针对Unit的每一种状态执行对应的操作
		switch( pGot->eState )
		{
		// 接收连接
		case ES_Accept:
			setsockopt(pClient->sock, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&m_Sock, sizeof(m_Sock));

			// hard shut down
			{
				linger lin;
				lin.l_linger = 0;
				lin.l_onoff = 1;
				nResult = setsockopt(pClient->sock, SOL_SOCKET, SO_LINGER, (char*)&lin, sizeof(lin));
			}

			m_lpfnGetAESockAddr(pGot->pBuffer, 0, sizeof(sockaddr_in)+16, sizeof(sockaddr_in)+16, 
				(sockaddr**)&pLocalAddr, (LPINT)&dwLocalAddrSize, (sockaddr**)&pRemoteAddr, (LPINT)&dwRemoteAddrSize);
			((tagLoginParam*)(&pGot->pBuffer[PARAM_INDEX]))->dwAddress = pRemoteAddr->sin_addr.S_un.S_addr;	// 得到客户端ip和端口
			((tagLoginParam*)(&pGot->pBuffer[PARAM_INDEX]))->dwPort = pRemoteAddr->sin_port;

			// 将该客户端的socket绑定到完成端口
			if( !CreateIoCompletionPort((HANDLE)pClient->sock, m_hPort, (ULONG_PTR)pClient, 0) )	// 绑定完成端口
			{
				DisconnectClient(pClient, pGot, EDR_CPortBindError); 
				break;
			}

			// 将客户端加入到未验证列表中，等待客户端发送来第一条消息
			pClient->dwConnectTime = timeGetTime();
			pClient->dwRecvSerial = 0;
			m_listUnauthClient.PushBack(pClient);


			// 接收第一条消息，长度
			for( pBag=0;!pBag; ) 
			{
				pBag = (tagBag*)m_Pool[nFakeRandom].TryAlloc(BAG_SIZE(MAX_RECV_SIZE));
				if( !pBag && ++nFakeRandom >= MEM_POOL_NUM )
					nFakeRandom = 0;
			} 

			pBag->pPool	= &m_Pool[nFakeRandom];
			pBag->ov.hEvent	= NULL;
			pBag->eState = ES_Recv;
			pBag->dwReady = 0;
			pBag->pNext	= pGot;

			wbuf.buf = pBag->pBuffer;
			wbuf.len = MAX_RECV_SIZE;
			if( 0 != WSARecv(pClient->sock, &wbuf, 1, &dwBytes, &dwFlags, &pBag->ov, NULL) )
			{
				if( WSA_IO_PENDING != WSAGetLastError() )
					DisconnectClient(pClient, pBag, EDR_WSARecvError);
			}	
			break;

		case ES_Recv:
			if( dwBytesComp == 0 )	// FIN
			{
				DisconnectClient(pClient, pGot, EDR_FIN);
				break;
			}

			// 累加收到的包长度
			pGot->dwReady += dwBytesComp;

			// 还没收至少一个DWORD，就继续收
			if( pGot->dwReady < sizeof(DWORD) )	
			{
				pGot->ov.hEvent = NULL;	// 没收完，继续接收
				wbuf.buf = pGot->pBuffer + pGot->dwReady;
				wbuf.len = MAX_RECV_SIZE - pGot->dwReady;

				if( WSARecv(pClient->sock, &wbuf, 1, &dwBytes, &dwFlags, &pGot->ov, NULL) )
				{
					if( WSA_IO_PENDING != WSAGetLastError() )
						DisconnectClient(pClient, pGot, EDR_WSARecvError);
				}		
				break;	// 跳出
			}
__again:
			// 超长的包
			if( pGot->dwSize > MAX_RECV_SIZE || pGot->dwSize == 0 )	
			{
				DisconnectClient(pClient, pGot, EDR_TooBigRecv);	// 直接踢掉
				break;	// 跳出
			}

			// 收到一个或者超过一个包的内容
			if( pGot->dwReady >= pGot->dwSize + sizeof(DWORD) )
			{
				// 先处理收到的包，解密
				DWORD dwSerial = (pClient->dwRecvSerial++) % 0x1000;
				for(DWORD dw=0; dw<pGot->dwSize; ++dw)
					pGot->pMsg[dw] ^= g_SqrtTable[dwSerial+dw];

				if( pGot->pNext )	// 是此客户端第一个包
				{
					// 如果还没有超过验证超时时间，则进行上层处理
					if( TRUE == m_listUnauthClient.Erase(pClient) )
					{
						// 通知上层，有一个客户端连接上来
						tagLoginParam* pParam = (tagLoginParam*)(&pGot->pNext->pBuffer[PARAM_INDEX]);
						pClient->dwClientID = m_Config.pLogHandle->Logon(pGot->pMsg, pGot->dwSize, pParam);

						// 上层没有通过身份验证，则踢出该客户端
						if( INVALID == pClient->dwClientID )
						{
							DisconnectClient(pClient, pGot, EDR_FailedAuth);
							break;
						}

						pGot->pNext->pPool->Free(pGot->pNext);
						pGot->pNext = NULL;
					}
					else	// 已经超时，则删除用户
					{
						DisconnectClient(pClient, pGot, EDR_AuthTimeOut);
						break;
					}
				}

				DWORD dwOver = pGot->dwReady - pGot->dwSize - sizeof(DWORD);	// 接收超出
				for( pNew = 0;!pNew; ) 
				{
					pNew = (tagBag*)m_Pool[nFakeRandom].TryAlloc(BAG_SIZE(MAX_RECV_SIZE));
					if( !pNew && ++nFakeRandom >= MEM_POOL_NUM )
						nFakeRandom = 0;
				} 
				pNew->pPool = &m_Pool[nFakeRandom];
				pNew->ov.hEvent	= NULL;
				pNew->eState = ES_Recv;
				pNew->dwReady = dwOver;
				pNew->dwSize = 0;
				pNew->pNext = NULL;

				if( dwOver > 0 )	// 这个包包含了更多的数据
				{
					memcpy(pNew->pBuffer, pGot->pBuffer + pGot->dwSize + sizeof(DWORD), dwOver); // 超出的部分拷到新包
				}

				// 将老包加入到接收队列里面去
				if( FALSE == pClient->pRecvQueue->Push(pGot) )
				{
					DisconnectClient(pClient, pGot, EDR_TooManyRecv);
					pNew->pPool->Free(pNew);
					break;
				}

				InterlockedIncrementEx((LONG*)&m_nRecvCast);

				if( pNew->dwReady >= pNew->dwSize + sizeof(DWORD) )	// pNew包也完全收到了
				{
					pGot = pNew;
					goto __again;	// 重复上面的过程
				}
				else
				{
					// 接收新包
					wbuf.buf = pNew->pBuffer + pNew->dwReady;
					wbuf.len = MAX_RECV_SIZE - pNew->dwReady;
					if( WSARecv(pClient->sock, &wbuf, 1, &dwBytes, &dwFlags, &pNew->ov, NULL) )
					{
						if( WSA_IO_PENDING != WSAGetLastError() )
							DisconnectClient(pClient, pNew, EDR_WSARecvError);
					}		
				}

			}
			else
			{
				pGot->ov.hEvent = NULL;	// 没收完，继续接收
				wbuf.buf = pGot->pBuffer + pGot->dwReady;
				wbuf.len = MAX_RECV_SIZE - pGot->dwReady;

				if( WSARecv(pClient->sock, &wbuf, 1, &dwBytes, &dwFlags, &pGot->ov, NULL) )
				{
					if( WSA_IO_PENDING != WSAGetLastError() )
						DisconnectClient(pClient, pGot, EDR_WSARecvError);
				}		
			}
			break;


		case ES_Send:
			InterlockedExchangeAdd((LPLONG)&pClient->lSendCastSize, -((LONG)pGot->dwSize));
			InterlockedExchangeAdd((LPLONG)&m_nSendCastSize, -(LONG)pGot->dwSize);
			pGot->pPool->Free(pGot);
			InterlockedDecrementEx(&pClient->lSendCast);
			break;

		default:
			ERR(_T("xserver error state!!!"));
			break;
			
		}
	}
	
	return 0;
}



//-----------------------------------------------------------------------------
// 发送线程函数
//-----------------------------------------------------------------------------
DWORD Server::TDSend(INT nThreadIndex, INT nThreadNum)
{
	// 运行时参数
	CHAR* pBuff	= new CHAR[m_Config.dwMaxTickSend];
	tagRaw*	pRaw = NULL;
	tagBag*	pBag = NULL;
	DWORD dwBytes =	0;
	DWORD dwTotal =	0;
	LONG lPopNum = 0;
	WSABUF wbuf;
	INT	nFakeRandom = (nThreadIndex + 16) & 0x1F;
	tagClient* pClient = NULL;

	// 压缩参数
	tagFiltParam param;
	param.bCompress	 = TRUE;
	param.eFilter	 = EF_Lzo1X;
	param.pMemIn	 = pBuff;
	param.dwInBuffer = m_Config.dwMaxTickSend;

	while( !m_bCloseThread )
	{
		for(INT n = nThreadIndex; n < m_Config.nMaxLoad; n+=nThreadNum)
		{
			// 查看是否有要发送的消息
			pClient = &m_Client[n];
			if( 0 == pClient->pSendQueue->GetNum() )
				continue;

			// 客户端已经关闭
			if( pClient->bShutDown )
				continue;

			// 取包并进行数据拼合
			dwTotal = 0;
			lPopNum = 0;

			while( pRaw = pClient->pSendQueue->Pop() )
			{
				if( pRaw->dwSize + dwTotal < m_Config.dwMaxTickSend )	// 确认没有超过临时内存上限
					memcpy(pBuff + dwTotal, pRaw->pMsg, pRaw->dwSize);

				++lPopNum;
				dwTotal += pRaw->dwSize;

				// 删除pRaw
				if( pRaw->lCounter != INVALID )	// 处理多播包
				{
					if( InterlockedDecrementEx(&pRaw->lCounter) == 0 )
						pRaw->pPool->Free(pRaw);
				}
				else
				{
					pRaw->pPool->Free(pRaw);
				}

				if( dwTotal >= m_Config.dwMaxTickSend )	// 超过临时内存上限
				{
					DisconnectClient(pClient, NULL, EDR_TooBigSend);	// 踢掉
					dwTotal = 0;	// 下面不发送
					break;
				}

				
			}

			// 统计用
			InterlockedExchangeAdd((LONG*)&m_nSendCast, lPopNum);

			// 如果是空包，则不需要再压缩发送了
			if( dwTotal == 0 )
			{
				InterlockedExchangeAdd((LPLONG)&pClient->lSendCast, -lPopNum);
				continue;
			}

			// 下面要发送了，所以+1
			InterlockedExchangeAdd((LPLONG)&pClient->lSendCast, -lPopNum + 1);

			// 找到一个内存池，并申请足够大的内存，存放压缩后的数据
			for( pBag=0;!pBag; ) 
			{
				pBag = (tagBag*)m_Pool[nFakeRandom].TryAlloc(BAG_SIZE(dwTotal)+LZO_ADD(dwTotal)+sizeof(DWORD)*2);
				if( !pBag && ++nFakeRandom >= MEM_POOL_NUM )
					nFakeRandom = 0;
			} 

			// 压缩
			param.stInData = dwTotal;
			param.dwOutBuffer = dwTotal + LZO_ADD(dwTotal);
			param.stOutData = 0;
			param.pMemOut = pBag->pBuffer + sizeof(DWORD) * 2;
			m_pFilter->Filt(&param);

			// 填写压缩前大小和压缩后大小
			*(DWORD*)pBag->pBuffer = (DWORD)param.stOutData;
			*(DWORD*)(pBag->pBuffer + sizeof(DWORD)) = dwTotal;

			pBag->ov.hEvent	= NULL;
			pBag->dwReady	= 0;
			pBag->pPool		= &m_Pool[nFakeRandom];
			pBag->eState	= ES_Send;
			pBag->pNext		= NULL;

			InterlockedExchangeAdd((LPLONG)&pClient->lSendCastSize, (LONG)pBag->dwSize);
			InterlockedExchangeAdd((LPLONG)&m_nSendCastSize, (LONG)pBag->dwSize);
			if( pClient->lSendCast > m_Config.lMaxSendCast )
			{
				DisconnectClient(pClient, pBag, EDR_TooManySendCast);
				break;
			}

			if( pClient->lSendCastSize > m_Config.lMaxSendCastSize )
			{
				DisconnectClient(pClient, pBag, EDR_TooMuchSendCastSize);
				break;
			}

			// 发送压缩后的数据
			wbuf.buf = pBag->pBuffer;
			wbuf.len = (DWORD)param.stOutData + sizeof(DWORD) * 2;
			if( WSASend(pClient->sock, &wbuf, 1, &dwBytes, 0, &pBag->ov, NULL) )
			{
				if( WSA_IO_PENDING != WSAGetLastError() )
				{
					DisconnectClient(pClient, pBag, EDR_WSASendError);
					break;
				}
			}
			
			// 累加统计信息
			m_n64SendSize += dwTotal + sizeof(DWORD) * 2; 
			m_n64RealSendSize += param.stOutData + sizeof(DWORD);
		}

		WaitForSingleObject(m_vecEventSend[nThreadIndex], INFINITE);
	}
	
	SAFE_DEL_ARRAY(pBuff);
	return 0;
}



//-----------------------------------------------------------------------------
// 踢出未认证客户端线程函数
//-----------------------------------------------------------------------------
DWORD Server::TDUnauthHeartbeat(DWORD dwWaitForAuthTime)
{
	tagClient* pClient = NULL;

	while( !m_bCloseThread )
	{
		if( m_listUnauthClient.Size() > 0 )
		{
			DWORD dwTime = timeGetTime();
			
			m_listUnauthClient.Lock();	// ----------------------------lock
			
			m_listUnauthClient._ResetIterator();
			while( m_listUnauthClient._Peek(pClient) )
			{
				// 超时了，则销毁该客户端
				if( dwTime - pClient->dwConnectTime > dwWaitForAuthTime )
				{
					DisconnectClient(pClient, NULL, EDR_AuthTimeOut);
					m_listUnauthClient._EraseCurrent();
				}
				else
				{
					// 没有超时，则处理下一个
					m_listUnauthClient._AddIterator();
				}
			}

			m_listUnauthClient.Unlock();	// ---------------------unlock
		}
		
		Sleep(SLEEP_TIME);
	}

	return 0;
}


//-----------------------------------------------------------------------------
// 资源回收线程
//-----------------------------------------------------------------------------
DWORD Server::TDDestroy()
{
	tagClient*	pClient = NULL;
	tagRaw*	pRaw = NULL;
	tagBag*	pBag = NULL;

	while( !m_bCloseThread )
	{
		pClient = m_listDestroyClient.PopFront();
		if( !VALID(pClient) )
		{
			WaitForSingleObject(m_hEventDestroy, SLEEP_TIME);
			continue;
		}

		if( VALID(pClient->dwClientID) )
		{
			m_Config.pLogHandle->PreLogoff(pClient->dwClientID, pClient->eReason, pClient->dwDebugMsg);	// 回调用户登出函数
			pClient->dwClientID = INVALID;
		}
		
		if( TRUE == pClient->bCanClear )
		{
			if( pClient->pSendQueue->GetNum() > 0 )
			{
				while( pRaw = pClient->pSendQueue->Pop() )
				{
					InterlockedDecrementEx(&pClient->lSendCast);
					if( pRaw->lCounter != INVALID )	// 释放多播包
					{
						if( InterlockedDecrementEx(&pRaw->lCounter) == 0 )
						{
							pRaw->pPool->Free(pRaw);
						}
					}
					else
					{
						pRaw->pPool->Free(pRaw);
					}
				}
			}
		}

		if( pClient->pRecvQueue->GetNum() > 0 )
		{
			while( pBag = pClient->pRecvQueue->Pop() )
				pBag->pPool->Free(pBag);
		}
		
		// 检测所有的完成通知是不是都已经处理
		if( 0 == pClient->lSendCast && 0 == pClient->lRecvCast && TRUE == pClient->bCanClear )
		{
			DestroyClient(pClient);
		}
		else
		{
			WaitForSingleObject(m_hEventDestroy, 100);
			m_listDestroyClient.PushBack(pClient);
		}
	}

	return 0;
}


//-----------------------------------------------------------------------------
// 回收资源
//-----------------------------------------------------------------------------
VOID Server::DisconnectClient(tagClient* pClient, tagBag* pUnit, EDisconnectReason eReason, DWORD dwDebugMsg)
{
	if( FALSE == InterlockedCompareExchange((LPLONG)(&pClient->bShutDown), TRUE, FALSE) )
	{
		shutdown(pClient->sock, SD_BOTH);
		InterlockedIncrementEx(&m_lKickNum[eReason]);

		if( !VALID(pClient->dwClientID) )	// 未经过上层验证的客户端，不需要等待上层设置“可删除”
			pClient->bCanClear = TRUE;

		pClient->eReason = eReason;
		pClient->dwDebugMsg = dwDebugMsg;
			
		m_listDestroyClient.PushBack(pClient);
		SetEvent(m_hEventDestroy);
		closesocket(pClient->sock);
	}

	if( pUnit )
	{
		if( ES_Send == pUnit->eState )	// 处理引用计数
			InterlockedDecrementEx(&pClient->lSendCast);
		else
			InterlockedDecrementEx(&pClient->lRecvCast);

		if( pUnit->pNext )	// 释放unit
			pUnit->pNext->pPool->Free(pUnit->pNext);
		pUnit->pPool->Free(pUnit);
	}
}


//-----------------------------------------------------------------------------
// 回收资源
//-----------------------------------------------------------------------------
VOID Server::DestroyClient(tagClient* pClient)
{
	m_listUnauthClient.Erase(pClient);

	// 将所有包都丢弃
	if( pClient->pRecvQueue->GetNum() > 0 )
	{
		tagBag* pBag = NULL;
		while( pBag = pClient->pRecvQueue->Pop() )
			pBag->pPool->Free(pBag);
	}

	if( pClient->pSendQueue->GetNum() > 0 )
	{
		tagRaw* pRaw = NULL;
		while( pRaw = pClient->pSendQueue->Pop() )
		{
			if( pRaw->lCounter != INVALID )	// 释放多播包
			{
				if( InterlockedDecrementEx(&pRaw->lCounter) == 0 )
					pRaw->pPool->Free(pRaw);
			}
			else
			{
				pRaw->pPool->Free(pRaw);
			}
		}
	}

	pClient->bShutDown = TRUE;
	pClient->bCanClear = TRUE;
	pClient->lSendCast = 0;
	pClient->lRecvCast = 0;
	pClient->lSendCastSize = 0;
	
	pClient->dwClientID = INVALID;
	pClient->dwConnectTime = 0;
	pClient->dwRecvSerial = 0;

	pClient->sock = INVALID_SOCKET;
	pClient->eReason = EDR_Normal;

	// 收回client data
	m_listFreeClient.PushBack(pClient);

}


}
}
