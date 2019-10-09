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
// ����
//-----------------------------------------------------------------------------
BOOL IniLoader::Load(LPCSTR szVFS, LPCWSTR szFileName, std::list<pair<wstring, wstring>>* pIDList)
{
	m_pVFS = szVFS;

	// ʹ���ļ�ϵͳ��INIһ���Զ���
	DWORD dwDataSize = m_pVFS->GetSize(szFileName);
	if( !VALID(dwDataSize) )
		return FALSE;

	// �����㹻��Ŀռ�
	LPBYTE pData = (LPBYTE)MCALLOC(dwDataSize+sizeof(WCHAR)*2+sizeof(DWORD));
	if( !pData ) 
		return FALSE;

	// �����ļ�
	m_pVFS->LoadFile(pData, szFileName);

	// ���ļ������ӻس����з�(�ܶ��ļ���������0d0a����)
	*(DWORD*)(pData + dwDataSize) = 0x000a000d; // little-endian

	wstring strKey, strValue, strField;
	DWORD dwFieldID = 0, dwKeyID = 0;
	INT nFieldOffset = INVALID, nValueOffset = INVALID, nKeyOffset = 1; // ����΢��BOM
	INT	nKeySize = 0;
	INT nCommentStart = INVALID;
	BOOL bNewLineEqual = TRUE;	// һ��ֻ��һ���Ⱥ�
	WCHAR* pChar = (WCHAR*)pData;

	for(DWORD n = 0; n < (dwDataSize+sizeof(WCHAR)*2) / sizeof(WCHAR); ++n)
	{
		switch( *(pChar+n) )
		{
		case L'[':	// ��ʼ��һ�����ļ�¼
			if( INVALID != nCommentStart )
				continue;
			
			nFieldOffset	=	n + 1;
			nKeyOffset		=	INVALID;
			nValueOffset	=	INVALID;
			break;
			
		case L']':	// ������Ϣ����
			if( INVALID != nCommentStart )
				continue;

			// ��¼���������
			if( nFieldOffset != INVALID && n - nFieldOffset > 0 )
			{
				strField.assign(pChar+nFieldOffset, n-nFieldOffset);
				dwFieldID = Crc32(strField.c_str());
				m_mapField.insert(std::make_pair(dwFieldID, strField));
			}

			nFieldOffset = INVALID;
			break;
			
		case L'=':	// ����ֵ�ļ��
			if( INVALID != nKeyOffset && INVALID == nCommentStart && bNewLineEqual == TRUE)
			{
				if( n - nKeyOffset > 0 )	// key�ĳ��Ȳ���Ϊ��
				{
					nKeySize = n - nKeyOffset;
					nValueOffset = n + 1;

					strKey.assign(pChar+nKeyOffset, nKeySize); // ��
					replace(strKey.begin(), strKey.end(), L'\t', L' ');	// tab���ɿո�

					if( strKey.find_first_not_of(L' ') != -1 ) // ȥ��ͷβ�Ŀո�
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
			
		case L';':	// ����ע��
			nCommentStart = n;
			break;

		case 0x0a:	// ��β
			if( INVALID != nKeyOffset && INVALID != nValueOffset )
			{
				if( INVALID != nCommentStart )
					strValue.assign(pChar+nValueOffset, nCommentStart - nValueOffset);
				else
					strValue.assign(pChar+nValueOffset, n - 1 - nValueOffset);

				replace(strValue.begin(), strValue.end(), L'\t', L' ');	// tab���ɿո�

				if( strValue.find_first_not_of(L' ') != -1 ) // ȥ��ͷβ�Ŀո�
					strValue.assign(strValue,strValue.find_first_not_of(L' '),
					strValue.find_last_not_of(L' ')-strValue.find_first_not_of(L' ')+1);
				else
					strValue.clear();	// ȫ�ǿո�
				
				UINT64 id = ((UINT64)dwFieldID << 32) | dwKeyID;
				m_map.insert(std::make_pair(id, strValue));
				bNewLineEqual = TRUE;	// һ��ֻ����һ���Ⱥ�
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
// ����
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
