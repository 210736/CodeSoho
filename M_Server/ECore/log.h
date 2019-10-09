//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2003/12/8
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {

#define LOG TObj<Log>()->Write
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class ECORE_API Log
{
public:
	//-------------------------------------------------------------------------
	// 创建和关闭
	//-------------------------------------------------------------------------
	BOOL	Create(LPCWSTR szFileName=NULL);
	BOOL	Create(LPCSTR szFileName);
	VOID	Close();

	//-------------------------------------------------------------------------
	// 写日志
	//-------------------------------------------------------------------------
	BOOL	Write(LPCWSTR lpFormat, ...);
	BOOL	Write(LPCSTR lpFormat, ...);
	VOID	Flush();
	void	WriteFileLog(LPCSTR lpStr, HANDLE hHandle);
	void	WriteFileLog(LPCWSTR lpStr, HANDLE hHandle);

	LPCWSTR	GetFileName()	{ return m_szFileName; }
	LPCSTR	GetFileNameA()	{ return m_szFileNameA; }
	Log():m_hFileHandle(INVALID_HANDLE){ m_szFileName[0]=0; m_szFileNameA[0]=0; }
	~Log() { Close(); }

private:
	TObj<DiskIO>		m_pDiskIO;
	AtomMutex			m_FileLogLock;
	HANDLE				m_hFileHandle;
	WCHAR				m_szFileName[HUGE_STRING];
	CHAR				m_szFileNameA[HUGE_STRING];
};




} // namespace ECore {
