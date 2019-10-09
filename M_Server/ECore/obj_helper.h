//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2004/3/31
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {
//-----------------------------------------------------------------------------
// ָ����ֶ���
//-----------------------------------------------------------------------------
template<typename T> class TObj
{
public:
	T* operator->()	{ return (T*)m_pRaw; }	// ָ��
	operator T*()	{ return (T*)m_pRaw; }	// ת��Ϊָ������

	T* operator=(LPCSTR sz) 
	{ 
		if( ECore::g_pObjMgr )
			m_pRaw = sz ? ECore::g_pObjMgr->Get(sz) : ECore::g_pObjMgr->GetByType(typeid(T*).raw_name());
		return (T*)m_pRaw;
	}

	TObj(LPCSTR szObjName=NULL):m_pRaw(0) { operator=(szObjName); }

private:
	LPVOID	m_pRaw;
};


} // namespace ECore {
