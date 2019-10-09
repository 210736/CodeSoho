//-----------------------------------------------------------------------------
//!\file:	word_filter.h
//!\brief:	�ʹ���
//!\date:	2010-11-10
//!\author:	Sxg
//!\desc:	ע: 1.�������ַ������������н�����
//!\			2.����й��˴ʱ�, ����˴ʱ��еĹ��˴ʱ���ȫ��Сд
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// ����
//-----------------------------------------------------------------------------
namespace WordFilter
{
	// �����붨��
	enum
	{
		E_OK			= 0,	// �Ϸ�

		E_TooShort		= 1,	// ̫��
		E_TooLong		= 2,	// ����
		E_Illegal		= 3,	// �зǷ��ַ�
		E_InFilter		= 4,	// �ڹ��˴ʱ���
	};

	// ������Ŀ
	enum EFilterMask
	{
		EFM_None		= 0,
		EFM_Chat		= 0x0003,	// ����
		EFM_CreateRole	= 0x000C,	// ��������
		EFM_Naming		= 0x0030,	// ����
		EFM_Shop		= 0x00C0,	// ��̯
		EFM_RoleInfo	= 0x0300,	// ��������
	};

	// �����Ƿ�Ϸ�
	ECORE_API DWORD	CheckName(LPCTSTR szName,const INT32 nLenMin,const INT32 nLenMax,vector<tstring>* pFilter=NULL,vector<WORD>* pFlag=NULL,EFilterMask mask=EFM_None);

	// �����Ƿ�Ϸ�
	ECORE_API DWORD	CheckPassword(LPCTSTR szName,const INT32 nLenMin,const INT32 nLenMax);

	// ���˶Ի�, �޸��ڹ��˴ʱ��е��ַ�Ϊ'*'
	ECORE_API VOID	FilterDialog(tstring& strText,const vector<tstring> &vectFilter,const vector<WORD>& vectFlag,EFilterMask mask);

	// �ַ��Ƿ�Ϸ�
	BOOL			IsWordCharValid(LPCTSTR szName, const INT32 nLen);

	// �ַ��Ƿ�����ĸ����
	BOOL			IsWordAlphanumeric(LPCTSTR szName, const INT32 nLen);

	// �Ƿ��ڹ��˴���
	ECORE_API BOOL	IsWordInFilter(LPCTSTR szName,const vector<tstring> &vectFilter,const vector<WORD>& vectFlag,EFilterMask mask);

}//namespace WordFilter