//-----------------------------------------------------------------------------
// Auth: lyp
// Date: 2010-01-21
//-----------------------------------------------------------------------------
#pragma once
#include "hz2pinyin_table.h"

namespace ECore {
namespace SS{
#define MASKBITS 0x3F
#define MASKBYTE 0x80
#define MASK2BYTES 0xC0
#define MASK3BYTES 0xE0
//-----------------------------------------------------------------------------
// 转为Ansi字符串
//-----------------------------------------------------------------------------
class ECORE_API ToA
{
public:
	ToA(DWORD dw) {Cache(LONG_STRING); _itoa(dw, m_pCache, 10);}
	ToA(INT32 i) {Cache(LONG_STRING); _itoa(i, m_pCache, 10);}
	ToA(LONG l) {Cache(LONG_STRING); _ltoa(l, m_pCache, 10);}
	ToA(FLOAT f) {Cache(LONG_STRING); _gcvt(f, 6, m_pCache);}
	ToA(INT64 n) {Cache(LONG_STRING); *_i64toa(n, m_pCache, 10);}
	ToA(UINT64 n) {Cache(LONG_STRING); *_ui64toa(n, m_pCache, 10);}

	ToA(const Vector2& vec)
	{
		Cache(MID_STRING); 

		sprintf_s(m_pCache, MID_STRING, "%f,%f", vec.x, vec.y);
	}
	
	ToA(LPCWSTR sz):m_pCache(0) {if(0==sz) return; Cache(EXTREME_STRING); WideCharToMultiByte(CP_ACP, 0, sz, -1, m_pCache, EXTREME_STRING, 0, 0);}

	INLINE operator LPCSTR() const	{ return m_pCache; }
	INLINE operator LPCSTR()		{ return m_pCache; }
	INLINE operator LPSTR()const	{ return m_pCache; }

	~ToA()	{ if(m_pCache) { MCFREE(m_pCache); m_pCache = NULL; } }

	VOID Set(LPCWSTR sz)		{ ASSERT(sz != NULL); if(!m_pCache) Cache(EXTREME_STRING); WideCharToMultiByte(CP_ACP, 0, sz, -1, m_pCache, EXTREME_STRING, 0, 0); }

private:
	LPSTR m_pCache;
	INLINE VOID Cache(DWORD dwSize) {m_pCache = (LPSTR)MCALLOC(dwSize);}
};

//-----------------------------------------------------------------------------
// 转为UTF16字符串
//-----------------------------------------------------------------------------
class ECORE_API ToW
{
public:
	ToW(DWORD dw) {Cache(LONG_STRING); _itow(dw, m_pCache, 10);}
	ToW(INT32 i) {Cache(LONG_STRING); _itow(i, m_pCache, 10);}
	ToW(LONG l) {Cache(LONG_STRING); _ltow(l, m_pCache, 10);}
	ToW(FLOAT f) {CHAR temp[LONG_STRING]; Cache(LONG_STRING); _gcvt(f, 6, temp); MultiByteToWideChar(CP_ACP, 0, temp, -1, m_pCache, LONG_STRING);}
	ToW(INT64 n) {Cache(LONG_STRING); _i64tow(n, m_pCache, 10);}
	ToW(UINT64 n) {Cache(LONG_STRING); _ui64tow(n, m_pCache, 10);}
	
	// 注意下面这个是ANSI to Wide, 而不是UTF8 to wide
	ToW(LPCSTR sz):m_pCache(0) {if(0==sz) return; Cache(EXTREME_STRING); MultiByteToWideChar(CP_ACP, 0, sz, -1, m_pCache, EXTREME_STRING);}

	INLINE operator LPCWSTR() { return m_pCache; }
	INLINE operator LPWSTR() { return m_pCache; }

	~ToW() { if(m_pCache) MCFREE(m_pCache); }
private:
	LPWSTR m_pCache;
	INLINE VOID Cache(DWORD dwSize) {m_pCache = (LPWSTR)MCALLOC(dwSize*2);}
};


//-----------------------------------------------------------------------------
// 转为UTF8字符串
//-----------------------------------------------------------------------------
class ECORE_API To8
{
public:
	To8(DWORD dw) {Cache(LONG_STRING); _itoa(dw, m_pCache, 10);}
	To8(INT32 i) {Cache(LONG_STRING); _itoa(i, m_pCache, 10);}
	To8(LONG l) {Cache(LONG_STRING); _ltoa(l, m_pCache, 10);}
	To8(FLOAT f) {Cache(LONG_STRING); _gcvt(f, 6, m_pCache);}
	To8(INT64 n) {Cache(LONG_STRING); *_i64toa(n, m_pCache, 10);}
	To8(UINT64 n) {Cache(LONG_STRING); *_ui64toa(n, m_pCache, 10);}
	To8(bool b) {Cache(SHORT_STRING); m_pCache[0] = b ? '1' : '0'; m_pCache[1] = 0;}

	To8(LPCTSTR sz) // U16 TO U8
	{
		Cache(EXTREME_STRING); 
		LPSTR szTemp = m_pCache;
		UINT uCount = 0;
		while( *sz && uCount < EXTREME_STRING-4 )
		{
			if(*sz < 0x80)	// 单字节，形如0xxxxxxx形式
			{
				*szTemp++ = (BYTE)*sz;
				++uCount;
			}
			else if( *sz < 0x800 )	// 双字节，形如110xxxxx 10xxxxxx形式
			{
				*szTemp++ = ((BYTE)(MASK2BYTES | *sz >> 6));
				*szTemp++ = ((BYTE)(MASKBYTE | *sz & MASKBITS));
				++uCount; ++uCount;
			}
			else	// 三字节，形如1110xxxx 10xxxxxx 10xxxxxx形式
			{
				*szTemp++ = ((BYTE)(MASK3BYTES | *sz >> 12));
				*szTemp++ = ((BYTE)(MASKBYTE | *sz >> 6 & MASKBITS));
				*szTemp++ = ((BYTE)(MASKBYTE | *sz & MASKBITS));
				++uCount; ++uCount; ++uCount;
			}
			++sz;
		}
		*szTemp = '\0';	// 末尾补0
	}

	To8(const Vector2& vec)
	{
		Cache(MID_STRING); 

		sprintf_s(m_pCache, MID_STRING, "%f,%f", vec.x, vec.y);
	}

	To8(const Vector3& vec)
	{
		Cache(MID_STRING); 

		sprintf_s(m_pCache, MID_STRING, "%f,%f,%f", vec.x, vec.y, vec.z);
	}

	To8(const Vector4& vec)
	{
		Cache(MID_STRING); 

		sprintf_s(m_pCache, MID_STRING, "%f,%f,%f,%f", vec.x, vec.y, vec.z, vec.w);
	}

	To8(const Color4f& color)
	{
		Cache(MID_STRING); 

		sprintf_s(m_pCache, MID_STRING, "%f,%f,%f,%f",color.A,color.R,color.G,color.B);
	}

	INLINE operator LPCSTR() { return m_pCache; }
	INLINE operator LPSTR() { return m_pCache; }

	~To8() { MCFREE(m_pCache); }
private:
	LPSTR m_pCache;
	INLINE VOID Cache(DWORD dwSize) {m_pCache = (LPSTR)MCALLOC(dwSize);}
};



//-----------------------------------------------------------------------------
// 转为UTF16字符串
//-----------------------------------------------------------------------------
class ECORE_API To16
{
public:
	To16(DWORD dw) {Cache(LONG_STRING); _itow(dw, m_pCache, 10);}
	To16(INT32 i) {Cache(LONG_STRING); _itow(i, m_pCache, 10);}
	To16(LONG l) {Cache(LONG_STRING); _ltow(l, m_pCache, 10);}
	To16(FLOAT f) {CHAR temp[LONG_STRING]; Cache(LONG_STRING); _gcvt(f, 6, temp); MultiByteToWideChar(CP_ACP, 0, temp, -1, m_pCache, LONG_STRING);}
	To16(INT64 n) {Cache(LONG_STRING); _i64tow(n, m_pCache, 10);}
	To16(UINT64 n) {Cache(LONG_STRING); _ui64tow(n, m_pCache, 10);}

	To16(LPCSTR sz)		//U8 TO U16
	{
		Cache(EXTREME_STRING); 
		LPWSTR szTemp = m_pCache;
		UINT uCount = 0;
		while( *sz && uCount < EXTREME_STRING-4 )
		{
			if( !(*sz & MASKBYTE) )	// 单字节，形如0xxxxxxx形式
			{
				*szTemp++ = *sz++;
			}
			else if( (*sz & MASK3BYTES) == MASK3BYTES )	// 三字节，形如1110xxxx 10xxxxxx 10xxxxxx形式,注意这里必须先处理三字节的情况
			{
				*szTemp++ = ((*sz & 0x0F) << 12) | ((*(sz+1) & MASKBITS) << 6) | (*(sz+2) & MASKBITS);
				++sz; ++sz; ++sz;
			}
			else if( (*sz & MASK2BYTES) == MASK2BYTES )	// 双字节，形如110xxxxx 10xxxxxx形式
			{
				*szTemp++ = ((*sz & 0x1F) << 6) | (*(sz+1) & MASKBITS);
				++sz; ++sz;
			}
			++uCount;
		}
		*szTemp = 0;	// 末尾补0
	}

	INLINE operator LPCWSTR() { return m_pCache; }
	INLINE operator LPWSTR() { return m_pCache; }

	~To16() { MCFREE(m_pCache); }
private:
	LPWSTR m_pCache;
	INLINE VOID Cache(DWORD dwSize) {m_pCache = (LPWSTR)MCALLOC(dwSize*2);}
};


//-----------------------------------------------------------------------------
// 转拼音
//-----------------------------------------------------------------------------
class ECORE_API ToPinYin
{
public:
    ToPinYin(LPCWSTR sz) : m_strCache(L"")
    {
        if (0 == sz)
            return;

        wchar_t wch;
        int i = 0;

        while ((wch = sz[i++]) != L'\0')
        {
            size_t index = (UINT16)wch - 0x4e00;

            if (index >= 0 && index < 20902)
            {
                m_strCache += ToW(_pinyin_table_[index]);
            }
            else
            {
                m_strCache += wch;
            }
        }
    }
    ~ToPinYin(){}

    INLINE operator LPCWSTR() const	{ return m_strCache.c_str(); }

private:

    wstring m_strCache;
};

//-----------------------------------------------------------------------------
// 转拼音首字母
//-----------------------------------------------------------------------------
class ECORE_API ToPinYinFirstLetter
{
public:
    ToPinYinFirstLetter(LPCWSTR sz) : m_strCache(L"")
    {
        if (0 == sz)
            return;

        wchar_t wch;
        int i = 0;

        while ((wch = sz[i++]) != L'\0')
        {
            size_t index = wch - 0x4e00;

            if (index >= 0 && index < 20902)
            {
                string firstLetter(_pinyin_table_[index]);
                m_strCache += ToW(firstLetter.substr(0, 1).c_str());
            }
        }
    }
    ~ToPinYinFirstLetter(){}

    INLINE operator LPCWSTR() const	{ return m_strCache.c_str(); }

private:
    wstring m_strCache;
};


//-----------------------------------------------------------------------------
// 将各种字符串转为其他类型
//-----------------------------------------------------------------------------
INLINE INT8		ECORE_API ToINT8(LPCWSTR sz) { return sz?(INT8)wcstol(sz, NULL, 10):0; }
INLINE INT16	ECORE_API ToINT16(LPCWSTR sz) { return sz?(INT16)wcstol(sz, NULL, 10):0; }
INLINE INT32	ECORE_API ToINT32(LPCWSTR sz) { return sz?(INT32)wcstol(sz, NULL, 10):0; }
INLINE LONG		ECORE_API ToLONG(LPCWSTR sz) { return sz?(LONG)wcstol(sz, NULL, 10):0; }
INLINE INT64	ECORE_API ToINT64(LPCWSTR sz) { return sz?(INT64)_wtoi64(sz):0; }
INLINE BOOL		ECORE_API ToBOOL(LPCWSTR sz) { return sz?wcstol(sz, NULL, 10)!=0:0; }
INLINE BYTE		ECORE_API ToBYTE(LPCWSTR sz) { return sz?(BYTE)wcstoul(sz, NULL, 10):0; }
INLINE UINT16	ECORE_API ToUINT16(LPCWSTR sz) { return sz?(UINT16)wcstoul(sz, NULL, 10):0; }
INLINE FLOAT	ECORE_API ToFLOAT(LPCWSTR sz) { return sz?(FLOAT)_wtof(sz):0; }
INLINE DOUBLE	ECORE_API ToDOUBLE(LPCWSTR sz) { return sz?(FLOAT)_wtof(sz):0; }
INLINE DWORD	ECORE_API ToDWORD(LPCWSTR sz) { return sz?(DWORD)wcstoul(sz, NULL, 10):0; }
INLINE bool		ECORE_API ToBool(LPCWSTR sz) { return sz?(sz[0]!=0 && sz[0]!=L'0'):0; }

INLINE INT8		ECORE_API ToINT8(LPCSTR sz) { return sz?(INT8)strtol(sz, NULL, 10):0; }
INLINE INT16	ECORE_API ToINT16(LPCSTR sz) { return sz?(INT16)strtol(sz, NULL, 10):0; }
INLINE INT32	ECORE_API ToINT32(LPCSTR sz) { return sz?(INT32)strtol(sz, NULL, 10):0; }
INLINE LONG		ECORE_API ToLONG(LPCSTR sz) { return sz?(LONG)strtol(sz, NULL, 10):0; }
INLINE INT64	ECORE_API ToINT64(LPCSTR sz) { return sz?(INT64)_atoi64(sz):0; }
INLINE BOOL		ECORE_API ToBOOL(LPCSTR sz) { return sz?strtol(sz, NULL, 10)!=0:0; }
INLINE BYTE		ECORE_API ToBYTE(LPCSTR sz) { return sz?(BYTE)strtoul(sz, NULL, 10):0; }
INLINE UINT16	ECORE_API ToUINT16(LPCSTR sz) { return sz?(UINT16)strtoul(sz, NULL, 10):0; }
INLINE FLOAT	ECORE_API ToFLOAT(LPCSTR sz) { return sz?(FLOAT)atof(sz):0; }
INLINE DOUBLE	ECORE_API ToDOUBLE(LPCSTR sz) { return sz?(FLOAT)atof(sz):0; }
INLINE DWORD	ECORE_API ToDWORD(LPCSTR sz) {  return sz?(DWORD)strtoul(sz, NULL, 10):0; }
INLINE bool		ECORE_API ToBool(LPCSTR sz) { return sz?(sz[0]!=0 && sz[0]!='0'):0; }


INLINE INT8		ECORE_API ToINT8(LPCSTR sz, INT8 def) { return sz?(INT8)strtol(sz, NULL, 10):def; }
INLINE INT16	ECORE_API ToINT16(LPCSTR sz, INT16 def) { return sz?(INT16)strtol(sz, NULL, 10):def; }
INLINE INT32	ECORE_API ToINT32(LPCSTR sz, INT32 def) { return sz?(INT32)strtol(sz, NULL, 10):def; }
INLINE LONG		ECORE_API ToLONG(LPCSTR sz, LONG def) { return sz?(LONG)strtol(sz, NULL, 10):def; }
INLINE INT64	ECORE_API ToINT64(LPCSTR sz, INT64 def) { return sz?(INT64)_atoi64(sz):def; }
INLINE BOOL		ECORE_API ToBOOL(LPCSTR sz, BOOL def) { return sz?strtol(sz, NULL, 10)!=0:def; }
INLINE BYTE		ECORE_API ToBYTE(LPCSTR sz, BYTE def) { return sz?(BYTE)strtoul(sz, NULL, 10):def; }
INLINE UINT16	ECORE_API ToUINT16(LPCSTR sz, UINT16 def) { return sz?(UINT16)strtoul(sz, NULL, 10):def; }
INLINE FLOAT	ECORE_API ToFLOAT(LPCSTR sz, FLOAT def) { return sz?(FLOAT)atof(sz):def; }
INLINE DOUBLE	ECORE_API ToDOUBLE(LPCSTR sz, DOUBLE def) { return sz?(FLOAT)atof(sz):def; }
INLINE DWORD	ECORE_API ToDWORD(LPCSTR sz, DWORD def) {  return sz?(DWORD)strtoul(sz, NULL, 10):def; }
INLINE bool		ECORE_API ToBool(LPCSTR sz, bool def) { return sz?(sz[0]!=0 && sz[0]!='0'):def; }


//-----------------------------------------------------------------------------
// ToNumber -- 该类为方便转换用，只保存了指针
//		使用：INT8 n8 = ToNumber("45");
//-----------------------------------------------------------------------------
class ECORE_API ToNumber
{
public:
	ToNumber(LPCSTR sz):m_szValue(sz),m_szValueT(0){}
	ToNumber(LPCWSTR sz):m_szValueT(sz),m_szValue(0){}

	INLINE operator INT8()		{ return m_szValue?ToINT8(m_szValue):ToINT8(m_szValueT); }
	INLINE operator INT16()	{ return m_szValue?ToINT16(m_szValue):ToINT16(m_szValueT); }
	INLINE operator INT32()	{ return m_szValue?ToINT32(m_szValue):ToINT32(m_szValueT); }
	INLINE operator LONG()		{ return m_szValue?ToLONG(m_szValue):ToLONG(m_szValueT); }
	INLINE operator INT64()	{ return m_szValue?ToINT64(m_szValue):ToINT64(m_szValueT); }
	INLINE operator BYTE()		{ return m_szValue?ToBYTE(m_szValue):ToBYTE(m_szValueT); }
	INLINE operator UINT16()	{ return m_szValue?ToUINT16(m_szValue):ToUINT16(m_szValueT); }
	INLINE operator FLOAT()	{ return m_szValue?ToFLOAT(m_szValue):ToFLOAT(m_szValueT); }
	INLINE operator DOUBLE()	{ return m_szValue?ToDOUBLE(m_szValue):ToDOUBLE(m_szValueT); }
	INLINE operator DWORD()	{ return m_szValue?ToDWORD(m_szValue):ToDWORD(m_szValueT); }

	INLINE operator bool()		{ return m_szValue?ToBool(m_szValue):ToBool(m_szValueT); }

private:
	LPCSTR		m_szValue;
	LPCWSTR		m_szValueT;
};

//-----------------------------------------------------------------------------
// ToFilePart
//-----------------------------------------------------------------------------
INLINE LPCWSTR ECORE_API ToFilePart(LPCWSTR szSrc)
{
	LPCWSTR pResult = (LPCWSTR)wcsrchr(szSrc, L'\\');
	if( pResult )
		return ++pResult;

	pResult = (LPCWSTR)wcsrchr(szSrc, L'/');
	if( pResult )
		return ++pResult;
	
	return szSrc;
}


//-----------------------------------------------------------------------------
// ToFilePart
//-----------------------------------------------------------------------------
INLINE LPCSTR ECORE_API ToFilePart(LPCSTR szSrc)
{
	LPCSTR pResult = (LPCSTR)strrchr(szSrc, '\\');
	if( pResult )
		return ++pResult;

	pResult = (LPCSTR)strrchr(szSrc, '/');
	if( pResult )
		return ++pResult;
	
	return szSrc;
}


//-------------------------------------------------------------------------
// 从字符串解析Vector2
//-------------------------------------------------------------------------
INLINE Vector2 ECORE_API StrToVec2( const CHAR* szStr )
{
	//将["255,255"]字符串转换成Vector2
	float c[2]={0,0};
	if( szStr && szStr[0] != 0 )
	{
		LPSTR pStart = (LPSTR)szStr, pEnd = (LPSTR)szStr;
		INT i = 0;
		while( i<2 )
		{
			if( *pEnd == ',' || *pEnd == 0 )
			{
				c[i] = (float)atof(pStart);
				if(*pEnd == 0)
					break;

				++pEnd;
				pStart = pEnd;
				++i;
			}
			else
				++pEnd;
		}
	}

	return Vector2(c[0],c[1]);
}


//-------------------------------------------------------------------------
// 从字符串解析Vector3
//-------------------------------------------------------------------------
INLINE Vector3 ECORE_API StrToVec3( const CHAR* szStr )
{
	//将["255,255,255"]字符串转换成Vector3
	float c[3]={0,0,0};
	if( szStr && szStr[0] != 0 )
	{
		LPSTR pStart = (LPSTR)szStr, pEnd = (LPSTR)szStr;
		INT i = 0;
		while( i<3 )
		{
			if( *pEnd == ',' || *pEnd == 0 )
			{
				c[i] = (float)atof(pStart);
				if(*pEnd == 0)
					break;

				++pEnd;
				pStart = pEnd;
				++i;
			}
			else
				++pEnd;
		}
	}

	return Vector3(c[0],c[1],c[2]);
}


//-------------------------------------------------------------------------
// 从字符串解析Vector4
//-------------------------------------------------------------------------
INLINE Vector4 ECORE_API StrToVec4( const CHAR* szStr )
{
	//将["255,255,255,255"]字符串转换成Vector4
	float c[4]={0,0,0,0};
	if( szStr && szStr[0] != 0 )
	{
		LPSTR pStart = (LPSTR)szStr, pEnd = (LPSTR)szStr;
		INT i = 0;
		while( i<4 )
		{
			if( *pEnd == ',' || *pEnd == 0 )
			{
				c[i] = (float)atof(pStart);
				if(*pEnd == 0)
					break;

				++pEnd;
				pStart = pEnd;
				++i;
			}
			else
				++pEnd;
		}
	}

	return Vector4(c[0],c[1],c[2],c[3]);
}



//-------------------------------------------------------------------------
// 从字符串解析Color4f
//-------------------------------------------------------------------------
INLINE Color4f ECORE_API StrToColor4f( const CHAR* szStr )
{
	//将["255,255,255,255"]字符串转换成Color4f
	float c[4]={0.0f,0.0f,0.0f,0.0f};
	if( szStr && szStr[0] != 0 )
	{
		LPSTR pStart = (LPSTR)szStr, pEnd = (LPSTR)szStr;
		INT i = 0;
		while( i<4 )
		{
			if( *pEnd == ',' || *pEnd == 0 )
			{
				c[i] = (float)atof(pStart);
				if(*pEnd == 0)
					break;

				++pEnd;
				pStart = pEnd;
				++i;
			}
			else
				++pEnd;
		}
	}

	return Color4f(c[0],c[1],c[2],c[3]);
}




}
}