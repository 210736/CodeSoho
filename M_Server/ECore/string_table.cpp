//-----------------------------------------------------------------------------
// author Lyp
// date 2008-05-27
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "string_table.h"

namespace ECore {
//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL StrTab::Init(LPCSTR szVFS, LPCWSTR szFileName)
{
	m_pBox = new IniLoader;
	if( !m_pBox )
		return FALSE;

	if( FALSE == m_pBox->Load(szVFS, szFileName) )
		return FALSE;

	m_strFileName = szFileName;
	return TRUE;
}


//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
VOID StrTab::Destroy()
{
	SAFE_DEL(m_pBox)
}


}

