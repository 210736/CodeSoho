//-----------------------------------------------------------------------------
// author Lyp
// date 2004-04-03
// last 2010-02-24
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "ini_loader.h"
#include "vfs.h"
#include "disk_io.h"

namespace ECore {
//-----------------------------------------------------------------------------
// 加载
//-----------------------------------------------------------------------------
BOOL IniLoader::Load(LPCSTR szVFS, LPCWSTR szFileName, std::list<pair<wstring, wstring>>* pIDList)
{
	m_pVFS = szVFS;

	// 使用文件系统将INI一次性读入
	DWORD dwDataSize = m_pVFS->GetSize(szFileName);
	if( !VALID(dwDataSize) )
		return FALSE;

	// 申请足够大的空间
	LPBYTE pData = (LPBYTE)MCALLOC(dwDataSize+sizeof(WCHAR)*2+sizeof(DWORD));
	if( !pData ) 
		return FALSE;

	// 加载文件
	m_pVFS->LoadFile(pData, szFileName);

	// 在文件最后添加回车换行符(很多文件都不是以0d0a结束)
	*(DWORD*)(pData + dwDataSize) = 0x000a000d; // little-endian

	wstring strKey, strValue, strField;
	DWORD dwFieldID = 0, dwKeyID = 0;
	INT nFieldOffset = INVALID, nValueOffset = INVALID, nKeyOffset = 1; // 跳过微软BOM
	INT	nKeySize = 0;
	INT nCommentStart = INVALID;
	BOOL bNewLineEqual = TRUE;	// 一行只收一个等号
	WCHAR* pChar = (WCHAR*)pData;

	for(DWORD n = 0; n < (dwDataSize+sizeof(WCHAR)*2) / sizeof(WCHAR); ++n)
	{
		switch( *(pChar+n) )
		{
		case L'[':	// 开始下一个区的记录
			if( INVALID != nCommentStart )
				continue;
			
			nFieldOffset	=	n + 1;
			nKeyOffset		=	INVALID;
			nValueOffset	=	INVALID;
			break;
			
		case L']':	// 区域信息结束
			if( INVALID != nCommentStart )
				continue;

			// 记录区域的名字
			if( nFieldOffset != INVALID && n - nFieldOffset > 0 )
			{
				strField.assign(pChar+nFieldOffset, n-nFieldOffset);
				dwFieldID = Crc32(strField.c_str());
				m_mapField.insert(std::make_pair(dwFieldID, strField));
			}

			nFieldOffset = INVALID;
			break;
			
		case L'=':	// 键与值的间隔
			if( INVALID != nKeyOffset && INVALID == nCommentStart && bNewLineEqual == TRUE)
			{
				if( n - nKeyOffset > 0 )	// key的长度不能为零
				{
					nKeySize = n - nKeyOffset;
					nValueOffset = n + 1;

					strKey.assign(pChar+nKeyOffset, nKeySize); // 键
					replace(strKey.begin(), strKey.end(), L'\t', L' ');	// tab换成空格

					if( strKey.find_first_not_of(L' ') != -1 ) // 去掉头尾的空格
						strKey.assign(strKey,strKey.find_first_not_of(L' '),
						strKey.find_last_not_of(L' ')-strKey.find_first_not_of(L' ')+1);

					dwKeyID = Crc32(strKey.c_str());
					bNewLineEqual = FALSE;
					m_mapKey.insert(std::make_pair(dwKeyID, strKey));
					if( pIDList )
						pIDList->push_back(std::make_pair(strKey, strField));
				}
				else
					nKeyOffset = INVALID;
			}
			break;
			
		case L';':	// 整行注释
			nCommentStart = n;
			break;

		case 0x0a:	// 行尾
			if( INVALID != nKeyOffset && INVALID != nValueOffset )
			{
				if( INVALID != nCommentStart )
					strValue.assign(pChar+nValueOffset, nCommentStart - nValueOffset);
				else
					strValue.assign(pChar+nValueOffset, n - 1 - nValueOffset);

				replace(strValue.begin(), strValue.end(), L'\t', L' ');	// tab换成空格

				if( strValue.find_first_not_of(L' ') != -1 ) // 去掉头尾的空格
					strValue.assign(strValue,strValue.find_first_not_of(L' '),
					strValue.find_last_not_of(L' ')-strValue.find_first_not_of(L' ')+1);
				else
					strValue.clear();	// 全是空格
				
				UINT64 id = ((UINT64)dwFieldID << 32) | dwKeyID;
				m_map.insert(std::make_pair(id, strValue));
				bNewLineEqual = TRUE;	// 一行只接受一个等号
			}

			nFieldOffset = nValueOffset = INVALID;
			nKeyOffset = n+1;
			nCommentStart = INVALID;
			break;
		}
	}

	MCFREE(pData);
	return TRUE;
}



//-----------------------------------------------------------------------------
// 存盘
//-----------------------------------------------------------------------------
BOOL IniLoader::Save(LPCWSTR szFileName)
{
	if( !m_pDiskIO->IsFileExist(szFileName) )
	{
		HANDLE hFileHandle = m_pDiskIO->Create(szFileName, GENERIC_WRITE, 
			FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if( VALID(hFileHandle) )
		{
			// add M$ BOM
			BYTE BOM[2] = {0xFF, 0xFE};
			DWORD dwWrite = 0;
			m_pDiskIO->Write(hFileHandle, BOM, sizeof(BOM), &dwWrite, NULL);
			m_pDiskIO->Close(hFileHandle);
		}
	}

	std::map<UINT64, wstring>::iterator it;
	for(it=m_map.begin(); it!=m_map.end(); ++it)
	{
		DWORD dwKeyID = (DWORD)(it->first);
		DWORD dwFieldID = (DWORD)(it->first >> 32);
		wstring strKey = m_mapKey.find(dwKeyID)->second;
		wstring strField = m_mapField.find(dwFieldID)->second;
		wstring strValue = it->second;
		::WritePrivateProfileStringW(strField.c_str(), strKey.c_str(), strValue.c_str(), szFileName);
	}

	return TRUE;
}




} // namespace ECore {
