//-----------------------------------------------------------------------------
//!\file	word_filter.cpp
//!\brief	词过滤
//!\date	2010-11-10
//!\author	Sxg
//!\desc 
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "word_filter.h"

namespace WordFilter
{

#define MCHECK_ACSII_1(c)		(c>=32 && c<=47)			//space!"#$%&'()*+,-./
#define MCHECK_ACSII_2(c)		(c>=58 && c<=64)			//:;<=>?@
#define MCHECK_ACSII_3(c)		(c>=91 && c<=96)			//[\]^_`
#define MCHECK_ACSII_4(c)		(c>=123 && c<=127)			//{|}~
#define MCHECK_ASCII_Tab(c)		(9 == c)					//Tab
#define MCHECK_ASCII_Num(c)		(c>=48 && c<=57)			//0-9
#define MCHECK_ASCII_Alpha(c)	((c>=65 && c<=90) || (c>=97 && c<=122))

#define MCHECK_UNICODE_1(c)		(c>=0x2000 && c<=0x2050)
#define MCHECK_UNICODE_2(c)		(c>=0x3000 && c<=0x301F)
#define MCHECK_UNICODE_3(c)		(c>=0xFF00 && c<=0xFF0F)
#define MCHECK_UNICODE_4(c)		(c>=0xFF1A && c<=0xFF20)
#define MCHECK_UNICODE_5(c)		(c>=0xFF3B && c<=0xFF40)
#define MCHECK_UNICODE_6(c)		(c>=0xFF5B && c<=0xFF65)

//-----------------------------------------------------------------------------
// 名称是否合法 -- 注: strName必须有字符串结束符
//-----------------------------------------------------------------------------
DWORD CheckName( LPCTSTR szName,const INT32 nLenMin,const INT32 nLenMax,vector<tstring>* pFilter/*=NULL*/,vector<WORD>* pFlag/*=NULL*/,EFilterMask mask/*=EFM_None*/ )
{
	assert(nLenMin <= nLenMax);
	assert(pFilter->size() == pFlag->size());

	// 获取长度
	INT32 nLen = (INT32)_tcslen(szName);

	// 长度检查
	if(nLen < nLenMin)
	{
		return E_TooShort;
	}
	else if(nLen > nLenMax)
	{
		return E_TooLong;
	}

	// 字符是否合法
	if(!IsWordCharValid(szName, nLen))
	{
		return E_Illegal;
	}

	// 是否在过滤词表中
	if(pFilter && pFlag && IsWordInFilter(szName, *pFilter, *pFlag, mask))
	{
		return E_InFilter;
	}

	return E_OK;
}

ECORE_API DWORD CheckPassword( LPCTSTR szName,const INT32 nLenMin,const INT32 nLenMax )
{
	assert(nLenMin <= nLenMax);

	// 获取长度
	INT32 nLen = (INT32)_tcslen(szName);

	// 长度检查
	if(nLen < nLenMin)
	{
		return E_TooShort;
	}
	else if(nLen > nLenMax)
	{
		return E_TooLong;
	}

	// 字符是否合法
	if(!IsWordAlphanumeric(szName, nLen))
	{
		return E_Illegal;
	}

	return E_OK;
}

//-----------------------------------------------------------------------------
// 字符是否合法
//-----------------------------------------------------------------------------
BOOL IsWordCharValid(LPCTSTR szName, const INT32 nLen)
{
	for(INT32 i=0; i<nLen; i++)
	{
		if( MCHECK_ACSII_1(szName[i]) 
			|| MCHECK_ACSII_2(szName[i]) 
			|| MCHECK_ACSII_3(szName[i]) 
			|| MCHECK_ACSII_4(szName[i]) 
			|| MCHECK_ASCII_Tab(szName[i]) 
			|| MCHECK_UNICODE_1(szName[i]) 
			|| MCHECK_UNICODE_2(szName[i]) 
			|| MCHECK_UNICODE_3(szName[i]) 
			|| MCHECK_UNICODE_4(szName[i]) 
			|| MCHECK_UNICODE_5(szName[i]) 
			|| MCHECK_UNICODE_6(szName[i]) )
		{
			return FALSE;
		}
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 字符是否是字母数字
//-----------------------------------------------------------------------------
BOOL IsWordAlphanumeric( LPCTSTR szName, const INT32 nLen )
{
	for(INT32 i=0; i<nLen; i++)
	{
		if( !MCHECK_ASCII_Num(szName[i]) && !MCHECK_ASCII_Alpha(szName[i]) )
		{
			return FALSE;
		}
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 是否在过滤词中
//-----------------------------------------------------------------------------
BOOL IsWordInFilter( LPCTSTR szName,const vector<tstring> &vectFilter,const vector<WORD>& vectFlag,EFilterMask mask )
{
	assert(vectFilter.size()==vectFlag.size());

	// 将待检查字符复制一份, 并转换为小写
	tstring strName = szName;
	transform(strName.begin(), strName.end(), strName.begin(), _totlower);

	//待检测的字符串的长度
	size_t	nameLen = strName.size();

	WORD mod = mask & 0x5555;

	// 遍历过滤词
	for(size_t i=0; i<vectFilter.size(); ++i)
	{
		// 比过滤词短
		if(nameLen < vectFilter.at(i).size())
		{
			continue;
		}

		WORD flag = vectFlag[i] & mask;
		if( flag>3 ) flag/=mod;
		if( flag == 0 )//忽略
		{
			continue;
		}
		else if( flag == 1 )//模糊匹配
		{
			// 在待检查字符串中搜索
			if( strName.find(vectFilter.at(i)) != tstring::npos )
			{
				return TRUE;
			}
		}
		else if( flag == 2 )//完全匹配
		{
			// 在待检查字符串中搜索
			if( nameLen==vectFilter.at(i).size() && strName.find(vectFilter.at(i)) != tstring::npos )
			{
				return TRUE;
			}
		}
		else
		{
			assert(_T("wordfilter:invalid filter flag"));
		}
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 过滤对话, 修改在过滤词表中的字符为'*'
//-----------------------------------------------------------------------------
VOID FilterDialog( tstring& strText,const vector<tstring> &vectFilter,const vector<WORD>& vectFlag,EFilterMask mask )
{
	assert( vectFilter.size()==vectFlag.size() );

	//待检测的字符串的长度
	if( strText.empty() )
	{
		return;
	}

	// 备份待检查字符串
	tstring strTemp = strText;

	// 待检查字符串转为全小写
	transform(strText.begin(), strText.end(), strText.begin(), _totlower);

	WORD mod = mask & 0x5555;

	// 遍历过滤词
	for(size_t i=0; i<vectFilter.size(); ++i)
	{
		// 过滤词长度
		size_t filterLen = vectFilter.at(i).size();
		if(strText.size() < filterLen)
		{
			continue;
		}

		WORD flag = vectFlag[i] & mask;
		if( flag>3 ) flag/=mod;
		if( flag == 0 )//忽略
		{
			continue;
		}
		else if( flag == 1 )//模糊匹配
		{
			// 搜索过滤词
			for(size_t index = strText.find(vectFilter.at(i), 0); 
				index != string::npos; 
				index = strText.find(vectFilter.at(i), index + filterLen))
			{
				// 找到, 替换为'*'
				strText.replace(index, filterLen, filterLen, _T('*'));
			}
		}
		else if( flag == 2 )//完全匹配
		{
			// 描述对话之类的应该不需要完全匹配
			assert(_T("wordfilter:dialog do not need complete match"));
		}
		else
		{
			assert(_T("wordfilter:invalid filter flag"));
		}
	}

	// 将转为小写的字符替换回去
	for(size_t i=0; i<strText.size(); ++i)
	{
		if(strText[i]>='a' && strText[i]<='z')
		{
			strText.replace(i, 1, 1, strTemp[i]);
		}
	}
}

}//namespace WordFilter