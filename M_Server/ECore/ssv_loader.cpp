//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2004/04/02
// Last: 2004/11/17
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "ssv_loader.h"
#include "vfs.h"

namespace ECore {
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
VOID SSVLoader::Clear() 
{ 
	m_mapCrc.Clear();

	if(m_pMap)
	{
		for(SIZE_T n=0; n<m_pMap->Size(); ++n)
			MCFREE(m_pMap->PeekViaIndex(n));

		SAFE_DEL(m_pMap); 
	}

	if(m_pBuff) 
	{
		MCFREE(m_pBuff); 
		m_pBuff = 0;
	}
}


//-----------------------------------------------------------------------------
// ��ȡ�ļ�
//-----------------------------------------------------------------------------
BOOL SSVLoader::Load(LPCSTR szVFS, LPCSTR szFileName, WCHAR szSeparatedSym, std::list<LPCTSTR>& listID)
{
	if( szVFS )
		m_pVFS = szVFS;

	DWORD dwSize = m_pVFS->GetSize(szFileName);
	if( !VALID(dwSize) || dwSize < 2 )
		return FALSE;

	Clear();
	
	m_pBuff = (LPBYTE)MCALLOC(dwSize + sizeof(DWORD));
	if( m_pVFS->LoadFile(m_pBuff, szFileName) != dwSize ) 
		return FALSE;

	ASSERT( m_pBuff[0] == (BYTE)0xFF && m_pBuff[1] == (BYTE)0xFE );	// ȷ���� utf16
	*(DWORD*)(&m_pBuff[dwSize]) = 0L;	// �ļ�ĩβ��ֵ4��0

	//-----------------------------------------------------------------------------��ʼ����
	m_pMap = new FixMap<LPVOID, LPVOID*, 131072>;
	LPWSTR p = LPWSTR(m_pBuff + 2);	// ����microsoft bom

	// �ȶ���һ�б�ͷ
	for(INT nCol=0; *p != L'\n'; ++nCol )
	{
		LPWSTR szName = p;
		while( *p != szSeparatedSym && *p != L'\r' ) ++p;	// �ҵ��ָ���
		*p++ = 0;												// �����ַ�������

		m_mapCrc.Add(Crc32(szName), nCol);
	}

	// �������ʵ������
	while(*(++p))
	{
		// ����id��¼
		LPVOID* pNode = (LPVOID*)MCALLOC(sizeof(LPVOID)*MAX_COL);
		ZeroMemory(pNode, sizeof(LPVOID)*MAX_COL);

		m_pMap->Add(p, pNode);
		listID.push_back((LPCTSTR)p);

		for(INT nCol = 0; *p != L'\n'; ++nCol)	// ��һ��
		{
			ASSERT(nCol < MAX_COL);
			pNode[nCol] = p;
			while( *p != szSeparatedSym && *p != L'\r' ) ++p;		// �����ָ���
			*p++ = 0;
		}
	}

	return TRUE;
}



//-----------------------------------------------------------------------------
// ��ȡ�ļ�
//-----------------------------------------------------------------------------
BOOL SSVLoader::Load(LPCSTR szVFS, LPCSTR szFileName, CHAR szSeparatedSym, std::list<LPCSTR>& listID)
{
	if( szVFS )
		m_pVFS = szVFS;

	DWORD dwSize = m_pVFS->GetSize(szFileName);
	if( !VALID(dwSize) || dwSize < 2 )
		return FALSE;

	Clear();

	m_pBuff = (LPBYTE)MCALLOC(dwSize + sizeof(DWORD));
	if( m_pVFS->LoadFile(m_pBuff, szFileName) != dwSize ) 
		return FALSE;

	*(DWORD*)(&m_pBuff[dwSize]) = 0L;	// �ļ�ĩβ��ֵ4��0

	//-----------------------------------------------------------------------------��ʼ����
	m_pMap = new FixMap<LPVOID, LPVOID*, 131072>;
	LPSTR p = (LPSTR)m_pBuff;	// ����microsoft bom

	// �ȶ���һ�б�ͷ
	for(INT nCol=0; *p != '\n'; ++nCol )
	{
		LPSTR szName = p;
		while( *p != szSeparatedSym && *p != '\r' ) ++p;	// �ҵ��ָ���
		*p++ = 0;												// �����ַ�������

		m_mapCrc.Add(Crc32(szName), nCol);
	}

	// �������ʵ������
	while(*(++p))
	{
		// ����id��¼
		LPVOID* pNode = (LPVOID*)MCALLOC(sizeof(LPVOID)*MAX_COL);
		ZeroMemory(pNode, sizeof(LPVOID)*MAX_COL);

		m_pMap->Add(p, pNode);
		listID.push_back((LPCSTR)p);

		for(INT nCol = 0; *p != '\n'; ++nCol)	// ��һ��
		{
			ASSERT(nCol < MAX_COL);
			pNode[nCol] = p;
			while( *p != szSeparatedSym && *p != '\r' ) ++p;		// �����ָ���
			*p++ = 0;
		}
	}

	return TRUE;
}





} // namespace ECore {
