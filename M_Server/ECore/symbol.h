//-----------------------------------------------------------------------------
// author Lyp
// data 2004/3/1
// last 2010/1/26
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {

struct ECORE_API tagSymbol
{
	string					strName;
	string					strType;
	BOOL					bPointer;
	BOOL					bArray;
	DWORD					dwOffset;
	ULONG64					u64Size;
	ULONG64					u64ElementSize;

	DWORD					dwTag;
	DWORD					dwTypeTag;
	DWORD					dwBasicType;


	std::list<tagSymbol>	listChild;
	tagSymbol():bPointer(0),bArray(0),dwOffset(0),u64Size(0),u64ElementSize(0),dwTag(0),dwTypeTag(0),dwBasicType(0){}
};


//-----------------------------------------------------------------------------
// 符号解析
//-----------------------------------------------------------------------------
class ECORE_API Symbol
{
public:
	VOID Init();
	VOID Destroy();

	BOOL AnalyseByName(tagSymbol& symOut, PSTR szName);
	BOOL AnalyseByAddress(tagSymbol& symOut, LPVOID pAddress);




private:
	HANDLE	m_hProcess;
	DWORD64	m_BaseOfDll;


	BOOL DumpTypeIndex(tagSymbol& symOut, DWORD64 Base64, DWORD dwTypeIndex, INT nCycle);

	// 计算Symbol路径
	VOID GetSymbolPath(std::string& strPath);

	// 得到类型名
	LPCSTR GetBasicType(DWORD dwBasicType);
};



} // namespace ECore {
