//-----------------------------------------------------------------------------
//!\file functor.h
//!\brief 通用函数对象类
//!
//!\date 2010-01-14
//! last 2010-01-14
//!\author Aslan
//! desc 通用函数对象类，可将不同类型的同种参数的成员函数统一处理
//-----------------------------------------------------------------------------
#pragma once

namespace ECore{

//-----------------------------------------------------------------------------
// 参数模板
//-----------------------------------------------------------------------------
class FunctorBaseP0
{
public:
	virtual DWORD Execute() = 0;
};

template<typename P1>
class FunctorBaseP1
{
public:
	virtual DWORD Execute(P1) = 0;
};

template<typename P1, typename P2>
class FunctorBaseP2
{
public:
	virtual DWORD Execute(P1, P2) = 0;
};

template<typename P1, typename P2, typename P3>
class FunctorBaseP3
{
public:
	virtual DWORD Execute(P1, P2, P3) = 0;
};

template<typename P1, typename P2, typename P3, typename P4>
class FunctorBaseP4
{
public:
	virtual DWORD Execute(P1, P2, P3, P4) = 0;
};

//-----------------------------------------------------------------------------
// 函数对象模板
//-----------------------------------------------------------------------------
template<typename T>
class FunctorP0 : public FunctorBaseP0
{
public:
	typedef DWORD(T::*Method)();

	FunctorP0(T* pObj, Method func) : m_pObj(pObj), m_pFunc(func) {}

	DWORD Execute() { return (m_pObj->*m_pFunc)(); }

private:
	T*			m_pObj;
	Method		m_pFunc;
};

template<typename T, typename P1>
class FunctorP1 : public FunctorBaseP1<P1>
{
public:
	typedef DWORD (T::*Method)(P1);

	FunctorP1(T* pObj, Method func) : m_pObj(pObj), m_pFunc(func) {}

	DWORD Execute(P1 p1)	{ return (m_pObj->*m_pFunc)(p1); }

private:
	T*			m_pObj;
	Method		m_pFunc;
};

template<typename T, typename P1, typename P2>
class FunctorP2 : public FunctorBaseP2<P1, P2>
{
public:
	typedef DWORD (T::*Method)(P1, P2);

	FunctorP2(T* pObj, Method func) : m_pObj(pObj), m_pFunc(func) {}

	DWORD Execute(P1 p1, P2 p2)	{ return (m_pObj->*m_pFunc)(p1, p2); }

private:
	T*			m_pObj;
	Method		m_pFunc;
};

template<typename T, typename P1, typename P2, typename P3>
class FunctorP3 : public FunctorBaseP3<P1, P2, P3>
{
public:
	typedef DWORD (T::*Method)(P1, P2, P3);

	FunctorP3(T* pObj, Method func) : m_pObj(pObj), m_pFunc(func) {}

	DWORD Execute(P1 p1, P2 p2, P3 p3)	{ return (m_pObj->*m_pFunc)(p1, p2, p3); }

private:
	T*			m_pObj;
	Method		m_pFunc;
};

template<typename T, typename P1, typename P2, typename P3, typename P4>
class FunctorP4 : public FunctorBaseP4<P1, P2, P3, P4>
{
public:
	typedef DWORD (T::*Method)(P1, P2, P3, P4);

	FunctorP4(T* pObj, Method func) : m_pObj(pObj), m_pFunc(func) {}

	DWORD Execute(P1 p1, P2 p2, P3 p3, P4 p4)	{ return (m_pObj->*m_pFunc)(p1, p2, p3, p4); }

private:
	T*			m_pObj;
	Method		m_pFunc;
};

}
