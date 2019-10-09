//-----------------------------------------------------------------------------
//!\author Lyp
//!\data 2004/3/11
//! last 2004/3/14
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {

//-----------------------------------------------------------------------------
//! 把一些基本接口绑定到指定类
//-----------------------------------------------------------------------------
class BinderAbstract
{
protected:

public:
	virtual LPVOID	Create()=0;			// 创建对象的接口
	virtual VOID	Destroy(LPVOID)=0;	// 删除对象的接口
};


//-----------------------------------------------------------------------------
//! 把create,destroy绑定到指定类,与类厂等配合使用
//-----------------------------------------------------------------------------
template<typename concrete_class>
class TBinder : public BinderAbstract
{
public:
	LPVOID	Create()			{ return (LPVOID)(new concrete_class); }
	VOID	Destroy(LPVOID p)	{ delete((concrete_class*)p); }
};

} // namespace ECore {
