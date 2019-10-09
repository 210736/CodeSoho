//-----------------------------------------------------------------------------
// File: callback.h
// Desc: �ṩͨ�õĻص����ƣ�������Ա��������ͨ�����Ļص�
// Auth: Aslan
// Date: 2009-12-09
// Last: 2009-12-09
//-----------------------------------------------------------------------------
#pragma once

namespace ECore{

//-----------------------------------------------------------------------------
// �ص�����
//-----------------------------------------------------------------------------
class CallbackBase
{
public:
	virtual DWORD Execute() = 0;			// ִ�нӿ�
	virtual ~CallbackBase() {};
};

//-----------------------------------------------------------------------------
// ��ͨ�����ص��ӿ�
//-----------------------------------------------------------------------------
class CallbackFP
{
	typedef DWORD (*callback)();

public:
	CallbackFP(callback cb) : m_mycallback(cb) {}

	//-------------------------------------------------------------------------
	// ִ��
	//-------------------------------------------------------------------------
	DWORD Execute();

private:
	callback	m_mycallback;			// ����
};

//-----------------------------------------------------------------------------
// �������������Ա�����ص��ӿ�
//-----------------------------------------------------------------------------
template<typename T>
class CallbackP0 : public CallbackBase
{
public:
	typedef DWORD (T::*Method)();

	CallbackP0(T* pObj, Method func) : m_pObj(pObj), m_pFunc(func) {}

	//--------------------------------------------------------------------------
	// ִ�нӿ�
	//--------------------------------------------------------------------------
	DWORD Execute() { return (m_pObj->*m_pFunc)(); }

private:
	T*				m_pObj;
	Method			m_pFunc;
};


//------------------------------------------------------------------------------
// ��һ�����������Ա�����ص��ӿ�
//------------------------------------------------------------------------------
template<typename T, typename P1>
class CallbackP1 : public CallbackBase
{
public:
	typedef DWORD (T::*Method)(P1);

	CallbackP1(T* pObj, Method func, P1 p1) : m_pObj(pObj), m_pFunc(func), m_p1(p1) {}

	//---------------------------------------------------------------------------
	// ִ�нӿ�
	//---------------------------------------------------------------------------
	DWORD Execute()							{ return (m_pObj->*m_pFunc)(m_p1); }

private:
	T*				m_pObj;
	Method			m_pFunc;
	P1				m_p1;
};

//-------------------------------------------------------------------------------
// ���������������Ա�����ص��ӿ�
//-------------------------------------------------------------------------------
template<typename T, typename P1, typename P2>
class CallbackP2 : public CallbackBase
{
public:
	typedef DWORD (T::*Method)(P1, P2);

	CallbackP2(T* pObj, Method func, P1 p1, P2 p2) : m_pObj(pObj), m_pFunc(func), m_p1(p1), m_p2(p2) {}

	//---------------------------------------------------------------------------
	// ִ�нӿ�
	//---------------------------------------------------------------------------
	DWORD Execute()							{ return (m_pObj->*m_pFunc)(m_p1, m_p2); }

private:
	T*				m_pObj;
	Method			m_pFunc;
	P1				m_p1;
	P2				m_p2;
};

//-------------------------------------------------------------------------------
// ���������������Ա�ص�����
//-------------------------------------------------------------------------------
template<typename T, typename P1, typename P2, typename P3>
class CallbackP3 : public CallbackBase
{
public:
	typedef DWORD (T::*Method)(P1, P2, P3);

	CallbackP3(T* pObj, Method func, P1 p1, P2 p2, P3 p3) : m_pObj(pObj), m_pFunc(func), m_p1(p1), m_p2(p2), m_p3(p3) {}

	//-----------------------------------------------------------------------------
	// ִ�нӿ�
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
// ���ĸ����������Ա�ص�����
//----------------------------------------------------------------------------------
template<typename T, typename P1, typename P2, typename P3, typename P4>
class CallbackP4 : public CallbackBase
{
public:
	typedef DWORD (T::*Method)(P1, P2, P3, P4);

	CallbackP4(T* pObj, Method func, P1 p1, P2 p2, P3 p3, P4 p4) : m_pObj(pObj), m_pFunc(func), m_p1(p1), m_p2(p2), m_p3(p3), m_p4(p4) {}

	//------------------------------------------------------------------------------
	// ִ�нӿ�
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
