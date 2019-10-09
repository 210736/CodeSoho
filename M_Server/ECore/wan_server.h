//-----------------------------------------------------------------------------
// author Lyp
// date 2004-04-03
// last 2009-03-19
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {
namespace Wan {
//-----------------------------------------------------------------------------
// �������˴���ײ�
//-----------------------------------------------------------------------------
class ECORE_API Server
{
public:
	BOOL	Init(tagConfig& param);
	BOOL	Destroy();

	//-----------------------------------------------------------------------------
	// ��������
	//-----------------------------------------------------------------------------
	INLINE LPBYTE GetRaw(DWORD dwSize, DWORD dwRandHint);	// �õ��洢�ռ䣬һ����������
	INLINE VOID FreeRaw(LPVOID p);	// �ͷŴ洢�ռ䣬���û�е��ù�(Multi)Send,����Ҫ���ô˺���
	INLINE BOOL Send(HANDLE hHandle, LPVOID pMsg);
	INLINE VOID MultiSend(HANDLE hHandle, LPVOID pMsg);
	INLINE VOID	ActiveSend() { for(size_t n=0; n<m_vecEventSend.size(); ++n) ::SetEvent(m_vecEventSend[n]); m_nSendCast = 0; m_nRecvCast = 0; m_lGetRawNum = 0; }

	INLINE LPBYTE Recv(HANDLE hHandle, DWORD& dwSize);
	INLINE VOID	FreeRecved(LPBYTE pMsg);
	
	INLINE VOID Logoff(HANDLE hHandle);	// �ϲ��յ�logoff�ص��󣬵��ô˺���ȷ����Դ���ͷ�
	INLINE VOID Kick(HANDLE hHandle) { DisconnectClient((tagClient*)hHandle, NULL, EDR_Kick); }

	//-----------------------------------------------------------------------------
	// ��Ϣ����
	//-----------------------------------------------------------------------------
	INLINE tagConfig* GetConfig()	{ return &m_Config; }
	INLINE tagClient* GetClient(HANDLE hHandle) { return ((tagClient*)hHandle); }
	INLINE INT	GetFreeClientNum() { return m_listFreeClient.Size(); }

	INLINE INT64 GetSendSize() { return m_n64SendSize; } // �ϲ㷢�ʹ�С
	INLINE INT64 GetRealSendSize() { return m_n64RealSendSize; } // ѹ����ʵ�ʷ��ʹ�С
	
	INLINE INT	GetPoolNum() { return MEM_POOL_NUM; }
	INLINE MemCache<AtomMutex>* GetPool(INT n)	{ return &m_Pool[n]; }

	INLINE INT	GetSendCastSize() { return m_nSendCastSize; }	// �ȴ����͵��ֽ���
	INLINE INT	GetSendCast() { return m_nSendCast; }			// �ϲ㱾��������Ҫ���͵İ���
	INLINE INT	GetRecvCast() { return m_nRecvCast; }			// �ײ㱾�������յ��İ���
	
	INLINE LONG	GetGetRawNum() { return m_lGetRawNum; }
	INLINE INT GetDestroyListSize() { return m_listDestroyClient.Size(); }
	
	INLINE LONG GetKickNum(EDisconnectReason eReason) { return m_lKickNum[eReason]; }

	Server();
	~Server();

private:
	Filter*						m_pFilter;
	ThreadMgr*					m_pThreadMgr;

	// Ϊ�Ż�׼���ĺ���ָ��
	LPFN_ACCEPTEX				m_lpfnAcceptEx;
	LPFN_GETACCEPTEXSOCKADDRS	m_lpfnGetAESockAddr;

	// �ڴ�����
	MemCache<AtomMutex>*		m_Pool;				

	// �ͻ������
	tagClient*					m_Client;
	List<tagClient*>			m_listFreeClient;
	List<tagClient*>			m_listUnauthClient;
	List<tagClient*>			m_listDestroyClient;

	// ϵͳ����
	tagConfig					m_Config;
	
	// �������
	SOCKET						m_Sock;
	HANDLE						m_hPort;
	INT	VOLATILE				m_nAcceptEx;

	// �߳����
	BOOL						m_bCloseThread;
	HANDLE						m_hEventAccepEx;
	vector<HANDLE>				m_vecEventSend;
	HANDLE						m_hEventDestroy;

	// ͳ����Ϣ
	INT64	VOLATILE			m_n64SendSize;
	INT64	VOLATILE			m_n64RealSendSize;

	INT		VOLATILE			m_nSendCast;
	INT		VOLATILE			m_nSendCastSize;
	INT		VOLATILE			m_nRecvCast;
	LONG	VOLATILE			m_lGetRawNum;

	LONG	VOLATILE			m_lKickNum[EDR_End];

	// �߳����
	BOOL CreateMemCache();
	BOOL CreateTDSetupAccepEx();
	BOOL CreatTDWorker(INT nThreadNum);
	BOOL CreateTDUnauthHeartbeat();
	BOOL CreateTDSend(INT nThreadNum);
	BOOL CreateTDDestroy();
	
	DWORD TDSetupAcceptEx();							// ��������������߳�
	DWORD TDWorker(INT nThreadIndex);					// ���֪ͨ�����߳�
	DWORD TDUnauthHeartbeat(DWORD dwWaitForAuthTime);	// �߳�δ��֤�ͻ����߳�
	DWORD TDSend(INT nThreadIndex, INT nThreadNum);		// ����������߳�	0
	DWORD TDDestroy();									// ��Դ�����߳�
	DWORD TDGC();										// GC�߳�
	
	INLINE DWORD CloseThread()	{ InterlockedExchangeEx((LONG*)&m_bCloseThread, TRUE); ActiveSend(); return 0; }

	//-------------------------------------------------------------------------------
	// ������Դ
	//-------------------------------------------------------------------------------
	VOID DisconnectClient(tagClient* pClient, tagBag* pUnit, EDisconnectReason eReason=EDR_Normal, DWORD dwDebugMsg=0);
	VOID DestroyClient(tagClient* pClient);
};


//-----------------------------------------------------------------------------
// ����unit,�����Եõ���ǰ�յ���unit��Ŀ�ܺ�
//-----------------------------------------------------------------------------
INLINE LPBYTE Server::Recv(HANDLE hHandle, DWORD& dwSize)
{
	tagClient* pClient = (tagClient*)hHandle;

	if( !pClient->bShutDown )
	{
		tagBag* pBag = pClient->pRecvQueue->Pop();

		if( !pBag )
			return NULL;

		dwSize = pBag->dwSize;
		return (LPBYTE)pBag->pMsg;
	}
	return NULL;
}


//-----------------------------------------------------------------------------
// ����unit,�����Եõ���ǰ�յ���unit��Ŀ�ܺ�
//-----------------------------------------------------------------------------
INLINE VOID Server::FreeRecved(LPBYTE pMsg)
{
	tagBag* pBag = CONTAINING_RECORD(pMsg, tagBag, pMsg);
	pBag->pPool->Free(pBag);
}


//-----------------------------------------------------------------------------
// �õ��洢�ռ䣬һ����������
//-----------------------------------------------------------------------------
INLINE LPBYTE Server::GetRaw(DWORD dwSize, DWORD dwRandHint)
{
	InterlockedIncrementEx(&m_lGetRawNum);

	// �����ⲿ�����������ʾ��Ѱ�ҿ��Խ����������ڴ��
	INT nRand = dwRandHint & 0x1F;

	// ������, Ѱ�ҿ��Խ����������ڴ��
	tagRaw* pRaw = NULL;
	while( !pRaw )
	{
		pRaw = (tagRaw*)m_Pool[nRand].TryAlloc(RAWBAG_SIZE(dwSize));
		if( !pRaw )
		{
			if( ++nRand >= MEM_POOL_NUM )
				nRand = 0;
		}
	}

	// ��ʼ��Unit
	pRaw->pPool = &m_Pool[nRand];
	pRaw->dwSize = dwSize;
	pRaw->lCounter = 1;
	return (LPBYTE)(pRaw->pMsg);
}


//-----------------------------------------------------------------------------
// �ͷŴ洢�ռ䣬���û�е��ù�(Multi)Send,����Ҫ���ô˺���
//-----------------------------------------------------------------------------
INLINE VOID Server::FreeRaw(LPVOID p)
{
	tagRaw* pRaw = CONTAINING_RECORD(p, tagRaw, pMsg);
	if( InterlockedDecrementEx(&pRaw->lCounter) == 0 )
		pRaw->pPool->Free(pRaw);
}


//-----------------------------------------------------------------------------
// ͨ��Rawֱ�ӷ���
//-----------------------------------------------------------------------------
INLINE BOOL Server::Send(HANDLE hHandle, LPVOID pMsg)
{
	tagClient* pClient = (tagClient*)hHandle;
	tagRaw* pRaw = CONTAINING_RECORD(pMsg, tagRaw, pMsg);
	if( pClient->bShutDown )
	{
		pRaw->pPool->Free(pRaw);
		return FALSE;
	}

	ASSERT( 1 == pRaw->lCounter );
	InterlockedExchangeEx((LONG*)&pRaw->lCounter, INVALID);

	InterlockedIncrementEx(&pClient->lSendCast);
	
	if( FALSE == pClient->pSendQueue->Push(pRaw) )
	{
		InterlockedDecrementEx(&pClient->lSendCast);
		DisconnectClient(pClient, NULL, EDR_TooManySend, *(DWORD*)(pRaw->pMsg));
		pRaw->pPool->Free(pRaw);
		return FALSE;
	}

	
	return TRUE;
}


//-----------------------------------------------------------------------------
// �ಥ
//-----------------------------------------------------------------------------
INLINE VOID Server::MultiSend(HANDLE hHandle, LPVOID pMsg)
{
	// ����ͻ���shutdown,�ಥ������Ҫ�ͷ�
	tagClient* pClient = (tagClient*)hHandle;
	if( pClient->bShutDown )
		return;

	tagRaw* pRaw = CONTAINING_RECORD(pMsg, tagRaw, pMsg);
	ASSERT( INVALID != pRaw->lCounter );


	// Ѱ�ҿ��Խ����������ڴ�أ����
	// ������
	InterlockedIncrementEx(&pRaw->lCounter);
	InterlockedIncrementEx(&pClient->lSendCast);

	if( FALSE == pClient->pSendQueue->Push(pRaw) )
	{
		InterlockedDecrementEx(&pRaw->lCounter);
		InterlockedDecrementEx(&pClient->lSendCast);
		DisconnectClient(pClient, NULL, EDR_TooManySend, *(DWORD*)(pRaw->pMsg));
		return;
	}


}


//-----------------------------------------------------------------------------
// �ϲ��յ�logoff�ص��󣬵��ô˺���ȷ����Դ���ͷ�
//-----------------------------------------------------------------------------
INLINE VOID Server::Logoff(HANDLE hHandle)
{
	tagClient* pClient = (tagClient*)hHandle;
	if( pClient->bShutDown )
		pClient->bCanClear = TRUE;
}


}
}
