//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2004/04/02
// Last: 2004/11/17
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "xml_loader.h"

#include "super_string.h"
#include "disk_io.h"
#include "vfs.h"



#define XL_SKIP_WHITE(p) { while(*p==' ' || *p=='\n' || *p=='\r' || *p=='\t') ++p; }	// 跳过空格

namespace ECore {
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
VOID XmlLoader::Clear() 
{ 
	if(m_pMap)
	{
		for(SIZE_T n=0; n<m_pMap->Size(); ++n)
		{
			tagNode* pNode = m_pMap->PeekViaIndex(n);
			for(INT m=0; m<MAX_NODE && pNode[m].dwCrc!=0; ++m)
			{
				if( 0 == pNode[m].pValue )
					MCFREE(pNode[m].pString);
			}
			MCFREE(pNode);
		}

		delete(m_pMap); 
		m_pMap = 0;
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
BOOL XmlLoader::Load(LPCSTR szVFS, LPCSTR szFileName, std::list<LPCSTR>& listID)
{
	if( szVFS )	m_pVFS = szVFS;

	DWORD dwSize = m_pVFS->GetSize(szFileName);
	if( !VALID(dwSize) || dwSize == 0 )
		return FALSE;

	Clear();

	m_pMap = new FixMap<LPCSTR, tagNode*, 65536>;

	m_pBuff = (CHAR*)MCALLOC(dwSize + sizeof(DWORD));
	if( m_pVFS->LoadFile(m_pBuff, szFileName) != dwSize ) 
	{
		MCFREE(m_pBuff);
		return FALSE;
	}
	*(DWORD*)(&m_pBuff[dwSize]) = 0L;	// 赋值4个0

	if( m_pBuff[0] == (CHAR)0xEF && m_pBuff[1] == (CHAR)0xBB && m_pBuff[2] == (CHAR)0xBF )
		return Parse( m_pBuff+3, listID );	// 跳过bom
	else
		return Parse( m_pBuff, listID );
}


//-----------------------------------------------------------------------------
// 分析文件
//-----------------------------------------------------------------------------
BOOL XmlLoader::Parse(LPSTR p, std::list<LPCSTR>& listID)
{
	static DWORD dwIDCrc = Crc32("id");
	tagNode* pNode = 0;
	
	while(*p++)
	{
		if( *(DWORD*)p == '=di ')  // // 找到 id="??"
		{ 
			p += 5; // 跳过引号
			LPSTR szID = p;
			while( *p != '\"' ) ++p;	// 找到下一个引号
			*p++ = 0;					// 结束
	
			listID.push_back(szID);

			// 加入id记录
			tagNode* pNode = (tagNode*)MCALLOC(sizeof(tagNode)*MAX_NODE);
			pNode->dwCrc = dwIDCrc;
			pNode->pValue = szID;

			m_pMap->Add(szID, pNode);
			INT nNode = 0;

			while(1)
			{
				// 加入记录
				++nNode;
				ASSERT(nNode < MAX_NODE);

				XL_SKIP_WHITE(p);
				if( *p == '/' )
				{
					pNode[nNode].dwCrc = 0;				// 没属性了，做个标记
					break;								// 找下一个Element
				}

				LPSTR szName = p;						// 定位name
				while( *p != ' ' && *p != '=' ) ++p;	// 跳过name
				*p++ = 0;								// 结束
				while( *p++ != '\"' );					// 找到并跳过下一个引号

				BOOL bSpec = FALSE;
				pNode[nNode].pValue = p;				// 定位value
				while( *p != '\"' )
				{
					if( *p == '&' )
					{
						bSpec = TRUE;
						if( *(p+1) == '#' )	// 特殊字符
							*p++ = 0x18;
						else if( *(p+2) == 'm' ) *p++ = '&';	// { "&amp;",  5, '&' }
						else if( *(p+1) == 'l' ) *p++ = '<';	// { "&lt;",   4, '<' }
						else if( *(p+1) == 'g' ) *p++ = '>';	// { "&gt;",   4, '>' }
						else if( *(p+1) == 'q' ) *p++ = '\"';	// { "&quot;", 6, '\"' }
						else if( *(p+1) == 'a' ) *p++ = '\'';	// { "&apos;", 6, '\'' }
						while( *p != ';' ) *p++ = 0x18; // 找到结束符";"
						*p = 0x18;
					}
					++p;								// 找到下一个引号
				}
				*p++ = 0;								// 结束

				if( bSpec ) // 处理特殊字符,删除所有0x18
				{
					estring estr(pNode[nNode].pValue);
					estr.erase(remove(estr.begin(), estr.end(), 0x18), estr.end());
					size_t len = estr.length();
					pNode[nNode].pString = 0;
					if( len > 0 )
					{
						pNode[nNode].pString = (LPSTR)MCALLOC((DWORD)(len+sizeof(CHAR)));
						memcpy(pNode[nNode].pString, estr.c_str(), len+sizeof(CHAR));
					}
					pNode[nNode].pValue = 0;
				}

				if( pNode[nNode].pValue )	// 跳过空属性
				{
					if( pNode[nNode].pValue[0] == 0 )
					{
						--nNode;
						continue;
					}
				}
				else
				{
					if( pNode[nNode].pString == 0 || pNode[nNode].pString[0] == 0 )
					{
						--nNode;
						continue;
					}
				}
				pNode[nNode].dwCrc = Crc32(szName);		// 最后计算name的crc
			}
		}
	}

	return TRUE;
}


} // namespace ECore {
