#pragma once

namespace ECore
{

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//

	/** 生成FourCC
	*/

	INLINE DWORD _MakeFourCC( LPCWSTR szStr ) 
	{ 
		if( 0 == szStr[0] )
			return 0;

		if( 0 == szStr[2] )
			return szStr[0];

		if( 0 == szStr[4] )
			return (szStr[2]<<8) | szStr[0];

		if( 0 == szStr[6] )
			return (szStr[4]<<16) | (szStr[2]<<8) | szStr[0];

		return (szStr[6]<<24) | (szStr[4]<<16) | (szStr[2]<<8) | szStr[0]; 
	}

	INLINE DWORD _MakeFourCC( LPCSTR szStr ) 
	{ 
		if( 0 == szStr[0] )
			return 0;

		if( 0 == szStr[1] )
			return szStr[0];

		if( 0 == szStr[2] )
			return (szStr[1]<<8) | szStr[0];

		if( 0 == szStr[3] )
			return (szStr[2]<<16) | (szStr[1]<<8) | szStr[0];

		return (szStr[3]<<24) | (szStr[2]<<16) | (szStr[1]<<8) | szStr[0]; 
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	/** FourCC转成String
	*/
	INLINE wstring _FourCC2Str(DWORD dw)
	{
		WCHAR str[5];
		str[0]=(WCHAR)(dw&0xff); 
		str[1]=(WCHAR)((dw>>8)&0xff); 
		str[2]=(WCHAR)((dw>>16)&0xff); 
		str[3]=(WCHAR)((dw>>24)&0xff);
		str[4]=0;
		return wstring(str);
	}


	INLINE string _FourCC2StrA(DWORD dw)
	{
		char str[5];
		str[0]=(char)(dw&0xff); 
		str[1]=(char)((dw>>8)&0xff); 
		str[2]=(char)((dw>>16)&0xff); 
		str[3]=(char)((dw>>24)&0xff);
		str[4]=0;
		return string(str);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//


	/** FourCC转成String
	*/
	INLINE void _FourCC2Str(DWORD dw,WCHAR* str)
	{
#ifdef _UNICODE
		str[0]=(WCHAR)(dw&0xff); 
		str[1]=(WCHAR)((dw>>8)&0xff); 
		str[2]=(WCHAR)((dw>>16)&0xff); 
		str[3]=(WCHAR)((dw>>24)&0xff);
		str[4]=0;
#else
		str[0]=(char)(dw&0xff); 
		str[1]=(char)((dw>>8)&0xff); 
		str[2]=(char)((dw>>16)&0xff); 
		str[3]=(char)((dw>>24)&0xff);
		str[4]=0;
#endif
	}


}//namespace ECore