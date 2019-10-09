//-----------------------------------------------------------------------------
// File: callback.h
// Desc: 提供通用的回调机制，包括成员函数和普通函数的回调
// Auth: Aslan
// Date: 2009-12-09
// Last: 2009-12-09
//-----------------------------------------------------------------------------
#pragma once

namespace ECore{

//-----------------------------------------------------------------------------
// 回调基类
//-----------------------------------------------------------------------------
class CallbackBase
{
public:
	virtual DWORD Execute() = 0;			// 执行接口
	virtual ~CallbackBase() {};
};

//-----------------------------------------------------------------------------
// 普通函数回调接口
//-----------------------------------------------------------------------------
class CallbackFP
{
	typedef DWORD (*callback)();

public:
	CallbackFP(callback cb) : m_mycallback(cb) {}

	//-------------------------------------------------------------------------
	// 执行
	//-------------------------------------------------------------------------
	DWORD Execute();

private:
	callback	m_mycallback;			// 函数
};

//-----------------------------------------------------------------------------
// 不带参数的类成员函数回调接口
//-----------------------------------------------------------------------------
template<typename T>
class CallbackP0 : public CallbackBase
{
public:
	typedef DWORD (T::*Method)();

	CallbackP0(T* pObj, Method func) : m_pObj(pObj), m_pFunc(func) {}

	//--------------------------------------------------------------------------
	// 执行接口
	//--------------------------------------------------------------------------
	DWORD Execute() { return (m_pObj->*m_pFunc)(); }

private:
	T*				m_pObj;
	Method			m_pFunc;
};


//------------------------------------------------------------------------------
// 带一个参数的类成员函数回调接口
//------------------------------------------------------------------------------
template<typename T, typename P1>
class CallbackP1 : public CallbackBase
{
public:
	typedef DWORD (T::*Method)(P1);

	CallbackP1(T* pObj, Method func, P1 p1) : m_pObj(pObj), m_pFunc(func), m_p1(p1) {}

	//---------------------------------------------------------------------------
	// 执行接口
	//---------------------------------------------------------------------------
	DWORD Execute()							{ return (m_pObj->*m_pFunc)(m_p1); }

private:
	T*				m_pObj;
	Method			m_pFunc;
	P1				m_p1;
};

//-------------------------------------------------------------------------------
// 带两个参数的类成员函数回调接口
//-------------------------------------------------------------------------------
template<typename T, typename P1, typename P2>
class CallbackP2 : public CallbackBase
{
public:
	typedef DWORD (T::*Method)(P1, P2);

	CallbackP2(T* pObj, Method func, P1 p1, P2 p2) : m_pObj(pObj), m_pFunc(func), m_p1(p1), m_p2(p2) {}

	//---------------------------------------------------------------------------
	// 执行接口
	//---------------------------------------------------------------------------
	DWORD Execute()							{ return (m_pObj->*m_pFunc)(m_p1, m_p2); }

private:
	T*				m_pObj;
	Method			m_pFunc;
	P1				m_p1;
	P2				m_p2;
};

//-------------------------------------------------------------------------------
// 带三个参数的类成员回调函数
//-------------------------------------------------------------------------------
template<typename T, typename P1, typename P2, typename P3>
class CallbackP3 : public CallbackBase
{
public:
	typedef DWORD (T::*Method)(P1, P2, P3);

	CallbackP3(T* pObj, Method func, P1 p1, P2 p2, P3 p3) : m_pObj(pObj), m_pFunc(func), m_p1(p1), m_p2(p2), m_p3(p3) {}

	//-----------------------------------------------------------------------------
	// 执行接口
	//-----------------------------------------------------------------------------
	DWORD Execute()							{ return (m_pObj->*m_pFunc)(m_p1, m_p2, m_p3); }

private:
	T*				m_pObj;
	Method			m_pFunc;
	P1				m_p1;
	P2				m_p2;
	P3				m_p3;
};

//----------------------------------------------------------------------------------
// 带四个参数的类成员回调函数
//----------------------------------------------------------------------------------
template<typename T, typename P1, typename P2, typename P3, typename P4>
class CallbackP4 : public CallbackBase
{
public:
	typedef DWORD (T::*Method)(P1, P2, P3, P4);

	CallbackP4(T* pObj, Method func, P1 p1, P2 p2, P3 p3, P4 p4) : m_pObj(pObj), m_pFunc(func), m_p1(p1), m_p2(p2), m_p3(p3), m_p4(p4) {}

	//------------------------------------------------------------------------------
	// 执行接口
	//------------------------------------------------------------------------------
	DWORD Execute()								{ return (m_pObj->*m_pFunc)(m_p1, m_p2, m_p3, m_p4); }

private:
	T*				m_pObj;
	Method			m_pFunc;
	P1				m_p1;
	P2				m_p2;
	P3				m_p3;
	P4				m_p4;
};

} // namespace ECore{
