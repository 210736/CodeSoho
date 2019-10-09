//-----------------------------------------------------------------------------
// filename: shared_memory.h
// author: Sxg
// actor:
// data: 2009-12-21
// last:
// brief: �����ڴ��װ -- ͨ����װFileMappingʵ��
//-----------------------------------------------------------------------------
#pragma once

namespace ECore{

//-----------------------------------------------------------------------------
// �����ڴ�FileMapping
//-----------------------------------------------------------------------------
class ECORE_API SharedMem
{
public:
	//-------------------------------------------------------------------------
	// ���������
	//-------------------------------------------------------------------------
	SharedMem(LPCTSTR szSharedMemName, INT64 n64SharedMemSize);
	~SharedMem();

	//-------------------------------------------------------------------------
	// �����ڴ����
	//-------------------------------------------------------------------------
	BOOL		Create();
	BOOL		Open();
	VOID		Close();

	//-------------------------------------------------------------------------
	// ���ݻ�ȡ�ӿ�
	//-------------------------------------------------------------------------
	INLINE LPCTSTR		GetMapFileName() const		{ return m_szSharedMemName; }
	INLINE INT64		GetMapFileSize() const		{ return m_n64SharedMemSize; }
	INLINE UINT			GetLastError() const		{ return m_dwLastError; }

	LPBYTE		GetData();

private:
	TCHAR		m_szSharedMemName[MAX_PATH];		// �����ڴ�����
	INT64		m_n64SharedMemSize;					// �����ڴ��С

	HANDLE		m_hHandle;							// �����ڴ���
	LPBYTE		m_pData;							// �����ڴ�ͷָ��
	DWORD		m_dwLastError;						// ������
};

//-----------------------------------------------------------------------------
// �õ������ڴ�ͷָ��
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