//-----------------------------------------------------------------------------
//!\author Lyp
//!\data 2004/3/11
//! last 2004/3/14
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {

//-----------------------------------------------------------------------------
//! ��һЩ�����ӿڰ󶨵�ָ����
//-----------------------------------------------------------------------------
class BinderAbstract
{
protected:

public:
	virtual LPVOID	Create()=0;			// ��������Ľӿ�
	virtual VOID	Destroy(LPVOID)=0;	// ɾ������Ľӿ�
};


//-----------------------------------------------------------------------------
//! ��create,destroy�󶨵�ָ����,���೧�����ʹ��
//-----------------------------------------------------------------------------
template<typename concrete_class>
class TBinder : public BinderAbstract
{
public:
	LPVOID	Create()			{ return (LPVOID)(new concrete_class); }
	VOID	Destroy(LPVOID p)	{ delete((concrete_class*)p); }
};

} // namespace ECore {
