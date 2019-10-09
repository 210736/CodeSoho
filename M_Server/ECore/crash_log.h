//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2003/12/11
//-----------------------------------------------------------------------------
#pragma once

#ifndef X64

namespace ECore {
#define CRASH_LOG_PROTECT_START	_set_se_translator( ECore::SEH_TranslateLog ); try{
#define CRASH_LOG_PROTECT_END	} catch(ECore::CrashLog){ if(::IsDebuggerPresent()) throw; else abort(); }

// 以上组合不方便使用时，可用
#define CRASH_LOG_PROTECT ::SetUnhandledExceptionFilter(ECore::SEH_FilterLog);


struct tagCrashSymbol
{
	string					strName;
	string					strType;
	string					strValue;

	BOOL					bPointer;
	BOOL					bArray;
	DWORD					dwOffset;
	DWORD					dwTag;
	DWORD					dwTypeTag;
	DWORD					dwBasicType;

	ULONG64					u64Size;
	ULONG64					u64ElementSize;

	std::list<tagCrashSymbol>	listChild;
	tagCrashSymbol():bPointer(0),bArray(0),dwOffset(0),u64Size(0),u64ElementSize(0),dwTag(0),dwTypeTag(0),dwBasicType(0){}
};


class ECORE_API CrashLog
{
public:

	CrashLog(UINT unCode, EXCEPTION_POINTERS* pExp);
	~CrashLog();

private:
	VOID InitDebugHelp();
	BOOL GetFunctionName(LPSTR szDest, PVOID pFunction);
	
	INT  DumpAll(EXCEPTION_POINTERS *pEx);
	VOID DumpExceptionInfo(EXCEPTION_POINTERS *pEx);	// 异常信息
	VOID DumpSystemInfo();								// 普通系统信息
	VOID DumpCallStack(PCONTEXT pContext);				// 调用堆栈
	VOID DumpRegisters(PCONTEXT pContext);				// 寄存器状态
	VOID DumpInstructions(PCONTEXT pContext);			// 发生异常处的指令
	VOID DumpVariable(PCONTEXT pContext);				// 变量

	VOID CallCrashReport();
	VOID GetSymbolPath(std::string& strPath);

	// 根据异常码得到异常描述字符串
	LPCSTR GetExceptionDescription(DWORD ExceptionCode);
	
	static BOOL CALLBACK SymEnumSymbolsProc(PSYMBOL_INFO, ULONG, PVOID);
	static BOOL FormatSymbolValue(PSYMBOL_INFO pSym, STACKFRAME * sf);

	static BOOL DumpTypeIndex(tagCrashSymbol& symOut, DWORD64 Base64, DWORD dwTypeIndex, INT nCycle);
	static VOID PrintSym(tagCrashSymbol& sym, INT nTab);
	static VOID GetValueRecursive(tagCrashSymbol& sym, ULONG64 u64Address);
	static VOID GetValue(tagCrashSymbol& sym, ULONG64 u64Address);

	static LPCSTR GetBasicType(DWORD dwBasicType);

	static UINT		m_unExceptionCode;
	static HANDLE	m_hProcess;
	static Log*		m_pLog;
};


}	// namespace ECore {

#endif
