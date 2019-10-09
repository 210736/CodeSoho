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

	// �߳��¼�
	m_hEventAccepEx	= ::CreateEvent(NULL, FALSE, TRUE, NULL);		// �Զ�����
	m_hEventDestroy	= ::CreateEvent(NULL, FALSE, TRUE, NULL);		// �Զ�����

	// ѹ����
	m_pThreadMgr = new ThreadMgr;
	m_pFilter = new Filter;
	m_pFilter->Init(EF_Lzo1X);
}


//-----------------------------------------------------------------------------
//! destruction
//-----------------------------------------------------------------------------
Server::~Server()
{
	// ����ѹ����
	SAFE_DEL(m_pThreadMgr);
	SAFE_DEL(m_pFilter);

	// �ر��߳��¼�����
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
	
	// ����������Ϣ�еĿͻ����������ߣ����ɿͻ�������
	m_Client = new tagClient[m_Config.nMaxLoad];
	if( !m_Client ) 
		return FALSE;

	// ��ʼ�������ÿһ���ͻ��ˣ�����������б�����
	ZeroMemory(m_Client, sizeof(tagClient)*m_Config.nMaxLoad);
	for(INT n = 0; n < m_Config.nMaxLoad; ++n)
	{
		m_Client[n].pSendQueue = new RingQueue<tagRaw*>(m_Config.lMaxSendBag);
		m_Client[n].pRecvQueue = new RingQueue<tagBag*>(m_Config.lMaxRecvBag);
		m_listFreeClient.PushBack(&m_Client[n]);
	}

	// ��ʼ������socket���������������
	m_Sock = socket(AF_INET, SOCK_STREAM, 0);

	// reuse��ɶ�ΰ󶨳ɹ������ܲ����ҵ���ȷ���õĶ˿�
	// �����̨����������ͬһ̨������ʹ��ͬһ���˿ڣ���ɻ���
	DWORD dwReUseAddr = TRUE;	
	setsockopt(m_Sock, SOL_SOCKET, SO_REUSEADDR, (char*)&dwReUseAddr, sizeof(dwReUseAddr) );

	// �ر�Nagle�㷨
	BOOL bNoDelay = TRUE;
	setsockopt(m_Sock, IPPROTO_TCP, TCP_NODELAY, (char*)&bNoDelay, sizeof(bNoDelay));

	// �󶨶˿�
	sockaddr_in address;
    ZeroMemory(&address, sizeof(address));
    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port        = htons(m_Config.nPort);

    while( SOCKET_ERROR == bind(m_Sock, (sockaddr*)&address, sizeof(address)) )
	{
		if( WSAEADDRINUSE == WSAGetLastError() )
		{
			++m_Config.nPort;	// ���γ��Ժ���Ķ˿�
			address.sin_port = htons(m_Config.nPort);
		}
		else
		{
			closesocket(m_Sock);
			ERR(_T("bind() failed"));
			return FALSE;
		}
	}

	// ������ɶ˿ڣ���������socket�󶨵�����ɶ˿�
    m_hPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    listen(m_Sock, SOMAXCONN);
	CreateIoCompletionPort((HANDLE)m_Sock, m_hPort, (ULONG_PTR)0, 0);

	// ���AcceptEx\GetAcceptExSockAddrs�ĺ���ָ��
	GUID Guid = WSAID_ACCEPTEX, Guid2 = WSAID_GETACCEPTEXSOCKADDRS;
	DWORD dwBytes = 0;
	WSAIoctl(m_Sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &Guid,
		sizeof(GUID), &m_lpfnAcceptEx, sizeof(m_lpfnAcceptEx), &dwBytes, 0, 0);
	WSAIoctl(m_Sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &Guid2,
		sizeof(GUID), &m_lpfnGetAESockAddr,sizeof(m_lpfnGetAESockAddr), &dwBytes, 0, 0);


	// �����ڴ���Լ�GC�߳�
	if( FALSE == CreateMemCache() )
		return FALSE;

	// �������ļ�ָ������Ŀ���������߳�
	SYSTEM_INFO  sysinfo;
	GetSystemInfo(&sysinfo);
	INT nWorkerThread = (INT)sysinfo.dwNumberOfProcessors * m_Config.nPerCpuThread;
	
	if( FALSE == CreatTDWorker(nWorkerThread) )	// ���������߳��Լ��ڴ��
		return FALSE;
	
	if( FALSE == CreateTDSetupAccepEx() )		// �������������߳�
		return FALSE;

	if( FALSE == CreateTDSend((INT)sysinfo.dwNumberOfProcessors) )				// ���������߳�
		return FALSE;
	
	if( FALSE == CreateTDUnauthHeartbeat() )	// �����߳�δ��֤�ͻ����߳�
		return FALSE;

	if( FALSE == CreateTDDestroy() )			// ������Դ�����߳�
		return FALSE;

	return TRUE;
}


//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
BOOL Server::Destroy()
{
	// �ر���ɶ˿�
	SAFE_CLOSE_HANDLE(m_hPort);

	// �ر��̹߳�����
	m_pThreadMgr->ShutDown();

	// ���ٸ����ͻ��˵�����
	if( m_Client )
	{
		for(INT n = 0; n < m_Config.nMaxLoad; ++n)	// ���� index
		{
			if( !m_Client[n].bShutDown )
			{
				closesocket(m_Client[n].sock);

				SAFE_DEL(m_Client[n].pRecvQueue);
				SAFE_DEL(m_Client[n].pSendQueue);
			}
		}
	}

	// ����Ԥ�����ڴ�
	SAFE_DEL_ARRAY(m_Pool);
	SAFE_DEL_ARRAY(m_Client);
	return TRUE;
}


//-----------------------------------------------------------------------------
// �����ڴ���Լ�GC�߳�
//-----------------------------------------------------------------------------
BOOL Server::CreateMemCache()
{
	// ��ʼ���ڴ��
	m_Pool = new MemCache<AtomMutex>[MEM_POOL_NUM];
	for(INT n = 0; n < MEM_POOL_NUM; ++n)
		m_Pool[n].SetMaxSize(m_Config.dwPoolSize);


	// ����GC�߳�
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
// �������������߳�
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

	// ����event
	::SetEvent(m_hEventAccepEx);

	return TRUE;
}


//-----------------------------------------------------------------------------
// ���������߳�
//-----------------------------------------------------------------------------
BOOL Server::CreatTDWorker(INT nThreadNum)
{
	for(INT n = 0; n < nThreadNum; ++n)
	{
		// ���������߳�
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
// ���������߳�
//-----------------------------------------------------------------------------
BOOL Server::CreateTDSend(INT nThreadNum)
{
	
	for(INT n = 0; n < nThreadNum; ++n)
	{
		m_vecEventSend.push_back(::CreateEvent(NULL, FALSE, TRUE, NULL));		// �Զ�����

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
// �����߳�δ��֤�ͻ����߳�
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
// ������Դ�����߳�
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
// GC�̺߳���,����ά���ڴ�ص�ƽ��
//-----------------------------------------------------------------------------
DWORD Server::TDGC()
{
	while( !m_bCloseThread )
	{
		// ��ѯ�ڴ��
		for(INT n = 0; n < MEM_POOL_NUM; ++n)
		{
			if( m_Pool[n].GetSize() > m_Config.dwPoolSize / 2 )
			{
				m_Pool[n].TryGC( m_Config.dwPoolSize / 64 );
				Sleep(SLEEP_TIME);	// ÿ��GC��SLEEP
			}
		}

		Sleep(SLEEP_TIME);
	}
	return 0;
}



//-----------------------------------------------------------------------------
// ���������̺߳���
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
			// �õ�һ�����пͻ���
			pClient = m_listFreeClient.PopFront();
			if( !VALID(pClient) ) 
			{
				WaitForSingleObject(m_hEventAccepEx, SLEEP_TIME);
				continue;
			}

			// ��ʼ���ÿͻ���
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
		
			// ����һ���������ڽ�����������
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
			
			// ����������Ӳ��ɹ�����黹�ÿͻ��˵���Դ
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
// �����̺߳�����������ɶ˿ڵķ���֪ͨ������Ӧ�ĺ�������
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
		// �õ���ɶ˿ڵ����֪ͨ
		INT nResult = GetQueuedCompletionStatus(m_hPort, &dwBytesComp, (PULONG_PTR)&pClient, &pOverlap, SLEEP_TIME);
		if( !pOverlap ) 
			continue;

		// �����֪ͨ�еõ�Unit
		pGot = CONTAINING_RECORD(pOverlap, tagBag, ov);

		// �����Unit�ǽ���ʱ���ģ���ô��Ӧ��clientָ��ʽ��buff��õ���
		if( ES_Accept == pGot->eState )
		{
			pClient = (tagClient*)(((tagLoginParam*)(&pGot->pBuffer[PARAM_INDEX]))->hHandle);
			InterlockedDecrementEx((LONG*)&m_nAcceptEx);	// �ٴ���AcceptEx
			SetEvent(m_hEventAccepEx);
		}

		// ��ɶ˿�ʧ�ܣ���ִ�пͻ��˵�Ԥ���ٲ���
		if( !nResult )
		{
			DisconnectClient(pClient, pGot, EDR_CPortError); 
			continue;
		}

		// ���Unit��ÿһ��״ִ̬�ж�Ӧ�Ĳ���
		switch( pGot->eState )
		{
		// ��������
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
			((tagLoginParam*)(&pGot->pBuffer[PARAM_INDEX]))->dwAddress = pRemoteAddr->sin_addr.S_un.S_addr;	// �õ��ͻ���ip�Ͷ˿�
			((tagLoginParam*)(&pGot->pBuffer[PARAM_INDEX]))->dwPort = pRemoteAddr->sin_port;

			// ���ÿͻ��˵�socket�󶨵���ɶ˿�
			if( !CreateIoCompletionPort((HANDLE)pClient->sock, m_hPort, (ULONG_PTR)pClient, 0) )	// ����ɶ˿�
			{
				DisconnectClient(pClient, pGot, EDR_CPortBindError); 
				break;
			}

			// ���ͻ��˼��뵽δ��֤�б��У��ȴ��ͻ��˷�������һ����Ϣ
			pClient->dwConnectTime = timeGetTime();
			pClient->dwRecvSerial = 0;
			m_listUnauthClient.PushBack(pClient);


			// ���յ�һ����Ϣ������
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

			// �ۼ��յ��İ�����
			pGot->dwReady += dwBytesComp;

			// ��û������һ��DWORD���ͼ�����
			if( pGot->dwReady < sizeof(DWORD) )	
			{
				pGot->ov.hEvent = NULL;	// û���꣬��������
				wbuf.buf = pGot->pBuffer + pGot->dwReady;
				wbuf.len = MAX_RECV_SIZE - pGot->dwReady;

				if( WSARecv(pClient->sock, &wbuf, 1, &dwBytes, &dwFlags, &pGot->ov, NULL) )
				{
					if( WSA_IO_PENDING != WSAGetLastError() )
						DisconnectClient(pClient, pGot, EDR_WSARecvError);
				}		
				break;	// ����
			}
__again:
			// �����İ�
			if( pGot->dwSize > MAX_RECV_SIZE || pGot->dwSize == 0 )	
			{
				DisconnectClient(pClient, pGot, EDR_TooBigRecv);	// ֱ���ߵ�
				break;	// ����
			}

			// �յ�һ�����߳���һ����������
			if( pGot->dwReady >= pGot->dwSize + sizeof(DWORD) )
			{
				// �ȴ����յ��İ�������
				DWORD dwSerial = (pClient->dwRecvSerial++) % 0x1000;
				for(DWORD dw=0; dw<pGot->dwSize; ++dw)
					pGot->pMsg[dw] ^= g_SqrtTable[dwSerial+dw];

				if( pGot->pNext )	// �Ǵ˿ͻ��˵�һ����
				{
					// �����û�г�����֤��ʱʱ�䣬������ϲ㴦��
					if( TRUE == m_listUnauthClient.Erase(pClient) )
					{
						// ֪ͨ�ϲ㣬��һ���ͻ�����������
						tagLoginParam* pParam = (tagLoginParam*)(&pGot->pNext->pBuffer[PARAM_INDEX]);
						pClient->dwClientID = m_Config.pLogHandle->Logon(pGot->pMsg, pGot->dwSize, pParam);

						// �ϲ�û��ͨ�������֤�����߳��ÿͻ���
						if( INVALID == pClient->dwClientID )
						{
							DisconnectClient(pClient, pGot, EDR_FailedAuth);
							break;
						}

						pGot->pNext->pPool->Free(pGot->pNext);
						pGot->pNext = NULL;
					}
					else	// �Ѿ���ʱ����ɾ���û�
					{
						DisconnectClient(pClient, pGot, EDR_AuthTimeOut);
						break;
					}
				}

				DWORD dwOver = pGot->dwReady - pGot->dwSize - sizeof(DWORD);	// ���ճ���
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

				if( dwOver > 0 )	// ����������˸��������
				{
					memcpy(pNew->pBuffer, pGot->pBuffer + pGot->dwSize + sizeof(DWORD), dwOver); // �����Ĳ��ֿ����°�
				}

				// ���ϰ����뵽���ն�������ȥ
				if( FALSE == pClient->pRecvQueue->Push(pGot) )
				{
					DisconnectClient(pClient, pGot, EDR_TooManyRecv);
					pNew->pPool->Free(pNew);
					break;
				}

				InterlockedIncrementEx((LONG*)&m_nRecvCast);

				if( pNew->dwReady >= pNew->dwSize + sizeof(DWORD) )	// pNew��Ҳ��ȫ�յ���
				{
					pGot = pNew;
					goto __again;	// �ظ�����Ĺ���
				}
				else
				{
					// �����°�
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
				pGot->ov.hEvent = NULL;	// û���꣬��������
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
// �����̺߳���
//-----------------------------------------------------------------------------
DWORD Server::TDSend(INT nThreadIndex, INT nThreadNum)
{
	// ����ʱ����
	CHAR* pBuff	= new CHAR[m_Config.dwMaxTickSend];
	tagRaw*	pRaw = NULL;
	tagBag*	pBag = NULL;
	DWORD dwBytes =	0;
	DWORD dwTotal =	0;
	LONG lPopNum = 0;
	WSABUF wbuf;
	INT	nFakeRandom = (nThreadIndex + 16) & 0x1F;
	tagClient* pClient = NULL;

	// ѹ������
	tagFiltParam param;
	param.bCompress	 = TRUE;
	param.eFilter	 = EF_Lzo1X;
	param.pMemIn	 = pBuff;
	param.dwInBuffer = m_Config.dwMaxTickSend;

	while( !m_bCloseThread )
	{
		for(INT n = nThreadIndex; n < m_Config.nMaxLoad; n+=nThreadNum)
		{
			// �鿴�Ƿ���Ҫ���͵���Ϣ
			pClient = &m_Client[n];
			if( 0 == pClient->pSendQueue->GetNum() )
				continue;

			// �ͻ����Ѿ��ر�
			if( pClient->bShutDown )
				continue;

			// ȡ������������ƴ��
			dwTotal = 0;
			lPopNum = 0;

			while( pRaw = pClient->pSendQueue->Pop() )
			{
				if( pRaw->dwSize + dwTotal < m_Config.dwMaxTickSend )	// ȷ��û�г�����ʱ�ڴ�����
					memcpy(pBuff + dwTotal, pRaw->pMsg, pRaw->dwSize);

				++lPopNum;
				dwTotal += pRaw->dwSize;

				// ɾ��pRaw
				if( pRaw->lCounter != INVALID )	// ����ಥ��
				{
					if( InterlockedDecrementEx(&pRaw->lCounter) == 0 )
						pRaw->pPool->Free(pRaw);
				}
				else
				{
					pRaw->pPool->Free(pRaw);
				}

				if( dwTotal >= m_Config.dwMaxTickSend )	// ������ʱ�ڴ�����
				{
					DisconnectClient(pClient, NULL, EDR_TooBigSend);	// �ߵ�
					dwTotal = 0;	// ���治����
					break;
				}

				
			}

			// ͳ����
			InterlockedExchangeAdd((LONG*)&m_nSendCast, lPopNum);

			// ����ǿհ�������Ҫ��ѹ��������
			if( dwTotal == 0 )
			{
				InterlockedExchangeAdd((LPLONG)&pClient->lSendCast, -lPopNum);
				continue;
			}

			// ����Ҫ�����ˣ�����+1
			InterlockedExchangeAdd((LPLONG)&pClient->lSendCast, -lPopNum + 1);

			// �ҵ�һ���ڴ�أ��������㹻����ڴ棬���ѹ���������
			for( pBag=0;!pBag; ) 
			{
				pBag = (tagBag*)m_Pool[nFakeRandom].TryAlloc(BAG_SIZE(dwTotal)+LZO_ADD(dwTotal)+sizeof(DWORD)*2);
				if( !pBag && ++nFakeRandom >= MEM_POOL_NUM )
					nFakeRandom = 0;
			} 

			// ѹ��
			param.stInData = dwTotal;
			param.dwOutBuffer = dwTotal + LZO_ADD(dwTotal);
			param.stOutData = 0;
			param.pMemOut = pBag->pBuffer + sizeof(DWORD) * 2;
			m_pFilter->Filt(&param);

			// ��дѹ��ǰ��С��ѹ�����С
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

			// ����ѹ���������
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
			
			// �ۼ�ͳ����Ϣ
			m_n64SendSize += dwTotal + sizeof(DWORD) * 2; 
			m_n64RealSendSize += param.stOutData + sizeof(DWORD);
		}

		WaitForSingleObject(m_vecEventSend[nThreadIndex], INFINITE);
	}
	
	SAFE_DEL_ARRAY(pBuff);
	return 0;
}



//-----------------------------------------------------------------------------
// �߳�δ��֤�ͻ����̺߳���
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
				// ��ʱ�ˣ������ٸÿͻ���
				if( dwTime - pClient->dwConnectTime > dwWaitForAuthTime )
				{
					DisconnectClient(pClient, NULL, EDR_AuthTimeOut);
					m_listUnauthClient._EraseCurrent();
				}
				else
				{
					// û�г�ʱ��������һ��
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
// ��Դ�����߳�
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
			m_Config.pLogHandle->PreLogoff(pClient->dwClientID, pClient->eReason, pClient->dwDebugMsg);	// �ص��û��ǳ�����
			pClient->dwClientID = INVALID;
		}
		
		if( TRUE == pClient->bCanClear )
		{
			if( pClient->pSendQueue->GetNum() > 0 )
			{
				while( pRaw = pClient->pSendQueue->Pop() )
				{
					InterlockedDecrementEx(&pClient->lSendCast);
					if( pRaw->lCounter != INVALID )	// �ͷŶಥ��
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
		
		// ������е����֪ͨ�ǲ��Ƕ��Ѿ�����
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
// ������Դ
//-----------------------------------------------------------------------------
VOID Server::DisconnectClient(tagClient* pClient, tagBag* pUnit, EDisconnectReason eReason, DWORD dwDebugMsg)
{
	if( FALSE == InterlockedCompareExchange((LPLONG)(&pClient->bShutDown), TRUE, FALSE) )
	{
		shutdown(pClient->sock, SD_BOTH);
		InterlockedIncrementEx(&m_lKickNum[eReason]);

		if( !VALID(pClient->dwClientID) )	// δ�����ϲ���֤�Ŀͻ��ˣ�����Ҫ�ȴ��ϲ����á���ɾ����
			pClient->bCanClear = TRUE;

		pClient->eReason = eReason;
		pClient->dwDebugMsg = dwDebugMsg;
			
		m_listDestroyClient.PushBack(pClient);
		SetEvent(m_hEventDestroy);
		closesocket(pClient->sock);
	}

	if( pUnit )
	{
		if( ES_Send == pUnit->eState )	// �������ü���
			InterlockedDecrementEx(&pClient->lSendCast);
		else
			InterlockedDecrementEx(&pClient->lRecvCast);

		if( pUnit->pNext )	// �ͷ�unit
			pUnit->pNext->pPool->Free(pUnit->pNext);
		pUnit->pPool->Free(pUnit);
	}
}


//-----------------------------------------------------------------------------
// ������Դ
//-----------------------------------------------------------------------------
VOID Server::DestroyClient(tagClient* pClient)
{
	m_listUnauthClient.Erase(pClient);

	// �����а�������
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
			if( pRaw->lCounter != INVALID )	// �ͷŶಥ��
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

	// �ջ�client data
	m_listFreeClient.PushBack(pClient);

}


}
}
