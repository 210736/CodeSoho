//-----------------------------------------------------------------------------
// File: md5.cpp
// Desc: md5算法
// Auth: Lyp
// Date: 2003/11/13
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "md5.h"

namespace ECore{
//--------------------------------------------------------------------------------
// 对齐标志
//--------------------------------------------------------------------------------
static BYTE PADDING[64] =
{
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


//--------------------------------------------------------------------------------
// 初始化
//--------------------------------------------------------------------------------
VOID MD5Hash::MD5Init()
{
	m_context.count[0] = m_context.count[1] = 0;

	m_context.state[0] = 0x67452301;
	m_context.state[1] = 0xefcdab89;
	m_context.state[2] = 0x98badcfe;
	m_context.state[3] = 0x10325476;
}


//---------------------------------------------------------------------------------
// 计算
//---------------------------------------------------------------------------------
VOID MD5Hash::MD5Update(LPBYTE pInput, UINT uInputLen)
{
	UINT i = 0;
	UINT uIndex = 0;
	UINT uPartLen = 0;

	uIndex = (UINT)( (m_context.count[0] >> 3) & 0x3F );
	if( (m_context.count[0] += (uInputLen << 3)) < (uInputLen << 3) )
		m_context.count[1]++;
	m_context.count[1] += (uInputLen >> 29);
	uPartLen = 64 - uIndex;

	if( uInputLen >= uPartLen )
	{
		memcpy(m_context.buffer+uIndex, pInput, uPartLen);
		MD5Transform(m_context.state, m_context.buffer);

		for(i = uPartLen; i+63 < uInputLen; i+=64)
			MD5Transform(m_context.state, pInput+i);

		uIndex = 0;
	}
	else 
		i = 0;
	memcpy(m_context.buffer+uIndex, pInput+i, uInputLen-i);
}


//-----------------------------------------------------------------------
// 结果
//-----------------------------------------------------------------------
void MD5Hash::MD5Final(BYTE digest[16])
{
	BYTE bits[8];
	UINT uIndex = 0;
	UINT uPadLen;

	Encode(bits, m_context.count, 8);

	uIndex = (UINT)((m_context.count[0] >> 3) & 0x3f);
	uPadLen = (uIndex < 56) ? (56-uIndex) : (120-uIndex);
	
	MD5Update(PADDING, uPadLen);
	MD5Update(bits, 8);
	Encode(digest, m_context.state, 16);

	memset(&m_context, 0, sizeof(MD5_CTX));
}

//-----------------------------------------------------------------------
//计算文件的MD5校验值
//-----------------------------------------------------------------------
BOOL MD5Hash::MD5ForFile(LPCSTR szFileName, BYTE digest[16])
{
	if( NULL == szFileName )
		return FALSE;

	HANDLE hFile = ::CreateFileA(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	if( INVALID_HANDLE_VALUE == hFile ) //文件打不开
		return FALSE;


	INT64 n64FileSize = 0;
	INT64 n64Current = 0;
	SYSTEM_INFO si;
	::GetSystemInfo(&si); 
	// 每次映射不要超过128MB
	INT64 n64PerSection = (1024*1024*128) / si.dwAllocationGranularity * si.dwAllocationGranularity;

	::GetFileSizeEx(hFile, (PLARGE_INTEGER)&n64FileSize);	// 处理大尺寸文件

	HANDLE hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if( NULL == hFileMap )
	{
		::CloseHandle(hFile);
		return FALSE;
	}

	MD5Init();
	LPVOID pBuffer = NULL;
	while( n64FileSize > 0 )
	{
		if( n64FileSize > n64PerSection )
		{
			pBuffer = ::MapViewOfFile(hFileMap, FILE_MAP_READ, (DWORD)(n64Current>>32), (DWORD)n64Current, (DWORD)n64PerSection);
			MD5Update((BYTE*)pBuffer, (DWORD)n64PerSection);
			n64Current += n64PerSection;
			n64FileSize -= n64PerSection;
		}
		else
		{
			pBuffer = ::MapViewOfFile(hFileMap, FILE_MAP_READ, (DWORD)(n64Current>>32), (DWORD)n64Current, (DWORD)n64FileSize);
			MD5Update((BYTE*)pBuffer, (DWORD)n64FileSize);
			n64FileSize = 0;
		}

		::UnmapViewOfFile(pBuffer);
	}

	MD5Final(digest);
	CloseHandle(hFileMap);
	CloseHandle(hFile);
	return TRUE;
}


//-----------------------------------------------------------------------
//计算字符串的MD5摘要信息
//-----------------------------------------------------------------------
BOOL MD5Hash::MD5ForString(LPCSTR szString, BYTE digest[16])
{
	UINT uLen = (UINT)strlen(szString);
	return MD5ForBuffer((BYTE*)szString, uLen, digest);

}


//-----------------------------------------------------------------------
//计算buffer的MD5摘要信息
//-----------------------------------------------------------------------
BOOL MD5Hash::MD5ForBuffer(BYTE *pBuffer, UINT uBufferLen, BYTE digest[16])
{
	MD5Init();
	MD5Update(pBuffer, uBufferLen);
	MD5Final(digest);

	return TRUE;
}


//-----------------------------------------------------------------------
// 计算字符串的MD5摘要信息
//-----------------------------------------------------------------------
VOID MD5Hash::MD5ToString(std::string& str, BYTE byDigest[16])
{
	CHAR szBuf[256];

	// 转成字符串
	sprintf_s(szBuf, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", 
					byDigest[0], byDigest[1], byDigest[2], byDigest[3], byDigest[4], byDigest[5], 
					byDigest[6], byDigest[7], byDigest[8], byDigest[9], byDigest[10], byDigest[11], 
					byDigest[12], byDigest[13], byDigest[14], byDigest[15]);

	str = szBuf;
}


} // namespace ECore{

