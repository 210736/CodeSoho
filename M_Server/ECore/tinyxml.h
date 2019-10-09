#pragma once

#include "Crc.h"
namespace ECore{
class XmlDocument;
class XmlElement;
class XmlComment;
class XmlAttribute;
class XmlText;
class XmlDeclaration;


class ECORE_API XmlBase : public MemCacheObj
{
	friend class XmlNode;
	friend class XmlElement;
	friend class XmlDocument;

public:
	virtual ~XmlBase() {}

	virtual VOID Print( FILE* cfile, INT depth ) = 0;
	virtual LPSTR Parse( LPSTR p ) { return 0; }

	// Expands entities in a string. Note this should not contian the tag's '<', '>', etc, or they will be transformed into entities!
	STATIC void EncodeString( const estring& str, estring* out );
};


// The parent class for everything in the Document Object Model. (Except for attributes).
// Nodes have siblings, a parent, and children. A node can be in a document, or stand on its own. The type of a XmlNode
// can be queried, and it can be cast to its more defined type.
class ECORE_API XmlNode : public XmlBase
{
	friend class XmlDocument;
	friend class XmlElement;

public:
	virtual ~XmlNode()
	{
		XmlNode* node = firstChild;
		XmlNode* temp = 0;
		while ( node )
		{
			temp = node;
			node = node->next;
			delete(temp);
		}
	}

	INLINE DWORD ID()					{ return dwID; }
	INLINE LPCSTR Value()				{ if(pValue) return pValue; else return value.c_str(); }
	INLINE void SetValue(LPCSTR _value)	{ pValue = 0; value = _value; dwID = Crc32(_value);	}

	// Delete all the children of this node. Does not affect 'this'.
	INLINE void Clear()
	{
		XmlNode* node = firstChild;
		XmlNode* temp = 0;

		while ( node )
		{
			temp = node;
			node = node->next;
			delete(temp);
		}	

		firstChild = 0;
		lastChild = 0;
		pValue = 0;
		value.clear();
		DWORD dwID = 0;
	}

	INLINE XmlNode* FirstChild() { return firstChild; }
	INLINE XmlNode* FirstChild( LPCSTR _value ) { return FirstChild(Crc32(_value)); }
	INLINE XmlNode* FirstChild( DWORD id )
	{
		for( XmlNode* node = firstChild; node; node = node->next )
			if ( node->ID() == id )
				return node;
		return 0;
	}

	// Add a new node related to this. Adds a child past the LastChild.
	// Returns a pointer to the new object or NULL if an error occured.
	INLINE XmlNode* InsertEndChild( XmlNode& addThis )
	{
		XmlNode* node = addThis.Clone();
		if ( !node )
			return 0;

		return LinkEndChild( node );
	}

	// Add a new node related to this. Adds a child past the LastChild.
	// NOTE: the node to be added is passed by pointer, and will be henceforth owned (and deleted) by tinyXml. This method is efficient
	// and avoids an extra copy, but should be used with care as it uses a different memory model than the other insert functions.
	INLINE XmlNode* LinkEndChild( XmlNode* node )
	{
		node->prev = lastChild;
		node->next = 0;

		if ( lastChild )
			lastChild->next = node;
		else
			firstChild = node;			// it was an empty list.

		lastChild = node;
		return node;
	}

	INLINE XmlNode* NextSibling() { return next; }

	// Navigate to a sibling node with the given 'pValue'.
	INLINE XmlNode* NextSibling( LPCSTR _value ) { DWORD id = Crc32(_value); return NextSibling(id); }

	INLINE XmlNode* NextSibling( DWORD id )
	{
		for( XmlNode* node = next; node; node = node->next )
			if ( node->ID() == id )
				return node;
		return 0;
	}

	// Convenience function to get through elements.
	// Calls NextSibling and ToElement. Will skip all non-Element
	// nodes. Returns 0 if there is not another element.
	INLINE XmlElement* NextSiblingElement()
	{
		for( XmlNode* node = NextSibling(); node; node = node->NextSibling() )
			if( node->ToElement() )	return node->ToElement();
		return 0;
	}

	// Convenience function to get through elements. Calls NextSibling and ToElement. Will skip all non-Element
	// nodes. Returns 0 if there is not another element.
	INLINE XmlElement* NextSiblingElement( LPCSTR _value )
	{
		DWORD id = Crc32(_value);
		for( XmlNode* node = NextSibling( id ); node; node = node->NextSibling( id ) )
			if ( node->ToElement() ) return node->ToElement();
		return 0;
	}

	INLINE XmlElement* NextSiblingElement( DWORD id )
	{
		for( XmlNode* node = NextSibling( id ); node; node = node->NextSibling( id ) )
			if ( node->ToElement() ) return node->ToElement();
		return 0;
	}

	// Convenience function to get through elements.
	INLINE XmlElement* FirstChildElement()
	{
		for( XmlNode* node = FirstChild(); node; node = node->NextSibling() )
			if( node->ToElement() )	return node->ToElement();
		return 0;
	}

	// Convenience function to get through elements.
	INLINE XmlElement* FirstChildElement( LPCSTR _value )
	{
		DWORD id = Crc32(_value);
		for( XmlNode* node = FirstChild( id ); node; node = node->NextSibling( id ) )
			if( node->ToElement() )	return node->ToElement();
		return 0;
	}

	INLINE XmlElement* FirstChildElement( DWORD id )
	{
		for( XmlNode* node = FirstChild( id ); node; node = node->NextSibling( id ) )
			if( node->ToElement() ) return node->ToElement();
		return 0;
	}

	virtual XmlElement*     ToElement()      { return 0; }
	virtual XmlText*        ToText()         { return 0; }

	// Create an exact duplicate of this node and return it. The memory must be deleted	by the caller. 
	virtual XmlNode* Clone() = 0;

protected:
	XmlNode():firstChild(0), lastChild(0), prev(0), next(0), dwID(0), pValue(0) {}
	XmlNode(LPCSTR _value):firstChild(0), lastChild(0), prev(0), next(0), dwID(0), pValue(0), value(_value) { dwID = Crc32(_value); }

	// Copy to the allocated object. Shared functionality between Clone, Copy constructor,
	// and the assignment operator.
	void CopyTo( XmlNode* target ) { if(pValue) target->SetValue(pValue); else target->SetValue(value.c_str()); }

	XmlNode*	firstChild;
	XmlNode*	lastChild;
	XmlNode*	prev;
	XmlNode*	next;

	DWORD		dwID;
	LPSTR		pValue;
	estring		value;
};


class ECORE_API XmlAttribute : public XmlBase
{
	friend class XmlAttributeSet;
	friend class XmlElement;

public:
	XmlAttribute():dwID(0), prev(0), next(0), pName(0), pValue(0) {}
	XmlAttribute(LPCSTR _name, LPCSTR _value): prev(0), next(0), pName(0), pValue(0), name(_name), value(_value) { dwID = Crc32(_name); }

	INLINE DWORD	ID() 			{ return dwID; }
	INLINE LPCSTR	Name()  		{ if(pName) return pName; else return name.c_str(); }
	INLINE LPCSTR	Value() 		{ if(pValue) return pValue; else return value.c_str(); }
	INLINE INT		IntValue()		{ if(pValue) return atoi(pValue); else return atoi(value.c_str()); }
	INLINE double	DoubleValue()	{ if(pValue) return atof(pValue); else return atof(value.c_str()); }

	INLINE VOID SetName(LPCSTR _name)		{ pName = 0; name = _name; dwID = Crc32(_name); }
	INLINE VOID SetValue(LPCSTR _value)	{ pValue = 0; value = _value; }

	/// Get the next sibling attribute in the DOM. Returns null at end.
	INLINE XmlAttribute* Next() { 	if( !next->pName && next->name.empty() ) return 0; else return next; }

	/// Get the previous sibling attribute in the DOM. Returns null at beginning.
	INLINE XmlAttribute* Previous() { 	if( !next->pName && next->name.empty() ) return 0; else return prev; }

	// Prints this Attribute to a FILE stream.
	virtual void Print( FILE* cfile, int depth );

	virtual LPSTR Parse( LPSTR p );

private:
	DWORD dwID;
	XmlAttribute*	prev;
	XmlAttribute*	next;

	LPSTR pName;
	LPSTR pValue;
	estring name;
	estring value;
};


class ECORE_API XmlText : public XmlNode
{
	friend class XmlElement;
public:
	// Constructor for text element. By default, it is treated as normal, encoded text. If you want it be output as a CDATA text
	// element, set the parameter _cdata to 'true'
	XmlText() {}
	XmlText(LPCSTR _value ):XmlNode(_value) {}
	XmlText( XmlText& copy ) { copy.CopyTo( this ); }
	INLINE void operator=( XmlText& base )	{ base.CopyTo( this ); }

	// Write this text object to a FILE stream.
	virtual void Print( FILE* cfile, int depth );

	virtual LPSTR Parse( LPSTR p );

	//< Cast to a more defined type. Will return null not of the requested type.
	virtual XmlText* ToText() { return this; } 

protected :
	// [internal use] Creates a new Element and returns it.
	virtual XmlNode* Clone()
	{
		XmlText* clone = 0;
		clone = new XmlText;

		if ( !clone )
			return 0;

		CopyTo( clone );
		return clone;
	}

	INLINE void CopyTo( XmlText* target ) { XmlNode::CopyTo( target ); }
	INLINE bool Empty() { return pValue ? pValue[0] == 0 : value.empty(); }	// returns true if all white space and new lines
};


//	A class used to manage a group of attributes.
//	It is only used internally, both by the ELEMENT and the DECLARATION.
//	The set can be changed transparent to the Element and Declaration
//	classes that use it, but NOT transparent to the Attribute
//	which has to implement a next() and previous() method. Which makes
//	it a bit problematic and prevents the use of STL.
//	This version is implemented with circular lists because:
//		- I like circular lists
//		- it demonstrates some independence from the (typical) doubly linked list.
class ECORE_API  XmlAttributeSet
{
	friend class XmlElement;
public:
	XmlAttributeSet() {	sentinel.next = &sentinel; sentinel.prev = &sentinel; }
	~XmlAttributeSet() { assert( sentinel.next == &sentinel ); assert( sentinel.prev == &sentinel ); }

	INLINE void Add( XmlAttribute* addMe )
	{
		addMe->next = &sentinel;
		addMe->prev = sentinel.prev;
		sentinel.prev->next = addMe;
		sentinel.prev = addMe;
	}

	INLINE void Remove( XmlAttribute* removeMe )
	{
		for( XmlAttribute* node = sentinel.next; node != &sentinel; node = node->next )
			if ( node == removeMe )
			{
				node->prev->next = node->next;
				node->next->prev = node->prev;
				node->next = 0;
				node->prev = 0;
				return;
			}
		assert( 0 );		// we tried to remove a non-linked attribute.
	}

	INLINE void Clear()
	{
		XmlAttribute* node;
		while( sentinel.next != &sentinel )
		{
			node = sentinel.next;
			sentinel.next = sentinel.next->next;
			delete(node);
		}
		sentinel.prev = &sentinel;
	}

	INLINE XmlAttribute* First()	{ return ( sentinel.next == &sentinel ) ? 0 : sentinel.next; }
	INLINE XmlAttribute* Last()	{ return ( sentinel.prev == &sentinel ) ? 0 : sentinel.prev; }

	INLINE XmlAttribute* Find( DWORD dwID )
	{
		for( XmlAttribute* node = sentinel.next; node != &sentinel; node = node->next )
			if ( node->dwID == dwID )
				return node;
		return 0;
	}

	INLINE XmlAttribute* Find( LPCSTR _name ) { DWORD dwID = Crc32(_name); return Find(dwID); }

private:
	XmlAttributeSet( XmlAttributeSet& );	// not allowed
	void operator=( XmlAttributeSet& );	// not allowed (as XmlAttribute)

	XmlAttribute sentinel;
};


// The element is a container class. It has a pValue, the element name,
// and can contain other elements, text, comments, and unknowns.
// Elements also contain an arbitrary number of attributes.
class ECORE_API XmlElement : public XmlNode
{
public:
	/// Construct an element.
	XmlElement(){}
	XmlElement(LPCSTR _value):XmlNode(_value) {}
	XmlElement(XmlElement& copy){ copy.CopyTo( this ); }

	INLINE void operator=( XmlElement& base ) { ClearThis(); base.CopyTo( this ); }

	virtual ~XmlElement() { ClearThis(); }

	// Given an attribute name, Attribute() returns the pValue for the attribute of that name, or null if none exists.
	INLINE LPCSTR Attribute( LPCSTR name ) { XmlAttribute* node = attributeSet.Find( name ); if( node ) return node->Value(); else return 0; }
	INLINE LPCSTR Attribute( DWORD dwID ) { XmlAttribute* node = attributeSet.Find( dwID ); if( node ) return node->Value(); else return 0; }
	INLINE LPCSTR AttributeSafe( LPCSTR name, LPCSTR defaultVal ) {	LPCSTR p = Attribute(name);	return p ? p : defaultVal; }
	INLINE LPCSTR AttributeSafe( DWORD dwID, LPCSTR defaultVal ) {	LPCSTR p = Attribute(dwID);	return p ? p : defaultVal; }

	// Sets an attribute of name to a given pValue. The attribute will be created if it does not exist, or changed if it does.
	INLINE void SetAttribute( LPCSTR cname, LPCSTR cvalue )
	{
		XmlAttribute* node = attributeSet.Find( cname );
		if( node )
		{
			node->SetValue( cvalue );
			return;
		}

		XmlAttribute* attrib = new XmlAttribute( cname, cvalue );
		if( attrib )
		{
			// Ð¹Â©¼ì²âÖúÊÖ
#ifdef MEM_TRACE
			SetMemoryTraceDesc(attrib, cname);
#endif
			attributeSet.Add( attrib );
		}
	}

	// Sets an attribute of name to a given pValue. The attribute will be created if it does not exist, or changed if it does.
	INLINE void SetAttribute( LPCSTR name, int val )
	{
		char buf[64];
		sprintf_s( buf, "%d", val );
		SetAttribute( name, buf );
	}

	INLINE XmlAttribute* FirstAttribute()	{ return attributeSet.First(); }
	INLINE XmlAttribute* LastAttribute()	{ return attributeSet.Last(); }

	INLINE LPCSTR GetText()
	{
		XmlNode* child = this->FirstChild();
		if ( child ) 
		{
			XmlText* childText = child->ToText();
			if ( childText )
				return childText->Value();
		}
		return 0;
	}

	// Creates a new Element and returns it - the returned element is a copy.
	virtual XmlNode* Clone()
	{
		XmlElement* clone = new XmlElement( Value() );
		if ( !clone )
			return 0;

		CopyTo( clone );
		return clone;
	}

	// Print the Element to a FILE stream.
	virtual void Print( FILE* cfile, int depth );

	//	Attribtue parsing starts: next char past '<'  returns: next char past '>'
	virtual LPSTR Parse( LPSTR p );

	virtual XmlElement* ToElement() { return this; } ///< Cast to a more defined type. Will return null not of the requested type.

	INLINE void ClearThis() { Clear(); attributeSet.Clear(); }// like clear, but initializes 'this' object as well

protected:
	void CopyTo( XmlElement* target )
	{
		XmlNode::CopyTo( target );
		for( XmlAttribute* attribute = attributeSet.First(); attribute;	attribute = attribute->Next() )
			target->SetAttribute( attribute->Name(), attribute->Value() );

		for( XmlNode* node = firstChild; node; node = node->NextSibling() )
			target->LinkEndChild( node->Clone() );
	}

private:
	XmlAttributeSet attributeSet;
};


class ECORE_API XmlComment : public XmlNode
{
public:
	/// Constructs an empty comment.
	XmlComment(){}
	/// Construct a comment from text.
	XmlComment( LPCSTR _value ):XmlNode(_value) {}
	XmlComment( XmlComment& copy ) { copy.CopyTo( this ); }
	INLINE void operator=( XmlComment& base ) { Clear(); base.CopyTo( this ); }

	virtual ~XmlComment() {}

	// Returns a copy of this Comment.
	virtual XmlNode* Clone() 
	{
		XmlComment* clone = new XmlComment();
		if ( !clone )
			return 0;

		CopyTo( clone );
		return clone;
	}

	// Write this Comment to a FILE stream.
	virtual void Print( FILE* cfile, int depth );

	// Attribtue parsing starts: at the ! of the !-- returns: next char past '>'
	virtual LPSTR Parse( LPSTR p );

protected:
	INLINE void CopyTo( XmlComment* target ) { XmlNode::CopyTo( target ); }

};


// In correct XML the declaration is the first entry in the file.
// <?xml version="1.0" standalone="yes"?>
// TinyXml will happily read or write files without a declaration,
// however. There are 3 possible attributes to the declaration:
// version, encoding, and standalone.
class ECORE_API XmlDeclaration : public XmlNode
{
public:
	/// Construct an empty declaration.
	XmlDeclaration(){}

	/// Construct.
	XmlDeclaration(LPCSTR _version, LPCSTR _encoding, LPCSTR _standalone):version(_version), encoding(_encoding), standalone(_standalone) {}


	XmlDeclaration( XmlDeclaration& copy ) { copy.CopyTo( this ); }
	void operator=( XmlDeclaration& copy ) { Clear(); copy.CopyTo( this ); }

	virtual ~XmlDeclaration()	{}

	INLINE LPCSTR Version()		{ return version.c_str (); }
	INLINE LPCSTR Encoding()		{ return encoding.c_str (); }
	INLINE LPCSTR Standalone()		{ return standalone.c_str (); }

	/// Creates a copy of this Declaration and returns it.
	virtual XmlNode* Clone() 
	{
		XmlDeclaration* clone = new XmlDeclaration();
		if( !clone )
			return 0;

		CopyTo( clone );
		return clone;
	}

	// Print this declaration to a FILE stream.
	virtual void Print( FILE* cfile, int depth );

	virtual LPSTR Parse( LPSTR p );

protected:
	INLINE void CopyTo( XmlDeclaration* target );

private:
	estring version;
	estring encoding;
	estring standalone;
};



// Always the top level node. A document binds together all the XML pieces. It can be saved, loaded, and printed to the screen.
// The 'pValue' of a document node is the xml file name.
class ECORE_API XmlDocument : public XmlNode
{
public:
	/// Create a document with a name. The name of the document is also the filename of the xml.
	XmlDocument():m_pBuff(0) {}
	XmlDocument( LPCSTR _value ):XmlNode(_value), m_pBuff(0) {}
	XmlDocument( XmlDocument& copy ):m_pBuff(0) { copy.CopyTo( this ); }
	INLINE void operator=( XmlDocument& copy ) { Clear(); copy.CopyTo( this ); }

	~XmlDocument() { if(m_pBuff) MCFREE(m_pBuff); }

	/// Load a file using the given filename. Returns true if successful.
	bool LoadXML(LPCSTR szVFSName, LPCSTR szFilename);
	bool LoadXML(VFS* pFS, LPCSTR szFilename);
	/// Save a file using the given filename. Returns true if successful.
	bool SaveFile( LPCSTR filename );
	
	virtual LPSTR Parse( LPSTR p );

	// Print this Document to a FILE stream.
	virtual void Print( FILE* cfile, int depth = 0 );

protected :
	// [internal use]
	virtual XmlNode* Clone()
	{
		XmlDocument* clone = new XmlDocument();
		if ( !clone )
			return 0;

		CopyTo( clone );
		return clone;
	}

private:
	INLINE void CopyTo( XmlDocument* target ) 
	{
		XmlNode::CopyTo( target );

		XmlNode* node = 0;
		for ( node = firstChild; node; node = node->NextSibling() )
		{
			target->LinkEndChild( node->Clone() );
		}	
	}

	LPSTR m_pBuff;
};



class ECORE_API XmlHandle
{
public:
	// Create a handle from any node (at any depth of the tree.) This can be a null pointer.
	XmlHandle( XmlNode* _node ):node(_node) {}

	// Return a handle to the first child element.
	INLINE XmlHandle FirstChildElement() 
	{
		if ( node )
		{
			XmlElement* child = node->FirstChildElement();
			if ( child )
				return XmlHandle( child );
		}
		return XmlHandle( 0 );
	}
	
	// Return a handle to the first child element with the given name.
	INLINE XmlHandle FirstChildElement( LPCSTR pValue )
	{
		if ( node )
		{
			XmlElement* child = node->FirstChildElement( pValue );
			if ( child )
				return XmlHandle( child );
		}
		return XmlHandle( 0 );
	}

	// Return the handle as a XmlNode. This may return null.
	INLINE XmlNode* ToNode() { return node; } 

	// Return the handle as a XmlElement. This may return null.
	INLINE XmlElement* ToElement() { return ( ( node && node->ToElement() ) ? node->ToElement() : 0 ); }

	// @deprecated use ToNode. 
	// Return the handle as a XmlNode. This may return null.
	INLINE XmlNode* Node() { return ToNode(); } 
	
	// @deprecated use ToElement. 
	// Return the handle as a XmlElement. This may return null.
	INLINE XmlElement* Element() { return ToElement(); }


private:
	XmlNode* node;
};


}