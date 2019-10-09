//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2003/12/11
// Last: 2010/1/27
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "crash_log.h"

#ifndef X64

// ʹ����������
#include "super_string.h"
using namespace ECore::SS;

namespace ECore {
CONST INT MAX_CODE_BYTE_DUMP	= 16;	// ��¼�����ֽ�ָ��
CONST INT ONE_MEGA				= 1024*1024;

UINT	CrashLog::m_unExceptionCode = 0;
HANDLE	CrashLog::m_hProcess = 0;
Log*	CrashLog::m_pLog = 0;

// ��¼�����쳣��Ϣ�ַ���
struct ExceptionNames
{
	DWORD	ExceptionCode;
	LPCSTR	ExceptionName;
};

ExceptionNames ExceptionMap[] =
{
	{0x40010005, "Control-C"},
	{0x40010008, "Control-Break"},
	{0x80000002, "Datatype Misalignment"},
	{0x80000003, "Breakpoint"},
	{0xc0000005, "Access Violation"},
	{0xc0000006, "In Page Error"},
	{0xc0000017, "No Memory"},
	{0xc000001d, "Illegal Instruction"},
	{0xc0000025, "Noncontinuable Exception"},
	{0xc0000026, "Invalid Disposition"},
	{0xc000008c, "Array Bounds Exceeded"},
	{0xc000008d, "Float Denormal Operand"},
	{0xc000008e, "Float Divide by Zero"},
	{0xc000008f, "Float Inexact Result"},
	{0xc0000090, "Float Invalid Operation"},
	{0xc0000091, "Float Overflow"},
	{0xc0000092, "Float Stack Check"},
	{0xc0000093, "Float Underflow"},
	{0xc0000094, "Integer Divide by Zero"},
	{0xc0000095, "Integer Overflow"},
	{0xc0000096, "Privileged Instruction"},
	{0xc00000fD, "Stack Overflow"},
	{0xc0000142, "DLL Initialization Failed"},
	{0xe06d7363, "Microsoft C++ Exception"},
};

static INT nCrashLogTimes = 0;
//-----------------------------------------------------------------------------
// construction / destruction
//-----------------------------------------------------------------------------
CrashLog::CrashLog(UINT unCode, EXCEPTION_POINTERS* pExp)
{
	m_pLog = (Log*)GetObj(CreateObj("CrashLog", "Log"));

	// ��ֹ����
	++nCrashLogTimes;
	if( nCrashLogTimes > 1 )
		return;

	m_unExceptionCode = unCode;
	InitDebugHelp();
	DumpAll(pExp);

	CallCrashReport();	// ���
}


CrashLog::~CrashLog()
{
	KillObj("CrashLog");
	SymCleanup(GetCurrentProcess());
}


//-----------------------------------------------------------------------------
// ����Symbol·��
//-----------------------------------------------------------------------------
VOID CrashLog::GetSymbolPath(std::string& strPath)
{
	CHAR szPath[MAX_PATH*2];
	GetModuleFileNameA(NULL, szPath, MAX_PATH);
	CHAR *pResult = strrchr(szPath, '\\');	// ��ȥ·��
	if(pResult)
		*pResult = 0;
	sprintf_s(szPath, "%s\\symbol", szPath);	// �õ��˾���·��

	CHAR szCurPath[MAX_PATH*2];
	::GetCurrentDirectoryA(MAX_PATH*2-1, szCurPath);

	if( 0 == strcmp(szPath, szCurPath) )
	{
		// ��ǰ·�����Ƿ��ű�·��
		strPath = ".";
		return;
	}

	// �������·��,��Ϊ����·�����ܰ��������ַ�����SymInitialize�Ⱥ����޷�ʶ�������ַ�
	pResult = strstr(szPath, szCurPath);	// ��ȫ·����Ѱ�ҵ�ǰ·��
	if( pResult && strlen(szPath) > strlen(szCurPath) )
	{
		// �õ����·��
		CHAR* pStart = szPath + strlen(szCurPath) + 1;
		strPath = pStart;
	}
	else
	{
		// �޷�ȡ�����·����ֻ���þ���·����
		strPath = szPath;		
	}
}


//-----------------------------------------------------------------------------
// ��ʼ�����ű�
//-----------------------------------------------------------------------------
VOID CrashLog::InitDebugHelp()
{
	m_pLog->Create(CRASH_FILE_NAME);

	std::string strSymPath;
	this->GetSymbolPath(strSymPath);

	SymSetOptions(SYMOPT_DEFERRED_LOADS|SYMOPT_UNDNAME|SYMOPT_LOAD_LINES|SYMOPT_OMAP_FIND_NEAREST);

	m_hProcess = GetCurrentProcess();

	if( FALSE == SymInitialize(m_hProcess, (PSTR)strSymPath.c_str(), TRUE) )
	{
		typedef  BOOL (WINAPI *MYPROC1)(HANDLE, HMODULE*, DWORD, LPDWORD); 
		typedef DWORD (WINAPI *MYPROC2)(HANDLE, HMODULE, LPSTR, DWORD);

		MYPROC1 ProcEnumProcessModules;
		MYPROC2 ProcGetModuleFileNameEx;

		HMODULE hHandle = LoadLibraryA("psapi.dll");	// for EnumProcessModules
		if( NULL != hHandle )
		{
			ProcEnumProcessModules = (MYPROC1)GetProcAddress(hHandle, "EnumProcessModules");
			ProcGetModuleFileNameEx = (MYPROC2)GetProcAddress(hHandle, "GetModuleFileNameExA");
			if( NULL != ProcEnumProcessModules && NULL != ProcGetModuleFileNameEx )
			{
				// ���÷���Ѱ��·��
				SymSetSearchPath(m_hProcess, (PSTR)strSymPath.c_str());

				HMODULE hMods[1024];
				DWORD cbNeeded;
				// ��������ģ��ķ��ű�
				// Get a list of all the modules in this process.
				HANDLE hProcess = ::GetCurrentProcess();
				if( (ProcEnumProcessModules)(hProcess, hMods, sizeof(hMods), &cbNeeded) )
				{
					for( unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
					{
						CHAR szModName[MAX_PATH+1];

						// Get the full path to the module's file.
						if( (ProcGetModuleFileNameEx)( hProcess, hMods[i], szModName, sizeof(szModName)))
						{
							// load all modules' symbol
							SymLoadModule(m_hProcess, NULL, szModName, NULL, 0, 0);
						}
					}
				}
			}
		}
	}
}


//-----------------------------------------------------------------------------
// ͨ��������ʼ��ַ��ú�����
//-----------------------------------------------------------------------------
BOOL CrashLog::GetFunctionName(LPSTR szDest, PVOID pFunction)
{    
	CHAR SymbolBuffer[sizeof(IMAGEHLP_SYMBOL64)+1024];   
	DWORD64 dwTemp64 = 0;
	
	PIMAGEHLP_SYMBOL64 pSymbol = (PIMAGEHLP_SYMBOL64)SymbolBuffer;    
	memset(SymbolBuffer, 0, sizeof(SymbolBuffer));    
	pSymbol->SizeOfStruct = sizeof(SymbolBuffer);    
	pSymbol->MaxNameLength = 512;
	if( SymGetSymFromAddr64(m_hProcess, (DWORD64)pFunction, &dwTemp64, pSymbol) )
	{        
		IMAGEHLP_LINE64 l;
		ZeroMemory(&l, sizeof(l));
		l.SizeOfStruct = sizeof(l);

		// ��ȡԴ�ļ�����Դ������
		DWORD dwTemp = 0;
		if( SymGetLineFromAddr64(m_hProcess, (DWORD64)pFunction, &dwTemp, &l) )
		{
			sprintf(szDest, "%s() %s Line%lu", pSymbol->Name, ToFilePart(l.FileName), l.LineNumber);
		}
		else
		{
			strcpy_s(szDest, LONG_STRING, pSymbol->Name);
			strcat_s(szDest, LONG_STRING, "()");

			CHAR szModulePathName[MAX_PATH];
			ZeroMemory(szModulePathName, sizeof(szModulePathName));
			MEMORY_BASIC_INFORMATION MemInfo;

			// �跨ȡ�ú�������ģ������
			if( VirtualQuery(pFunction, &MemInfo, sizeof(MemInfo)) 
				&& (GetModuleFileNameA((HINSTANCE)MemInfo.AllocationBase, szModulePathName, MAX_PATH-2) > 0) )
			{
				strcat(szDest, " in ");
				strcat(szDest, ToFilePart(szModulePathName) );
			}
		}

		return TRUE;    
	}
	else	// �ò�����������
	{
		CHAR szModulePathName[MAX_PATH];
		ZeroMemory(szModulePathName, sizeof(szModulePathName));
		MEMORY_BASIC_INFORMATION MemInfo;

		// ����ò�����������,���跨ȡ�ú�������ģ������
		if( VirtualQuery(pFunction, &MemInfo, sizeof(MemInfo))
			&& (GetModuleFileNameA((HINSTANCE)MemInfo.AllocationBase, szModulePathName, MAX_PATH-2) > 0) )
		{
			strcpy_s( szDest, LONG_STRING, ToFilePart(szModulePathName) );
		}
		else
		{
			strcpy_s( szDest, LONG_STRING, "Unknown Function" );
		}
		return FALSE;
	}
}


//-----------------------------------------------------------------------------
// ��¼��ǰ����
//-----------------------------------------------------------------------------
INT CrashLog::DumpAll(EXCEPTION_POINTERS *pEx)
{
	static BOOL bFirstTime = TRUE;
	
	// ����ݹ����,�ݹ���ñ�ʾ�˺����������쳣
	if( !bFirstTime )
		return EXCEPTION_CONTINUE_SEARCH;
	bFirstTime = FALSE;

	HANDLE hFile = ::CreateFileA(DUMP_FILE_NAME, GENERIC_WRITE, 
		FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	MINIDUMP_EXCEPTION_INFORMATION info;
	info.ThreadId = GetCurrentThreadId();
	info.ExceptionPointers = pEx;
	info.ClientPointers = FALSE;
	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile,  MiniDumpNormal, 
		&info, NULL, NULL);

	::CloseHandle(hFile);

	// ע��������Щ�����в��ܶ�pEx��ָ�����������ݽ����޸�
	// �����̵���û����������
	DumpExceptionInfo(pEx);
	DumpSystemInfo();
	DumpCallStack(pEx->ContextRecord);
	DumpRegisters(pEx->ContextRecord);
	DumpInstructions(pEx->ContextRecord);
	DumpVariable(pEx->ContextRecord);


	// ʹϵͳֹͣ���������쳣ƥ��
	return EXCEPTION_EXECUTE_HANDLER;
}


//-----------------------------------------------------------------------------
// ��¼�쳣��������Ϣ
//-----------------------------------------------------------------------------
VOID CrashLog::DumpExceptionInfo(EXCEPTION_POINTERS *pEx)
{
	// ��¼���������û�����ʱ��
	CHAR szModuleName[MAX_PATH];
	ZeroMemory(szModuleName, sizeof(szModuleName));
	if( GetModuleFileNameA(0, szModuleName, MAX_PATH-2) <= 0 )
		strcpy_s(szModuleName, "Unknown");

	LPCSTR pszFilePart = ToFilePart(szModuleName);

	// Extract the file name portion and remove it's file extension
	CHAR szFileName[MAX_PATH];
	strcpy_s(szFileName, pszFilePart);

	CHAR szUserName[MAX_PATH];
	ZeroMemory(szUserName, sizeof(szUserName));
	DWORD UserNameSize = MAX_PATH-2;
	if( !GetUserNameA(szUserName, &UserNameSize) )
		strcpy_s(szUserName, "Unknown");

	FILETIME CurrentTime;
	GetSystemTimeAsFileTime(&CurrentTime);
	CHAR szTime[MAX_PATH];
	WORD wDate, wTime;
	if (FileTimeToLocalFileTime(&CurrentTime, &CurrentTime) &&
		FileTimeToDosDateTime(&CurrentTime, &wDate, &wTime))
	{
		sprintf(szTime, "%d/%d/%d %02d:%02d:%02d",
			(wDate / 32) & 15, wDate & 31, (wDate / 512) + 1980,
			(wTime >> 11), (wTime >> 5) & 0x3F, (wTime & 0x1F) * 2);
	}

	m_pLog->Write("%s run by %s in %s crashed.\r\n", szFileName, szUserName, szTime);

	// ��¼�쳣����
	PEXCEPTION_RECORD Exception = pEx->ExceptionRecord;
	PCONTEXT	      Context   = pEx->ContextRecord;
	m_pLog->Write("The program caused an exception:%s (0x%08x)\r\n",
				GetExceptionDescription(Exception->ExceptionCode), Exception->ExceptionCode);

	// ��¼�����쳣��ģ��
	CHAR szCrashModulePathName[MAX_PATH];
	ZeroMemory(szCrashModulePathName, sizeof(szCrashModulePathName));

	LPCSTR pszCrashModuleFileName = "Unknown";
	MEMORY_BASIC_INFORMATION MemInfo;

	// VirtualQuery can be used to get the allocation base associated with a
	// code address, which is the same as the ModuleHandle. This can be used
	// to get the filename of the module that the crash happened in.
	if( VirtualQuery((LPVOID)(SIZE_T)Context->Eip, &MemInfo, sizeof(MemInfo)) &&
		(GetModuleFileNameA((HINSTANCE)MemInfo.AllocationBase, szCrashModulePathName, MAX_PATH-2) > 0) )
	{
		pszCrashModuleFileName = ToFilePart(szCrashModulePathName);
	}

	m_pLog->Write("in module %s at %04x:%08x.\r\n",
				pszCrashModuleFileName, Context->SegCs, Context->Eip );

	// ����������ǷǷ�����,��¼�Ƕ�����д�����Լ���ͼ������λ��
	if( Exception->ExceptionCode == STATUS_ACCESS_VIOLATION &&
				Exception->NumberParameters >= 2)
	{
		CHAR szDebugMessage[1024];
		LPCSTR readwrite = "Access violation: read from";
		if( Exception->ExceptionInformation[0])
			readwrite = "Access violation: write to";
		sprintf(szDebugMessage, "%s location %08x\r\n",
					readwrite, Exception->ExceptionInformation[1]);

#ifdef	_DEBUG
		// Visual C++ ������ȱ�ٴ˹���,����Ҳ��������
		OutputDebugStringA("CrashLog handler: ");
		OutputDebugStringA(szDebugMessage);
#endif

		m_pLog->Write(szDebugMessage);
	}
}


//-----------------------------------------------------------------------------
// ��¼ϵͳ��Ϣ
//-----------------------------------------------------------------------------
VOID CrashLog::DumpSystemInfo()
{
	// ����¼��һ��CPU���ͺ�
	SYSTEM_INFO	SystemInfo;
	GetSystemInfo(&SystemInfo);
	m_pLog->Write("\r\n%d processor(s), type %d.\r\n",
				SystemInfo.dwNumberOfProcessors, SystemInfo.dwProcessorType);

	MEMORYSTATUS MemInfo;
	MemInfo.dwLength = sizeof(MemInfo);
	GlobalMemoryStatus(&MemInfo);

	// Print out info on memory, rounded up.
	m_pLog->Write("Physical memory: Total-%dMB Free-%dMB.\r\n",
		(MemInfo.dwTotalPhys + ONE_MEGA - 1) / ONE_MEGA,
		(MemInfo.dwAvailPhys + ONE_MEGA - 1) / ONE_MEGA );

	m_pLog->Write("Paging File: Total-%dMB Free-%dMB.\r\n",
		(MemInfo.dwTotalPageFile + ONE_MEGA - 1) / ONE_MEGA,
		(MemInfo.dwAvailPageFile + ONE_MEGA - 1) / ONE_MEGA );
}



//-----------------------------------------------------------------------------
// ��¼���ö�ջ
//-----------------------------------------------------------------------------
VOID CrashLog::DumpCallStack(PCONTEXT pContext)
{
	m_pLog->Write("\r\nCallStack:\r\n");

	STACKFRAME StackFrame;
	memset(&StackFrame, 0, sizeof(STACKFRAME));
	StackFrame.AddrPC.Mode = AddrModeFlat;    
	StackFrame.AddrPC.Offset = pContext->Eip;    
	StackFrame.AddrStack.Mode = AddrModeFlat;    
	StackFrame.AddrStack.Offset = pContext->Esp;    
	StackFrame.AddrFrame.Mode = AddrModeFlat;    
	StackFrame.AddrFrame.Offset = pContext->Ebp;

	BOOL bRet = TRUE;
	CHAR szName[1024];
	CHAR szTemp[1024];
	
	// StackWalk�����context,�����������Ǳ��븴��һ�ݸ�����
	// ��ΪCONTEXT�ṹ��û��ָ���Ա,���Կ��԰�ȫ����ȫ����
	CONTEXT context;
	memcpy( &context, pContext, sizeof(context) );

	for(INT nCount=0; nCount < 256 && bRet; nCount++)    
	{       
		// ���ݺ���
		bRet = StackWalk(IMAGE_FILE_MACHINE_I386, GetCurrentProcess(), 
			GetCurrentThread(), &StackFrame, &context, NULL, 
			SymFunctionTableAccess, SymGetModuleBase, NULL);

		if( bRet )
		{
			GetFunctionName(szName, (PVOID)(SIZE_T)StackFrame.AddrPC.Offset);
			sprintf(szTemp, "%s at %#08lx\r\n", szName, 
				StackFrame.AddrPC.Offset);

			m_pLog->Write(szTemp);
		}
	}
}


//-----------------------------------------------------------------------------
// ��¼�Ĵ���״̬
//-----------------------------------------------------------------------------
VOID CrashLog::DumpRegisters(PCONTEXT pContext)
{
	// Print out the register values in an XP error window compatible format.
	m_pLog->Write("\r\nContext:\r\n");
	m_pLog->Write("EDI:    0x%08x  ESI: 0x%08x  EAX:   0x%08x\r\n",
				pContext->Edi, pContext->Esi, pContext->Eax );
	m_pLog->Write("EBX:    0x%08x  ECX: 0x%08x  EDX:   0x%08x\r\n",
				pContext->Ebx, pContext->Ecx, pContext->Edx);
	m_pLog->Write("EIP:    0x%08x  EBP: 0x%08x  SegCs: 0x%08x\r\n",
				pContext->Eip, pContext->Ebp, pContext->SegCs);
	m_pLog->Write("EFlags: 0x%08x  ESP: 0x%08x  SegSs: 0x%08x\r\n",
				pContext->EFlags, pContext->Esp, pContext->SegSs);
				
}


//-----------------------------------------------------------------------------
// ��¼��ǰָ��
//-----------------------------------------------------------------------------
VOID CrashLog::DumpInstructions(PCONTEXT pContext)
{
	// �п���EIP�Ĵ���ָ����Ч��ַ,����������Ҫ���쳣����
	m_pLog->Write("Bytes at CS:EIP:  (XX means unaccessible)\r\n");
	LPBYTE pCode = (LPBYTE)(SIZE_T)pContext->Eip;
	for( int nCodeByte = 0; nCodeByte < MAX_CODE_BYTE_DUMP; nCodeByte++ )
	{
		__try
		{
			m_pLog->Write("%02x ", pCode[nCodeByte]);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			m_pLog->Write("XX ");
		}
	}
	
	m_pLog->Write("\r\n");
}


//-----------------------------------------------------------------------------
// ��¼����
//-----------------------------------------------------------------------------
VOID CrashLog::DumpVariable(PCONTEXT pContext)
{
	m_pLog->Write("\r\nVariable:");

	STACKFRAME StackFrame;
	memset(&StackFrame, 0, sizeof(STACKFRAME));
	StackFrame.AddrPC.Mode = AddrModeFlat;    
	StackFrame.AddrPC.Offset = pContext->Eip;    
	StackFrame.AddrStack.Mode = AddrModeFlat;    
	StackFrame.AddrStack.Offset = pContext->Esp;    
	StackFrame.AddrFrame.Mode = AddrModeFlat;    
	StackFrame.AddrFrame.Offset = pContext->Ebp;
	
	BOOL bRet = TRUE;
	CHAR szName[1024];
	CHAR szTemp[1024];
	
	// StackWalk�����context,�����������Ǳ��븴��һ�ݸ�����
	// ��ΪCONTEXT�ṹ��û��ָ���Ա,���Կ��԰�ȫ����ȫ����
	CONTEXT context;
	memcpy( &context, pContext, sizeof(context) );

	for(INT nCount=0; nCount < 256 && bRet; nCount++)    
	{       
		// ���ݺ���
		bRet = StackWalk(IMAGE_FILE_MACHINE_I386, m_hProcess, 
			GetCurrentThread(), &StackFrame, &context, NULL, 
			SymFunctionTableAccess, SymGetModuleBase, NULL);

		if( bRet )
		{
			BOOL bResult = GetFunctionName(szName, (PVOID)(SIZE_T)StackFrame.AddrPC.Offset);
			if( FALSE == bResult )
				continue;

			sprintf(szTemp, "\r\n%s at %#08lx\r\n", szName, StackFrame.AddrPC.Offset);
			m_pLog->Write(szTemp);

			IMAGEHLP_STACK_FRAME iStackFrame;
			ZeroMemory(&iStackFrame, sizeof(iStackFrame));
			iStackFrame.InstructionOffset = StackFrame.AddrPC.Offset;
			iStackFrame.StackOffset = StackFrame.AddrStack.Offset;
			iStackFrame.FrameOffset = StackFrame.AddrFrame.Offset;
			iStackFrame.ReturnOffset = StackFrame.AddrReturn.Offset;
			iStackFrame.BackingStoreOffset = StackFrame.AddrBStore.Offset;
			iStackFrame.FuncTableEntry = (ULONG64)StackFrame.FuncTableEntry;
			bResult = SymSetContext( m_hProcess, &iStackFrame, 0 );

			// ���ǽ�context�����ص�����,�Ա���ʹ��
			SymEnumSymbols( m_hProcess, 0, NULL, SymEnumSymbolsProc, &StackFrame );
		}
	}
}


//-----------------------------------------------------------------------------
// �ֲ�����ö�ٻص�����
//-----------------------------------------------------------------------------
BOOL CALLBACK CrashLog::SymEnumSymbolsProc(PSYMBOL_INFO pSym, ULONG dwSize, 
											 PVOID pContext)
{
    __try
    {
        if( FALSE == FormatSymbolValue(pSym, (STACKFRAME*)pContext) )
			m_pLog->Write("failed on formatting symbol %s\r\n", pSym->Name );
    }
    __except( 1 )
    {
        m_pLog->Write("kicked out at symbol %s\r\n", pSym->Name );
    }

    return TRUE;
}


//-----------------------------------------------------------------------------
// Given a SYMBOL_INFO representing a particular variable, displays its
// contents.  If it's a user defined type, display the members and their
// values.
//-----------------------------------------------------------------------------
BOOL CrashLog::FormatSymbolValue(PSYMBOL_INFO pSym, STACKFRAME * sf)
{
    // ��������
    if ( pSym->Tag == 5 )   // SymTagFunction from CVCONST.H from the DIA SDK
        return FALSE;

    DWORD dwVariable = 0;    // ָ�������ʵ������
    if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_REGRELATIVE )
    {
		dwVariable = sf->AddrFrame.Offset;
		dwVariable += (DWORD)pSym->Address;
    }
    else 
	{
		if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_REGISTER )
	        return FALSE;   // Don't try to report register variable
		else
            dwVariable = (DWORD)pSym->Address;
    }

    // �����ǲ������Ǳ���
    if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_PARAMETER )
        m_pLog->Write("\t-Parameter ");
    else if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_LOCAL )
        m_pLog->Write("\t-Local ");
	else
		m_pLog->Write("\t-Unknow data kind ");

	if( strcmp(pSym->Name, "cfg") == 0 )
	{
		int a3=0;
	}

	tagCrashSymbol symbol;
	DumpTypeIndex(symbol, pSym->ModBase, pSym->TypeIndex, 0);
	symbol.strName = pSym->Name;
	GetValueRecursive(symbol, dwVariable);
	PrintSym(symbol, 2);
    return true;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOL CrashLog::DumpTypeIndex(tagCrashSymbol& symOut, DWORD64 Base64, DWORD dwTypeIndex, INT nCycle)
{
	INT nResult = 0;
	if( nCycle > 3 )	// �ݹ����̫��
		return FALSE;

	symOut.dwTag = INVALID;
	SymGetTypeInfo(m_hProcess, Base64, dwTypeIndex, TI_GET_SYMTAG, &symOut.dwTag);
	if( 5 == symOut.dwTag )	// function
		return FALSE;

	// ������
	CHAR* pwszName;
	if( SymGetTypeInfo(m_hProcess, Base64, dwTypeIndex, TI_GET_SYMNAME, &pwszName) )
	{
		symOut.strName = pwszName;
		LocalFree( pwszName );
	}

	if( 7 == symOut.dwTag || 18 == symOut.dwTag )	// ����ת����
	{
		SymGetTypeInfo(m_hProcess, Base64, dwTypeIndex, TI_GET_TYPEID, &dwTypeIndex);
	}

	// �õ���С
	symOut.u64Size = INVALID;
	SymGetTypeInfo(m_hProcess, Base64, dwTypeIndex, TI_GET_LENGTH, &symOut.u64Size);

	symOut.dwTypeTag = INVALID;
	SymGetTypeInfo(m_hProcess, Base64, dwTypeIndex, TI_GET_SYMTAG, &symOut.dwTypeTag);

	if( 0xe == symOut.dwTypeTag )
	{
		symOut.bPointer = TRUE;	// ָ����Ҫ���������ҵ�ָ�������
		SymGetTypeInfo(m_hProcess, Base64, dwTypeIndex,	TI_GET_TYPEID, &dwTypeIndex);
	}
	if( 0xf == symOut.dwTypeTag )
	{
		symOut.bArray = TRUE;	// ����Ҳ��Ҫ���������ҵ����������
		SymGetTypeInfo(m_hProcess, Base64, dwTypeIndex,	TI_GET_TYPEID, &dwTypeIndex);
	}

	// �õ���������
	symOut.dwBasicType = INVALID;
	SymGetTypeInfo(m_hProcess, Base64, dwTypeIndex, TI_GET_BASETYPE, &symOut.dwBasicType);
	if( symOut.dwBasicType )
		symOut.strType = GetBasicType(symOut.dwBasicType);
	else if( SymGetTypeInfo(m_hProcess, Base64, dwTypeIndex, TI_GET_SYMNAME, &pwszName) )
	{
		symOut.strType = pwszName;
		LocalFree( pwszName );
	}

	// �õ�������С
	symOut.u64ElementSize = INVALID;
	SymGetTypeInfo(m_hProcess, Base64, dwTypeIndex, TI_GET_LENGTH, &symOut.u64ElementSize);


	// ����ж��ٸ���Ա
	DWORD dwChildrenCount = 0;
	SymGetTypeInfo(m_hProcess, Base64, dwTypeIndex, TI_GET_CHILDRENCOUNT, &dwChildrenCount);
	if( !dwChildrenCount )	// û�г�Ա
		return TRUE;

	// ������Ա
	struct FINDCHILDREN : TI_FINDCHILDREN_PARAMS
	{
		ULONG   MoreChildIds[1024];
		FINDCHILDREN(){Count = sizeof(MoreChildIds) / sizeof(MoreChildIds[0]);}
	} children;
	children.Count = dwChildrenCount;
	children.Start= 0;
	if( !SymGetTypeInfo(m_hProcess, Base64, dwTypeIndex, TI_FINDCHILDREN, &children ) )
		return TRUE;

	// ���ÿ����Ա
	for(DWORD i = 0; i < dwChildrenCount; i++)
	{
		tagCrashSymbol symChild;
		// �ݹ�������Ա
		if( DumpTypeIndex(symChild, Base64, children.ChildId[i], nCycle+1) )
		{
			// �õ���Ա����ڸ��ṹ��ƫ����
			symChild.dwOffset = INVALID;
			SymGetTypeInfo(m_hProcess, Base64, children.ChildId[i], TI_GET_OFFSET, &symChild.dwOffset);

			symOut.listChild.push_back(symChild);
		}
	}

	return TRUE;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
VOID CrashLog::PrintSym(tagCrashSymbol& sym, INT nTab)
{
	if( sym.bPointer )
		m_pLog->Write("%s* %s = %s(size:%I64d offset:%d)\r\n", sym.strType.c_str(), sym.strName.c_str(), sym.strValue.c_str(), sym.u64Size, sym.dwOffset);
	else if( sym.bArray )
		m_pLog->Write("%s %s[%I64d] = %s (size:%I64d offset:%d)\r\n", sym.strType.c_str(), sym.strName.c_str(), sym.u64Size/sym.u64ElementSize, sym.strValue.c_str(), sym.u64Size, sym.dwOffset);
	else
		m_pLog->Write("%s %s = %s (size:%I64d offset:%d)\r\n", sym.strType.c_str(), sym.strName.c_str(), sym.strValue.c_str(), sym.u64Size, sym.dwOffset);

	if( sym.strType[0] != 's' || sym.strType[1] != 't' || sym.strType[2] != 'd' || sym.strType[3] != ':' || sym.strType[4] != ':')
	{
		std::list<tagCrashSymbol>::iterator it;
		for(it=sym.listChild.begin(); it!=sym.listChild.end(); ++it)
		{
			for(INT n=0; n<nTab; n++)
				m_pLog->Write("\t");

			PrintSym(*it, nTab+1);
		}
	}
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
VOID CrashLog::GetValueRecursive(tagCrashSymbol& sym, ULONG64 u64Address)
{
	GetValue(sym, u64Address + sym.dwOffset);
//	if( sym.bPointer )
//		u64Address = *(DWORD*)u64Address;


	for(std::list<tagCrashSymbol>::iterator it=sym.listChild.begin(); it!=sym.listChild.end(); ++it)
	{
		GetValueRecursive(*it, u64Address + sym.dwOffset);
	}
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
VOID CrashLog::GetValue(tagCrashSymbol& sym, ULONG64 u64Address)
{
	CHAR szValue[HUGE_STRING] = {"unknow"};

	__try
	{
		switch( sym.dwBasicType )
		{
		case 1: // VOID
			sprintf(szValue, "0x%X", *(PDWORD)u64Address);
			break;

		case 2:	// CHAR
			if( sym.bPointer )
			{
				if( !IsBadStringPtrA( (LPSTR)u64Address, HUGE_STRING) )
					sprintf_s(szValue, "\"%s\"", *(PDWORD)u64Address );
				else
					sprintf_s(szValue, "x%X", *(PDWORD)u64Address );
			}
			break;

		case 3:	// WCHAR
			//if( sym.bPointer || sym.bArray )
			{
				//if( !IsBadStringPtr( (LPTSTR)(*(LPTSTR)u64Address), HUGE_STRING) )
				sprintf_s(szValue, "\"%s\"", *(PDWORD)u64Address );
				//else
				//	_stprintf(szValue, _T("x%X"), *(PDWORD)u64Address );
			}
			break;

		case 6:	// INT
			if( sym.bPointer )
			{
				sprintf(szValue, "0x%X", *(INT*)u64Address);
			}
			else if( sym.bArray )
			{
				sprintf(szValue, "%d", *(INT*)u64Address);
			}
			else 
			{
				sprintf(szValue, "%d", *(INT*)u64Address);
			}
			break;

		case 7:	// UINT
			if( sym.bPointer )
			{
				sprintf(szValue, "0x%X", *(DWORD*)u64Address);
			}
			else if( sym.bArray )
			{

			}
			else 
			{
				sprintf(szValue, "%u", *(DWORD*)u64Address);
			}
			break;

		case 8:	// FLOAT
			if( sym.bPointer )
			{
				sprintf(szValue, "0x%X", *(FLOAT*)u64Address);
			}
			else if( sym.bArray )
			{

			}
			else 
			{
				sprintf(szValue, "%lf", *(FLOAT*)u64Address);
			}
			break;
		case 10:	// bool
			if( sym.bPointer )
			{
				sprintf(szValue, "0x%X", *(PBYTE)u64Address);
			}
			else if( sym.bArray )
			{

			}
			else 
			{
				sprintf(szValue, "%d", *(PBYTE)u64Address);
			}
			break;
		case 13:	// long
			if( sym.bPointer )
			{
				sprintf(szValue, "0x%X", *(PLONG)u64Address);
			}
			else if( sym.bArray )
			{

			}
			else 
			{
				sprintf(szValue, "%l", *(PLONG)u64Address);
			}
			break;
		case 14:	// ulong
			if( sym.bPointer )
			{
				sprintf(szValue, "0x%X", *(PULONG)u64Address);
			}
			else if( sym.bArray )
			{

			}
			else 
			{
				sprintf(szValue, "%lu", *(PULONG)u64Address);
			}
			break;
		default:
			sprintf(szValue, "0x%X", *(PDWORD)u64Address);
			break;

/*		case 8:
			if( eType == EBT_Float )
				m_pLog->Write(" = %lf", *(double *)u64Address );
			else
				m_pLog->Write(" = 0x%I64X", *(DWORD64*)u64Address );*/
		}

		sym.strValue = szValue;
		
	}
	__except(1)
	{
		strcpy_s(szValue, "unable to get value");
	}
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
LPCSTR CrashLog::GetBasicType(DWORD dwBasicType)
{
	switch(dwBasicType)	// ���� CVCONST.H (Microsoft DIA 2.0 SDK)
	{
	case 0:
		return 0;
	case 1:
		return "VOID";
	case 2:
		return "CHAR";
	case 3:
		return "WCHAR";
	case 6:
		return "INT";
	case 7:
		return "UINT";
	case 8:
		return "FLOAT";
	case 9:
		return "BCD";
	case 10:
		return "bool";
	case 13:
		return "LONG";
	case 14:
		return "ULONG";
	case 30:
		return "BSTR";
	case 31:
		return "HRESULT";

	default:
		return "??";
	}
}


//-----------------------------------------------------------------------------
// �����쳣��õ��쳣�����ַ���
//-----------------------------------------------------------------------------
LPCSTR CrashLog::GetExceptionDescription(DWORD ExceptionCode)
{
	for (int i = 0; i < sizeof(ExceptionMap) / sizeof(ExceptionMap[0]); i++)
		if (ExceptionCode == ExceptionMap[i].ExceptionCode)
			return ExceptionMap[i].ExceptionName;

	return "Unknown CrashLog Type";
}


//-----------------------------------------------------------------------------
// ����CrashReport
//-----------------------------------------------------------------------------
VOID CrashLog::CallCrashReport()
{
	// ���ɺ��ʵ�log�ļ���
	CHAR szName[MAX_PATH], szPath[MAX_PATH];
	GetModuleFileNameA(NULL, szName, MAX_PATH);
	GetModuleFileNameA(NULL, szPath, MAX_PATH);

	// �õ�����
	CHAR* pResult = strrchr(szName, '\\');
	pResult = pResult ?	pResult+1 :	pResult = (CHAR*)szName;
	string strParam = pResult;	// EXE���ļ���
	strParam += " ";
	strParam += SS::ToA(TObj<Log>()->GetFileName());	// ����log�ļ���
	strParam += " ";	// ����svn�汾��
	strParam += SS::ToA(g_dwSVN);	// ����svn�汾��
	TObj<Log>()->Close();

	// �õ�Exe·��
	pResult = strrchr(szPath, '\\');
	pResult = pResult ?	pResult+1 :	pResult = (CHAR*)szName;
	*pResult = 0;

	ShellExecuteA(NULL, "open", "CrashReport.exe", strParam.c_str(), szPath, SW_SHOWNORMAL);
}




}

#pragma warning(default:4312)	// 'type cast' : conversion from 'DWORD' to 'LPVOID' of greater size
#endif