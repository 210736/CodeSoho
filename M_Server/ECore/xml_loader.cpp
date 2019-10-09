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



#define XL_SKIP_WHITE(p) { while(*p==' ' || *p=='\n' || *p=='\r' || *p=='\t') ++p; }	// �����ո�

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
// ��ȡ�ļ�
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
	*(DWORD*)(&m_pBuff[dwSize]) = 0L;	// ��ֵ4��0

	if( m_pBuff[0] == (CHAR)0xEF && m_pBuff[1] == (CHAR)0xBB && m_pBuff[2] == (CHAR)0xBF )
		return Parse( m_pBuff+3, listID );	// ����bom
	else
		return Parse( m_pBuff, listID );
}


//-----------------------------------------------------------------------------
// �����ļ�
//-----------------------------------------------------------------------------
BOOL XmlLoader::Parse(LPSTR p, std::list<LPCSTR>& listID)
{
	static DWORD dwIDCrc = Crc32("id");
	tagNode* pNode = 0;
	
	while(*p++)
	{
		if( *(DWORD*)p == '=di ')  // // �ҵ� id="??"
		{ 
			p += 5; // ��������
			LPSTR szID = p;
			while( *p != '\"' ) ++p;	// �ҵ���һ������
			*p++ = 0;					// ����
	
			listID.push_back(szID);

			// ����id��¼
			tagNode* pNode = (tagNode*)MCALLOC(sizeof(tagNode)*MAX_NODE);
			pNode->dwCrc = dwIDCrc;
			pNode->pValue = szID;

			m_pMap->Add(szID, pNode);
			INT nNode = 0;

			while(1)
			{
				// �����¼
				++nNode;
				ASSERT(nNode < MAX_NODE);

				XL_SKIP_WHITE(p);
				if( *p == '/' )
				{
					pNode[nNode].dwCrc = 0;				// û�����ˣ��������
					break;								// ����һ��Element
				}

				LPSTR szName = p;						// ��λname
				while( *p != ' ' && *p != '=' ) ++p;	// ����name
				*p++ = 0;								// ����
				while( *p++ != '\"' );					// �ҵ���������һ������

				BOOL bSpec = FALSE;
				pNode[nNode].pValue = p;				// ��λvalue
				while( *p != '\"' )
				{
					if( *p == '&' )
					{
						bSpec = TRUE;
						if( *(p+1) == '#' )	// �����ַ�
							*p++ = 0x18;
						else if( *(p+2) == 'm' ) *p++ = '&';	// { "&amp;",  5, '&' }
						else if( *(p+1) == 'l' ) *p++ = '<';	// { "&lt;",   4, '<' }
						else if( *(p+1) == 'g' ) *p++ = '>';	// { "&gt;",   4, '>' }
						else if( *(p+1) == 'q' ) *p++ = '\"';	// { "&quot;", 6, '\"' }
						else if( *(p+1) == 'a' ) *p++ = '\'';	// { "&apos;", 6, '\'' }
						while( *p != ';' ) *p++ = 0x18; // �ҵ�������";"
						*p = 0x18;
					}
					++p;								// �ҵ���һ������
				}
				*p++ = 0;								// ����

				if( bSpec ) // ���������ַ�,ɾ������0x18
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

				if( pNode[nNode].pValue )	// ����������
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
				pNode[nNode].dwCrc = Crc32(szName);		// ������name��crc
			}
		}
	}

	return TRUE;
}


} // namespace ECore {
