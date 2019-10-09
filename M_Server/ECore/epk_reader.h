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
// 精简读包
//-----------------------------------------------------------------------------
class ECORE_API EPK
{
public:
	//-------------------------------------------------------------------------
	// 加载和卸载 包
	//-------------------------------------------------------------------------
	BOOL Load(LPCWSTR szFileName);
	BOOL Load(LPCSTR szFileName);
	BOOL Unload();

	//-------------------------------------------------------------------------
	// 包中单个文件的操作
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
	// 特殊操作
	//-------------------------------------------------------------------------
	INLINE DWORD LoadFile(LPVOID lpMemOut, LPCWSTR szFileName); // 如果内存目标lpMemOut为空，就仅返回原文件(未压缩时)大小
	INLINE DWORD LoadFile(LPVOID lpMemOut, LPCSTR szFileName); // 如果内存目标lpMemOut为空，就仅返回原文件(未压缩时)大小
	INLINE BOOL GetTime(LPCWSTR szFileName, LPFILETIME pFileTime); // 得到文件最后修改时间
	INLINE BOOL GetTime(LPCSTR szFileName, LPFILETIME pFileTime); // 得到文件最后修改时间

	EPK();
	~EPK();


private:
	struct tagFile
	{
		PVOID			pMapAddress;	// Map开始位置
		PVOID			pStartAddress;	// Map中的文件开始位置
		PVOID			pMem;			// 指向文件实际内容
		DWORD			dwOffset;		// 实际Map起始地址与文件开始地址的偏差
		DWORD			dwFileSize;		// 文件实际内容大小
		DWORD			dwPointer;		// 文件指针
		BOOL			bCompressed;	// 文件是否被压缩
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
// 读包中单个文件的内容
//-----------------------------------------------------------------------------
DWORD EPK::Read(HANDLE hHandle, LPVOID lpMemOut, DWORD dwSize)
{
	tagFile* fp = (tagFile*)hHandle;
	if( fp->dwPointer >= fp->dwFileSize )	// 文件指针已经到文件尾
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

	return dwSize;	// 返回实际"成功"读取的字节数
}



//-----------------------------------------------------------------------------
// 定位包中单个文件的指针,返回当前文件指针位置
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
// 返回当前文件指针位置
//-----------------------------------------------------------------------------
DWORD EPK::Tell(HANDLE hHandle)
{ 
	return ((tagFile*)hHandle)->dwPointer; 
}


//-----------------------------------------------------------------------------
// 得到文件大小
//-----------------------------------------------------------------------------
DWORD EPK::GetSize(HANDLE hHandle)
{
	return ((tagFile*)hHandle)->dwFileSize; 
}



//-----------------------------------------------------------------------------
// 打开包中的单个文件
//-----------------------------------------------------------------------------
HANDLE EPK::Open(LPCWSTR szFileName)
{
	if( !m_bLoaded )	// 没有任何包载入
		return INVALID_HANDLE;

	// 找到文件对应的EPK中的表项
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
// 打开包中的单个文件
//-----------------------------------------------------------------------------
HANDLE EPK::Open(LPCSTR szFileName)
{
	if( !m_bLoaded )	// 没有任何包载入
		return INVALID_HANDLE;

	// 找到文件对应的EPK中的表项
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
// 为bink开包
//-----------------------------------------------------------------------------
HANDLE EPK::OpenForBink(LPCWSTR szFileName)
{
	if( !m_bLoaded )	// 没有任何包载入
		return INVALID_HANDLE;

	// 找到文件对应的EPK中的表项
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
// 以文件映射方式打开单个文件
//-----------------------------------------------------------------------------
HANDLE EPK::OpenForMap(tagEPKTable* pTable, LPCSTR szName)
{
	DWORD dwRealStartPos = pTable->dwStartPos;
	dwRealStartPos = dwRealStartPos - (dwRealStartPos % m_dwAllocGranularity);
	DWORD dwOffset = pTable->dwStartPos - dwRealStartPos;

	// 实际需要映射的字节
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

	// 泄漏检测助手
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
	fp->dwPointer = 0;	// 文件指针初始

	ASSERT( fp->dwFileSize<0x40000000 );

	// 如果是压缩文件，则需要先解压缩
	if( fp->bCompressed && fp->dwFileSize > 0 )
	{
		// 泄漏检测助手
		// SetMemoryTraceDesc(fp->pMem, szName);

		// 反过滤（解压）文件
		tagFiltParam param;
		param.bCompress = FALSE;
		param.dwInBuffer = pTable->dwPackedSize;
		param.stInData = pTable->dwPackedSize;
		param.dwOutBuffer = pTable->dwOriginSize;
		param.stOutData = pTable->dwOriginSize;
		param.eFilter = GET_FILTER(pTable->wFlag);
		param.pMemIn = fp->pStartAddress;
		param.pMemOut = fp->pMem;

		// 进行过滤文件信息
		m_pFilter->Filt(&param);
	}
	else	// 如果文件没有被压缩过
	{
		// 将其指向Map中的文件实际起始地址
		fp->pMem = fp->pStartAddress;
	}

	InterlockedIncrementEx((LPLONG)&m_nOpenedFileNum);
	return fp;
}



//-----------------------------------------------------------------------------
// 普通方式打开单个文件
//-----------------------------------------------------------------------------
HANDLE EPK::OpenForCommon(tagEPKTable* pTable)
{
	// 创建文件对象
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

	// 先读入
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

	// 如果是压缩文件，则需要先解压缩
	if( fp->bCompressed == TRUE && fp->dwFileSize > 0 )
	{
		// 反过滤（解压）文件
		tagFiltParam param;
		param.bCompress = FALSE;
		param.dwInBuffer = pTable->dwPackedSize;
		param.stInData = pTable->dwPackedSize;
		param.dwOutBuffer = pTable->dwOriginSize;
		param.stOutData = pTable->dwOriginSize;
		param.eFilter = GET_FILTER(pTable->wFlag);
		param.pMemIn = pPackMem;
		param.pMemOut = fp->pMem;

		// 进行过滤文件信息
		m_pFilter->Filt(&param);

		// 释放初始压缩文件内容之内存
		MCFREE(pPackMem);
	}
	else	// 如果文件没有被压缩过
	{
		// 将其指向初始文件内容存储的地址
		fp->pMem = pPackMem;
	}

	InterlockedIncrementEx((LPLONG)&m_nOpenedFileNum);
	return fp;
}



//-----------------------------------------------------------------------------
// 关闭包中的单个文件, 并且取消文件映射，参数是文件序号
//-----------------------------------------------------------------------------
BOOL EPK::Close(HANDLE hHandle)
{
	tagFile* fp = (tagFile*)hHandle;

#ifdef CHECK_OPEN_FILE
	m_mapAllFiles.erase(hHandle);
#endif

	if( !fp->pMapAddress )	// 以普通文件方式读取
	{
		if( fp->pMem != (BYTE*)fp + sizeof(tagFile) )	
		{
			MCFREE(fp->pMem);	
		}
	}
	else // 文件映射方式读取
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
// 读包中单个文件的全部内容，不会检查目标内存大小，返回原文件字节大小
//-----------------------------------------------------------------------------
DWORD EPK::LoadFile(LPVOID lpMemOut, LPCWSTR szFileName)
{
	if( !m_bLoaded )
	{
		ERR(_T("No package loaded"));
		return (DWORD)INVALID;
	}

	// 找到文件对应的EPK中的表项
	tagEPKTable* pTable = this->GetTable(szFileName);
	if( !VALID(pTable) )
	{
		//if( lpMemOut )	// 若外部只是想取得文件大小，不产生明显提示
		//	ERR(_T("could not find file %s in epk"), szFileName);

		return (DWORD)INVALID;
	}

	// 如果内存目标为空，就仅返回原文件大小
	if( lpMemOut == NULL )
		return pTable->dwOriginSize;

	BOOL bCompressed = IS_COMPRESSED( pTable->wFlag );

	// 先尝试是否可以映射
	if( m_hMapHandle != INVALID_HANDLE )
	{
		DWORD dwRealStartPos = pTable->dwStartPos;
		dwRealStartPos = dwRealStartPos - (dwRealStartPos % m_dwAllocGranularity);
		DWORD dwOffset = pTable->dwStartPos - dwRealStartPos;
		DWORD dwBytesToRead = pTable->dwPackedSize	+ pTable->wNameSize + dwOffset;
		PVOID pMapAddress = m_pDiskIO->MapFile(m_hMapHandle, FILE_MAP_READ, dwRealStartPos, dwBytesToRead);

		if( !pMapAddress )	// 无法映射，使用普通磁盘操作
			goto __disk_io;

		// 反过滤（解压）文件
		tagFiltParam param;
		param.bCompress = FALSE;
		param.dwInBuffer = pTable->dwPackedSize;
		param.stInData = pTable->dwPackedSize;
		param.dwOutBuffer = pTable->dwOriginSize;
		param.stOutData = pTable->dwOriginSize;
		param.eFilter = GET_FILTER( pTable->wFlag );
		param.pMemIn = (BYTE*)pMapAddress + dwOffset;
		param.pMemOut = lpMemOut;

		// 过滤文件信息
		m_pFilter->Filt(&param);

		// 解除映射
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

		// 如果是压缩文件，则需要先解压缩
		if( bCompressed && pTable->dwOriginSize > 0 )
		{
			// 为解压后真正的文件内容申请内存
			PVOID pPackMem = MCALLOC(pTable->dwPackedSize);
			if( !pPackMem )
				return (DWORD)INVALID;

			if( !m_pDiskIO->Read(m_hHandle, pPackMem, pTable->dwPackedSize) )
			{
				ERR("could not read epk file");
				MCFREE(pPackMem);
				return (DWORD)INVALID;
			}

			// 反过滤（解压）文件
			tagFiltParam param;
			param.bCompress = FALSE;
			param.dwInBuffer = pTable->dwPackedSize;
			param.stInData = pTable->dwPackedSize;
			param.dwOutBuffer = pTable->dwOriginSize;
			param.stOutData = pTable->dwOriginSize;
			param.eFilter = GET_FILTER(pTable->wFlag);
			param.pMemIn = pPackMem;
			param.pMemOut = lpMemOut;

			// 进行过滤文件信息
			m_pFilter->Filt(&param);

			//释放初始压缩文件内容之内存
			MCFREE(pPackMem);
		}
		else	// 如果文件没有被压缩过
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
// 读包中单个文件的全部内容，不会检查目标内存大小，返回原文件字节大小
//-----------------------------------------------------------------------------
DWORD EPK::LoadFile(LPVOID lpMemOut, LPCSTR szFileName)
{
	if( !m_bLoaded )
	{
		ERR("No package loaded");
		return (DWORD)INVALID;
	}

	// 找到文件对应的EPK中的表项
	tagEPKTable* pTable = this->GetTable(szFileName);
	if( !VALID(pTable) )
	{
		// if( lpMemOut )	// 若外部只是想取得文件大小，不产生明显提示
		//	ERR("could not find file %s in epk", szFileName);

		return (DWORD)INVALID;
	}

	// 如果内存目标为空，就仅返回原文件大小
	if( lpMemOut == NULL )
		return pTable->dwOriginSize;

	BOOL bCompressed = IS_COMPRESSED( pTable->wFlag );

	// 先尝试是否可以映射
	if( m_hMapHandle != INVALID_HANDLE )
	{
		DWORD dwRealStartPos = pTable->dwStartPos;
		dwRealStartPos = dwRealStartPos - (dwRealStartPos % m_dwAllocGranularity);
		DWORD dwOffset = pTable->dwStartPos - dwRealStartPos;
		DWORD dwBytesToRead = pTable->dwPackedSize	+ pTable->wNameSize + dwOffset;
		PVOID pMapAddress = m_pDiskIO->MapFile(m_hMapHandle, FILE_MAP_READ, dwRealStartPos, dwBytesToRead);

		if( !pMapAddress )	// 无法映射，使用普通磁盘操作
			goto __disk_io2;

		// 反过滤（解压）文件
		tagFiltParam param;
		param.bCompress = FALSE;
		param.dwInBuffer = pTable->dwPackedSize;
		param.stInData = pTable->dwPackedSize;
		param.dwOutBuffer = pTable->dwOriginSize;
		param.stOutData = pTable->dwOriginSize;
		param.eFilter = GET_FILTER( pTable->wFlag );
		param.pMemIn = (BYTE*)pMapAddress + dwOffset;
		param.pMemOut = lpMemOut;

		// 过滤文件信息
		m_pFilter->Filt(&param);

		// 解除映射
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

		// 如果是压缩文件，则需要先解压缩
		if( bCompressed && pTable->dwOriginSize > 0 )
		{
			// 为解压后真正的文件内容申请内存
			PVOID pPackMem = MCALLOC(pTable->dwPackedSize);
			if( !pPackMem )
				return (DWORD)INVALID;

			if( !m_pDiskIO->Read(m_hHandle, pPackMem, pTable->dwPackedSize) )
			{
				ERR("could not read epk file");
				MCFREE(pPackMem);
				return (DWORD)INVALID;
			}

			// 反过滤（解压）文件
			tagFiltParam param;
			param.bCompress = FALSE;
			param.dwInBuffer = pTable->dwPackedSize;
			param.stInData = pTable->dwPackedSize;
			param.dwOutBuffer = pTable->dwOriginSize;
			param.stOutData = pTable->dwOriginSize;
			param.eFilter = GET_FILTER(pTable->wFlag);
			param.pMemIn = pPackMem;
			param.pMemOut = lpMemOut;

			// 进行过滤文件信息
			m_pFilter->Filt(&param);

			//释放初始压缩文件内容之内存
			MCFREE(pPackMem);
		}
		else	// 如果文件没有被压缩过
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
// 得到文件最后修改时间
//-----------------------------------------------------------------------------
BOOL EPK::GetTime(LPCWSTR szFileName, LPFILETIME pFileTime)
{
	// 找到文件对应的EPK中的表项
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
// 得到文件最后修改时间
//-----------------------------------------------------------------------------
BOOL EPK::GetTime(LPCSTR szFileName, LPFILETIME pFileTime)
{
	// 找到文件对应的EPK中的表项
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
// 根据文件名找到对应的表项
//-----------------------------------------------------------------------------
tagEPKTable* EPK::GetTable(LPCWSTR pFullName)
{
	CHAR szFullNameAnsi[MAX_PATH];
	INT nIndex = 0;
	for(LPCWSTR p = pFullName; *p != 0; ++p, ++nIndex)
		szFullNameAnsi[nIndex] = (CHAR)(*p);
	szFullNameAnsi[nIndex] = 0;	// 此种转换只能识别英文

	DWORD dwCRC = Crc32Low(szFullNameAnsi);

	// 在一堆相同的CRC中有效的CRC肯定在最后
	INT nStart = 0, nEnd = m_Header.dwValidTableNum, nPos = 0;

	while( nEnd != nStart )	// 二分查找
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
// 根据文件名找到对应的表项
//-----------------------------------------------------------------------------
tagEPKTable* EPK::GetTable(LPCSTR pFullName)
{
	DWORD dwCRC = Crc32Low(pFullName);

	// 在一堆相同的CRC中有效的CRC肯定在最后
	INT nStart = 0, nEnd = m_Header.dwValidTableNum, nPos = 0;

	while( nEnd != nStart )	// 二分查找
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
