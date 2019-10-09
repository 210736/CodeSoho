#include "stdafx.h"

#include "dyna_array.h"
#include "csv_file.h"
#include "vfs.h"
#include "disk_io.h"

#pragma warning(disable: 4267)

namespace ECore
{
	bool CSVFile::OpenFile(VFS *pFS, LPCWSTR szFileName)
	{
		if( !szFileName )
			return false;

		int nBufSize = pFS->GetSize(szFileName);
		if( INVALID == nBufSize || nBufSize <=0 )
			return false;

		m_pBuffer = MCALLOC(nBufSize + sizeof(DWORD));

		pFS->LoadFile(m_pBuffer, szFileName);
		((BYTE*)m_pBuffer)[nBufSize] = 0;	// 结尾置0
		((BYTE*)m_pBuffer)[nBufSize+1] = 0; // 结尾置0
		m_nCharNum = nBufSize/sizeof(WCHAR);
		m_nCharIndex = 1;	// 跳过M$ BOM
		return true;
	}

	bool CSVFile::OpenFile(VFS *pFS, LPCSTR szFileName)
	{
		if( !szFileName )
			return false;

		int nBufSize = pFS->GetSize(szFileName);
		if( INVALID == nBufSize || nBufSize <= 0 )
			return false;

		m_pBuffer = MCALLOC(nBufSize + sizeof(DWORD));
		pFS->LoadFile(m_pBuffer, szFileName);
		((BYTE*)m_pBuffer)[nBufSize] = 0;	// 结尾置0
		((BYTE*)m_pBuffer)[nBufSize+1] = 0; // 结尾置0
		m_nCharNum = nBufSize;
		m_nCharIndex = 0;
		return true;
	}

	void CSVFile::BeginReadLine()
	{
		if( ((BYTE*)m_pBuffer)[0] == 0xFF && ((BYTE*)m_pBuffer)[1] == 0xFE )	// M$ BOM
			m_nCharIndex = 1;
		else
			m_nCharIndex = 0;
	}

	BOOL CSVFile::GetNextLine(vector<wstring>& vec)
	{
		if(m_nCharIndex >= m_nCharNum)
			return FALSE;

		WCHAR* next_token;
		WCHAR* token = wcstok_s( (WCHAR*)m_pBuffer + m_nCharIndex, L"\r\n", &next_token );
		if( !token )
			return FALSE;

		m_nCharIndex += (INT)(next_token - token) + 1;
		if( m_nCharIndex < m_nCharNum && ((WCHAR*)m_pBuffer)[m_nCharIndex] == L'\n' )	// 跳过\n
				++m_nCharIndex;

		vec.push_back( wcstok_s(token, L",", &next_token) );
		while( token = wcstok_s(NULL, L",", &next_token) )
			vec.push_back(token);

		return TRUE;
	}

	BOOL CSVFile::GetNextLine(vector<string>& vec)
	{
		if(m_nCharIndex >= m_nCharNum)
			return FALSE;

		CHAR* next_token;
		CHAR* token = strtok_s( (CHAR*)m_pBuffer + m_nCharIndex, "\r\n", &next_token );
		if( !token )
			return FALSE;

		m_nCharIndex += (INT)(next_token - token) + 1;
		if( m_nCharIndex < m_nCharNum && ((CHAR*)m_pBuffer)[m_nCharIndex] == '\n' )	// 跳过\n
				++m_nCharIndex;

		vec.push_back( strtok_s(token, ",", &next_token) );
		while( token = strtok_s(NULL, ",", &next_token) )
			vec.push_back(token);

		return TRUE;
	}

	BOOL CSVFile::GetNextLine(SimpleVector<LPCSTR>& vec)
	{
		if(m_nCharIndex >= m_nCharNum)
			return FALSE;

		CHAR* next_token;
		CHAR* token = strtok_s( (CHAR*)m_pBuffer + m_nCharIndex, "\r\n", &next_token );
		if( !token )
			return FALSE;

		m_nCharIndex += (INT)(next_token - token) + 1;
		if( m_nCharIndex < m_nCharNum && ((CHAR*)m_pBuffer)[m_nCharIndex] == '\n' )	// 跳过\n
		{
			((CHAR*)m_pBuffer)[m_nCharIndex-1] = 0;
			((CHAR*)m_pBuffer)[m_nCharIndex] = 0;
			++m_nCharIndex;
		}

		vec.push_back( strtok_s(token, ",", &next_token) );
		while( token = strtok_s(NULL, ",", &next_token) )
			vec.push_back(token);

		return TRUE;
	}

	void CSVFile::CloseFile()
	{
		if( m_pBuffer )
		{
			MCFREE(m_pBuffer);
			m_pBuffer = 0;
		}
		m_nCharNum=0;
	}
}//namespace ECore