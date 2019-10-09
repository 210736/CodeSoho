#pragma once

namespace ECore{

class RTTIObj;
/**	\class RTTInfo
\brief Runtime type info
*/
class ECORE_API RTTInfo
{
public:
	typedef RTTIObj* (CreateObj)();
	RTTInfo(LPCSTR szClassName, RTTInfo *pSuperClass, CreateObj *pFunc);


	bool IsKindOf(const RTTInfo *pInfo);
	bool IsKindOf(LPCSTR szClassName);

	LPCSTR			m_szClassName;
	DWORD			m_dwClassID;
	RTTInfo*		m_pSuperClass;
	CreateObj*		m_pfnCreateObj;	//�������Ҫ��ΪNULL
};


/**	\class RTTFactory
\brief Runtime type factor,�������ж����RunTimeClass����Ϣ,���ܸ���className����ָ�����Ķ���
\see RTTInfo,RTTIObj
*/
class ECORE_API RTTFactory
{
public:
	void				AddRuntimeClass(const RTTInfo *pInfo);
	RTTIObj*			CreateObj(LPCSTR szClassName) { return CreateObj(Crc32(szClassName)); }
	RTTIObj*			CreateObj(DWORD dwClassID);
	unsigned int		GetNumClass() { return (unsigned int)m_vec.size(); }
	const RTTInfo*		GetClass(unsigned int i) { return m_vec[i]; }
	static RTTFactory*	Inst() { static RTTFactory inst; return &inst; }

private:
	typedef EMap<DWORD, const RTTInfo*> RTTIMap;

	RTTIMap					m_map;	//map��ʽ�洢,�������ٲ���
	vector<const RTTInfo*>	m_vec;//vector��ʽ�洢,�����������
};

#define RTTI_NEW(className)\
static_cast<className*>(RTTFactory::Inst()->CreateObj(#className))

}

