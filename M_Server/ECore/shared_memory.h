//-----------------------------------------------------------------------------
// filename: shared_memory.h
// author: Sxg
// actor:
// data: 2009-12-21
// last:
// brief: 共享内存封装 -- 通过封装FileMapping实现
//-----------------------------------------------------------------------------
#pragma once

namespace ECore{

//-----------------------------------------------------------------------------
// 共享内存FileMapping
//-----------------------------------------------------------------------------
class ECORE_API SharedMem
{
public:
	//-------------------------------------------------------------------------
	// 构造和析构
	//-------------------------------------------------------------------------
	SharedMem(LPCTSTR szSharedMemName, INT64 n64SharedMemSize);
	~SharedMem();

	//-------------------------------------------------------------------------
	// 共享内存操作
	//-------------------------------------------------------------------------
	BOOL		Create();
	BOOL		Open();
	VOID		Close();

	//-------------------------------------------------------------------------
	// 数据获取接口
	//-------------------------------------------------------------------------
	INLINE LPCTSTR		GetMapFileName() const		{ return m_szSharedMemName; }
	INLINE INT64		GetMapFileSize() const		{ return m_n64SharedMemSize; }
	INLINE UINT			GetLastError() const		{ return m_dwLastError; }

	LPBYTE		GetData();

private:
	TCHAR		m_szSharedMemName[MAX_PATH];		// 共享内存名称
	INT64		m_n64SharedMemSize;					// 共享内存大小

	HANDLE		m_hHandle;							// 共享内存句柄
	LPBYTE		m_pData;							// 共享内存头指针
	DWORD		m_dwLastError;						// 错误码
};

//-----------------------------------------------------------------------------
// 得到共享内存头指针
//-----------------------------------------------------------------------------
INLINE LPBYTE SharedMem::GetData()
{
	ASSERT(m_hHandle != NULL);

	if( NULL == m_pData )
	{
		m_pData = (LPBYTE)::MapViewOfFile(m_hHandle, FILE_MAP_WRITE, 0, 0, 0);
		m_dwLastError = ::GetLastError();
	}

	return m_pData;
}

} // namespace ECore