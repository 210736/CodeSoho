//-----------------------------------------------------------------------------
// File: callback.cpp
// Desc: �ṩͨ�õĻص����ƣ�������Ա��������ͨ�����Ļص�
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
