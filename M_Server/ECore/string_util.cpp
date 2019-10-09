//-----------------------------------------------------------------------------
// File: stirng_util.cpp
// Desc: �ַ�������
// Auth: Aslan
// Date: 2009-12-20
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "string_util.h"

namespace ECore{
//-----------------------------------------------------------------------------
// �ַ���ת������䣬ͬʱע�����ŵ����⣬seperator����Ϊ����
//-----------------------------------------------------------------------------
VOID StringUtil::StringToToken(std::vector<wstring>& vecToken, LPCWSTR szString, WCHAR chSeparator)
{
	wstring str;
	vecToken.clear();

	if (!VALID(szString))	
		return;
	
	BOOL bIntoQuotMark = FALSE;	// �Ƿ�������Ų���
	for(LPCWSTR pStr = szString; *pStr; ++pStr)
	{
		if( *pStr == chSeparator )
		{
			if( !str.empty() )
			{
				if( bIntoQuotMark )
					str.append(chSeparator, 1);	// ����������ķָ���
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

	// ����ʣ����ַ�
	if( !str.empty() )
		vecToken.push_back(str);
}


//-----------------------------------------------------------------------------
// �ַ���ת������䣬ͬʱע�����ŵ����⣬seperator����Ϊ����
//-----------------------------------------------------------------------------
VOID StringUtil::StringToToken(std::vector<std::string>& vecToken, LPCSTR szString, CHAR chSeparator)
{
	std::string str;
	vecToken.clear();

	if (!VALID(szString))	return;

	BOOL bIntoQuotMark = FALSE;	// �Ƿ�������Ų���
	for(LPCSTR pStr = szString; *pStr; ++pStr)
	{
		if( *pStr == chSeparator )
		{
			if( !str.empty() )
			{
				if( bIntoQuotMark )
					str.append(chSeparator, 1);	// ����������ķָ���
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

	// ����ʣ����ַ�
	if( !str.empty() )
		vecToken.push_back(str);
}




//-----------------------------------------------------------------------------
// �ַ���ת������䣬ͬʱע�����ŵ����⣬seperator����Ϊ����
//-----------------------------------------------------------------------------
VOID StringUtil::StringToToken(std::list<std::string>& listToken, LPCSTR szString, CHAR chSeparator)
{
	std::string str;
	listToken.clear();

	BOOL bIntoQuotMark = FALSE;	// �Ƿ�������Ų���
	for(LPCSTR pStr = szString; *pStr; ++pStr)
	{
		if( *pStr == chSeparator )
		{
			if( !str.empty() )
			{
				if( bIntoQuotMark )
					str.append(chSeparator, 1);	// ����������ķָ���
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

	// ����ʣ����ַ�
	if( !str.empty() )
		listToken.push_back(str);
}

//-----------------------------------------------------------------------------
// �ַ���ת����DWORD���飬ͬʱע�����ŵ����⣬seperator����Ϊ����
//-----------------------------------------------------------------------------
VOID StringUtil::StringToToken( std::vector<DWORD>& vecToken, LPCSTR szString, CHAR chSeparator /*= ' '*/ )
{
	std::string str;
	vecToken.clear();

	if (!VALID(szString))	return;

	DWORD dwTemp = -1;
	BOOL bIntoQuotMark = FALSE;	// �Ƿ�������Ų���
	for(LPCSTR pStr = szString; *pStr; ++pStr)
	{
		if( *pStr == chSeparator )
		{
			if( !str.empty() )
			{
				if( bIntoQuotMark )
					str.append(chSeparator, 1);	// ����������ķָ���
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

	// ����ʣ����ַ�
	if( !str.empty() )
	{
		dwTemp = (DWORD)atol(str.c_str());
		vecToken.push_back(dwTemp);
	}
}

//--------------------------------------------------------------------------------
// ȥ���ַ�����β�ո�
//--------------------------------------------------------------------------------
VOID StringUtil::CutSpace(wstring& string)
{
	// ������:����ַ�����ֻ�пո�,���ܽ��д˲���
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
// ȥ���ַ�����β�ո�
//--------------------------------------------------------------------------------
VOID StringUtil::CutSpace(string& string)
{
	// ������:����ַ�����ֻ�пո�,���ܽ��д˲���
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
// ����ת����"%d��%d��"��ʽ���ַ���
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
		_stprintf(szBuf,_T("%d Сʱ %d �� %d ��"), dwHour, minute, second);
	}
	else if(minute>0)
	{
		_stprintf(szBuf,_T("%d �� %d ��"),minute,second);
	}
	else
	{
		_stprintf(szBuf,_T("%d ��"),second);
	}

	return tstring(szBuf);
}


} // namespace ECore