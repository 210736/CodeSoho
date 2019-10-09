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
// ���������
//-----------------------------------------------------------------------------
Client::Client() : m_SendMsgQueue(TRUE, TRUE), m_RecvMsgQueue(TRUE, FALSE)
{
	Clear();
	m_bBigEndian = 0;	// Ĭ��Little Endian
	m_pThreadMgr = new ThreadMgr;
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
	
	if( ECM_Package == m_eMode )	// ���ݷ���ģʽ�������Ƿ��ڰ�ǰ������С��Ϣ
		m_SendMsgQueue.SetAddSizeAhead(TRUE);
	else
		m_SendMsgQueue.SetAddSizeAhead(FALSE);

	// ����socket
	m_Sock = socket(AF_INET, SOCK_STREAM, 0);
	if( INVALID_SOCKET == m_Sock )
		return FALSE;

	// ʹ�÷�������ʽ,linger��Ϊ
	DWORD dwCmd = 1;
	INT nResult = ioctlsocket(m_Sock, FIONBIO, &dwCmd);
	if( SOCKET_ERROR == nResult )
	{
		closesocket(m_Sock);
		m_Sock = 0;
		return FALSE;
	}

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
void Client::Destroy()
{
	// �ر��̳߳�
	m_pThreadMgr->ShutDown();

	// ���������߳�ID
	m_dwConnectID = INVALID;

	// �����ر�socket
	if( m_bConnected )
	{
		CloseSocket(m_Sock, 20);	// ���� 20 ��
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

	INT nRetry = 0;
	fd_set fdread;
	timeval block_time;	// ��������ʱ��
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
// ���ð�ģʽ��SIZEͷ���ֽ���
//-----------------------------------------------------------------------------
BOOL Client::SetEndian(BOOL bBigEndian)
{
	if( ECM_Package != m_eMode )
		return FALSE;

	m_bBigEndian = bBigEndian;
	return TRUE;
}



//-----------------------------------------------------------------------------
// ��������ָ����ַ
//-----------------------------------------------------------------------------
VOID Client::TryConnect(LPCSTR szIP, INT nPort)
{
	this->Disconnect();

	m_strIP = szIP;
	m_nPort = nPort;

	// ���������߳�
	CallbackBase* pExec	= new CallbackP0<Client>(this, &Client::Connect);
	CallbackBase* pEnd	= new CallbackP0<Client>(this, &Client::CloseConnect);
	m_dwConnectID = m_pThreadMgr->CreateThread(new ExecutorFun(pExec, pEnd), TRUE);

	ASSERT(VALID(m_dwConnectID));
}


//-----------------------------------------------------------------------------
//! �����Ƿ����ڳ�������
//! ��������������ȷ������Զ��������û����������������ô�ͻ�һֱ��������
//! ����������緢�����󣬾��п���ֹͣ��������
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
// �����Ͽ�����
//-----------------------------------------------------------------------------
VOID Client::Disconnect()
{
	// ��������Ѿ��������ˣ���ôҲ��Ͽ�
	this->Destroy();
	this->Init(m_eMode);
}





//-----------------------------------------------------------------------------
// �����߳�
//-----------------------------------------------------------------------------
DWORD Client::Connect()
{
	sockaddr_in address;
	ZeroMemory(&address, sizeof(address));
	address.sin_addr.s_addr = inet_addr(m_strIP.c_str());
	address.sin_family = AF_INET;
	address.sin_port = htons(m_nPort);

	// ���IP�Ƿ�Ϸ�
	if( INADDR_NONE == address.sin_addr.s_addr )
		goto __connected_terminated;
	
	fd_set fdwrite;
	timeval block_time;
	block_time.tv_sec = 0;
	block_time.tv_usec = NET_BLOCK_TIME;

	INT nResult = connect(m_Sock, (sockaddr*)&(address), sizeof(address));	// ��������
	if( SOCKET_ERROR == nResult )
	{	
		if( WSAEWOULDBLOCK == WSAGetLastError() )
		{
			for(INT n = 0; n < 64; ++n) // ����
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
	CallbackBase* pSendExec	= new CallbackP0<Client>(this, &Client::Send);
	CallbackBase* pSendEnd	= new CallbackP0<Client>(this, &Client::CloseSend);
	m_pThreadMgr->CreateThread(new ExecutorFun(pSendExec, pSendEnd), TRUE);

	// ���������߳�
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

	// ���ӳɹ����߳�ֹͣ
	InterlockedExchangeEx((LONG*)&m_bConnected, TRUE);
	return 0;

__connected_terminated:
		
	// ����ʧ��
	InterlockedExchangeEx((LONG*)&m_bConnected, FALSE);
	return 0;
}

//-----------------------------------------------------------------------------
// �����߳�
//-----------------------------------------------------------------------------
DWORD Client::Send()
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

			if( m_bBigEndian )	// �����ֽ���
				*(u_long*)pMsg = htonl(*(u_long*)pMsg);

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
DWORD Client::Receive()
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

		if( m_bBigEndian )	// �����ֽ���
			dwMsgSize = ntohl(dwMsgSize);

		// ASSERT(dwMsgSize < 40960);	// �쳣������
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
						// �������
						shutdown(m_Sock, SD_SEND);
						MCFREE(pTemp);
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
						MCFREE(pTemp);
						goto __thread_receive_end;
					}
				}
			}
			
			// ��ʼ����Ϣͳ��
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
// ��ʽ�����̺߳���
//-----------------------------------------------------------------------------
DWORD Client::StreamReceive()
{
	fd_set fdread;
	timeval block_time;	// ��������ʱ��
	block_time.tv_sec = 0;
	block_time.tv_usec = NET_BLOCK_TIME;

	CONST INT RECV_BUFF_SIZE = 2048;	// 2KB recv buff
	INT nResult = 0;
	LPBYTE pBuffer = (LPBYTE)MCALLOC(RECV_BUFF_SIZE);

	// ѭ���������
	while( FALSE == m_bCloseReceive )
	{
		DWORD dwTime = timeGetTime();
		if( dwTime - m_dwLastStatInTime >= ONE_SECOND )	// ����ͳ��
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

				if( nResult == SOCKET_ERROR )	// �������
				{
					shutdown(m_Sock, SD_SEND);
					goto __thread_stream_receive_end;
				}
				else
				{
					m_dwByteInCounter += nResult;	// ����ͳ��
					++m_dwRecvPackNum;	// ����ͳ��
					m_RecvMsgQueue.AddMsg(pBuffer, nResult);	// ��������
				}
			}
		}
		else if( ECM_Text == m_eMode )	// �ı�ģʽ���ֽ���
		{
			CHAR* pTempPtr = (CHAR*)pBuffer;
			DWORD dwMsgSize = 0;
			while( FALSE == m_bCloseReceive )
			{
				FD_ZERO(&fdread);
				FD_SET(m_Sock, &fdread);
				if( 1 == select(0, &fdread, NULL, NULL, &block_time) )
				{
					nResult = recv(m_Sock, pTempPtr, 1, 0);	// ��1���ֽ�
					if( 0 == nResult )	// FIN
						goto __thread_stream_receive_end;

					if( nResult == SOCKET_ERROR )	// �������
					{
						shutdown(m_Sock, SD_SEND);
						goto __thread_stream_receive_end;
					}
					else
					{
						dwMsgSize += nResult;
						if( *pTempPtr == '\n' || dwMsgSize >= RECV_BUFF_SIZE )
						{
							//��ʼ����Ϣͳ��
							m_dwRecvPackNum++; 
							m_RecvMsgQueue.AddMsg(pBuffer, dwMsgSize);
							break;
						}

						// ����ͳ��
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
// �ı�ģʽ����ģʽ֮���л����ʺ�HTTP����
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

	// ���IP�Ƿ�Ϸ�
	if( INADDR_NONE == address.sin_addr.s_addr )
		goto __connected_terminated;

	fd_set fdwrite;
	timeval block_time;
	block_time.tv_sec = 0;
	block_time.tv_usec = NET_BLOCK_TIME;

	INT nResult = connect(m_Sock, (sockaddr*)&(address), sizeof(address));	// ��������
	if( SOCKET_ERROR == nResult )
	{	
		if( WSAEWOULDBLOCK == WSAGetLastError() )
		{
			for(INT n = 0; n < 64; ++n) // ����
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
	CallbackBase* pSendExec	= new CallbackP0<Client>(this, &Client::Send);
	CallbackBase* pSendEnd	= new CallbackP0<Client>(this, &Client::CloseSend);
	m_pThreadMgr->CreateThread(new ExecutorFun(pSendExec, pSendEnd), TRUE);

	// ���������߳�
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

	// ���ӳɹ����߳�ֹͣ
	InterlockedExchangeEx((LONG*)&m_bConnected, TRUE);
	return 1;

__connected_terminated:

	// ����ʧ��
	InterlockedExchangeEx((LONG*)&m_bConnected, FALSE);
	return 0;

}

}
}