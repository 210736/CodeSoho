//-----------------------------------------------------------------------------
// author Lyp
// date 2004-04-03
// last 2010-02-24
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {

//-----------------------------------------------------------------------------
// INI文件对象
//-----------------------------------------------------------------------------
class ECORE_API IniLoader
{
public:
	BOOL	Load(LPCSTR szVFS, LPCWSTR szFileName, std::list<pair<wstring, wstring>>* pIDList=NULL);
	BOOL	Save(LPCWSTR szFileName);
	VOID	Clear() { m_map.clear(); m_mapKey.clear(); m_mapField.clear(); }

	// 得到指定键的内容
	INLINE LPCWSTR Get(LPCWSTR szKey, LPCWSTR szField=NULL, LPCWSTR szDefault=NULL);
	INLINE LPCWSTR Get(LPCWSTR szKey, INT nKeyPostfix, LPCWSTR szField=NULL, LPCWSTR szDefault=NULL);
	
	// 设置或者增加
	INLINE VOID Set(LPCWSTR szValue, LPCWSTR szKey, LPCWSTR szField=NULL);

	INLINE INT GetSize() { return (INT)m_mapKey.size(); }

private:
	TObj<VFS>					m_pVFS;
	TObj<DiskIO>				m_pDiskIO;
	std::map<UINT64, wstring>	m_map;
	std::map<DWORD, wstring>	m_mapKey;
	std::map<DWORD, wstring>	m_mapField;
};


//-----------------------------------------------------------------------------
// 得到指定键的内容
//-----------------------------------------------------------------------------
LPCWSTR IniLoader::Get(LPCWSTR szKey, LPCWSTR szField, LPCWSTR szDefault)
{
	DWORD dwKeyID = Crc32(szKey);
	DWORD dwFieldID = szField ? Crc32(szField) : 0;
	UINT64 id = ((UINT64)dwFieldID << 32) | dwKeyID;

	std::map<UINT64, wstring>::iterator it = m_map.find(id);
	if( it == m_map.end() )
		return szDefault;	// 找不到

	return it->second.c_str();
}

LPCWSTR IniLoader::Get(LPCWSTR szKey, INT nKeyPostfix, LPCWSTR szField, LPCWSTR szDefault)
{
	ASSERT(wcsnlen(szKey, SHORT_STRING) < SHORT_STRING - 3);
	ASSERT(nKeyPostfix < 1000 && nKeyPostfix >= 0);

	WCHAR szTmpKey[SHORT_STRING];
	swprintf_s(szTmpKey, L"%s%d", szKey, nKeyPostfix);

	return Get(szTmpKey, szField, szDefault);
}

//-----------------------------------------------------------------------------
// 设置或者增加
//-----------------------------------------------------------------------------
VOID IniLoader::Set(LPCWSTR szValue, LPCWSTR szKey, LPCWSTR szField)
{
	DWORD dwKeyID = Crc32(szKey);
	DWORD dwFieldID = szField ? Crc32(szField) : 0;
	UINT64 id = ((UINT64)dwFieldID << 32) | dwKeyID;

	wstring str = szValue;
	wstring strKey = szKey;
	wstring strField = szField;

	m_mapKey.erase(dwKeyID);
	m_mapKey.insert(std::make_pair(dwKeyID, strKey));

	m_mapField.erase(dwFieldID);
	m_mapField.insert(std::make_pair(dwFieldID, strField));

	m_map.erase(id);
	m_map.insert(std::make_pair(id, str));
}


}	// namespace ECore
