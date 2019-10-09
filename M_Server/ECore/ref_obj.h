#pragma once
#include "none_copyable.h"
#include "RTTI_Obj.h"


namespace ECore
{
	/**	\class RefObj 
		\brief ʹ�����ü����Ķ���Ļ���, ���������඼�����Զ�����
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