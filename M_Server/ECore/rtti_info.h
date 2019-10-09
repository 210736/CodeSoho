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
	CreateObj*		m_pfnCreateObj;	//虚基类需要设为NULL
};


/**	\class RTTFactory
\brief Runtime type factor,保存所有定义的RunTimeClass的信息,并能根据className创建指定类别的对象
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

	RTTIMap					m_map;	//map方式存储,用来快速查找
	vector<const RTTInfo*>	m_vec;//vector方式存储,用来随机访问
};

#define RTTI_NEW(className)\
static_cast<className*>(RTTFactory::Inst()->CreateObj(#className))

}

