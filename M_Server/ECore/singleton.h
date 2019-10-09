//-----------------------------------------------------------------------------------------------
//!\file singleton.h
//!\author Aslan
//!\data 2010/01/11
//! last 2010/01/11
//!
//!\brief ������
//!\desc �ṩȫ��Ψһʵ������������Ҫ���Լ��Ĺ��캯������Ϊ�����Ĳſ��ԣ������øö���Ϊ�Լ�����Ԫ
//!\     �Զ��̻߳����£������ڵ��̳߳�ʼ���׶���ʾ�ĵ���һ��Instance���ɱ�֤�������ɶ������
//-----------------------------------------------------------------------------------------------
#pragma once

namespace ECore{

template <typename T>
class Singleton
{
protected:
	Singleton() {}
	virtual ~Singleton() {}

private:
	Singleton(const Singleton&);
	Singleton& operator=(const Singleton&);

public:
	INLINE static T& Instance()	{ static T theInstance; return theInstance; }
};

}
