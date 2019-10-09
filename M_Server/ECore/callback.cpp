//-----------------------------------------------------------------------------
// File: callback.cpp
// Desc: 提供通用的回调机制，包括成员函数和普通函数的回调
// Auth: Aslan
// Date: 2009-12-09
// Last: 2009-12-09
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "callback.h"

namespace ECore{

DWORD CallbackFP::Execute()
{
	return m_mycallback();
}

} // namespace ECore{
