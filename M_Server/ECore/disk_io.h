//-----------------------------------------------------------------------------
// Auth: Lyp
// Data: 2003-1-2
//-----------------------------------------------------------------------------
#pragma once
namespace ECore {

//-----------------------------------------------------------------------------
// ��ϵͳ�ļ�������װ��һ�㣬���˼����ٶȣ�ʲô��Ҳû��
//-----------------------------------------------------------------------------
class ECORE_API DiskIO
{
public:
	//-------------------------------------------------------------------------
	// �򿪺͹ر��ļ�
	//-------------------------------------------------------------------------
	HANDLE	Create(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, 
					LPVOID lpSecurityAttributes, DWORD dwCreationDisposition, 
					DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
	HANDLE	Create(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, 
					LPVOID lpSecurityAttributes, DWORD dwCreationDisposition, 
					DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
	BOOL	Close(HANDLE hFileHandle);

	//-------------------------------------------------------------------------
	// �򿪺͹ر��ڴ�ӳ���ļ�
	//-------------------------------------------------------------------------
	HANDLE	CreateMapping(HANDLE hFileHandle, LPVOID lpAttrib, DWORD dwProtect,
					DWORD dwMaxSize, LPCWSTR lpName);
	HANDLE	CreateMapping(HANDLE hFileHandle, LPVOID lpAttrib, DWORD dwProtect,
					DWORD dwMaxSize, LPCSTR lpName);
	BOOL	CloseMappingHandle(HANDLE hFileMappingHandle);

	//-------------------------------------------------------------------------
	// ӳ���ȡ���ڴ�ӳ���ļ�
	//-------------------------------------------------------------------------
	LPVOID	MapFile(HANDLE hFileMappingHandle, DWORD dwDesireAccess, 
					DWORD dwFileOffset, DWORD dwNumberOfBytesToMap);
	LPVOID	MapFileEx(HANDLE hFileMappingHandle, DWORD dwDesireAccess, 
					DWORD dwFileOffset, DWORD dwNumberOfBytesToMap, LPVOID* ppMem=0);
	BOOL	UnMapFile(LPCVOID lpBaseAddress);

	//-------------------------------------------------------------------------
	// ��ȡ
	//-------------------------------------------------------------------------
	BOOL	Read(HANDLE hFileHandle, LPVOID lpBuffer, DWORD dwNumberOfBytesToRead,
					DWORD* lpNumberOfBytesRead, LPVOID lpOverlapped);
	DWORD	Read(HANDLE hFileHandle, LPVOID lpBuffer, DWORD dwNumberOfBytesToRead);

	//-------------------------------------------------------------------------
	// ֱ�Ӷ�ȡ
	//-------------------------------------------------------------------------
	DWORD	LoadToMem(LPVOID pMem, LPCWSTR pFileName);
	DWORD	LoadToMem(LPVOID pMem, LPCSTR pFileName);

	//-------------------------------------------------------------------------
	// д��
	//-------------------------------------------------------------------------
	BOOL	Write(HANDLE hFileHandle, LPCVOID lpBuffer, DWORD dwNumberOfBytesToWrite,
					DWORD* lpNumberOfBytesWritten, LPVOID lpOverlapped);
	BOOL	Write(HANDLE hFileHandle, LPCVOID lpBuffer, DWORD dwNumberOfBytesToWrite);

	//-------------------------------------------------------------------------
	// Ѱַ
	//-------------------------------------------------------------------------
	DWORD	Seek(HANDLE hFileHandle, DWORD dwDistanceToMove, DWORD dwMoveMethod);
	DWORD	Seek(HANDLE hFileHandle, DWORD dwOffsetFromStart);
	BOOL	SeekEx(HANDLE hFileHandle, INT64 n64DistanceToMove, DWORD dwMoveMethod);

	//-------------------------------------------------------------------------
	// �õ���С
	//-------------------------------------------------------------------------
	DWORD	GetSize(LPCWSTR pFileName);
	DWORD	GetSize(LPCSTR pFileName);
	DWORD	GetSize(HANDLE hFileHandle);
	BOOL	GetSizeEx(HANDLE hFileHandle, INT64* pn64Size);

	//-------------------------------------------------------------------------
	// �����ļ�β
	//-------------------------------------------------------------------------
	DWORD	SetEOF(HANDLE hFileHandle);

	//-------------------------------------------------------------------------
	// �õ��ļ�����޸�����
	//-------------------------------------------------------------------------
	BOOL	GetTime(LPCWSTR szFileName, LPFILETIME lpLastWriteTime);
	BOOL	GetTime(LPCSTR szFileName, LPFILETIME lpLastWriteTime);

	//-------------------------------------------------------------------------
	// Ŀ¼���
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
	// ���ܺ���
	//-------------------------------------------------------------------------
	INT64	GetDriveFreeSpace(LPCTSTR szDriver);
	INLINE static BOOL IsInvalidHandle(HANDLE hHandle)	{ return (INVALID_HANDLE == hHandle); }
	INLINE DWORD GetAllocGranularity() { return m_dwAllocGranularity; }


	DiskIO();

private:
	DWORD		m_dwAllocGranularity;	// mapӳ������
};

} // namespace ECore{
