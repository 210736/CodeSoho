//-----------------------------------------------------------------------------
// author Lyp
// date 2003-03-17
// last 2010-04-27
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {

//-----------------------------------------------------------------------------
// HTTP����
//-----------------------------------------------------------------------------
class ECORE_API HttpClient
{
public:
	HttpClient();
	~HttpClient();

	BOOL		Download(LPCSTR szSrcFileName, BOOL bCache=TRUE);
	LPCSTR		GetDestFileName() { return m_strDestFileName.c_str(); }

	DWORD		Terminate() { InterlockedExchangeEx((LPLONG)&m_bTerminateThread, TRUE); return 0; }
	BOOL		GetResult() { return m_bResult; }

	INT64		GetFileSize() { return m_n64FileSize; }
	INT64		GetCompletedSize() { return m_n64CompletedSize; }


private:	
	ThreadMgr*		m_pThreadMgr;
	TObj<DiskIO>	m_pDiscIOMgr;
	CHAR			m_szRequestHeader[4096];		// ����ͷ
	CHAR			m_szResponseHeader[4096];		// ��Ӧͷ

	Lan::Client*	m_pStream;

	string			m_strSrcFileName;
	string			m_strDestFileName;

	HANDLE			m_hFileHandle;
	INT64			m_n64CompletedSize;
	INT64			m_n64FileSize;

	DWORD			m_dwThread;
	BOOL			m_bTerminateThread;
	BOOL			m_bResult;
	BOOL			m_bCache;

	INT		GetServerState();	// ���ط�����״̬�� -1��ʾ���ɹ�
	INT64	GetHttpFileSize();		// �����ļ����� -1��ʾ���ɹ�
	BOOL	GetNewLocation(std::string &strLocation);

	BOOL	GetResponseHeader(); // ��ȡ����ͷ
	size_t	FormatRequestHeader(LPCSTR pServer, LPCSTR pObject, INT64 nFrom=0);	//��ʽ������ͷ

	BOOL	SendRequest(LPCSTR pRequestHeader=NULL, size_t Length=0);
	BOOL	Connect(LPCSTR szHostName, INT nPort=80);

	DWORD	DownloadThread();
	BOOL	ParseDestFileName(LPCSTR szObjName);	// ����Ŀ���ļ���

	BOOL	AfxParseURL(LPCSTR pstrURL, DWORD& dwServiceType, string& strServer, string& strObject, INTERNET_PORT& nPort);
	BOOL	_AfxParseURLWorker(LPCSTR pstrURL, LPURL_COMPONENTSA lpComponents, DWORD& dwServiceType, INTERNET_PORT& nPort, DWORD dwFlags);

};

}