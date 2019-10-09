//-----------------------------------------------------------------------------
// File: stirng_util.h
// Desc: 字符串函数
// Auth: Aslan
// Date: 2009-12-20
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {
//-----------------------------------------------------------------------------
// 字符串功能函数
//-----------------------------------------------------------------------------
class ECORE_API StringUtil
{
public:
	//-------------------------------------------------------------------------
	// 根据某个分隔符将字符串分割成一个一个的语句
	//-------------------------------------------------------------------------
	static VOID	StringToToken(std::vector<wstring>& vecToken, LPCWSTR szString, WCHAR chSeparator = L' ');
	static VOID	StringToToken(std::vector<std::string>& vecToken, LPCSTR szString, CHAR chSeparator = ' ');
	static VOID	StringToToken(std::list<std::string>& vecToken, LPCSTR szString, CHAR chSeparator = ';');
	static VOID	StringToToken(std::vector<DWORD>& vecToken, LPCSTR szString, CHAR chSeparator = ' ');
	//-------------------------------------------------------------------------
	// 去掉首尾空格
	//-------------------------------------------------------------------------
	static VOID	CutSpace(wstring& string);
	static VOID	CutSpace(string& string);

	//-------------------------------------------------------------------------
	// 从字符串解析Vector2
	//-------------------------------------------------------------------------
	INLINE static Vector2 StrToVec2(const CHAR* szStr);

	//-------------------------------------------------------------------------
	// 将Vector2转换成字符串
	//-------------------------------------------------------------------------
	INLINE static LPCSTR Vec2ToStr(const Vector2& vec,CHAR* szStr,INT length);

	//-------------------------------------------------------------------------
	// 从字符串解析Vector3
	//-------------------------------------------------------------------------
	INLINE static Vector3 StrToVec3(const CHAR* szStr);

	//-------------------------------------------------------------------------
	// 从字符串解析Vector2
	//-------------------------------------------------------------------------
	INLINE static LPCSTR Vec3ToStr(const Vector3& vec,CHAR* szStr,INT length);

	//-------------------------------------------------------------------------
	// 从字符串解析Vector4
	//-------------------------------------------------------------------------
	INLINE static Vector4 StrToVec4(const CHAR* szStr);

	//-------------------------------------------------------------------------
	// 将Vector4转换成字符串
	//-------------------------------------------------------------------------
	INLINE static LPCSTR Vec4ToStr(const Vector4& vec,CHAR* szStr,INT length);

	//-------------------------------------------------------------------------
	// 从字符串解析Color4f
	//-------------------------------------------------------------------------
	INLINE static Color4f StrToColor4f(const CHAR* szStr);

	//-------------------------------------------------------------------------
	// 将Color4f转换成字符串
	//-------------------------------------------------------------------------
	INLINE static LPCSTR Color4fToStr(const Color4f& color,CHAR* szStr,INT length);

	//-------------------------------------------------------------------------
	// 将秒转换成"%d分%d秒"格式的字符串
	//-------------------------------------------------------------------------
	static tstring SecondToStr(DWORD second);
};



//-------------------------------------------------------------------------
// 从字符串解析Vector2
//-------------------------------------------------------------------------
Vector2 StringUtil::StrToVec2( const CHAR* szStr )
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
// 将Vector2转换成字符串
//-------------------------------------------------------------------------
LPCSTR StringUtil::Vec2ToStr( const Vector2& vec,CHAR* szStr,INT length )
{
	sprintf_s(szStr,length,"%f,%f",vec.x,vec.y);
	return szStr;
}


//-------------------------------------------------------------------------
// 从字符串解析Vector3
//-------------------------------------------------------------------------
Vector3 StringUtil::StrToVec3( const CHAR* szStr )
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
// 从字符串解析Vector3
//-------------------------------------------------------------------------
LPCSTR StringUtil::Vec3ToStr( const Vector3& vec,CHAR* szStr,INT length )
{
	sprintf_s(szStr,length,"%f,%f,%f",vec.x,vec.y,vec.z);

	return szStr;
}



//-------------------------------------------------------------------------
// 从字符串解析Vector4
//-------------------------------------------------------------------------
Vector4 StringUtil::StrToVec4( const CHAR* szStr )
{
	//将["255,255,255,255"]字符串转换成Vector4
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

	return Vector4(c[0],c[1],c[2],c[3]);
}


//-------------------------------------------------------------------------
// 将Vector4转换成字符串
//-------------------------------------------------------------------------
LPCSTR StringUtil::Vec4ToStr( const Vector4& vec,CHAR* szStr,INT length )
{
	sprintf_s(szStr,length,"%f,%f,%f,%f",vec.x,vec.y,vec.z,vec.w);

	return szStr;
}


//-------------------------------------------------------------------------
// 从字符串解析Color4f
//-------------------------------------------------------------------------
Color4f StringUtil::StrToColor4f( const CHAR* szStr )
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


//-------------------------------------------------------------------------
// 将Color4f转换成字符串
//-------------------------------------------------------------------------
LPCSTR StringUtil::Color4fToStr( const Color4f& color,CHAR* szStr,INT length )
{
	sprintf_s(szStr,length,"%f,%f,%f,%f",color.A,color.R,color.G,color.B);

	return szStr;
}

} // namespace ECore