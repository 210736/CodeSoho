#pragma once
#include "rtti_info.h"

namespace ECore{

/**	\class RTTIObj
	\biref Runtime type identify object, ��Ҫ����ʱ������Ϣ�Ķ���������
	\remraks ʵ��һ��RTTIϵͳ,�ܵõ�ĳ����������ʲô��,�Ƿ����cast��ĳ����(��������),��������Ϣ����������ʱ��������;
*/
class ECORE_API RTTIObj
{
public:
	bool	IsKindOf(LPCSTR szClassName) const { return ((RTTInfo*)GetRTTInfo())->IsKindOf(szClassName); }
	bool	IsStaticClass(LPCSTR szClassName) const { return Crc32(szClassName) == ((RTTInfo*)GetRTTInfo())->m_dwClassID; }
	bool	IsKindOf(CONST RTTInfo *pInfo) const { return ((RTTInfo*)GetRTTInfo())->IsKindOf(pInfo); }
	bool	IsStaticClass(CONST RTTInfo *pInfo) const { return pInfo == GetRTTInfo(); }

	INLINE LPCSTR GetClassName() { return GetRTTInfo()->m_szClassName; }
	virtual RTTInfo* GetRTTInfo() const { return &m_classRTTIObj; }
	

	//--RTTIϵ�еĺ��������Ķ���
	static	RTTInfo		m_classRTTIObj;
};

//--���ܶ�̬����ʵ����class������
#define DECL_RTTI(className)\
	public:\
	static	RTTInfo		m_class##className;\
	virtual RTTInfo*	GetRTTInfo() const;\
	static  RTTInfo* 	GetStaticRTTInfo() { return &m_class##className; }

#define IMP_RTTI(className,superClass)\
	RTTInfo		className::m_class##className(#className,&superClass::m_class##superClass,NULL);\
	RTTInfo*	className::GetRTTInfo() const { return &m_class##className; }

//--�ܶ�̬�ؽ�ʵ����class������
#define DECL_RTTI_DYNC(className)\
	public:\
	static	RTTInfo		m_class##className;\
	virtual RTTInfo*	GetRTTInfo() const;\
	static RTTIObj*		CreateObj();

#define IMP_RTTI_DYNC(className,superClass)\
	RTTIObj*	className::CreateObj(){ return new className;}\
	RTTInfo		className::m_class##className(#className,&superClass::m_class##superClass,className::CreateObj);\
	RTTInfo*	className::GetRTTInfo() const { return &m_class##className; }

//--
#define IS_KIND_OF(className)	IsKindOf(&className::m_class##className)
#define IS_STATIC_CLASS(className)	IsStaticClass(&className::m_class##className)

/*
---------------------------------------------------------------------------------
����ת��
---------------------------------------------------------------------------------
*/
template<typename T>
T* Cast(const RTTIObj* pObj)
{
	ASSERT( pObj->IsStaticClass(T::GetStaticRTTInfo()) || 
			pObj->IsKindOf(T::GetStaticRTTInfo()) );
	return (T*)pObj;
}

}


	
	
	