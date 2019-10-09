//-----------------------------------------------------------------------------
//!\file:	word_filter.h
//!\brief:	词过滤
//!\date:	2010-11-10
//!\author:	Sxg
//!\desc:	注: 1.方法中字符串参数必须有结束符
//!\			2.如果有过滤词表, 则过滤词表中的过滤词必须全部小写
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// 定义
//-----------------------------------------------------------------------------
namespace WordFilter
{
	// 错误码定义
	enum
	{
		E_OK			= 0,	// 合法

		E_TooShort		= 1,	// 太短
		E_TooLong		= 2,	// 过长
		E_Illegal		= 3,	// 有非法字符
		E_InFilter		= 4,	// 在过滤词表中
	};

	// 过滤条目
	enum EFilterMask
	{
		EFM_None		= 0,
		EFM_Chat		= 0x0003,	// 聊天
		EFM_CreateRole	= 0x000C,	// 创建人物
		EFM_Naming		= 0x0030,	// 命名
		EFM_Shop		= 0x00C0,	// 摆摊
		EFM_RoleInfo	= 0x0300,	// 人物资料
	};

	// 名称是否合法
	ECORE_API DWORD	CheckName(LPCTSTR szName,const INT32 nLenMin,const INT32 nLenMax,vector<tstring>* pFilter=NULL,vector<WORD>* pFlag=NULL,EFilterMask mask=EFM_None);

	// 密码是否合法
	ECORE_API DWORD	CheckPassword(LPCTSTR szName,const INT32 nLenMin,const INT32 nLenMax);

	// 过滤对话, 修改在过滤词表中的字符为'*'
	ECORE_API VOID	FilterDialog(tstring& strText,const vector<tstring> &vectFilter,const vector<WORD>& vectFlag,EFilterMask mask);

	// 字符是否合法
	BOOL			IsWordCharValid(LPCTSTR szName, const INT32 nLen);

	// 字符是否是字母数字
	BOOL			IsWordAlphanumeric(LPCTSTR szName, const INT32 nLen);

	// 是否在过滤词中
	ECORE_API BOOL	IsWordInFilter(LPCTSTR szName,const vector<tstring> &vectFilter,const vector<WORD>& vectFlag,EFilterMask mask);

}//namespace WordFilter