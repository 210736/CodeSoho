//-----------------------------------------------------------------------------
// Auth: Aslan
// Date: 2009-12-20
//-----------------------------------------------------------------------------
#pragma once

namespace ECore{
//-----------------------------------------------------------------------------
// �궨��
//-----------------------------------------------------------------------------
#define MASKBITS 0x3F
#define MASKBYTE 0x80
#define MASK2BYTES 0xC0
#define MASK3BYTES 0xE0

#define UTF8_CHECK_LENGTH 20

//-----------------------------------------------------------------------------
// �ж�һ��UNICODE�ַ��Ƿ�Ϊ����
//-----------------------------------------------------------------------------
INLINE BOOL ECORE_API IsZhCharacter(wchar_t wch)
{
    UINT16 u16ch = (UINT16)wch;

    return u16ch >= 0x4e00 && u16ch <= 0x9fa5;
}

//-----------------------------------------------------------------------------
// utf16����ת��Ϊutf8����
//-----------------------------------------------------------------------------
INLINE LPCSTR ECORE_API UTF16To8(LPCTSTR szSrc, LPSTR szDest, UINT uDestBuffSize)
{
	LPSTR szTemp = szDest;
	UINT uCount = 0;
	while( *szSrc && uCount < uDestBuffSize-4 )
	{
		// ���ֽڣ�����0xxxxxxx��ʽ
		if(*szSrc < 0x80)
		{
			*szTemp++ = (BYTE)*szSrc;
			uCount += 1;
		}
		// ˫�ֽڣ�����110xxxxx 10xxxxxx��ʽ
		else if( *szSrc < 0x800 )
		{
			*szTemp++ = ((BYTE)(MASK2BYTES | *szSrc >> 6));
			*szTemp++ = ((BYTE)(MASKBYTE | *szSrc & MASKBITS));
			uCount += 2;
		}
		// ���ֽڣ�����1110xxxx 10xxxxxx 10xxxxxx��ʽ
		else
		{
			*szTemp++ = ((BYTE)(MASK3BYTES | *szSrc >> 12));
			*szTemp++ = ((BYTE)(MASKBYTE | *szSrc >> 6 & MASKBITS));
			*szTemp++ = ((BYTE)(MASKBYTE | *szSrc & MASKBITS));
			uCount += 3;
		}
		++szSrc;
	}

	// ĩβ��0
	*szTemp = 0;
	return szDest;
}

//-----------------------------------------------------------------------------
// utf8����ת��Ϊutf16����
//-----------------------------------------------------------------------------
INLINE LPCTSTR	ECORE_API UTF8To16(LPCSTR szSrc, LPTSTR szDest, UINT uDestBuffSize)
{
	LPTSTR szTemp = szDest;
	UINT uCount = 0;
	while( *szSrc && uCount < uDestBuffSize-1 )
	{
		// ���ֽڣ�����0xxxxxxx��ʽ
		if( !(*szSrc & MASKBYTE) )
			*szTemp++ = *szSrc++;
		else if( (*szSrc & MASK3BYTES) == MASK3BYTES )	// ���ֽڣ�����1110xxxx 10xxxxxx 10xxxxxx��ʽ
		{
			*szTemp++ = ((*szSrc & 0x0F) << 12) | ((*(szSrc+1) & MASKBITS) << 6) | (*(szSrc+2) & MASKBITS);
			szSrc += 3;
		}
		// ˫�ֽڣ�����110xxxxx 10xxxxxx��ʽ
		else if( (*szSrc & MASK2BYTES) == MASK2BYTES )
		{
			*szTemp++ = ((*szSrc & 0x1F) << 6) | (*(szSrc+1) & MASKBITS);
			szSrc += 2;
		}

		++uCount;
	}

	// ĩβ��0
	*szTemp = 0;
	return szDest;
}


//-----------------------------------------------------------------------------
// ansi����ת����unicode����
//-----------------------------------------------------------------------------
INLINE std::wstring AnsiToUTF16(LPCSTR szAnsi)
{
	WCHAR buf[512] = {0};
	MultiByteToWideChar(CP_ACP,	0, szAnsi, -1, buf, 512);
	return std::wstring(buf);
}

//------------------------------------------------------------------------------
// unicode����ת����ansi����
//------------------------------------------------------------------------------
INLINE std::string UTF16ToAnsi(LPCWSTR szUTF16)
{
	char buf[1024] = {0};
	WideCharToMultiByte(CP_ACP, 0, szUTF16, -1, buf, 1024, NULL, NULL);
	return std::string(buf);
}

}