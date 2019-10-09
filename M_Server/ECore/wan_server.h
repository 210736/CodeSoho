//-----------------------------------------------------------------------------
// author Lyp
// date 2004-04-03
// last 2009-03-19
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {
namespace Wan {
//-----------------------------------------------------------------------------
// 服务器端传输底层
//-----------------------------------------------------------------------------
class ECORE_API Server
{
public:
	BOOL	Init(tagConfig& param);
	BOOL	Destroy();

	//-----------------------------------------------------------------------------
	// 基本操作
	//-----------------------------------------------------------------------------
	INLINE LPBYTE GetRaw(DWORD dwSize, DWORD dwRandHint);	// 得到存储空间，一般用来发送
	INLINE VOID FreeRaw(LPVOID p);	// 释放存储空间，如果没有调用过(Multi)Send,才需要调用此函数
	INLINE BOOL Send(HANDLE hHandle, LPVOID pMsg);
	INLINE VOID MultiSend(HANDLE hHandle, LPVOID pMsg);
	INLINE VOID	ActiveSend() { for(size_t n=0; n<m_vecEventSend.size(); ++n) ::SetEvent(m_vecEventSend[n]); m_nSendCast = 0; m_nRecvCast = 0; m_lGetRawNum = 0; }

	INLINE LPBYTE Recv(HANDLE hHandle, DWORD& dwSize);
	INLINE VOID	FreeRecved(LPBYTE pMsg);
	
	INLINE VOID Logoff(HANDLE hHandle);	// 上层收到logoff回调后，调用此函数确认资源可释放
	INLINE VOID Kick(HANDLE hHandle) { DisconnectClient((tagClient*)hHandle, NULL, EDR_Kick); }

	//-----------------------------------------------------------------------------
	// 信息反馈
	//-----------------------------------------------------------------------------
	INLINE tagConfig* GetConfig()	{ return &m_Config; }
	INLINE tagClient* GetClient(HANDLE hHandle) { return ((tagClient*)hHandle); }
	INLINE INT	GetFreeClientNum() { return m_listFreeClient.Size(); }

	INLINE INT64 GetSendSize() { return m_n64SendSize; } // 上层发送大小
	INLINE INT64 GetRealSendSize() { return m_n64RealSendSize; } // 压缩后实际发送大小
	
	INLINE INT	GetPoolNum() { return MEM_POOL_NUM; }
	INLINE MemCache<AtomMutex>* GetPool(INT n)	{ return &m_Pool[n]; }

	INLINE INT	GetSendCastSize() { return m_nSendCastSize; }	// 等待发送的字节数
	INLINE INT	GetSendCast() { return m_nSendCast; }			// 上层本次心跳需要发送的包数
	INLINE INT	GetRecvCast() { return m_nRecvCast; }			// 底层本次心跳收到的包数
	
	INLINE LONG	GetGetRawNum() { return m_lGetRawNum; }
	INLINE INT GetDestroyListSize() { return m_listDestroyClient.Size(); }
	
	INLINE LONG GetKickNum(EDisconnectReason eReason) { return m_lKickNum[eReason]; }

	Server();
	~Server();

private:
	Filter*						m_pFilter;
	ThreadMgr*					m_pThreadMgr;

	// 为优化准备的函数指针
	LPFN_ACCEPTEX				m_lpfnAcceptEx;
	LPFN_GETACCEPTEXSOCKADDRS	m_lpfnGetAESockAddr;

	// 内存池相关
	MemCache<AtomMutex>*		m_Pool;				

	// 客户端相关
	tagClient*					m_Client;
	List<tagClient*>			m_listFreeClient;
	List<tagClient*>			m_listUnauthClient;
	List<tagClient*>			m_listDestroyClient;

	// 系统配置
	tagConfig					m_Config;
	
	// 网络相关
	SOCKET						m_Sock;
	HANDLE						m_hPort;
	INT	VOLATILE				m_nAcceptEx;

	// 线程相关
	BOOL						m_bCloseThread;
	HANDLE						m_hEventAccepEx;
	vector<HANDLE>				m_vecEventSend;
	HANDLE						m_hEventDestroy;

	// 统计信息
	INT64	VOLATILE			m_n64SendSize;
	INT64	VOLATILE			m_n64RealSendSize;

	INT		VOLATILE			m_nSendCast;
	INT		VOLATILE			m_nSendCastSize;
	INT		VOLATILE			m_nRecvCast;
	LONG	VOLATILE			m_lGetRawNum;

	LONG	VOLATILE			m_lKickNum[EDR_End];

	// 线程相关
	BOOL CreateMemCache();
	BOOL CreateTDSetupAccepEx();
	BOOL CreatTDWorker(INT nThreadNum);
	BOOL CreateTDUnauthHeartbeat();
	BOOL CreateTDSend(INT nThreadNum);
	BOOL CreateTDDestroy();
	
	DWORD TDSetupAcceptEx();							// 接受连接请求的线程
	DWORD TDWorker(INT nThreadIndex);					// 完成通知处理线程
	DWORD TDUnauthHeartbeat(DWORD dwWaitForAuthTime);	// 踢出未认证客户端线程
	DWORD TDSend(INT nThreadIndex, INT nThreadNum);		// 网络包发送线程	0
	DWORD TDDestroy();									// 资源回收线程
	DWORD TDGC();										// GC线程
	
	INLINE DWORD CloseThread()	{ InterlockedExchangeEx((LONG*)&m_bCloseThread, TRUE); ActiveSend(); return 0; }

	//-------------------------------------------------------------------------------
	// 回收资源
	//-------------------------------------------------------------------------------
	VOID DisconnectClient(tagClient* pClient, tagBag* pUnit, EDisconnectReason eReason=EDR_Normal, DWORD dwDebugMsg=0);
	VOID DestroyClient(tagClient* pClient);
};


//-----------------------------------------------------------------------------
// 接收unit,并可以得到当前收到的unit数目总和
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
// 接收unit,并可以得到当前收到的unit数目总和
//-----------------------------------------------------------------------------
INLINE VOID Server::FreeRecved(LPBYTE pMsg)
{
	tagBag* pBag = CONTAINING_RECORD(pMsg, tagBag, pMsg);
	pBag->pPool->Free(pBag);
}


//-----------------------------------------------------------------------------
// 得到存储空间，一般用来发送
//-----------------------------------------------------------------------------
INLINE LPBYTE Server::GetRaw(DWORD dwSize, DWORD dwRandHint)
{
	InterlockedIncrementEx(&m_lGetRawNum);

	// 根据外部给出的随机提示，寻找可以进入锁定的内存池
	INT nRand = dwRandHint & 0x1F;

	// 包分配, 寻找可以进入锁定的内存池
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

	// 初始化Unit
	pRaw->pPool = &m_Pool[nRand];
	pRaw->dwSize = dwSize;
	pRaw->lCounter = 1;
	return (LPBYTE)(pRaw->pMsg);
}


//-----------------------------------------------------------------------------
// 释放存储空间，如果没有调用过(Multi)Send,才需要调用此函数
//-----------------------------------------------------------------------------
INLINE VOID Server::FreeRaw(LPVOID p)
{
	tagRaw* pRaw = CONTAINING_RECORD(p, tagRaw, pMsg);
	if( InterlockedDecrementEx(&pRaw->lCounter) == 0 )
		pRaw->pPool->Free(pRaw);
}


//-----------------------------------------------------------------------------
// 通过Raw直接发送
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
// 多播
//-----------------------------------------------------------------------------
INLINE VOID Server::MultiSend(HANDLE hHandle, LPVOID pMsg)
{
	// 如果客户端shutdown,多播包不需要释放
	tagClient* pClient = (tagClient*)hHandle;
	if( pClient->bShutDown )
		return;

	tagRaw* pRaw = CONTAINING_RECORD(pMsg, tagRaw, pMsg);
	ASSERT( INVALID != pRaw->lCounter );


	// 寻找可以进入锁定的内存池，随机
	// 包分配
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
// 上层收到logoff回调后，调用此函数确认资源可释放
//-----------------------------------------------------------------------------
INLINE VOID Server::Logoff(HANDLE hHandle)
{
	tagClient* pClient = (tagClient*)hHandle;
	if( pClient->bShutDown )
		pClient->bCanClear = TRUE;
}


}
}
