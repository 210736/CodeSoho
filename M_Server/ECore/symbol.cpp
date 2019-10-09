//-----------------------------------------------------------------------------
// author Lyp
// data 2004/3/1
// last 2010/1/26
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "symbol.h"

namespace ECore {
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
VOID Symbol::Init()
{
	std::string strSymPath;
	GetSymbolPath(strSymPath);

	SymSetOptions(SYMOPT_DEFERRED_LOADS|SYMOPT_UNDNAME|SYMOPT_LOAD_LINES|SYMOPT_OMAP_FIND_NEAREST);
	m_hProcess = GetCurrentProcess();
	
	SymInitialize(m_hProcess, (PSTR)strSymPath.c_str(), TRUE);

	CHAR szPath[MAX_PATH];
	GetModuleFileNameA(NULL, szPath, MAX_PATH);
	m_BaseOfDll = SymLoadModule64(m_hProcess, 0, szPath , szPath, (DWORD64)this, 0);
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
VOID Symbol::Destroy()
{
	SymUnloadModule64(m_hProcess, m_BaseOfDll);
	SymCleanup(m_hProcess);
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOL Symbol::AnalyseByName(tagSymbol& symOut, PSTR szName)
{
	ULONG64 buffer[(sizeof(SYMBOL_INFO)+MAX_SYM_NAME*sizeof(TCHAR))/sizeof(ULONG64)+1];
	PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
	pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	pSymbol->MaxNameLen = MAX_SYM_NAME;
		
	BOOL bResult = SymGetTypeFromName(m_hProcess, m_BaseOfDll, szName, pSymbol);
	if( !bResult )
		return FALSE;

	return DumpTypeIndex(symOut, pSymbol->ModBase, pSymbol->TypeIndex, 0);
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOL Symbol::DumpTypeIndex(tagSymbol& symOut, DWORD64 Base64, DWORD dwTypeIndex, INT nCycle)
{
	INT nResult = 0;
	if( nCycle > 4 )	// 递归次数太多
		return FALSE;

	symOut.dwTag = INVALID;
	SymGetTypeInfo(m_hProcess, Base64, dwTypeIndex, TI_GET_SYMTAG, &symOut.dwTag);
	if( 5 == symOut.dwTag )	// function
		return FALSE;

	// 符号名
	CHAR* pwszName;
	if( SymGetTypeInfo(m_hProcess, Base64, dwTypeIndex, TI_GET_SYMNAME, &pwszName) )
	{
		symOut.strName = pwszName;
		LocalFree( pwszName );
	}

	if( 7 == symOut.dwTag || 18 == symOut.dwTag )	// 名字转类型
	{
		SymGetTypeInfo(m_hProcess, Base64, dwTypeIndex, TI_GET_TYPEID, &dwTypeIndex);
	}

	// 得到大小
	symOut.u64Size = INVALID;
	SymGetTypeInfo(m_hProcess, Base64, dwTypeIndex, TI_GET_LENGTH, &symOut.u64Size);

	symOut.dwTypeTag = INVALID;
	SymGetTypeInfo(m_hProcess, Base64, dwTypeIndex, TI_GET_SYMTAG, &symOut.dwTypeTag);

	if( 0xe == symOut.dwTypeTag )
	{
		symOut.bPointer = TRUE;	// 指针需要继续往下找到指向的类型
		SymGetTypeInfo(m_hProcess, Base64, dwTypeIndex,	TI_GET_TYPEID, &dwTypeIndex);
	}
	if( 0xf == symOut.dwTypeTag )
	{
		symOut.bArray = TRUE;	// 数组也需要继续往下找到数组的类型
		SymGetTypeInfo(m_hProcess, Base64, dwTypeIndex,	TI_GET_TYPEID, &dwTypeIndex);
	}

	// 得到基础类型
	symOut.dwBasicType = INVALID;
	SymGetTypeInfo(m_hProcess, Base64, dwTypeIndex, TI_GET_BASETYPE, &symOut.dwBasicType);
	if( symOut.dwBasicType )
		symOut.strType = GetBasicType(symOut.dwBasicType);
	else if( SymGetTypeInfo(m_hProcess, Base64, dwTypeIndex, TI_GET_SYMNAME, &pwszName) )
	{
		symOut.strType = pwszName;
		LocalFree( pwszName );
	}

	// 得到基础大小
	symOut.u64ElementSize = INVALID;
	SymGetTypeInfo(m_hProcess, Base64, dwTypeIndex, TI_GET_LENGTH, &symOut.u64ElementSize);


	// 检查有多少个成员
	DWORD dwChildrenCount = 0;
	SymGetTypeInfo(m_hProcess, Base64, dwTypeIndex, TI_GET_CHILDRENCOUNT, &dwChildrenCount);
	if( !dwChildrenCount )	// 没有成员
		return TRUE;
	
	// 解析成员
	struct FINDCHILDREN : TI_FINDCHILDREN_PARAMS
	{
		ULONG   MoreChildIds[1024];
		FINDCHILDREN(){Count = sizeof(MoreChildIds) / sizeof(MoreChildIds[0]);}
	} children;
	children.Count = dwChildrenCount;
	children.Start= 0;
	if( !SymGetTypeInfo(m_hProcess, Base64, dwTypeIndex, TI_FINDCHILDREN, &children ) )
		return TRUE;

	// 检查每个成员
	for(DWORD i = 0; i < dwChildrenCount; i++)
	{
		tagSymbol symChild;
		// 递归搜索成员
		if( DumpTypeIndex(symChild, Base64, children.ChildId[i], nCycle+1) )
		{
			// 得到成员相对于父结构的偏移量
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
VOID Symbol::GetSymbolPath( std::string& strPath )
{
	CHAR szPath[MAX_PATH*2];
	GetModuleFileNameA(NULL, szPath, MAX_PATH);
	CHAR *pResult = strrchr(szPath, '\\');	// 截去路径
	if(pResult)
		*pResult = 0;
	sprintf_s(szPath, "%s\\symbol", szPath);	// 得到了绝对路径

	CHAR szCurPath[MAX_PATH*2];
	::GetCurrentDirectoryA(MAX_PATH*2-1, szCurPath);

	if( 0 == strcmp(szPath, szCurPath) )
	{
		// 当前路径既是符号表路径
		strPath = ".";
		return;
	}

	// 查找相对路径,因为绝对路径可能包含中文字符，而SymInitialize等函数无法识别中文字符
	pResult = strstr(szPath, szCurPath);	// 在全路径中寻找当前路径
	if( pResult && strlen(szPath) > strlen(szCurPath) )
	{
		// 得到相对路径
		CHAR* pStart = szPath + strlen(szCurPath) + 1;
		strPath = pStart;
	}
	else
	{
		// 无法取到相对路径，只能用绝对路径了
		strPath = szPath;		
	}	
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
LPCSTR Symbol::GetBasicType(DWORD dwBasicType)
{
	switch(dwBasicType)	// 来自 CVCONST.H (Microsoft DIA 2.0 SDK)
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




}