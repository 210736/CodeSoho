//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2003/12/01
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {
#define SEH_PROTECT_START	_set_se_translator(ECore::SEH_Translate); try{
#define SEH_PROTECT_END		} catch(ECore::Dump){ if(::IsDebuggerPresent()) throw; else abort(); }

// 以上组合不方便使用时，可用
#define SEH_PROTECT			::SetUnhandledExceptionFilter(ECore::SEH_Filter);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class ECORE_API Dump
{
public:

	Dump(UINT unCode, EXCEPTION_POINTERS* pExp);


private:
	static MINIDUMP_EXCEPTION_INFORMATION m_info;
	static HANDLE m_hFile;
};


}	// namespace ECore {
