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
// ������������
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
// ���
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
	
	// ����socket
	m_Sock = socket(AF_INET, SOCK_STREAM, 0);
	if( INVALID_SOCKET == m_Sock )
		return FALSE;

	// ʹ�÷�������ʽ,linger��Ϊ
	DWORD dwCmd = 1;
	ioctlsocket(m_Sock, FIONBIO, &dwCmd);

	// �ر�Nagle�㷨
	BOOL bNoDelay = TRUE;
	setsockopt(m_Sock, IPPROTO_TCP, TCP_NODELAY, (char*)&bNoDelay, sizeof(bNoDelay));

	// ���ö˿�����
	DWORD dwReUseAddr = TRUE;
	setsockopt(m_Sock, SOL_SOCKET, SO_REUSEADDR, (char*)&dwReUseAddr, sizeof(dwReUseAddr));

	// ָ��Ӳ�رշ�ʽ
	linger lin;
	lin.l_linger = 0;
	lin.l_onoff = 1;
	setsockopt(m_Sock, SOL_SOCKET, SO_LINGER, (char*)&lin, sizeof(lin));

	// ÿ������ͳ��
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
	// �ر��̳߳�
	m_pThreadMgr->ShutDown();

	// ���������߳�ID
	m_dwTDConnectID = INVALID;

	// �����ر�socket
	if( m_bConnected )
	{
		CloseSocket(m_Sock, 10);	// ���� 10 ��
	}
	else
	{
		// m_bConnected��δΪ��֮ǰ����ʱ���ڿ����Ѿ�����
		// ������shutdownһ�£����޸�����
		shutdown(m_Sock, SD_SEND);
		closesocket(m_Sock);
		m_Sock = 0;
	}

	// �رջ���
	InterlockedExchangeEx((LONG*)&m_bConnected, FALSE);
	m_SendMsgQueue.Clear();
	m_RecvMsgQueue.Clear();
}


//-----------------------------------------------------------------------------
// �����ر�socket
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
// ��������ָ����ַ
//-----------------------------------------------------------------------------
BOOL Client::TryConnect(LPCSTR szIP, INT nPort)
{
	this->Disconnect();
	
	m_strIP = szIP;
	m_nPort = nPort;

	// ���������߳�
	CallbackBase* pExec	= new CallbackP0<Client>(this, &Client::TDConnect);
	CallbackBase* pEnd	= new CallbackP0<Client>(this, &Client::CloseTDConnect);
	m_dwTDConnectID = m_pThreadMgr->CreateThread(new ExecutorFun(pExec, pEnd), TRUE);

	return VALID(m_dwTDConnectID);
}


//-----------------------------------------------------------------------------
// �Ƿ����ڳ�������
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
// �Ͽ�����
//-----------------------------------------------------------------------------
VOID Client::Disconnect()
{
	this->Destroy();
	this->Init();
}




//-----------------------------------------------------------------------------
// �����߳�
//-----------------------------------------------------------------------------
DWORD Client::TDConnect()
{
	INT nResult = 0;
	sockaddr_in address;
	ZeroMemory(&address, sizeof(address));
	address.sin_addr.s_addr = inet_addr(m_strIP.c_str());
	address.sin_family = AF_INET;
	address.sin_port = htons(m_nPort);

	// ���IP�Ƿ�Ϸ�
	if( INADDR_NONE == address.sin_addr.s_addr )
		goto __connected_terminated;
	
	// ��������
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
			for(INT n = 0; n < 64; ++n) // ��ͣ����,�ⲿ����ͨ������StopTryingConnectֹͣ����
			{
				FD_ZERO(&fdwrite);
				FD_SET(m_Sock, &fdwrite);

				if( 1 == select(0, NULL, &fdwrite, NULL, &block_time) )
					goto __connected;

				if( m_bCloseConnect )
					goto __connected_terminated;
			}
		}

		// ���ӳ���ʧ��
		goto __connected_terminated;
	}
	
__connected:

	// ��ʼ����Ϣͳ��
	m_dwSendPackNum = 0;
	m_dwRecvPackNum = 0; 

	// ���������߳�
	CallbackBase* pSendExec	= new CallbackP0<Client>(this, &Client::TDSend);
	CallbackBase* pSendEnd	= new CallbackP0<Client>(this, &Client::CloseTDSend);
	m_pThreadMgr->CreateThread(new ExecutorFun(pSendExec, pSendEnd), TRUE);

	// ���������߳�
	CallbackBase* pRecvExec	= new CallbackP0<Client>(this, &Client::TDReceive);
	CallbackBase* pRecvEnd	= new CallbackP0<Client>(this, &Client::CloseTDReceive);
	m_pThreadMgr->CreateThread(new ExecutorFun(pRecvExec, pRecvEnd), TRUE);

	// ���ӳɹ����߳�ֹͣ
	InterlockedExchangeEx((LONG*)&m_bConnected, TRUE);
	return 0;

__connected_terminated:
		
	// ����ʧ��
	InterlockedExchangeEx((LONG*)&m_bConnected, FALSE);
	return 0;
}



//-----------------------------------------------------------------------------
// �����̺߳���
//-----------------------------------------------------------------------------
DWORD Client::TDSend()
{
	fd_set fdwrite;
	timeval block_time;	// ���÷�������ʱ��
	block_time.tv_sec = 0;
	block_time.tv_usec = NET_BLOCK_TIME;	// 150ms

	LPBYTE	pMsg = NULL, pTempPtr = NULL;
	DWORD	dwMsgSize = 0;
	INT		nReturn = 0;

	while( FALSE == m_bCloseSend )
	{
		for(;;)	// ע���ڲ���break
		{
			// ����ͳ��
			DWORD dwTime = timeGetTime();
			if( dwTime - m_dwLastStatOutTime >= ONE_SECOND ) 
			{
				m_dwLastStatOutTime = dwTime;
				m_dwSendBPS = m_dwByteOutCounter;
				m_dwByteOutCounter = 0;
			}

			pTempPtr = pMsg = m_SendMsgQueue.GetMsg(dwMsgSize);
			if( NULL == pMsg )
				break;	// ����Ϊֹ

			// ����
			DWORD dwSerial = (m_dwSendPackNum++) % 0x1000;
			for(DWORD dw=0; dw<dwMsgSize-sizeof(DWORD); ++dw)
				pMsg[dw+sizeof(DWORD)] ^= g_SqrtTable[dwSerial+dw];

			// send ���ܲ���һ�ν����ݷ���
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
						// �ȴ����ݿ��Է��͵�ʱ��
						FD_ZERO(&fdwrite);
						FD_SET(m_Sock, &fdwrite);
						select(0, NULL, &fdwrite, NULL, &block_time);
						break;

					default:	// ����
						shutdown(m_Sock, SD_SEND);
						m_SendMsgQueue.FreeMsg(pMsg);
						goto __thread_send_end;
					}
				}
				else
				{
					pTempPtr += nReturn;
					dwMsgSize -= nReturn;
					m_dwByteOutCounter += nReturn;	// ����ͳ��
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
// �����߳�
//-----------------------------------------------------------------------------
DWORD Client::TDReceive()
{
	fd_set fdread;
	timeval block_time;	// ��������ʱ��
	block_time.tv_sec = 0;
	block_time.tv_usec = NET_BLOCK_TIME;

	INT nResult = 0;
	DWORD dwMsgSize = 0;
	DWORD dwTempSize = 0;
	CHAR* pTempPtr = NULL;

	// ѭ���������
	while( FALSE == m_bCloseReceive )
	{
		// �Ƚ���DWORD�͵���Ϣ������Ϣ
		dwTempSize = sizeof(DWORD);
		pTempPtr = (char*)&dwMsgSize;

		// ����ͳ��
		DWORD dwTime = timeGetTime();
		if( dwTime - m_dwLastStatInTime >= ONE_SECOND )
		{
			m_dwLastStatInTime = dwTime;
			m_dwRecvBPS = m_dwByteInCounter;
			m_dwByteInCounter = 0;
		}

		while( dwTempSize > 0 )
		{
			// ��ȡ�����ݵĴ�С
			FD_ZERO(&fdread);
			FD_SET(m_Sock, &fdread);
			if( 1 == select(0, &fdread, NULL, NULL, &block_time) )
			{
				nResult = recv(m_Sock, pTempPtr, dwTempSize, 0);
				if( 0 == nResult )	// FIN
					goto __thread_receive_end;

				if( SOCKET_ERROR == nResult )
				{
					// �������
					shutdown(m_Sock, SD_SEND);
					goto __thread_receive_end;
				}
				else
				{
					dwTempSize -= nResult;
					pTempPtr += nResult;

					// ����ͳ��
					m_dwByteInCounter += nResult;
				}
			}
			
			if( m_bCloseReceive )
				goto __thread_receive_end;
		}

		ASSERT(dwMsgSize < 40960);	// �쳣������
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
						// �������
						shutdown(m_Sock, SD_SEND);
						MCFREE(pBufferIn);
						goto __thread_receive_end;
					}
					else
					{
						dwTempSize -= nResult;
						pTempPtr += nResult;

						// ����ͳ��
						m_dwByteInCounter += nResult;
					}
					
					if( m_bCloseReceive )
					{
						MCFREE(pBufferIn);
						goto __thread_receive_end;
					}
				}
			}
			
			// ��ʼ����Ϣͳ��
			++m_dwRecvPackNum;

			CHAR* pBufferOut = (CHAR*)MCALLOC(*(DWORD*)pBufferIn+sizeof(DWORD));

			// �����ˣ���ѹ���ļ�
			tagFiltParam param;
			param.bCompress = FALSE;
			param.dwInBuffer = dwMsgSize+dwMsgSize/64+64;
			param.stInData = dwMsgSize;
			param.dwOutBuffer = *(DWORD*)pBufferIn;
			param.stOutData = 0;
			param.eFilter = EF_Lzo1X;
			param.pMemIn = pBufferIn + sizeof(DWORD);
			param.pMemOut = pBufferOut;

			// ���й����ļ���Ϣ
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