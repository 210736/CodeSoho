//-----------------------------------------------------------------------------
// Auth: Lyp
// Time: 2003-1-8
//-----------------------------------------------------------------------------
#pragma once

namespace ECore{

enum EFilter	// 种类
{
	EF_Null,
	EF_Changeless,
	EF_Lzo1X,
	EF_Lzo1X999,
	EF_LZMA,
	FMF_End
};
 
struct tagFiltParam	// 过滤器参数
{
	EFilter		eFilter;		// (in)进行何种过滤
	BOOL		bCompress;		// (in)正向或者逆向
	LPVOID		pMemIn;			// (in)源指针(指向输入缓冲)
	LPVOID		pMemOut;		// (in)目标指针(指向输出缓冲)
	LPVOID		pMemTemp;		// (in)临时缓冲指针LZO1X999用
	DWORD		dwInBuffer;		// (in)输入缓冲大小
	DWORD		dwOutBuffer;	// (in)输出缓冲大小
	SIZE_T		stInData;		// (in)输入数据大小
	SIZE_T		stOutData;		// (out)输出数据大小
	
};


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class ECORE_API Filter
{
public:

	BOOL Init(EFilter eFilter);
	VOID Filt(tagFiltParam* pParam);

	// 得到过滤器所需要的Dest缓冲附加值
	DWORD GetExtSize(DWORD dwSize, EFilter eFilter);

	Filter();

private:
	AtomMutex m_Lock;
};



} // namespace ECore{
