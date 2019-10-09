//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2004/04/02
// Last: 2004/11/17
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {
//-----------------------------------------------------------------------------
// ��Ҫ������ȡָ���ָ����Ķ�άunicode�ļ�,ע������key name����Ϊ"id"
//-----------------------------------------------------------------------------
class ECORE_API SSVLoader
{
public:
	// ��ȡ
	BOOL Load(LPCSTR szVFS, LPCSTR szFileName, WCHAR szSeparatedSym, std::list<LPCTSTR>& listID);
	BOOL Load(LPCSTR szVFS, LPCSTR szFileName, CHAR szSeparatedSym, std::list<LPCSTR>& listID);

	// ��ȡԪ��
	INLINE LPCWSTR Get(LPCWSTR szKey, LPCWSTR szField, LPCWSTR szDefault);
	INLINE LPCWSTR Get(DWORD dwNameCrc, LPCWSTR szField, LPCWSTR szDefault);
	INLINE LPCWSTR Get(LPCWSTR szName, INT nNamePostfix, LPCWSTR szField, LPCWSTR szDefault);
	INLINE LPCSTR Get(LPCSTR szKey, LPCSTR szField, LPCSTR szDefault);
	INLINE LPCSTR Get(DWORD dwNameCrc, LPCSTR szField, LPCSTR szDefault);
	INLINE LPCSTR Get(LPCSTR szName, INT nNamePostfix, LPCSTR szField, LPCSTR szDefault);


	VOID Clear();

	SSVLoader():m_pBuff(0),m_pMap(0) {}
	~SSVLoader(){Clear();}

private:
	TObj<VFS>		m_pVFS;
	LPBYTE			m_pBuff;

	STATIC CONST DWORD MAX_COL = 512;	// �������
	FixMap<DWORD, INT, MAX_COL>	m_mapCrc;	// ��keyCrc��Ӧ�±�
	FixMap<LPVOID, LPVOID*, 131072>*	m_pMap;
};


//-----------------------------------------------------------------------------
// ��ȡ�ַ���Ԫ��
//-----------------------------------------------------------------------------
LPCWSTR SSVLoader::Get(LPCWSTR szName, LPCWSTR szField, LPCWSTR szDefault)
{
	LPVOID pNode = m_pMap->Peek((LPVOID)szField);
	if( !VALID(pNode) )
		return szDefault;

	DWORD dwCrc = Crc32(szName);
	INT nCol = m_mapCrc.Peek(dwCrc);
	if( VALID(nCol) )
		return ((LPCWSTR*)pNode)[nCol];

	return szDefault;
}

//-----------------------------------------------------------------------------
// ��ȡ�ַ���Ԫ��
//-----------------------------------------------------------------------------
LPCWSTR SSVLoader::Get(LPCWSTR szName, INT nNamePostfix, LPCWSTR szField, LPCWSTR szDefault)
{
	WCHAR szTmpKey[SHORT_STRING];
	swprintf_s(szTmpKey, SHORT_STRING - 1, L"%s%d", szName, nNamePostfix);

	return Get(szTmpKey, szField, szDefault);
}

//-----------------------------------------------------------------------------
// ��ȡ�ַ���Ԫ��
//-----------------------------------------------------------------------------
LPCWSTR SSVLoader::Get(DWORD dwNameCrc, LPCWSTR szField, LPCWSTR szDefault)
{
	LPVOID pNode = m_pMap->Peek((LPVOID)szField);
	if( !VALID(pNode) )
		return szDefault;

	INT nCol = m_mapCrc.Peek(dwNameCrc);
	if( VALID(nCol) )
		return ((LPCWSTR*)pNode)[nCol];

	return szDefault;
}


//-----------------------------------------------------------------------------
// ��ȡ�ַ���Ԫ��
//-----------------------------------------------------------------------------
LPCSTR SSVLoader::Get(LPCSTR szName, LPCSTR szField, LPCSTR szDefault)
{
	LPVOID pNode = m_pMap->Peek((LPVOID)szField);
	if( !VALID(pNode) )
		return szDefault;

	DWORD dwCrc = Crc32(szName);
	INT nCol = m_mapCrc.Peek(dwCrc);
	if( VALID(nCol) )
		return ((LPCSTR*)pNode)[nCol];

	return szDefault;
}

//-----------------------------------------------------------------------------
// ��ȡ�ַ���Ԫ��
//-----------------------------------------------------------------------------
LPCSTR SSVLoader::Get(LPCSTR szName, INT nNamePostfix, LPCSTR szField, LPCSTR szDefault)
{
	CHAR szTmpKey[SHORT_STRING];
	sprintf_s(szTmpKey, SHORT_STRING - 1, "%s%d", szName, nNamePostfix);

	return Get(szTmpKey, szField, szDefault);
}

//-----------------------------------------------------------------------------
// ��ȡ�ַ���Ԫ��
//-----------------------------------------------------------------------------
LPCSTR SSVLoader::Get(DWORD dwNameCrc, LPCSTR szField, LPCSTR szDefault)
{
	LPVOID pNode = m_pMap->Peek((LPVOID)szField);
	if( !VALID(pNode) )
		return szDefault;

	INT nCol = m_mapCrc.Peek(dwNameCrc);
	if( VALID(nCol) )
		return ((LPCSTR*)pNode)[nCol];

	return szDefault;
}

} // namespace ECore {
