//-----------------------------------------------------------------------------
// File: stirng_util.cpp
// Desc: 字符串函数
// Auth: Aslan
// Date: 2009-12-20
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "string_util.h"

namespace ECore{
//-----------------------------------------------------------------------------
// 字符串转换成语句，同时注意引号的问题，seperator不能为引号
//-----------------------------------------------------------------------------
VOID StringUtil::StringToToken(std::vector<wstring>& vecToken, LPCWSTR szString, WCHAR chSeparator)
{
	wstring str;
	vecToken.clear();

	if (!VALID(szString))	
		return;
	
	BOOL bIntoQuotMark = FALSE;	// 是否进入引号部分
	for(LPCWSTR pStr = szString; *pStr; ++pStr)
	{
		if( *pStr == chSeparator )
		{
			if( !str.empty() )
			{
				if( bIntoQuotMark )
					str.append(chSeparator, 1);	// 保留引号里的分隔符
				else
				{
					vecToken.push_back(str);
					str.clear();
				}
			}
		}
		else if( *pStr == L'"' )
		{
			bIntoQuotMark = !bIntoQuotMark;
		}
		else
		{
			str.append(pStr, 1);
		}
	}

	// 加上剩余的字符
	if( !str.empty() )
		vecToken.push_back(str);
}


//-----------------------------------------------------------------------------
// 字符串转换成语句，同时注意引号的问题，seperator不能为引号
//-----------------------------------------------------------------------------
VOID StringUtil::StringToToken(std::vector<std::string>& vecToken, LPCSTR szString, CHAR chSeparator)
{
	std::string str;
	vecToken.clear();

	if (!VALID(szString))	return;

	BOOL bIntoQuotMark = FALSE;	// 是否进入引号部分
	for(LPCSTR pStr = szString; *pStr; ++pStr)
	{
		if( *pStr == chSeparator )
		{
			if( !str.empty() )
			{
				if( bIntoQuotMark )
					str.append(chSeparator, 1);	// 保留引号里的分隔符
				else
				{
					vecToken.push_back(str);
					str.clear();
				}
			}
		}
		else if( *pStr == '"' )
		{
			bIntoQuotMark = !bIntoQuotMark;
		}
		else
		{
			str.append(pStr, 1);
		}
	}

	// 加上剩余的字符
	if( !str.empty() )
		vecToken.push_back(str);
}




//-----------------------------------------------------------------------------
// 字符串转换成语句，同时注意引号的问题，seperator不能为引号
//-----------------------------------------------------------------------------
VOID StringUtil::StringToToken(std::list<std::string>& listToken, LPCSTR szString, CHAR chSeparator)
{
	std::string str;
	listToken.clear();

	BOOL bIntoQuotMark = FALSE;	// 是否进入引号部分
	for(LPCSTR pStr = szString; *pStr; ++pStr)
	{
		if( *pStr == chSeparator )
		{
			if( !str.empty() )
			{
				if( bIntoQuotMark )
					str.append(chSeparator, 1);	// 保留引号里的分隔符
				else
				{
					listToken.push_back(str);
					str.clear();
				}
			}
		}
		else if( *pStr == '"' )
		{
			bIntoQuotMark = !bIntoQuotMark;
		}
		else
		{
			str.append(pStr, 1);
		}
	}

	// 加上剩余的字符
	if( !str.empty() )
		listToken.push_back(str);
}

//-----------------------------------------------------------------------------
// 字符串转换成DWORD数组，同时注意引号的问题，seperator不能为引号
//-----------------------------------------------------------------------------
VOID StringUtil::StringToToken( std::vector<DWORD>& vecToken, LPCSTR szString, CHAR chSeparator /*= ' '*/ )
{
	std::string str;
	vecToken.clear();

	if (!VALID(szString))	return;

	DWORD dwTemp = -1;
	BOOL bIntoQuotMark = FALSE;	// 是否进入引号部分
	for(LPCSTR pStr = szString; *pStr; ++pStr)
	{
		if( *pStr == chSeparator )
		{
			if( !str.empty() )
			{
				if( bIntoQuotMark )
					str.append(chSeparator, 1);	// 保留引号里的分隔符
				else
				{
					dwTemp = (DWORD)atol(str.c_str());
					vecToken.push_back(dwTemp);
					str.clear();
				}
			}
		}
		else if( *pStr == '"' )
		{
			bIntoQuotMark = !bIntoQuotMark;
		}
		else
		{
			str.append(pStr, 1);
		}
	}

	// 加上剩余的字符
	if( !str.empty() )
	{
		dwTemp = (DWORD)atol(str.c_str());
		vecToken.push_back(dwTemp);
	}
}

//--------------------------------------------------------------------------------
// 去掉字符串首尾空格
//--------------------------------------------------------------------------------
VOID StringUtil::CutSpace(wstring& string)
{
	// 必须检查:如果字符串中只有空格,不能进行此操作
	if( string.find_first_not_of(L" ") != -1 )
	{
		string.assign(string, string.find_first_not_of(L" "),
			string.find_last_not_of(L" ")-string.find_first_not_of(L" ")+1);
	}
	else
	{
		string.clear();
	}
}

//--------------------------------------------------------------------------------
// 去掉字符串首尾空格
//--------------------------------------------------------------------------------
VOID StringUtil::CutSpace(string& string)
{
	// 必须检查:如果字符串中只有空格,不能进行此操作
	if( string.find_first_not_of(" ") != -1 )
	{
		string.assign(string, string.find_first_not_of(" "),
			string.find_last_not_of(" ")-string.find_first_not_of(" ")+1);
	}
	else
	{
		string.clear();
	}
}



//-------------------------------------------------------------------------
// 将秒转换成"%d分%d秒"格式的字符串
//-------------------------------------------------------------------------
tstring StringUtil::SecondToStr( DWORD second )
{
	TCHAR szBuf[100];

	DWORD minute = second / 60;
	second %= 60;

	if(minute>=60)
	{
		DWORD dwHour = minute / 60;
		minute %= 60;
		_stprintf(szBuf,_T("%d 小时 %d 分 %d 秒"), dwHour, minute, second);
	}
	else if(minute>0)
	{
		_stprintf(szBuf,_T("%d 分 %d 秒"),minute,second);
	}
	else
	{
		_stprintf(szBuf,_T("%d 秒"),second);
	}

	return tstring(szBuf);
}


} // namespace ECore