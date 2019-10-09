//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2004/04/02
// Last: 2004/11/17
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {
//-----------------------------------------------------------------------------
// ��Ҫ������ȡ��άXML,ע������key nameΪ"id"
//-----------------------------------------------------------------------------
class ECORE_API XmlLoader
{
public:
	// ��ȡ
	BOOL Load(LPCSTR szVFS, LPCSTR szFileName, std::list<LPCSTR>& listID);
	
	// ��ȡԪ��
	INLINE LPCSTR Get(LPCSTR szKey, LPCSTR szField, LPCSTR szDefault);
	INLINE LPCSTR Get(DWORD dwNameCrc, LPCSTR szField, LPCSTR szDefault);
	INLINE LPCSTR Get(LPCSTR szName, INT nNamePostfix, LPCSTR szField, LPCSTR szDefault);

	INLINE VOID Clear();


	XmlLoader():m_pBuff(0),m_pMap(0) {}
	~XmlLoader(){Clear();}


private:
	TObj<DiskIO>	m_pDisk;
	TObj<VFS>		m_pVFS;
	LPSTR			m_pBuff;

	STATIC CONST DWORD MAX_NODE = 512;
	struct tagNode
	{
		DWORD		dwCrc;
		LPSTR		pValue;
		LPSTR		pString;
	};

	FixMap<LPCSTR, tagNode*, 65536>*	m_pMap;

	// �����ļ�
	BOOL Parse(LPSTR p, std::list<LPCSTR>& listID);
};


//-----------------------------------------------------------------------------
// ��ȡ�ַ���Ԫ��
//-----------------------------------------------------------------------------
LPCSTR XmlLoader::Get(LPCSTR szName, LPCSTR szField, LPCSTR szDefault)
{
	tagNode* pNode = m_pMap->Peek(szField);
	if( !VALID(pNode) )
		return szDefault;

	DWORD dwCrc = Crc32(szName);
	for(INT n=0; n<MAX_NODE && pNode[n].dwCrc!=0; ++n)
	{
		if( pNode[n].dwCrc == dwCrc )
		{
			if(pNode[n].pValue)
				return pNode[n].pValue;
			else
				return pNode[n].pString;
		}
	}
	
	return szDefault;
}

//-----------------------------------------------------------------------------
// ��ȡ�ַ���Ԫ��
//-----------------------------------------------------------------------------
LPCSTR XmlLoader::Get(LPCSTR szName, INT nNamePostfix, LPCSTR szField, LPCSTR szDefault)
{
	CHAR szTmpKey[SHORT_STRING];
	sprintf_s(szTmpKey, SHORT_STRING - 1, "%s%d", szName, nNamePostfix);

	return Get(szTmpKey, szField, szDefault);
}


//-----------------------------------------------------------------------------
// ��ȡ�ַ���Ԫ��
//-----------------------------------------------------------------------------
LPCSTR XmlLoader::Get(DWORD dwNameCrc, LPCSTR szField, LPCSTR szDefault)
{
	tagNode* pNode = m_pMap->Peek(szField);
	if( !VALID(pNode) )
		return szDefault;

	for(INT n=0; n<MAX_NODE && pNode[n].dwCrc!=0; ++n)
	{
		if( pNode[n].dwCrc == dwNameCrc )
		{
			if(pNode[n].pValue)
				return pNode[n].pValue;
			else
				return pNode[n].pString;
		}
	}
	
	return szDefault;
}



} // namespace ECore {
