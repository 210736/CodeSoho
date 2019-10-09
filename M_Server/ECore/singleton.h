//-----------------------------------------------------------------------------------------------
//!\file singleton.h
//!\author Aslan
//!\data 2010/01/11
//! last 2010/01/11
//!
//!\brief 单件类
//!\desc 提供全局唯一实例，其派生类要将自己的构造函数声明为保护的才可以，并设置该对象为自己的友元
//!\     对多线程环境下，必须在单线程初始化阶段显示的调用一下Instance，可保证不会生成多个对象
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
