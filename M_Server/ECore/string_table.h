//-----------------------------------------------------------------------------
// author Lyp
// date 2008-05-27
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class ECORE_API StrTab
{
public:
	virtual ~StrTab() { }

	BOOL Init(LPCSTR szVFS, LPCWSTR szFileName);
	VOID Destroy();

	LPCWSTR Get(LPCWSTR szKey) { return operator[](szKey); }
	INT GetSize() { return m_pBox->GetSize(); }

	virtual LPCWSTR operator[](LPCWSTR szKey) 
	{ 
		LPCWSTR szValue = m_pBox->Get(szKey);
		if( VALID(szValue) )
			return szValue; 

		m_strError = szKey;
		m_strError.append(L":NOT in:");
		m_strError.append(m_strFileName);
		return m_strError.c_str();
	}

	LPCWSTR UnSafeGet(LPCWSTR szKey)
	{
		return m_pBox->Get(szKey);
	}

protected:
	IniLoader*		m_pBox;
	wstring			m_strError;
	wstring			m_strFileName;

};
}