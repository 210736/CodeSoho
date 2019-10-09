//-----------------------------------------------------------------------------
// File: crc.h
// Desc: crc�㷨
// Auth: Lyp
// Date: 2003/11/13
//-----------------------------------------------------------------------------
#pragma once

namespace ECore{
//-----------------------------------------------------------------------------
// CRC32�㷨
//-----------------------------------------------------------------------------
INLINE DWORD ECORE_API Crc32(LPCSTR lpData, DWORD dwCrc=0);
INLINE DWORD ECORE_API Crc32Low(LPCSTR lpData, DWORD dwCrc=0);
INLINE DWORD ECORE_API Crc32(LPCWSTR pData, DWORD dwCrc=0);
INLINE DWORD ECORE_API Crc32Low(LPCWSTR lpData, DWORD dwCrc=0);
INLINE DWORD ECORE_API Crc32(LPBYTE pData, INT nCount, DWORD dwCrc=0);


//--------------------------------------------------------------------
template<INT>DWORD TCrc32(LPCSTR sz) { static const DWORD dwCrc = Crc32(sz); return dwCrc; }
template<INT>DWORD TCrc32(LPCWSTR sz) { static const DWORD dwCrc = Crc32(sz); return dwCrc; }

//--------------------------------------------------------------------
#define CRC(s) TCrc32<__COUNTER__>(s)


//-----------------------------------------------------------------------------
// ����ASCII�ַ�����32λCRC
//-----------------------------------------------------------------------------
DWORD ECORE_API Crc32(LPCSTR lpData, DWORD dwCrc)
{
	LPBYTE p = (LPBYTE)lpData;
	dwCrc ^= 0xffffffff;

	while(*p)
	{
		dwCrc = (dwCrc >> 8) ^ g_CrcTable[(dwCrc & 0xFF) ^ *p++];
	}

	return dwCrc ^ 0xffffffff; 
}

//-----------------------------------------------------------------------------
// ����ASCII�ַ�����Сд32λCRC
//-----------------------------------------------------------------------------
DWORD ECORE_API Crc32Low(LPCSTR lpData, DWORD dwCrc)
{
	LPBYTE p = (LPBYTE)lpData;
	dwCrc ^= 0xffffffff;

	while(*p)
	{
		dwCrc = (dwCrc >> 8) ^ g_CrcTable[(dwCrc & 0xFF) ^ (((*p>=0x41 && *p<=0x5A)?(*p+0x20):*p))]; 
		++p;
	}

	return dwCrc ^ 0xffffffff; 
}


//-----------------------------------------------------------------------------
// ����UNICODE�ַ���32λCRC
//-----------------------------------------------------------------------------
DWORD ECORE_API Crc32(LPCWSTR lpData, DWORD dwCrc)
{
	LPBYTE p = (LPBYTE)lpData;
	dwCrc ^= 0xffffffff;

	INT nCount = 0;
	while( (nCount&1) ? true : (*p || *(p+1)) )
	{
		dwCrc = (dwCrc >> 8) ^ g_CrcTable[(dwCrc & 0xFF) ^ *p++]; 
		++nCount;
	}

	return dwCrc ^ 0xffffffff; 
}


//-----------------------------------------------------------------------------
// ����UNICODE�ַ�����Сд32λCRC
//-----------------------------------------------------------------------------
DWORD ECORE_API Crc32Low(LPCWSTR lpData, DWORD dwCrc)
{
	LPBYTE p = (LPBYTE)lpData;
	dwCrc ^= 0xffffffff;

	INT nCount = 0;
	while( (nCount&1) ? true : (*p || *(p+1)) )
	{
		if( !(nCount & 1) && (*(p+1) == 0) )
			dwCrc = (dwCrc >> 8) ^ g_CrcTable[(dwCrc & 0xFF) ^ (((*p>=0x41 && *p<=0x5A)?(*p+0x20):*p))]; 
		else
			dwCrc = (dwCrc >> 8) ^ g_CrcTable[(dwCrc & 0xFF) ^ *p]; 
		++nCount;
		++p;
	}

	return dwCrc ^ 0xffffffff; 
}


//-----------------------------------------------------------------------------
// ����32λCRC,�ֽ�����
//-----------------------------------------------------------------------------
DWORD ECORE_API Crc32(LPBYTE pData, INT nCount, DWORD dwCrc)
{
	dwCrc ^= 0xffffffff;

	while(nCount--) 
		dwCrc = (dwCrc >> 8) ^ g_CrcTable[(dwCrc & 0xFF) ^ *pData++]; 

	return dwCrc ^ 0xffffffff; 
}



} // namespace ECore{