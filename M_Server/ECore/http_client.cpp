//-----------------------------------------------------------------------------
// author Lyp
// date 2003-03-17
// last 2010-04-27
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "http_client.h"

#include "lan_client.h"
#include "disk_io.h"
#include "callback.h"
#include "executor_fun.h"
#include "thread_mgr.h"
#include "string_util.h"
#include "super_string.h"

#define RECV_TIME_OUT (1000*6)

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "wininet.lib")

namespace ECore {

//-----------------------------------------------------------------------------
// Construction/Destruction
//-----------------------------------------------------------------------------
HttpClient::HttpClient()
{
	ZeroMemory(m_szRequestHeader, sizeof(m_szRequestHeader));
	ZeroMemory(m_szResponseHeader, sizeof(m_szResponseHeader));

	m_pStream = new Lan::Client;
	m_pThreadMgr = new ThreadMgr;
	m_hFileHandle = INVALID_HANDLE;
	m_dwThread = 0;
	m_bTerminateThread = FALSE;
	m_bResult = FALSE;
}

HttpClient::~HttpClient()
{
	// �ر��̳߳�
	m_pThreadMgr->ShutDown();
	SAFE_DEL(m_pStream);
	SAFE_DEL(m_pThreadMgr);
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOL HttpClient::Connect(const char *szHostName, int nPort)
{
	if(!szHostName)
		return FALSE;

	m_pStream->Init(Lan::ECM_Text);

	///�������������IP��ַ
	hostent* pHostent = gethostbyname(szHostName);
	if( pHostent == NULL )
		return FALSE;

	///����
	struct in_addr ip_addr;
	memcpy(&ip_addr, pHostent->h_addr_list[0], 4); // h_addr_list[0]��4���ֽ�

	struct sockaddr_in destaddr;
	memset((void *)&destaddr,0,sizeof(destaddr)); 
	destaddr.sin_family=AF_INET;
	destaddr.sin_port=htons(nPort);
	destaddr.sin_addr=ip_addr;

	///����������IP��ַ�ַ���
	char szIP[256];
	sprintf(szIP,"%d.%d.%d.%d", destaddr.sin_addr.S_un.S_un_b.s_b1,	destaddr.sin_addr.S_un.S_un_b.s_b2,
		destaddr.sin_addr.S_un.S_un_b.s_b3,	destaddr.sin_addr.S_un.S_un_b.s_b4);

	m_pStream->TryConnect(szIP, nPort);
	return TRUE;
}

//-----------------------------------------------------------------------------
// ������������URL���HTTP����ͷ
//-----------------------------------------------------------------------------
size_t HttpClient::FormatRequestHeader(const char *pServer,const char *pObject, INT64 nFrom)
{
	ZeroMemory(m_szRequestHeader, sizeof(m_szRequestHeader));

	// ����,�����·��,�汾
	strcat(m_szRequestHeader,"GET ");
	strcat(m_szRequestHeader,pObject);
	strcat(m_szRequestHeader," HTTP/1.1");
	strcat(m_szRequestHeader,"\r\n");

	// ����
	strcat(m_szRequestHeader,"Host:");
	strcat(m_szRequestHeader,pServer);
	strcat(m_szRequestHeader,"\r\n");

	// ���յ���������
	strcat(m_szRequestHeader,"Accept:*/*");
	strcat(m_szRequestHeader,"\r\n");

	// ���������
	strcat(m_szRequestHeader,"User-Agent:Mozilla/4.0 (compatible; MSIE 5.00; Windows 98)");
	strcat(m_szRequestHeader,"\r\n");

	// ��������,����
	strcat(m_szRequestHeader,"Connection:Keep-Alive");
	strcat(m_szRequestHeader,"\r\n");

	if( !m_bCache )
	{
		strcat(m_szRequestHeader,"Cache-Control: no-cache");
		strcat(m_szRequestHeader,"\r\n");
	}

	// �����������ʼ�ֽ�λ��(�ϵ������Ĺؼ�)
	if( nFrom > 0 )
	{
		char szTemp[20];
		strcat(m_szRequestHeader,"Range: bytes=");
		_i64toa(nFrom,szTemp,10);
		strcat(m_szRequestHeader,szTemp);
		strcat(m_szRequestHeader,"-");
		strcat(m_szRequestHeader,"\r\n");
	}

	// ������
	strcat(m_szRequestHeader, "\r\n");

	///���ؽ��
	return strlen(m_szRequestHeader);
}


//-----------------------------------------------------------------------------
///��������ͷ
//-----------------------------------------------------------------------------
BOOL HttpClient::SendRequest(const char *pRequestHeader, size_t Length)
{
	if( !m_pStream->IsConnected() )
		return FALSE;

	m_pStream->Send((LPVOID)pRequestHeader, (DWORD)Length);
	return TRUE;
}


//-----------------------------------------------------------------------------
//��ȡHTTP����ķ���ͷ
//-----------------------------------------------------------------------------
BOOL HttpClient::GetResponseHeader()
{
	INT nIndex = 0;
	ZeroMemory(m_szResponseHeader, sizeof(m_szResponseHeader));
	DWORD dwLastRecvTime = timeGetTime();
	while( nIndex < (sizeof(m_szResponseHeader)/sizeof(CHAR)) )
	{
		DWORD dwMsgSize = 0;
		LPBYTE pRecv = m_pStream->Recv(dwMsgSize);
		if( !pRecv )
		{
			if( !m_pStream->IsConnected() || timeGetTime() - dwLastRecvTime > RECV_TIME_OUT*2)
				return FALSE;

			::Sleep(100);
			if( m_bTerminateThread )
				return FALSE;

			continue;
		}

		dwLastRecvTime = timeGetTime();
		if( nIndex + dwMsgSize >= (sizeof(m_szResponseHeader)/sizeof(CHAR)) )
			return FALSE;
		memcpy(&m_szResponseHeader[nIndex], pRecv, dwMsgSize);
		m_pStream->FreeRecved(pRecv);

		nIndex += dwMsgSize;
		if(nIndex >= 4)
		{
			if( m_szResponseHeader[nIndex - 4] == '\r' && m_szResponseHeader[nIndex - 3] == '\n'
				&& m_szResponseHeader[nIndex - 2] == '\r' && m_szResponseHeader[nIndex - 1] == '\n')
				break;
		}

	}

	return TRUE;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
INT64 HttpClient::GetHttpFileSize()
{
	LPCSTR szContent1 = "content-range: bytes";
	LPCSTR szContent2 = "content-length";
	std::string strValue = "-1";


	//ȡ��ĳ����ֵ
	std::string strRespons = m_szResponseHeader;
	transform(strRespons.begin(), strRespons.end(), strRespons.begin(), tolower);
	
	size_t nPos = -1;
	nPos = strRespons.find(szContent1, 0);
	if( nPos != strRespons.npos )	// �ҵ���
	{
		size_t nPos1 = strRespons.find("/", nPos);
		size_t nPos2 = strRespons.find("\r\n", nPos);
		strValue = strRespons.substr(nPos1+1, nPos2-nPos1-1);
	}
	else
	{
		nPos = strRespons.find(szContent2, 0);
		if( nPos != strRespons.npos )
		{
			size_t nPos1 = strRespons.find(":", nPos);
			size_t nPos2 = strRespons.find("\r\n", nPos);
			strValue = strRespons.substr(nPos1+1, nPos2-nPos1-1);
		}
	}

	return _atoi64(strValue.c_str());
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOL HttpClient::GetNewLocation(std::string &strLocation)
{
	LPCSTR szContent = "location:";

	//ȡ����
	std::string strRespons = m_szResponseHeader;
	transform(strRespons.begin(), strRespons.end(), strRespons.begin(), tolower);

	size_t nPos = -1;
	nPos = strRespons.find(szContent, 0);
	if( nPos == strRespons.npos )	// û�ҵ�
		return FALSE;

	size_t nPos1 = strRespons.find(":",nPos);
	size_t nPos2 = strRespons.find("\r\n",nPos);
	strLocation = strRespons.substr(nPos1+2, nPos2-nPos1-2);
	return TRUE;
}


//-----------------------------------------------------------------------------
// ����Ŀ���ļ���
//-----------------------------------------------------------------------------
BOOL HttpClient::ParseDestFileName(LPCSTR szObjName)
{
	string strTemp = szObjName;
	size_t nPos = strTemp.rfind('/');
	if( nPos == strTemp.npos )
		nPos = 0;
	else
		nPos += 1;

	m_strDestFileName = "temp\\" + strTemp.substr(nPos, strTemp.npos);
	return TRUE;
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int HttpClient::GetServerState()
{
	char szState[3];
	szState[0] = m_szResponseHeader[9];
	szState[1] = m_szResponseHeader[10];
	szState[2] = m_szResponseHeader[11];

	return atoi(szState);
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
#define AFX_INET_SERVICE_FTP        INTERNET_SERVICE_FTP
#define AFX_INET_SERVICE_HTTP       INTERNET_SERVICE_HTTP
#define AFX_INET_SERVICE_GOPHER     INTERNET_SERVICE_GOPHER

#define AFX_INET_SERVICE_UNK        0x1000
#define AFX_INET_SERVICE_FILE       (AFX_INET_SERVICE_UNK+1)
#define AFX_INET_SERVICE_MAILTO     (AFX_INET_SERVICE_UNK+2)
#define AFX_INET_SERVICE_MID        (AFX_INET_SERVICE_UNK+3)
#define AFX_INET_SERVICE_CID        (AFX_INET_SERVICE_UNK+4)
#define AFX_INET_SERVICE_NEWS       (AFX_INET_SERVICE_UNK+5)
#define AFX_INET_SERVICE_NNTP       (AFX_INET_SERVICE_UNK+6)
#define AFX_INET_SERVICE_PROSPERO   (AFX_INET_SERVICE_UNK+7)
#define AFX_INET_SERVICE_TELNET     (AFX_INET_SERVICE_UNK+8)
#define AFX_INET_SERVICE_WAIS       (AFX_INET_SERVICE_UNK+9)
#define AFX_INET_SERVICE_AFS        (AFX_INET_SERVICE_UNK+10)
#define AFX_INET_SERVICE_HTTPS      (AFX_INET_SERVICE_UNK+11)


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOL HttpClient::AfxParseURL(LPCSTR pstrURL, DWORD& dwServiceType,
							   string& strServer, string& strObject, INTERNET_PORT& nPort)
{
	dwServiceType = AFX_INET_SERVICE_UNK;

	CHAR szHostName[INTERNET_MAX_URL_LENGTH+1];
	CHAR szUrlPath[INTERNET_MAX_URL_LENGTH+1];

	URL_COMPONENTSA urlComponents;
	memset(&urlComponents, 0, sizeof(URL_COMPONENTS));
	urlComponents.dwStructSize = sizeof(URL_COMPONENTS);

	urlComponents.dwHostNameLength = INTERNET_MAX_URL_LENGTH;
	urlComponents.lpszHostName = szHostName;
	urlComponents.dwUrlPathLength = INTERNET_MAX_URL_LENGTH;
	urlComponents.lpszUrlPath = szUrlPath;

	BOOL bRetVal = _AfxParseURLWorker(pstrURL, &urlComponents,
		dwServiceType, nPort, ICU_BROWSER_MODE);

	strServer = szHostName;
	strObject = szUrlPath;
	return bRetVal;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOL HttpClient::_AfxParseURLWorker(LPCSTR pstrURL,
										  LPURL_COMPONENTSA lpComponents, DWORD& dwServiceType,
										  INTERNET_PORT& nPort, DWORD dwFlags)
{
	LPSTR pstrCanonicalizedURL;
	CHAR szCanonicalizedURL[INTERNET_MAX_URL_LENGTH];
	DWORD dwNeededLength = INTERNET_MAX_URL_LENGTH;
	BOOL bRetVal;
	BOOL bMustFree = FALSE;

	// Decoding is done in InternetCrackUrl/UrlUnescape 
	// so we don't need the ICU_DECODE flag here.

	DWORD dwCanonicalizeFlags = dwFlags &
		(ICU_NO_ENCODE | ICU_NO_META |
		ICU_ENCODE_SPACES_ONLY | ICU_BROWSER_MODE);

	DWORD dwCrackFlags = 0;

	BOOL bUnescape = FALSE;

	if((dwFlags & (ICU_ESCAPE | ICU_DECODE)) && (lpComponents->dwUrlPathLength != 0) )
	{

		// We use only the ICU_ESCAPE flag for decoding even if
		// ICU_DECODE is passed.

		// Also, if ICU_BROWSER_MODE is passed we do the unescaping
		// manually because InternetCrackUrl doesn't do
		// Browser mode unescaping

		if (dwFlags & ICU_BROWSER_MODE)
			bUnescape = TRUE;
		else
			dwCrackFlags |= ICU_ESCAPE;
	}

	bRetVal = InternetCanonicalizeUrlA(pstrURL, szCanonicalizedURL,
		&dwNeededLength, dwCanonicalizeFlags);

	if (!bRetVal)
	{
		if (::GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			return FALSE;

		pstrCanonicalizedURL = new CHAR[dwNeededLength];
		if (pstrCanonicalizedURL == NULL)
			return FALSE;

		bMustFree = TRUE;
		bRetVal = InternetCanonicalizeUrlA(pstrURL, pstrCanonicalizedURL,
			&dwNeededLength, dwCanonicalizeFlags);
		if (!bRetVal)
		{
			delete [] pstrCanonicalizedURL;
			return FALSE;
		}
	}
	else
		pstrCanonicalizedURL = szCanonicalizedURL;

	// now that it's safely canonicalized, crack it

	bRetVal = InternetCrackUrlA(pstrCanonicalizedURL, 0,
		dwCrackFlags, lpComponents);

	if(bUnescape)
	{
		if(FAILED(UrlUnescapeA(lpComponents->lpszUrlPath,NULL,NULL,URL_UNESCAPE_INPLACE | URL_DONT_UNESCAPE_EXTRA_INFO)))
		{
			if (bMustFree)
				delete [] pstrCanonicalizedURL;

			return FALSE;
		}

		lpComponents->dwUrlPathLength = (DWORD)strlen(lpComponents->lpszUrlPath);
	}

	if (bMustFree)
		delete [] pstrCanonicalizedURL;

	// convert to MFC-style service ID

	if (!bRetVal)
		dwServiceType = AFX_INET_SERVICE_UNK;
	else
	{
		nPort = lpComponents->nPort;
		switch (lpComponents->nScheme)
		{
		case INTERNET_SCHEME_FTP:
			dwServiceType = AFX_INET_SERVICE_FTP;
			break;

		case INTERNET_SCHEME_GOPHER:
			dwServiceType = AFX_INET_SERVICE_GOPHER;
			break;

		case INTERNET_SCHEME_HTTP:
			dwServiceType = AFX_INET_SERVICE_HTTP;
			break;

		case INTERNET_SCHEME_HTTPS:
			dwServiceType = AFX_INET_SERVICE_HTTPS;
			break;

		case INTERNET_SCHEME_FILE:
			dwServiceType = AFX_INET_SERVICE_FILE;
			break;

		case INTERNET_SCHEME_NEWS:
			dwServiceType = AFX_INET_SERVICE_NNTP;
			break;

		case INTERNET_SCHEME_MAILTO:
			dwServiceType = AFX_INET_SERVICE_MAILTO;
			break;

		default:
			dwServiceType = AFX_INET_SERVICE_UNK;
		}
	}

	return bRetVal;
}



//-----------------------------------------------------------------------------
// ��������
//-----------------------------------------------------------------------------
BOOL HttpClient::Download(LPCSTR szSrcFileName, BOOL bCache)
{
	m_bResult = FALSE;
	m_strSrcFileName = szSrcFileName;
	m_bCache = bCache;

	// ���������߳�
	CallbackBase* pExec	= new CallbackP0<HttpClient>(this, &HttpClient::DownloadThread);
	CallbackBase* pEnd	= new CallbackP0<HttpClient>(this, &HttpClient::Terminate);
	m_dwThread = m_pThreadMgr->CreateThread(new ExecutorFun(pExec, pEnd), TRUE);
	return TRUE;
}



//-----------------------------------------------------------------------------
// �����߳�
//-----------------------------------------------------------------------------
DWORD HttpClient::DownloadThread()
{
	DWORD dwServiceType;
	INTERNET_PORT nPort;
	string strServer, strObject;
	std::vector<string> vecFileNames;
	StringUtil::StringToToken(vecFileNames, m_strSrcFileName.c_str(), ',');
	if( vecFileNames.empty() )
	{
		_endthreadex(0);
		return 0;
	}

	INT nVecIndex = 0;
	m_strSrcFileName = vecFileNames[nVecIndex].c_str();

_download_thread_start:

	if( INVALID_HANDLE != m_hFileHandle )
	{
		m_pDiscIOMgr->Close(m_hFileHandle);
		m_hFileHandle = INVALID_HANDLE;
	}

	CHAR mess[256];
	strcpy(mess, m_strSrcFileName.c_str());
	AfxParseURL(mess, dwServiceType, strServer, strObject, nPort);
	ParseDestFileName(strObject.c_str());

	m_n64CompletedSize = 0;
	m_hFileHandle = m_pDiscIOMgr->Create(m_strDestFileName.c_str(), GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);

	if( m_hFileHandle != INVALID_HANDLE )
		m_pDiscIOMgr->GetSizeEx(m_hFileHandle, &m_n64CompletedSize);

	size_t nLength = this->FormatRequestHeader(strServer.c_str(),strObject.c_str(), m_n64CompletedSize);	

	// try connect
	if( FALSE == this->Connect(strServer.c_str(), nPort) )
		goto _download_thread_exit;
	
	while(!m_pStream->IsConnected())
	{
		if( !m_pStream->IsTryingConnect() )
			goto _download_thread_retry;

		Sleep(10);
		if( m_bTerminateThread )
			goto _download_thread_exit;
	}

	this->SendRequest(m_szRequestHeader, nLength);	// ������������

	if( FALSE == this->GetResponseHeader() )
	{
		if( m_bTerminateThread )
			goto _download_thread_exit;

		m_pStream->Disconnect();
		goto _download_thread_retry;
	}


	INT nServerState = this->GetServerState();
	if( nServerState > 299 && nServerState < 399 )	// ��Ҫ�ض���
	{
		std::string strNewLocation ;
		if( this->GetNewLocation(strNewLocation) )
		{
			m_strSrcFileName = strNewLocation.c_str();
			m_pStream->Disconnect();
			goto _download_thread_start;
		}
	}

	m_n64FileSize = this->GetHttpFileSize();
	if( m_n64FileSize == m_n64CompletedSize )
		goto _download_thread_ok;	// �Ѿ��������

	if( m_n64FileSize == -1 || nServerState > 399 )	// �Ҳ����ļ���������߷��������ش���״̬
	{
		if( nServerState == 416 ) // �������뷶Χ
		{
			m_bResult = TRUE;
			goto _download_thread_ok;	// �Ѿ��������
		}

		m_pStream->Disconnect();
		goto _download_thread_retry;
	}

	if( m_hFileHandle == INVALID_HANDLE )
	{
		goto _download_thread_exit;
	}
	else
	{
		// �����̿ռ�
		INT64 n64Need = m_n64FileSize - m_n64CompletedSize + 4096;
		if( n64Need > m_pDiscIOMgr->GetDriveFreeSpace(NULL) )
				goto _download_thread_exit;
	}

	m_pDiscIOMgr->SeekEx(m_hFileHandle, m_n64CompletedSize, FILE_BEGIN);
	m_pStream->SwitchMode(Lan::ECM_Stream);

	DWORD dwLastRecvTime = timeGetTime();
	DWORD dwLastShowTime = timeGetTime();
	while(m_n64CompletedSize < m_n64FileSize)
	{
		DWORD dwRecvSize = 0;
		LPBYTE pRecv = m_pStream->Recv(dwRecvSize);
		if(!pRecv)
		{
			if( !m_pStream->IsConnected() || timeGetTime() - dwLastRecvTime > RECV_TIME_OUT )	// �Ƿ񻹴�������״̬
			{
				m_pStream->Disconnect();
				m_pDiscIOMgr->Close(m_hFileHandle);
				m_hFileHandle = INVALID_HANDLE;
				goto _download_thread_retry;
			}

			if( m_bTerminateThread )
				goto _download_thread_exit;

			Sleep(100);
			continue;
		}

		m_pDiscIOMgr->Write(m_hFileHandle, pRecv, dwRecvSize);
		m_n64CompletedSize += dwRecvSize;
		dwLastRecvTime = timeGetTime();

		m_pStream->FreeRecved(pRecv);
	}

_download_thread_ok:
	m_bResult = TRUE;
	m_pDiscIOMgr->Close(m_hFileHandle);
	m_hFileHandle = INVALID_HANDLE;
	m_pStream->Disconnect();
	_endthreadex(0);
	return 0;


_download_thread_retry:
	if( INVALID_HANDLE != m_hFileHandle )
	{
		m_pDiscIOMgr->Close(m_hFileHandle);
		m_hFileHandle = INVALID_HANDLE;
	}
	m_pStream->Disconnect();

	// �Ƿ� ���������ĵ�ַ���Գ���
	if( (INT)vecFileNames.size() > nVecIndex + 1)
	{
		nVecIndex++;
		m_strSrcFileName = vecFileNames[nVecIndex].c_str();
	}
	else
	{
		// �ӵ�һ����ַ���¿�ʼ��������
		nVecIndex = 0;
		m_strSrcFileName = vecFileNames[nVecIndex].c_str();
	}
	goto _download_thread_start;


_download_thread_exit:
	if( INVALID_HANDLE != m_hFileHandle )
	{
		m_pDiscIOMgr->Close(m_hFileHandle);
		m_hFileHandle = INVALID_HANDLE;
	}
	m_pStream->Disconnect();
	_endthreadex(0);
	return 0;
}

}