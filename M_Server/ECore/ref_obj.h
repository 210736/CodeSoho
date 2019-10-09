#pragma once
#include "none_copyable.h"
#include "RTTI_Obj.h"


namespace ECore
{
	/**	\class RefObj 
		\brief 使用引用计数的对象的基类, 所有派生类都不可自动拷贝
	*/
	class ECORE_API RefObj : public RTTIObj, private Noncopyable
	{
	public:
		RefObj(void);
		virtual ~RefObj(void);

		INLINE INT GetRef() const { return m_nRef; }
		INLINE void IncRef() { ++m_nRef; }
		INLINE void DecRef() { --m_nRef; }

	private:
		int m_nRef;

		DECL_RTTI(RefObj);
	};

}//endof namespace ECore