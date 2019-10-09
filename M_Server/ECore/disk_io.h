//-----------------------------------------------------------------------------
// Auth: Lyp
// Data: 2003-1-2
//-----------------------------------------------------------------------------
#pragma once
namespace ECore {

//-----------------------------------------------------------------------------
// 将系统文件操作包装了一层，除了减慢速度，什么用也没有
//-----------------------------------------------------------------------------
class ECORE_API DiskIO
{
public:
	//-------------------------------------------------------------------------
	// 打开和关闭文件
	//-------------------------------------------------------------------------
	HANDLE	Create(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, 
					LPVOID lpSecurityAttributes, DWORD dwCreationDisposition, 
					DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
	HANDLE	Create(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, 
					LPVOID lpSecurityAttributes, DWORD dwCreationDisposition, 
					DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
	BOOL	Close(HANDLE hFileHandle);

	//-------------------------------------------------------------------------
	// 打开和关闭内存映射文件
	//-------------------------------------------------------------------------
	HANDLE	CreateMapping(HANDLE hFileHandle, LPVOID lpAttrib, DWORD dwProtect,
					DWORD dwMaxSize, LPCWSTR lpName);
	HANDLE	CreateMapping(HANDLE hFileHandle, LPVOID lpAttrib, DWORD dwProtect,
					DWORD dwMaxSize, LPCSTR lpName);
	BOOL	CloseMappingHandle(HANDLE hFileMappingHandle);

	//-------------------------------------------------------------------------
	// 映射和取消内存映射文件
	//-------------------------------------------------------------------------
	LPVOID	MapFile(HANDLE hFileMappingHandle, DWORD dwDesireAccess, 
					DWORD dwFileOffset, DWORD dwNumberOfBytesToMap);
	LPVOID	MapFileEx(HANDLE hFileMappingHandle, DWORD dwDesireAccess, 
					DWORD dwFileOffset, DWORD dwNumberOfBytesToMap, LPVOID* ppMem=0);
	BOOL	UnMapFile(LPCVOID lpBaseAddress);

	//-------------------------------------------------------------------------
	// 读取
	//-------------------------------------------------------------------------
	BOOL	Read(HANDLE hFileHandle, LPVOID lpBuffer, DWORD dwNumberOfBytesToRead,
					DWORD* lpNumberOfBytesRead, LPVOID lpOverlapped);
	DWORD	Read(HANDLE hFileHandle, LPVOID lpBuffer, DWORD dwNumberOfBytesToRead);

	//-------------------------------------------------------------------------
	// 直接读取
	//-------------------------------------------------------------------------
	DWORD	LoadToMem(LPVOID pMem, LPCWSTR pFileName);
	DWORD	LoadToMem(LPVOID pMem, LPCSTR pFileName);

	//-------------------------------------------------------------------------
	// 写入
	//-------------------------------------------------------------------------
	BOOL	Write(HANDLE hFileHandle, LPCVOID lpBuffer, DWORD dwNumberOfBytesToWrite,
					DWORD* lpNumberOfBytesWritten, LPVOID lpOverlapped);
	BOOL	Write(HANDLE hFileHandle, LPCVOID lpBuffer, DWORD dwNumberOfBytesToWrite);

	//-------------------------------------------------------------------------
	// 寻址
	//-------------------------------------------------------------------------
	DWORD	Seek(HANDLE hFileHandle, DWORD dwDistanceToMove, DWORD dwMoveMethod);
	DWORD	Seek(HANDLE hFileHandle, DWORD dwOffsetFromStart);
	BOOL	SeekEx(HANDLE hFileHandle, INT64 n64DistanceToMove, DWORD dwMoveMethod);

	//-------------------------------------------------------------------------
	// 得到大小
	//-------------------------------------------------------------------------
	DWORD	GetSize(LPCWSTR pFileName);
	DWORD	GetSize(LPCSTR pFileName);
	DWORD	GetSize(HANDLE hFileHandle);
	BOOL	GetSizeEx(HANDLE hFileHandle, INT64* pn64Size);

	//-------------------------------------------------------------------------
	// 设置文件尾
	//-------------------------------------------------------------------------
	DWORD	SetEOF(HANDLE hFileHandle);

	//-------------------------------------------------------------------------
	// 得到文件最后修改日期
	//-------------------------------------------------------------------------
	BOOL	GetTime(LPCWSTR szFileName, LPFILETIME lpLastWriteTime);
	BOOL	GetTime(LPCSTR szFileName, LPFILETIME lpLastWriteTime);

	//-------------------------------------------------------------------------
	// 目录相关
	//-------------------------------------------------------------------------
	static BOOL	IsFileExist(LPCWSTR pFileName);
	static BOOL	IsFileExist(LPCSTR pFileName);
	static BOOL DeleteFile(LPCWSTR pFileName);
	static BOOL DeleteFile(LPCSTR pFileName);
	static BOOL	IsDir(LPCWSTR pFileName);
	static BOOL IsDir(LPCSTR pFileName);
	static BOOL ClearDirectoryFiles( LPCWSTR szDirName );
	static BOOL ClearDirectoryFiles( LPCSTR szDirName );
	static HANDLE FindFirstFile(LPCTSTR pFileName, LPVOID lpFindData);
	static BOOL	FindNextFile(HANDLE hFileHandle, LPVOID lpFindData);
	static BOOL	FindClose(HANDLE hFileHandle);
	DWORD	GetFileNumInDir(LPCTSTR pFullName, DWORD& dwTotalBytes);
	VOID	GetFileNumInDir(LPCTSTR pFullName, DWORD& dwFileNum, DWORD& dwBytes);

	//-------------------------------------------------------------------------
	// 功能函数
	//-------------------------------------------------------------------------
	INT64	GetDriveFreeSpace(LPCTSTR szDriver);
	INLINE static BOOL IsInvalidHandle(HANDLE hHandle)	{ return (INVALID_HANDLE == hHandle); }
	INLINE DWORD GetAllocGranularity() { return m_dwAllocGranularity; }


	DiskIO();

private:
	DWORD		m_dwAllocGranularity;	// map映射粒度
};

} // namespace ECore{
