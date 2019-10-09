//-----------------------------------------------------------------------------
// Auth: Aslan
// Date: 2009-12-20
//-----------------------------------------------------------------------------
#pragma once

namespace ECore{
//-----------------------------------------------------------------------------
// 宏定义
//-----------------------------------------------------------------------------
#define MASKBITS 0x3F
#define MASKBYTE 0x80
#define MASK2BYTES 0xC0
#define MASK3BYTES 0xE0

#define UTF8_CHECK_LENGTH 20

//-----------------------------------------------------------------------------
// 判断一个UNICODE字符是否为汉字
//-----------------------------------------------------------------------------
INLINE BOOL ECORE_API IsZhCharacter(wchar_t wch)
{
    UINT16 u16ch = (UINT16)wch;

    return u16ch >= 0x4e00 && u16ch <= 0x9fa5;
}

//-----------------------------------------------------------------------------
// utf16编码转换为utf8编码
//-----------------------------------------------------------------------------
INLINE LPCSTR ECORE_API UTF16To8(LPCTSTR szSrc, LPSTR szDest, UINT uDestBuffSize)
{
	LPSTR szTemp = szDest;
	UINT uCount = 0;
	while( *szSrc && uCount < uDestBuffSize-4 )
	{
		// 单字节，形如0xxxxxxx形式
		if(*szSrc < 0x80)
		{
			*szTemp++ = (BYTE)*szSrc;
			uCount += 1;
		}
		// 双字节，形如110xxxxx 10xxxxxx形式
		else if( *szSrc < 0x800 )
		{
			*szTemp++ = ((BYTE)(MASK2BYTES | *szSrc >> 6));
			*szTemp++ = ((BYTE)(MASKBYTE | *szSrc & MASKBITS));
			uCount += 2;
		}
		// 三字节，形如1110xxxx 10xxxxxx 10xxxxxx形式
		else
		{
			*szTemp++ = ((BYTE)(MASK3BYTES | *szSrc >> 12));
			*szTemp++ = ((BYTE)(MASKBYTE | *szSrc >> 6 & MASKBITS));
			*szTemp++ = ((BYTE)(MASKBYTE | *szSrc & MASKBITS));
			uCount += 3;
		}
		++szSrc;
	}

	// 末尾补0
	*szTemp = 0;
	return szDest;
}

//-----------------------------------------------------------------------------
// utf8编码转换为utf16编码
//-----------------------------------------------------------------------------
INLINE LPCTSTR	ECORE_API UTF8To16(LPCSTR szSrc, LPTSTR szDest, UINT uDestBuffSize)
{
	LPTSTR szTemp = szDest;
	UINT uCount = 0;
	while( *szSrc && uCount < uDestBuffSize-1 )
	{
		// 单字节，形如0xxxxxxx形式
		if( !(*szSrc & MASKBYTE) )
			*szTemp++ = *szSrc++;
		else if( (*szSrc & MASK3BYTES) == MASK3BYTES )	// 三字节，形如1110xxxx 10xxxxxx 10xxxxxx形式
		{
			*szTemp++ = ((*szSrc & 0x0F) << 12) | ((*(szSrc+1) & MASKBITS) << 6) | (*(szSrc+2) & MASKBITS);
			szSrc += 3;
		}
		// 双字节，形如110xxxxx 10xxxxxx形式
		else if( (*szSrc & MASK2BYTES) == MASK2BYTES )
		{
			*szTemp++ = ((*szSrc & 0x1F) << 6) | (*(szSrc+1) & MASKBITS);
			szSrc += 2;
		}

		++uCount;
	}

	// 末尾补0
	*szTemp = 0;
	return szDest;
}


//-----------------------------------------------------------------------------
// ansi编码转换成unicode编码
//-----------------------------------------------------------------------------
INLINE std::wstring AnsiToUTF16(LPCSTR szAnsi)
{
	WCHAR buf[512] = {0};
	MultiByteToWideChar(CP_ACP,	0, szAnsi, -1, buf, 512);
	return std::wstring(buf);
}

//------------------------------------------------------------------------------
// unicode编码转换成ansi编码
//------------------------------------------------------------------------------
INLINE std::string UTF16ToAnsi(LPCWSTR szUTF16)
{
	char buf[1024] = {0};
	WideCharToMultiByte(CP_ACP, 0, szUTF16, -1, buf, 1024, NULL, NULL);
	return std::string(buf);
}

}