//-----------------------------------------------------------------------------
// author Lyp
// date 2003-03-17
// last 2010-04-27
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {

//-----------------------------------------------------------------------------
// HTTP下载
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
	CHAR			m_szRequestHeader[4096];		// 请求头
	CHAR			m_szResponseHeader[4096];		// 回应头

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

	INT		GetServerState();	// 返回服务器状态码 -1表示不成功
	INT64	GetHttpFileSize();		// 返回文件长度 -1表示不成功
	BOOL	GetNewLocation(std::string &strLocation);

	BOOL	GetResponseHeader(); // 获取返回头
	size_t	FormatRequestHeader(LPCSTR pServer, LPCSTR pObject, INT64 nFrom=0);	//格式化请求头

	BOOL	SendRequest(LPCSTR pRequestHeader=NULL, size_t Length=0);
	BOOL	Connect(LPCSTR szHostName, INT nPort=80);

	DWORD	DownloadThread();
	BOOL	ParseDestFileName(LPCSTR szObjName);	// 分析目标文件名

	BOOL	AfxParseURL(LPCSTR pstrURL, DWORD& dwServiceType, string& strServer, string& strObject, INTERNET_PORT& nPort);
	BOOL	_AfxParseURLWorker(LPCSTR pstrURL, LPURL_COMPONENTSA lpComponents, DWORD& dwServiceType, INTERNET_PORT& nPort, DWORD dwFlags);

};

}