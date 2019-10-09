#include "stdafx.h"
#include "ref_obj.h"

namespace ECore
{
	IMP_RTTI(RefObj,RTTIObj);

	RefObj::RefObj(void) : m_nRef(0)
	{
	}

	RefObj::~RefObj(void)
	{
	}
}//endof namespace ECore