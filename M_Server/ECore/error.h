//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2003/12/01
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {
//-----------------------------------------------------------------------------
// for debug
//-----------------------------------------------------------------------------
// 错误信息,由windows窗口给出提示,可以由用户决定继续或退出
#define ERR				TObj<Error>()->Msg

// 错误信息,由windows窗口给出提示,抛出异常并结束程序
#undef THROW
#define THROW			TObj<Error>()->Fault

//
#ifndef THROW_NULL
#define THROW_NULL(e,i)		do{if(!(e)) TObj<Error>()->Fault(_T("Fail:%s\r\nFile:%s\r\nLine:%d\r\n%s"), _T(#e), __TFILE__, __LINE__, i);}while(0)
#endif

#ifndef THROW_INVALID
#define THROW_INVALID(e,i)	do{if(!VALID(e)) TObj<Error>()->Fault(_T("Fail:%s\r\nFile:%s\r\nLine:%d\r\n%s"), _T(#e), __TFILE__, __LINE__, i);}while(0)
#endif

#ifndef THROW_FAILED
#define THROW_FAILED(h,i)	do{if(FAILED(h)) TObj<Error>()->Fault(_T("Fail:%s\r\nFile:%s\r\nLine:%d\r\n%s\r\nd3d err:%s"), _T(#h), __TFILE__, __LINE__, i, DXGetErrorString(h));}while(0)
#endif

#ifndef THROW_FALSE
#define THROW_FALSE(e,i)	THROW_NULL(e,i)
#endif

//
#ifndef ERR_NULL
#define ERR_NULL(e,i)		do{if(!(e)) TObj<Error>()->Msg(_T("Fail:%s\r\nFile:%s\r\nLine:%d\r\n%s"), _T(#e), __TFILE__, __LINE__, i);}while(0)
#endif

#ifndef ERR_INVALID
#define ERR_INVALID(e,i)	do{if(!VALID(e)) TObj<Error>()->Msg(_T("Fail:%s\r\nFile:%s\r\nLine:%d\r\n%s"), _T(#e), __TFILE__, __LINE__, i);}while(0)
#endif

#ifndef ERR_FAILED
#define ERR_FAILED(h,i)		do{if(FAILED(h)) TObj<Error>()->Msg(_T("Fail:%s\r\nFile:%s\r\nLine:%d\r\n%s\r\nd3d err:%s"), _T(#h), __TFILE__, __LINE__, i, DXGetErrorString(h));}while(0)
#endif

#ifndef ERR_FALSE
#define ERR_FALSE(e,i)		ERR_NULL(e, i)
#endif



// 重新定义ASSERT
#undef ASSERT
#ifdef _DEBUG
	// 给出windows窗口提示,可以由用户决定继续或退出并生成log
	#define ASSERT(f)	do{if( !(f) ) TObj<Error>()->Msg(_T("Failure:%s\r\nFile:%s\r\nLine:%d"), _T(#f), __TFILE__, __LINE__);}while(0)
#else
	#define ASSERT(f)	((void)0)
#endif

#undef CHECK
#define CHECK(f)		do{if( !(f) ) TObj<Error>()->Msg(_T("Failure:%s\r\nFile:%s\r\nLine:%d"), _T(#f), __TFILE__, __LINE__);}while(0)


// Trace
VOID ECORE_API Trace(LPCTSTR lpszFormat, ...);
#undef TRACE
#ifdef _DEBUG
#define TRACE Trace
#else
#define TRACE __noop
#endif



//-----------------------------------------------------------------------------
// class Error 开发过程中的调试工具
//-----------------------------------------------------------------------------
class ECORE_API Error
{
public:
	// 显示一般调试消息，并获取系统出错信息
	INT  Msg(LPCWSTR lpFormat, ...);
	INT  Msg(LPCSTR lpFormat, ...);

	// 显示一般调试消息，并抛出异常
	INT  Fault(LPCTSTR lpFormat, ...);

private:
	TObj<Log>		m_pLog;
};


} // namespace ECore{



