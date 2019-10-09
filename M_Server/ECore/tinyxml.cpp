#include "stdafx.h"
#include "tinyxml.h"


#define SKIP_WHITE(p) { while(*p==' ' || *p=='\n' || *p=='\r' || *p=='\t') ++p; }

#include "vfs.h"
namespace ECore{
void XmlBase::EncodeString( const estring& str, estring* outString )
{
	int i=0;
	while( i<(int)str.length() )
	{
		unsigned char c = (unsigned char) str[i];

		if ( c == '&' 
		     && i < ( (int)str.length() - 2 )
			 && str[i+1] == '#'
			 && str[i+2] == 'x' )
		{
			// Hexadecimal character reference.
			// Pass through unchanged.
			// &#xA9;	-- copyright symbol, for example.
			//
			// The -1 is a bug fix from Rob Laveaux. It keeps
			// an overflow from happening if there is no ';'.
			// There are actually 2 ways to exit this loop -
			// while fails (error case) and break (semicolon found).
			// However, there is no mechanism (currently) for
			// this function to return an error.
			while ( i<(int)str.length()-1 )
			{
				outString->append( str.c_str() + i, 1 );
				++i;
				if ( str[i] == ';' )
					break;
			}
		}
		else if ( c == '&' )
		{
			outString->append( "&amp;", 5 );
			++i;
		}
		else if ( c == '<' )
		{
			outString->append( "&lt;", 4 );
			++i;
		}
		else if ( c == '>' )
		{
			outString->append( "&gt;", 4 );
			++i;
		}
		else if ( c == '\"' )
		{
			outString->append( "&quot;", 6 );
			++i;
		}
		else if ( c == '\'' )
		{
			outString->append( "&apos;", 6 );
			++i;
		}
		else if ( c < 32 )
		{
			// Easy pass at non-alpha/numeric/symbol, below 32 is symbolic.
			char buf[ 32 ];
			_snprintf_s( buf, sizeof(buf), "&#x%02X;", (unsigned) ( c & 0xff ) );
			outString->append( buf, (int)strlen( buf ) );
			++i;
		}
		else
		{
			*outString += (char)c;
			++i;
		}
	}
}


void XmlElement::Print( FILE* cfile, int depth )
{
	for( int i=0; i<depth; i++ ) 
		fprintf( cfile, "    " );

	fprintf( cfile, "<%s", pValue ? pValue : value.c_str() );

	for( XmlAttribute* attrib = attributeSet.First(); attrib; attrib = attrib->Next() )
	{
		fprintf( cfile, " " );
		attrib->Print( cfile, depth );
	}

	// There are 3 different formatting approaches:
	// 1) An element without children is printed as a <foo /> node
	// 2) An element with only a text child is printed as <foo> text </foo>
	// 3) An element with children is printed on multiple lines.
	XmlNode* node;
	if ( !firstChild )
	{
		fprintf( cfile, " />" );
	}
	else if ( firstChild == lastChild && firstChild->ToText() )
	{
		fprintf( cfile, ">" );
		firstChild->Print( cfile, depth + 1 );
		fprintf( cfile, "</%s>", pValue ? pValue : value.c_str() );
	}
	else
	{
		fprintf( cfile, ">" );

		for ( node = firstChild; node; node=node->NextSibling() )
		{
			if ( !node->ToText() )
			{
				fprintf( cfile, "\n" );
			}
			node->Print( cfile, depth+1 );
		}
		fprintf( cfile, "\n" );
		for( int i=0; i<depth; ++i ) {
			fprintf( cfile, "    " );
		}
		fprintf( cfile, "</%s>", pValue ? pValue : value.c_str() );
	}
}

bool XmlDocument::SaveFile( LPCSTR filename )
{
	// The old c stuff lives on...
	FILE* fp = fopen( filename, "w" );
	if ( fp )
	{
		fputc( 0xEFU, fp );
		fputc( 0xBBU, fp );
		fputc( 0xBFU, fp );

		Print( fp, 0 );
		bool result = (ferror(fp) == 0);
		fclose( fp );
		return result;
	}
	return false;
}


void XmlDocument::Print( FILE* cfile, int depth )
{
	for( XmlNode* node=FirstChild(); node; node=node->NextSibling() )
	{
		node->Print( cfile, depth );
		fprintf( cfile, "\n" );
	}
}


void XmlAttribute::Print( FILE* cfile, int /*depth*/ )
{
	estring n, v;

	if( pName )
		EncodeString( pName, &n );
	else
		EncodeString( name, &n );
	if( pValue )
		EncodeString( pValue, &v );
	else
		EncodeString( value, &v );

	fprintf (cfile, "%s=\"%s\"", n.c_str(), v.c_str() );
}



void XmlComment::Print( FILE* cfile, int depth )
{
	for ( int i=0; i<depth; i++ )
		fprintf( cfile,  "    " );

	fprintf( cfile, "<!--%s-->", pValue ? pValue : value.c_str() );
}


void XmlText::Print( FILE* cfile, int depth )
{
	estring buffer;
	if( pValue)
		EncodeString( pValue, &buffer );
	else
		EncodeString( value, &buffer );
	fprintf( cfile, "%s", buffer.c_str() );
}



void XmlDeclaration::Print( FILE* cfile, int /*depth*/ )
{
	fprintf( cfile, "<?xml " );

	if ( !version.empty() )
		fprintf (cfile, "version=\"%s\" ", version.c_str ());

	if ( !encoding.empty() ) 
		fprintf (cfile, "encoding=\"%s\" ", encoding.c_str ());

	if ( !standalone.empty() )
		fprintf (cfile, "standalone=\"%s\" ", standalone.c_str ());

	fprintf( cfile, "?>" );
}


void XmlDeclaration::CopyTo( XmlDeclaration* target )
{
	XmlNode::CopyTo( target );

	target->version = version;
	target->encoding = encoding;
	target->standalone = standalone;
}




bool XmlDocument::LoadXML(VFS* pFS, LPCSTR szFilename )
{

	DWORD dwSize = pFS->GetSize(szFilename);
	if( !VALID(dwSize) || dwSize == 0 )
		return false;

	// Delete the existing data:
	Clear();

	if( m_pBuff )
		MCFREE(m_pBuff);
	m_pBuff = (CHAR*)MCALLOC(dwSize + sizeof(DWORD));
	if( pFS->LoadFile(m_pBuff, szFilename) != dwSize ) 
	{
		MCFREE(m_pBuff);
		return false;
	}
	*(DWORD*)(&m_pBuff[dwSize]) = 0L;

	if( m_pBuff[0] == (CHAR)0xEF && m_pBuff[1] == (CHAR)0xBB && m_pBuff[2] == (CHAR)0xBF )
		Parse( m_pBuff+3 );	// 跳过bom
	else
		Parse( m_pBuff );
	
	return true;
}


bool XmlDocument::LoadXML(LPCSTR szVFSName, LPCSTR szFilename )
{
	if( szVFSName )
		return LoadXML(TObj<VFS>(szVFSName),szFilename);
	else
		return LoadXML(TObj<VFS>("VFS"),szFilename);
}


LPSTR XmlDocument::Parse( LPSTR p )
{
    SKIP_WHITE( p );
	while( *p )
	{
		SKIP_WHITE( p );
		XmlNode* node;
		if( *(p+1) == '?' )	// - Decleration: <?xml
			node = new XmlDeclaration();
		else if( *(p+1) == '!' )	// - Comments: <!--
			node = new XmlComment();
		else
			node = new XmlElement;

		p = node->Parse( p );
		LinkEndChild( node );
		SKIP_WHITE( p );
	}

	return p;
}



LPSTR XmlElement::Parse( LPSTR p )
{
	SKIP_WHITE( p );
	++p;	// 跳过<
	SKIP_WHITE( p );

	// Read the pName.
	pValue = p;
	while(	*p && *p != ' ' && *p != '>' && *p != '=' && *p != '/' )
		++p;
	SIZE_T stValueLen = p - pValue;

	BOOL bTruncated = (*p == '>' ? 1 : 0);	// 大于号将被0消掉
	BOOL bEnd = (*p == '/' ? 1 : 0);	// 斜杠将被0消掉
	*p++ = 0;
	dwID = Crc32(pValue);

	if( bEnd )	// "/>" Empty tag.
		return ++p;

	// Check for and read attributes. Also look for an empty tag or an end tag.
	while( *p )
	{
		SKIP_WHITE( p );
		if( *p == '/' )	// "/>" Empty tag.
			return p+2;

		if( *p == '>' || bTruncated )	// Attrib处理完毕，检查子节点
		{
			p += bTruncated ? 0 : 1;
			// Read the value -- which can include other elements -- read the end tag, and return.
			while( *p )
			{
				SKIP_WHITE(p);
				if( *p == '<' )
				{
					if( *(p+1) == '/' )
						break;

					XmlNode* node;
					if( *(p+1) == '!' )	// - Comments: <!--
						node = new XmlComment();
					else
						node = new XmlElement;

					p = node->Parse( p );
					LinkEndChild( node );
				}
				else
				{
					// Take what we have, make a text element.
					XmlText* textNode = new XmlText;
					p = textNode->Parse( p );
					if ( !textNode->Empty() )
						LinkEndChild( textNode );
					else
						delete textNode;
					if( *p == '/' )
						break;
				}
			}
			return p + stValueLen + ( *p == '/' ? 2 : 3);	// We should find the end tag now </ + value + >
		}

		// Try to read an attribute:
		XmlAttribute* pAttrib = new XmlAttribute();

		pAttrib->pName = p;
		while( *p != '=' && *p != ' ' )	++p;
		*p++ = 0;
		pAttrib->dwID = Crc32(pAttrib->pName);

		SKIP_WHITE( p );
		if( *p == '=' )	++p; // 跳过等号
		SKIP_WHITE( p );
		++p; // 跳过引号
		pAttrib->pValue = p;

		BOOL bSpec = FALSE;
		while( *p != '\"' )
		{
			if( *p == '&' )
			{
				bSpec = TRUE;
				if( *(p+1) == '#' )	*p++ = 0x18; // 特殊字符
				else if( *(p+2) == 'm' ) *p++ = '&';	// { "&amp;",  5, '&' }
				else if( *(p+1) == 'l' ) *p++ = '<';	// { "&lt;",   4, '<' }
				else if( *(p+1) == 'g' ) *p++ = '>';	// { "&gt;",   4, '>' }
				else if( *(p+1) == 'q' ) *p++ = '\"';	// { "&quot;", 6, '\"' }
				else if( *(p+1) == 'a' ) *p++ = '\'';	// { "&apos;", 6, '\'' }
				while( *p != ';' )	*p++ = 0x18; // 找到结束符";"
				*p = 0x18;
			}
			++p;
		}                        

		*p++ = 0;
		if( bSpec )	// 处理特殊字符,删除所有0x18
		{
			pAttrib->value = pAttrib->pValue;
			pAttrib->value.erase(remove(pAttrib->value.begin(), pAttrib->value.end(), 0x18), pAttrib->value.end());
			pAttrib->pValue = 0;
		}

		pAttrib->next = &attributeSet.sentinel;
		pAttrib->prev = attributeSet.sentinel.prev;
		attributeSet.sentinel.prev->next = pAttrib;
		attributeSet.sentinel.prev = pAttrib;
	}
	return p;
}


LPSTR XmlComment::Parse( LPSTR p )
{
	SKIP_WHITE( p );
	p += 4; // "<!--"

	pValue = p;
	while( !(*p == '-' && *(p+1) == '-' && *(p+2) == '>' ) ) ++p;	// "-->"
	*p = 0;
	return p + 3;	// "-->"
}


LPSTR XmlText::Parse( LPSTR p )
{
	pValue = p;

	// Keep all the white space.
	BOOL bSpec = FALSE;
	while(	*p && *p != '<' )
	{
		if( *p == '&' )
		{
			bSpec = TRUE;
			if( *(p+1) == '#' )	// 特殊字符
				*p++ = 0x18;
			else if( *(p+2) == 'm' || *(p+2) == 'M' ) *p++ = '&';	// { "&amp;",  5, '&' }
			else if( *(p+1) == 'l' || *(p+1) == 'L' ) *p++ = '<';	// { "&lt;",   4, '<' }
			else if( *(p+1) == 'g' || *(p+1) == 'G' ) *p++ = '>';	// { "&gt;",   4, '>' }
			else if( *(p+1) == 'q' || *(p+1) == 'Q' ) *p++ = '\"';	// { "&quot;", 6, '\"' }
			else if( *(p+1) == 'a' || *(p+1) == 'A' ) *p++ = '\'';	// { "&apos;", 6, '\'' }
			while( *p != ';' ) *p++ = 0x18; // 找到结束符";"
			*p = 0x18;
		}
		++p;
	}                        
	
	*p = 0;
	if( bSpec ) // 处理特殊字符,删除所有0x18
	{
		value = pValue;
		value.erase(remove(value.begin(), value.end(), 0x18), value.end());
		pValue = 0;
	}
	return ++p;	// warningg! we truncated the '<'
}


LPSTR XmlDeclaration::Parse( LPSTR p )
{
	SKIP_WHITE( p );

	p += 5;	// "<?xml"
	while( *p )
	{
		if( *p == '>' )
			return ++p;

		SKIP_WHITE( p );
		if( *p == 'v' || *p == 'V' ) // "version"
		{
			XmlAttribute attrib;
			p = attrib.Parse( p );		
			version = attrib.Value();
		}
		else if( *p == 'e' || *p == 'E' ) // "encoding"
		{
			XmlAttribute attrib;
			p = attrib.Parse( p );		
			encoding = attrib.Value();
		}
		else if( *p == 's' || *p == 'S' ) // "standalone"
		{
			XmlAttribute attrib;
			p = attrib.Parse( p );		
			standalone = attrib.Value();
		}
		else
		{
			// Read over whatever it is.
			while( *p != '>' && !( *p == ' ' || *p == '\n' || *p == '\r' || *p == '\t' ) )
				++p;
		}
	}
	return p;
}


LPSTR XmlAttribute::Parse( LPSTR p )
{
	// Read the pName, the '=' and the value.
	pName = p;
	while(	*p != ' ' && *p != '>' && *p != '='  ) ++p;
	*p++ = 0;
	dwID = Crc32(pName);

	SKIP_WHITE( p );
	if( *p == '=' )	++p; // 跳过等号
	SKIP_WHITE( p );
	++p; // 跳过引号
	
	pValue = p;
	while( *p != '\"' ) ++p;	// Keep all the white space.
	
	*p = 0;
	return ++p;
}
}