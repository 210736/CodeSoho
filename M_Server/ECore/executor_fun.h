//-----------------------------------------------------------------------------
// File: executor_fun.h
// Desc: 基于函数的线程执行体，针对于某个类要将它里面的多个成员函数封装成线程函数所用
//       它接收某个CallbackBase指针作为线程函数，同时提供一个CallBackBase留作OnShutSown调用
//       CallbackBase是对某个对象的成员函数的封装，用于执行
// Auth: Aslan
// Date: 2009-12-09
// Last: 2009-12-09
//-----------------------------------------------------------------------------
#pragma once

#include "executor.h"

namespace ECore{

class CallbackBase;

//-----------------------------------------------------------------------------
// 基于对象的线程执行体
//-----------------------------------------------------------------------------
class ECORE_API ExecutorFun : public Executor
{
public:
	ExecutorFun(CallbackBase* pExec, CallbackBase* pEnd);
	virtual ~ExecutorFun();

	//-------------------------------------------------------------------------
	// 执行接口，这个直接调用m_pExec的Execute接口就可以
	//-------------------------------------------------------------------------
	virtual VOID	Run();

	//-------------------------------------------------------------------------
	// 提供关闭线程时的回调接口，只需要调用m_pEnd的Execute接口就可以
	//-------------------------------------------------------------------------
	virtual VOID	OnShutDown();

private:
	CallbackBase*	m_pExec;
	CallbackBase*	m_pEnd;
};

} // namespace ECore{