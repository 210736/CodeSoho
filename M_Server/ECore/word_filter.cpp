//-----------------------------------------------------------------------------
//!\file	word_filter.cpp
//!\brief	�ʹ���
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
// �����Ƿ�Ϸ� -- ע: strName�������ַ���������
//-----------------------------------------------------------------------------
DWORD CheckName( LPCTSTR szName,const INT32 nLenMin,const INT32 nLenMax,vector<tstring>* pFilter/*=NULL*/,vector<WORD>* pFlag/*=NULL*/,EFilterMask mask/*=EFM_None*/ )
{
	assert(nLenMin <= nLenMax);
	assert(pFilter->size() == pFlag->size());

	// ��ȡ����
	INT32 nLen = (INT32)_tcslen(szName);

	// ���ȼ��
	if(nLen < nLenMin)
	{
		return E_TooShort;
	}
	else if(nLen > nLenMax)
	{
		return E_TooLong;
	}

	// �ַ��Ƿ�Ϸ�
	if(!IsWordCharValid(szName, nLen))
	{
		return E_Illegal;
	}

	// �Ƿ��ڹ��˴ʱ���
	if(pFilter && pFlag && IsWordInFilter(szName, *pFilter, *pFlag, mask))
	{
		return E_InFilter;
	}

	return E_OK;
}

ECORE_API DWORD CheckPassword( LPCTSTR szName,const INT32 nLenMin,const INT32 nLenMax )
{
	assert(nLenMin <= nLenMax);

	// ��ȡ����
	INT32 nLen = (INT32)_tcslen(szName);

	// ���ȼ��
	if(nLen < nLenMin)
	{
		return E_TooShort;
	}
	else if(nLen > nLenMax)
	{
		return E_TooLong;
	}

	// �ַ��Ƿ�Ϸ�
	if(!IsWordAlphanumeric(szName, nLen))
	{
		return E_Illegal;
	}

	return E_OK;
}

//-----------------------------------------------------------------------------
// �ַ��Ƿ�Ϸ�
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
// �ַ��Ƿ�����ĸ����
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
// �Ƿ��ڹ��˴���
//-----------------------------------------------------------------------------
BOOL IsWordInFilter( LPCTSTR szName,const vector<tstring> &vectFilter,const vector<WORD>& vectFlag,EFilterMask mask )
{
	assert(vectFilter.size()==vectFlag.size());

	// ��������ַ�����һ��, ��ת��ΪСд
	tstring strName = szName;
	transform(strName.begin(), strName.end(), strName.begin(), _totlower);

	//�������ַ����ĳ���
	size_t	nameLen = strName.size();

	WORD mod = mask & 0x5555;

	// �������˴�
	for(size_t i=0; i<vectFilter.size(); ++i)
	{
		// �ȹ��˴ʶ�
		if(nameLen < vectFilter.at(i).size())
		{
			continue;
		}

		WORD flag = vectFlag[i] & mask;
		if( flag>3 ) flag/=mod;
		if( flag == 0 )//����
		{
			continue;
		}
		else if( flag == 1 )//ģ��ƥ��
		{
			// �ڴ�����ַ���������
			if( strName.find(vectFilter.at(i)) != tstring::npos )
			{
				return TRUE;
			}
		}
		else if( flag == 2 )//��ȫƥ��
		{
			// �ڴ�����ַ���������
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
// ���˶Ի�, �޸��ڹ��˴ʱ��е��ַ�Ϊ'*'
//-----------------------------------------------------------------------------
VOID FilterDialog( tstring& strText,const vector<tstring> &vectFilter,const vector<WORD>& vectFlag,EFilterMask mask )
{
	assert( vectFilter.size()==vectFlag.size() );

	//�������ַ����ĳ���
	if( strText.empty() )
	{
		return;
	}

	// ���ݴ�����ַ���
	tstring strTemp = strText;

	// ������ַ���תΪȫСд
	transform(strText.begin(), strText.end(), strText.begin(), _totlower);

	WORD mod = mask & 0x5555;

	// �������˴�
	for(size_t i=0; i<vectFilter.size(); ++i)
	{
		// ���˴ʳ���
		size_t filterLen = vectFilter.at(i).size();
		if(strText.size() < filterLen)
		{
			continue;
		}

		WORD flag = vectFlag[i] & mask;
		if( flag>3 ) flag/=mod;
		if( flag == 0 )//����
		{
			continue;
		}
		else if( flag == 1 )//ģ��ƥ��
		{
			// �������˴�
			for(size_t index = strText.find(vectFilter.at(i), 0); 
				index != string::npos; 
				index = strText.find(vectFilter.at(i), index + filterLen))
			{
				// �ҵ�, �滻Ϊ'*'
				strText.replace(index, filterLen, filterLen, _T('*'));
			}
		}
		else if( flag == 2 )//��ȫƥ��
		{
			// �����Ի�֮���Ӧ�ò���Ҫ��ȫƥ��
			assert(_T("wordfilter:dialog do not need complete match"));
		}
		else
		{
			assert(_T("wordfilter:invalid filter flag"));
		}
	}

	// ��תΪСд���ַ��滻��ȥ
	for(size_t i=0; i<strText.size(); ++i)
	{
		if(strText[i]>='a' && strText[i]<='z')
		{
			strText.replace(i, 1, 1, strTemp[i]);
		}
	}
}

}//namespace WordFilter