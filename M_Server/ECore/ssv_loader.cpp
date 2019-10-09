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
// 读取文件
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

	ASSERT( m_pBuff[0] == (BYTE)0xFF && m_pBuff[1] == (BYTE)0xFE );	// 确认是 utf16
	*(DWORD*)(&m_pBuff[dwSize]) = 0L;	// 文件末尾赋值4个0

	//-----------------------------------------------------------------------------开始解析
	m_pMap = new FixMap<LPVOID, LPVOID*, 131072>;
	LPWSTR p = LPWSTR(m_pBuff + 2);	// 跳过microsoft bom

	// 先读第一行表头
	for(INT nCol=0; *p != L'\n'; ++nCol )
	{
		LPWSTR szName = p;
		while( *p != szSeparatedSym && *p != L'\r' ) ++p;	// 找到分隔符
		*p++ = 0;												// 设置字符串结束

		m_mapCrc.Add(Crc32(szName), nCol);
	}

	// 读下面的实际数据
	while(*(++p))
	{
		// 加入id记录
		LPVOID* pNode = (LPVOID*)MCALLOC(sizeof(LPVOID)*MAX_COL);
		ZeroMemory(pNode, sizeof(LPVOID)*MAX_COL);

		m_pMap->Add(p, pNode);
		listID.push_back((LPCTSTR)p);

		for(INT nCol = 0; *p != L'\n'; ++nCol)	// 读一行
		{
			ASSERT(nCol < MAX_COL);
			pNode[nCol] = p;
			while( *p != szSeparatedSym && *p != L'\r' ) ++p;		// 跳过分隔符
			*p++ = 0;
		}
	}

	return TRUE;
}



//-----------------------------------------------------------------------------
// 读取文件
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

	*(DWORD*)(&m_pBuff[dwSize]) = 0L;	// 文件末尾赋值4个0

	//-----------------------------------------------------------------------------开始解析
	m_pMap = new FixMap<LPVOID, LPVOID*, 131072>;
	LPSTR p = (LPSTR)m_pBuff;	// 跳过microsoft bom

	// 先读第一行表头
	for(INT nCol=0; *p != '\n'; ++nCol )
	{
		LPSTR szName = p;
		while( *p != szSeparatedSym && *p != '\r' ) ++p;	// 找到分隔符
		*p++ = 0;												// 设置字符串结束

		m_mapCrc.Add(Crc32(szName), nCol);
	}

	// 读下面的实际数据
	while(*(++p))
	{
		// 加入id记录
		LPVOID* pNode = (LPVOID*)MCALLOC(sizeof(LPVOID)*MAX_COL);
		ZeroMemory(pNode, sizeof(LPVOID)*MAX_COL);

		m_pMap->Add(p, pNode);
		listID.push_back((LPCSTR)p);

		for(INT nCol = 0; *p != '\n'; ++nCol)	// 读一行
		{
			ASSERT(nCol < MAX_COL);
			pNode[nCol] = p;
			while( *p != szSeparatedSym && *p != '\r' ) ++p;		// 跳过分隔符
			*p++ = 0;
		}
	}

	return TRUE;
}





} // namespace ECore {
