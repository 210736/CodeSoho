//-----------------------------------------------------------------------------
// Time: 2003-1-2
// Auth: Lyp
//-----------------------------------------------------------------------------
#pragma once

#include "filter.h"
#include "disk_io.h"
#include "mem_cache.h"


// #define CHECK_OPEN_FILE
namespace ECore{
//-----------------------------------------------------------------------------
// �������
//-----------------------------------------------------------------------------
class ECORE_API EPK
{
public:
	//-------------------------------------------------------------------------
	// ���غ�ж�� ��
	//-------------------------------------------------------------------------
	BOOL Load(LPCWSTR szFileName);
	BOOL Load(LPCSTR szFileName);
	BOOL Unload();

	//-------------------------------------------------------------------------
	// ���е����ļ��Ĳ���
	//-------------------------------------------------------------------------
	INLINE HANDLE Open(LPCWSTR szFileName);
	INLINE HANDLE Open(LPCSTR szFileName);
	INLINE HANDLE OpenForBink(LPCWSTR szFileName);
	INLINE BOOL Close(HANDLE hHandle);
	INLINE DWORD Read(HANDLE hHandle, PVOID lpMemOut, DWORD dwSize);
	INLINE DWORD Seek(HANDLE hHandle, INT nOffset, DWORD dwOrigin);
	INLINE DWORD Tell(HANDLE hHandle);
	INLINE DWORD GetSize(HANDLE hHandle);
	

	//-------------------------------------------------------------------------
	// �������
	//-------------------------------------------------------------------------
	INLINE DWORD LoadFile(LPVOID lpMemOut, LPCWSTR szFileName); // ����ڴ�Ŀ��lpMemOutΪ�գ��ͽ�����ԭ�ļ�(δѹ��ʱ)��С
	INLINE DWORD LoadFile(LPVOID lpMemOut, LPCSTR szFileName); // ����ڴ�Ŀ��lpMemOutΪ�գ��ͽ�����ԭ�ļ�(δѹ��ʱ)��С
	INLINE BOOL GetTime(LPCWSTR szFileName, LPFILETIME pFileTime); // �õ��ļ�����޸�ʱ��
	INLINE BOOL GetTime(LPCSTR szFileName, LPFILETIME pFileTime); // �õ��ļ�����޸�ʱ��

	EPK();
	~EPK();


private:
	struct tagFile
	{
		PVOID			pMapAddress;	// Map��ʼλ��
		PVOID			pStartAddress;	// Map�е��ļ���ʼλ��
		PVOID			pMem;			// ָ���ļ�ʵ������
		DWORD			dwOffset;		// ʵ��Map��ʼ��ַ���ļ���ʼ��ַ��ƫ��
		DWORD			dwFileSize;		// �ļ�ʵ�����ݴ�С
		DWORD			dwPointer;		// �ļ�ָ��
		BOOL			bCompressed;	// �ļ��Ƿ�ѹ��
	};

	TObj<DiskIO>			m_pDiskIO;
	TObj<Filter>			m_pFilter;

	BOOL			m_bLoaded;
	HANDLE			m_hHandle;
	HANDLE			m_hMapHandle;

	wstring			m_strFileName;
	tagEPKHeader	m_Header;
	tagEPKTable*	m_pTable;
	
	DWORD			m_dwAllocGranularity;
	INT VOLATILE 	m_nOpenedFileNum;

#ifdef CHECK_OPEN_FILE
	std::map<HANDLE, tstring>	m_mapAllFiles;
#endif


	INLINE tagEPKTable* GetTable(LPCWSTR pFullName);
	INLINE tagEPKTable* GetTable(LPCSTR pFullName);
	INLINE HANDLE OpenForMap(tagEPKTable* pTable, LPCSTR);
	INLINE HANDLE OpenForCommon(tagEPKTable* pTable);
};



//-----------------------------------------------------------------------------
// �����е����ļ�������
//-----------------------------------------------------------------------------
DWORD EPK::Read(HANDLE hHandle, LPVOID lpMemOut, DWORD dwSize)
{
	tagFile* fp = (tagFile*)hHandle;
	if( fp->dwPointer >= fp->dwFileSize )	// �ļ�ָ���Ѿ����ļ�β
		return 0;

	if( fp->dwPointer + dwSize > fp->dwFileSize )
		dwSize = fp->dwFileSize -  fp->dwPointer;

	if( 1 == dwSize )	
	{
		*(BYTE*)lpMemOut = *(BYTE*)((BYTE*)fp->pMem + fp->dwPointer);
	}
	else if( 2 == dwSize )
	{
		*(WORD*)lpMemOut = *(WORD*)((BYTE*)fp->pMem + fp->dwPointer);
	}
	else if( 4 == dwSize )
	{
		*(DWORD*)lpMemOut = *(DWORD*)((BYTE*)fp->pMem + fp->dwPointer);
	}
	else if( 8 == dwSize )
	{
		*(INT64*)lpMemOut = *(INT64*)((BYTE*)fp->pMem + fp->dwPointer);
	}
	else if( 12 == dwSize )
	{
		*(INT64*)lpMemOut = *(INT64*)((BYTE*)fp->pMem + fp->dwPointer);
		*((DWORD*)lpMemOut + 2) = *((DWORD*)((BYTE*)fp->pMem + fp->dwPointer) + 2);
	}
	else if( 16 == dwSize )
	{
		*(INT64*)lpMemOut = *(INT64*)((BYTE*)fp->pMem + fp->dwPointer);
		*((INT64*)lpMemOut + 1) = *((INT64*)((BYTE*)fp->pMem + fp->dwPointer) + 1);
	}
	else if( 20 == dwSize )
	{
		*(INT64*)lpMemOut = *(INT64*)((BYTE*)fp->pMem + fp->dwPointer);
		*((INT64*)lpMemOut + 1) = *((INT64*)((BYTE*)fp->pMem + fp->dwPointer) + 1);
		*((DWORD*)lpMemOut + 4) = *((DWORD*)((BYTE*)fp->pMem + fp->dwPointer) + 4);
	}
	else if( 24 == dwSize )
	{
		*(INT64*)lpMemOut = *(INT64*)((BYTE*)fp->pMem + fp->dwPointer);
		*((INT64*)lpMemOut + 1) = *((INT64*)((BYTE*)fp->pMem + fp->dwPointer) + 1);
		*((INT64*)lpMemOut + 2) = *((INT64*)((BYTE*)fp->pMem + fp->dwPointer) + 2);
	}
	else if( 28 == dwSize )
	{
		*(INT64*)lpMemOut = *(INT64*)((BYTE*)fp->pMem + fp->dwPointer);
		*((INT64*)lpMemOut + 1) = *((INT64*)((BYTE*)fp->pMem + fp->dwPointer) + 1);
		*((INT64*)lpMemOut + 2) = *((INT64*)((BYTE*)fp->pMem + fp->dwPointer) + 2);
		*((DWORD*)lpMemOut + 6) = *((DWORD*)((BYTE*)fp->pMem + fp->dwPointer) + 6);
	}
	else if( 32 == dwSize )
	{
		*(INT64*)lpMemOut = *(INT64*)((BYTE*)fp->pMem + fp->dwPointer);
		*((INT64*)lpMemOut + 1) = *((INT64*)((BYTE*)fp->pMem + fp->dwPointer) + 1);
		*((INT64*)lpMemOut + 2) = *((INT64*)((BYTE*)fp->pMem + fp->dwPointer) + 2);
		*((INT64*)lpMemOut + 3) = *((INT64*)((BYTE*)fp->pMem + fp->dwPointer) + 3);
	}
	else
	{
		memcpy(lpMemOut, (BYTE*)fp->pMem + fp->dwPointer, dwSize);
	}

	fp->dwPointer += dwSize;

	return dwSize;	// ����ʵ��"�ɹ�"��ȡ���ֽ���
}



//-----------------------------------------------------------------------------
// ��λ���е����ļ���ָ��,���ص�ǰ�ļ�ָ��λ��
//-----------------------------------------------------------------------------
DWORD EPK::Seek(HANDLE hHandle, INT nOffset, DWORD dwOrigin)
{
	tagFile* fp = (tagFile*)hHandle;
	switch( dwOrigin )
	{
	case SEEK_SET:
		fp->dwPointer = nOffset;
		break;
	case SEEK_CUR:
		fp->dwPointer += nOffset;
		break;
	case SEEK_END:
		fp->dwPointer = fp->dwFileSize + nOffset;
		break;
	default:
		ASSERT(0);
		return (DWORD)INVALID;		
	}

	return fp->dwPointer;
}



//-----------------------------------------------------------------------------
// ���ص�ǰ�ļ�ָ��λ��
//-----------------------------------------------------------------------------
DWORD EPK::Tell(HANDLE hHandle)
{ 
	return ((tagFile*)hHandle)->dwPointer; 
}


//-----------------------------------------------------------------------------
// �õ��ļ���С
//-----------------------------------------------------------------------------
DWORD EPK::GetSize(HANDLE hHandle)
{
	return ((tagFile*)hHandle)->dwFileSize; 
}



//-----------------------------------------------------------------------------
// �򿪰��еĵ����ļ�
//-----------------------------------------------------------------------------
HANDLE EPK::Open(LPCWSTR szFileName)
{
	if( !m_bLoaded )	// û���κΰ�����
		return INVALID_HANDLE;

	// �ҵ��ļ���Ӧ��EPK�еı���
	tagEPKTable* pTable = this->GetTable(szFileName);
	if( !VALID(pTable) )
		return INVALID_HANDLE;

	HANDLE hHandle = 0;
	if( m_hMapHandle != INVALID_HANDLE )
		hHandle = OpenForMap(pTable, 0);

	if( !hHandle )
		hHandle = OpenForCommon(pTable);

	if( !hHandle )
	{
		ERR(_T("Could not open file %s in epk:%s"), 
			szFileName, m_strFileName.c_str());
	}

#ifdef CHECK_OPEN_FILE
	m_mapAllFiles.insert(make_pair<HANDLE, tstring>(hHandle, szFileName));
#endif

	return hHandle;
}


//-----------------------------------------------------------------------------
// �򿪰��еĵ����ļ�
//-----------------------------------------------------------------------------
HANDLE EPK::Open(LPCSTR szFileName)
{
	if( !m_bLoaded )	// û���κΰ�����
		return INVALID_HANDLE;

	// �ҵ��ļ���Ӧ��EPK�еı���
	tagEPKTable* pTable = this->GetTable(szFileName);
	if( !VALID(pTable) )
		return INVALID_HANDLE;

	HANDLE hHandle = 0;
	if( m_hMapHandle != INVALID_HANDLE )
		hHandle = OpenForMap(pTable, szFileName);

	if( !hHandle )
		hHandle = OpenForCommon(pTable);

	if( !hHandle )
	{
		ERR(_T("Could not open file %s in epk:%s"), 
			SS::To16(szFileName), m_strFileName.c_str());
	}

	return hHandle;
}



//-----------------------------------------------------------------------------
// Ϊbink����
//-----------------------------------------------------------------------------
HANDLE EPK::OpenForBink(LPCWSTR szFileName)
{
	if( !m_bLoaded )	// û���κΰ�����
		return INVALID_HANDLE;

	// �ҵ��ļ���Ӧ��EPK�еı���
	tagEPKTable* pTable = this->GetTable(szFileName);
	if( !VALID(pTable) )
		return INVALID_HANDLE;

	if( !m_pDiskIO->Seek(m_hHandle, pTable->dwStartPos) )
	{
		return INVALID_HANDLE;
	}

	return m_hHandle;
}



//-----------------------------------------------------------------------------
// ���ļ�ӳ�䷽ʽ�򿪵����ļ�
//-----------------------------------------------------------------------------
HANDLE EPK::OpenForMap(tagEPKTable* pTable, LPCSTR szName)
{
	DWORD dwRealStartPos = pTable->dwStartPos;
	dwRealStartPos = dwRealStartPos - (dwRealStartPos % m_dwAllocGranularity);
	DWORD dwOffset = pTable->dwStartPos - dwRealStartPos;

	// ʵ����Ҫӳ����ֽ�
	DWORD dwBytesToRead = pTable->dwPackedSize + pTable->wNameSize + dwOffset;

	tagFile* fp = NULL;
	if( IS_COMPRESSED(pTable->wFlag) && pTable->dwOriginSize > 0 )
	{
		fp = (tagFile*)MCALLOC(sizeof(tagFile) + pTable->dwOriginSize + sizeof(DWORD));
		if( !fp )
			return NULL;

		fp->pMem = (BYTE*)fp + sizeof(tagFile);
	}
	else
	{
		fp = (tagFile*)MCALLOC(sizeof(tagFile));
		if( !fp )
			return NULL;
	}

	// й©�������
	// SetMemoryTraceDesc(fp, szName);

	fp->pMapAddress = m_pDiskIO->MapFile(m_hMapHandle, FILE_MAP_READ, dwRealStartPos, dwBytesToRead);
	if( !fp->pMapAddress )
	{
		MCFREE(fp);
		return NULL;
	}

	fp->pStartAddress = (LPVOID)((BYTE*)fp->pMapAddress + dwOffset);
	fp->bCompressed = IS_COMPRESSED(pTable->wFlag);
	fp->dwFileSize = pTable->dwOriginSize;
	fp->dwOffset = dwOffset;
	fp->dwPointer = 0;	// �ļ�ָ���ʼ

	ASSERT( fp->dwFileSize<0x40000000 );

	// �����ѹ���ļ�������Ҫ�Ƚ�ѹ��
	if( fp->bCompressed && fp->dwFileSize > 0 )
	{
		// й©�������
		// SetMemoryTraceDesc(fp->pMem, szName);

		// �����ˣ���ѹ���ļ�
		tagFiltParam param;
		param.bCompress = FALSE;
		param.dwInBuffer = pTable->dwPackedSize;
		param.stInData = pTable->dwPackedSize;
		param.dwOutBuffer = pTable->dwOriginSize;
		param.stOutData = pTable->dwOriginSize;
		param.eFilter = GET_FILTER(pTable->wFlag);
		param.pMemIn = fp->pStartAddress;
		param.pMemOut = fp->pMem;

		// ���й����ļ���Ϣ
		m_pFilter->Filt(&param);
	}
	else	// ����ļ�û�б�ѹ����
	{
		// ����ָ��Map�е��ļ�ʵ����ʼ��ַ
		fp->pMem = fp->pStartAddress;
	}

	InterlockedIncrementEx((LPLONG)&m_nOpenedFileNum);
	return fp;
}



//-----------------------------------------------------------------------------
// ��ͨ��ʽ�򿪵����ļ�
//-----------------------------------------------------------------------------
HANDLE EPK::OpenForCommon(tagEPKTable* pTable)
{
	// �����ļ�����
	tagFile* fp = NULL;
	if( IS_COMPRESSED( pTable->wFlag ) && pTable->dwOriginSize > 0 )
	{
		fp = (tagFile*)MCALLOC(sizeof(tagFile) + fp->dwFileSize + sizeof(DWORD));
		if( !fp )
			return NULL;
		fp->pMem = (BYTE*)fp + sizeof(tagFile);
	}
	else
	{
		fp = (tagFile*)MCALLOC(sizeof(tagFile));
		if( !fp )
			return NULL;
	}

	fp->pMapAddress = 0;
	fp->pStartAddress = 0;
	fp->bCompressed = IS_COMPRESSED( pTable->wFlag );
	fp->dwFileSize = pTable->dwOriginSize;
	fp->dwOffset = 0;
	fp->dwPointer = 0;

	ASSERT( fp->dwFileSize<0x40000000 );

	// �ȶ���
	LPVOID pPackMem = MCALLOC(pTable->dwPackedSize);
	if( !pPackMem )
	{
		MCFREE(fp);
		return NULL;
	}

	if( !m_pDiskIO->Seek(m_hHandle, pTable->dwStartPos) )
	{
		MCFREE(pPackMem);
		MCFREE(fp);
		return NULL;
	}

	if( !m_pDiskIO->Read(m_hHandle, pPackMem, pTable->dwPackedSize) )
	{
		MCFREE(pPackMem);
		MCFREE(fp);
		return NULL;
	}

	// �����ѹ���ļ�������Ҫ�Ƚ�ѹ��
	if( fp->bCompressed == TRUE && fp->dwFileSize > 0 )
	{
		// �����ˣ���ѹ���ļ�
		tagFiltParam param;
		param.bCompress = FALSE;
		param.dwInBuffer = pTable->dwPackedSize;
		param.stInData = pTable->dwPackedSize;
		param.dwOutBuffer = pTable->dwOriginSize;
		param.stOutData = pTable->dwOriginSize;
		param.eFilter = GET_FILTER(pTable->wFlag);
		param.pMemIn = pPackMem;
		param.pMemOut = fp->pMem;

		// ���й����ļ���Ϣ
		m_pFilter->Filt(&param);

		// �ͷų�ʼѹ���ļ�����֮�ڴ�
		MCFREE(pPackMem);
	}
	else	// ����ļ�û�б�ѹ����
	{
		// ����ָ���ʼ�ļ����ݴ洢�ĵ�ַ
		fp->pMem = pPackMem;
	}

	InterlockedIncrementEx((LPLONG)&m_nOpenedFileNum);
	return fp;
}



//-----------------------------------------------------------------------------
// �رհ��еĵ����ļ�, ����ȡ���ļ�ӳ�䣬�������ļ����
//-----------------------------------------------------------------------------
BOOL EPK::Close(HANDLE hHandle)
{
	tagFile* fp = (tagFile*)hHandle;

#ifdef CHECK_OPEN_FILE
	m_mapAllFiles.erase(hHandle);
#endif

	if( !fp->pMapAddress )	// ����ͨ�ļ���ʽ��ȡ
	{
		if( fp->pMem != (BYTE*)fp + sizeof(tagFile) )	
		{
			MCFREE(fp->pMem);	
		}
	}
	else // �ļ�ӳ�䷽ʽ��ȡ
	{
		if( !m_pDiskIO->UnMapFile(fp->pMapAddress) )
		{
			ERR(_T("could not unmap file in epk close"));
			return FALSE;
		}
	}

	MCFREE(fp);
	InterlockedDecrementEx(&m_nOpenedFileNum);
	return TRUE;
}


//-----------------------------------------------------------------------------
// �����е����ļ���ȫ�����ݣ�������Ŀ���ڴ��С������ԭ�ļ��ֽڴ�С
//-----------------------------------------------------------------------------
DWORD EPK::LoadFile(LPVOID lpMemOut, LPCWSTR szFileName)
{
	if( !m_bLoaded )
	{
		ERR(_T("No package loaded"));
		return (DWORD)INVALID;
	}

	// �ҵ��ļ���Ӧ��EPK�еı���
	tagEPKTable* pTable = this->GetTable(szFileName);
	if( !VALID(pTable) )
	{
		//if( lpMemOut )	// ���ⲿֻ����ȡ���ļ���С��������������ʾ
		//	ERR(_T("could not find file %s in epk"), szFileName);

		return (DWORD)INVALID;
	}

	// ����ڴ�Ŀ��Ϊ�գ��ͽ�����ԭ�ļ���С
	if( lpMemOut == NULL )
		return pTable->dwOriginSize;

	BOOL bCompressed = IS_COMPRESSED( pTable->wFlag );

	// �ȳ����Ƿ����ӳ��
	if( m_hMapHandle != INVALID_HANDLE )
	{
		DWORD dwRealStartPos = pTable->dwStartPos;
		dwRealStartPos = dwRealStartPos - (dwRealStartPos % m_dwAllocGranularity);
		DWORD dwOffset = pTable->dwStartPos - dwRealStartPos;
		DWORD dwBytesToRead = pTable->dwPackedSize	+ pTable->wNameSize + dwOffset;
		PVOID pMapAddress = m_pDiskIO->MapFile(m_hMapHandle, FILE_MAP_READ, dwRealStartPos, dwBytesToRead);

		if( !pMapAddress )	// �޷�ӳ�䣬ʹ����ͨ���̲���
			goto __disk_io;

		// �����ˣ���ѹ���ļ�
		tagFiltParam param;
		param.bCompress = FALSE;
		param.dwInBuffer = pTable->dwPackedSize;
		param.stInData = pTable->dwPackedSize;
		param.dwOutBuffer = pTable->dwOriginSize;
		param.stOutData = pTable->dwOriginSize;
		param.eFilter = GET_FILTER( pTable->wFlag );
		param.pMemIn = (BYTE*)pMapAddress + dwOffset;
		param.pMemOut = lpMemOut;

		// �����ļ���Ϣ
		m_pFilter->Filt(&param);

		// ���ӳ��
		if( !m_pDiskIO->UnMapFile(pMapAddress) )
		{
			ERR(_T("ummap failed in epk loadfile"));
			return (DWORD)INVALID;
		}
	}
	else
	{

__disk_io:
		if( !m_pDiskIO->Seek(m_hHandle, pTable->dwStartPos) )
		{
			ERR("could not read epk file");
			return (DWORD)INVALID;
		}

		// �����ѹ���ļ�������Ҫ�Ƚ�ѹ��
		if( bCompressed && pTable->dwOriginSize > 0 )
		{
			// Ϊ��ѹ���������ļ����������ڴ�
			PVOID pPackMem = MCALLOC(pTable->dwPackedSize);
			if( !pPackMem )
				return (DWORD)INVALID;

			if( !m_pDiskIO->Read(m_hHandle, pPackMem, pTable->dwPackedSize) )
			{
				ERR("could not read epk file");
				MCFREE(pPackMem);
				return (DWORD)INVALID;
			}

			// �����ˣ���ѹ���ļ�
			tagFiltParam param;
			param.bCompress = FALSE;
			param.dwInBuffer = pTable->dwPackedSize;
			param.stInData = pTable->dwPackedSize;
			param.dwOutBuffer = pTable->dwOriginSize;
			param.stOutData = pTable->dwOriginSize;
			param.eFilter = GET_FILTER(pTable->wFlag);
			param.pMemIn = pPackMem;
			param.pMemOut = lpMemOut;

			// ���й����ļ���Ϣ
			m_pFilter->Filt(&param);

			//�ͷų�ʼѹ���ļ�����֮�ڴ�
			MCFREE(pPackMem);
		}
		else	// ����ļ�û�б�ѹ����
		{
			if( !m_pDiskIO->Read(m_hHandle, lpMemOut, pTable->dwPackedSize) )
			{
				ERR("could not read epk file");
				return (DWORD)INVALID;
			}
		}

	}
	return pTable->dwOriginSize;
}


//-----------------------------------------------------------------------------
// �����е����ļ���ȫ�����ݣ�������Ŀ���ڴ��С������ԭ�ļ��ֽڴ�С
//-----------------------------------------------------------------------------
DWORD EPK::LoadFile(LPVOID lpMemOut, LPCSTR szFileName)
{
	if( !m_bLoaded )
	{
		ERR("No package loaded");
		return (DWORD)INVALID;
	}

	// �ҵ��ļ���Ӧ��EPK�еı���
	tagEPKTable* pTable = this->GetTable(szFileName);
	if( !VALID(pTable) )
	{
		// if( lpMemOut )	// ���ⲿֻ����ȡ���ļ���С��������������ʾ
		//	ERR("could not find file %s in epk", szFileName);

		return (DWORD)INVALID;
	}

	// ����ڴ�Ŀ��Ϊ�գ��ͽ�����ԭ�ļ���С
	if( lpMemOut == NULL )
		return pTable->dwOriginSize;

	BOOL bCompressed = IS_COMPRESSED( pTable->wFlag );

	// �ȳ����Ƿ����ӳ��
	if( m_hMapHandle != INVALID_HANDLE )
	{
		DWORD dwRealStartPos = pTable->dwStartPos;
		dwRealStartPos = dwRealStartPos - (dwRealStartPos % m_dwAllocGranularity);
		DWORD dwOffset = pTable->dwStartPos - dwRealStartPos;
		DWORD dwBytesToRead = pTable->dwPackedSize	+ pTable->wNameSize + dwOffset;
		PVOID pMapAddress = m_pDiskIO->MapFile(m_hMapHandle, FILE_MAP_READ, dwRealStartPos, dwBytesToRead);

		if( !pMapAddress )	// �޷�ӳ�䣬ʹ����ͨ���̲���
			goto __disk_io2;

		// �����ˣ���ѹ���ļ�
		tagFiltParam param;
		param.bCompress = FALSE;
		param.dwInBuffer = pTable->dwPackedSize;
		param.stInData = pTable->dwPackedSize;
		param.dwOutBuffer = pTable->dwOriginSize;
		param.stOutData = pTable->dwOriginSize;
		param.eFilter = GET_FILTER( pTable->wFlag );
		param.pMemIn = (BYTE*)pMapAddress + dwOffset;
		param.pMemOut = lpMemOut;

		// �����ļ���Ϣ
		m_pFilter->Filt(&param);

		// ���ӳ��
		if( !m_pDiskIO->UnMapFile(pMapAddress) )
		{
			ERR("ummap failed in epk loadfile");
			return (DWORD)INVALID;
		}
	}
	else
	{

__disk_io2:
		if( !m_pDiskIO->Seek(m_hHandle, pTable->dwStartPos) )
		{
			ERR("could not read epk file");
			return (DWORD)INVALID;
		}

		// �����ѹ���ļ�������Ҫ�Ƚ�ѹ��
		if( bCompressed && pTable->dwOriginSize > 0 )
		{
			// Ϊ��ѹ���������ļ����������ڴ�
			PVOID pPackMem = MCALLOC(pTable->dwPackedSize);
			if( !pPackMem )
				return (DWORD)INVALID;

			if( !m_pDiskIO->Read(m_hHandle, pPackMem, pTable->dwPackedSize) )
			{
				ERR("could not read epk file");
				MCFREE(pPackMem);
				return (DWORD)INVALID;
			}

			// �����ˣ���ѹ���ļ�
			tagFiltParam param;
			param.bCompress = FALSE;
			param.dwInBuffer = pTable->dwPackedSize;
			param.stInData = pTable->dwPackedSize;
			param.dwOutBuffer = pTable->dwOriginSize;
			param.stOutData = pTable->dwOriginSize;
			param.eFilter = GET_FILTER(pTable->wFlag);
			param.pMemIn = pPackMem;
			param.pMemOut = lpMemOut;

			// ���й����ļ���Ϣ
			m_pFilter->Filt(&param);

			//�ͷų�ʼѹ���ļ�����֮�ڴ�
			MCFREE(pPackMem);
		}
		else	// ����ļ�û�б�ѹ����
		{
			if( !m_pDiskIO->Read(m_hHandle, lpMemOut, pTable->dwPackedSize) )
			{
				ERR("could not read epk file");
				return (DWORD)INVALID;
			}
		}

	}
	return pTable->dwOriginSize;
}


//-----------------------------------------------------------------------------
// �õ��ļ�����޸�ʱ��
//-----------------------------------------------------------------------------
BOOL EPK::GetTime(LPCWSTR szFileName, LPFILETIME pFileTime)
{
	// �ҵ��ļ���Ӧ��EPK�еı���
	tagEPKTable* pTable = this->GetTable(szFileName);
	if( !VALID(pTable) )
	{
		// ERR(L"could not find file %s in epk", szFileName);
		return FALSE;
	}

	pFileTime->dwLowDateTime = pTable->LastWriteTime.dwLowDateTime;
	pFileTime->dwHighDateTime = pTable->LastWriteTime.dwHighDateTime;
	return TRUE;
}


//-----------------------------------------------------------------------------
// �õ��ļ�����޸�ʱ��
//-----------------------------------------------------------------------------
BOOL EPK::GetTime(LPCSTR szFileName, LPFILETIME pFileTime)
{
	// �ҵ��ļ���Ӧ��EPK�еı���
	tagEPKTable* pTable = this->GetTable(szFileName);
	if( !VALID(pTable) )
	{
		// ERR("could not find file %s in epk", szFileName);
		return FALSE;
	}

	pFileTime->dwLowDateTime = pTable->LastWriteTime.dwLowDateTime;
	pFileTime->dwHighDateTime = pTable->LastWriteTime.dwHighDateTime;
	return TRUE;
}


//-----------------------------------------------------------------------------
// �����ļ����ҵ���Ӧ�ı���
//-----------------------------------------------------------------------------
tagEPKTable* EPK::GetTable(LPCWSTR pFullName)
{
	CHAR szFullNameAnsi[MAX_PATH];
	INT nIndex = 0;
	for(LPCWSTR p = pFullName; *p != 0; ++p, ++nIndex)
		szFullNameAnsi[nIndex] = (CHAR)(*p);
	szFullNameAnsi[nIndex] = 0;	// ����ת��ֻ��ʶ��Ӣ��

	DWORD dwCRC = Crc32Low(szFullNameAnsi);

	// ��һ����ͬ��CRC����Ч��CRC�϶������
	INT nStart = 0, nEnd = m_Header.dwValidTableNum, nPos = 0;

	while( nEnd != nStart )	// ���ֲ���
	{
		nPos = nStart + (nEnd-nStart)/2;
		if( dwCRC == m_pTable[nPos].dwCRC && IS_FILE_OR_DIR_VALID(m_pTable[nPos].wFlag) ) 
			return &m_pTable[nPos];

		if( nEnd == nStart+1 )
			return (tagEPKTable*)INVALID;

		if( dwCRC >=  m_pTable[nPos].dwCRC )
			nStart = nPos;
		else
			nEnd = nPos;
	}

	return (tagEPKTable*)INVALID;
}


//-----------------------------------------------------------------------------
// �����ļ����ҵ���Ӧ�ı���
//-----------------------------------------------------------------------------
tagEPKTable* EPK::GetTable(LPCSTR pFullName)
{
	DWORD dwCRC = Crc32Low(pFullName);

	// ��һ����ͬ��CRC����Ч��CRC�϶������
	INT nStart = 0, nEnd = m_Header.dwValidTableNum, nPos = 0;

	while( nEnd != nStart )	// ���ֲ���
	{
		nPos = nStart + (nEnd-nStart)/2;
		if( dwCRC == m_pTable[nPos].dwCRC && IS_FILE_OR_DIR_VALID(m_pTable[nPos].wFlag) ) 
			return &m_pTable[nPos];

		if( nEnd == nStart+1 )
			return (tagEPKTable*)INVALID;

		if( dwCRC >=  m_pTable[nPos].dwCRC )
			nStart = nPos;
		else
			nEnd = nPos;
	}

	return (tagEPKTable*)INVALID;
}


} // namespace ECore {
