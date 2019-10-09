//-----------------------------------------------------------------------------
// File: crc.h
// Desc: crc�㷨
// Auth: Lyp
// Date: 2003/11/13
//-----------------------------------------------------------------------------
#pragma once

namespace ECore{
namespace Reg{
//-----------------------------------------------------------------------------
// ���ע�����ָ����ֵ�Ƿ����
//-----------------------------------------------------------------------------
INLINE BOOL Check(HKEY key, LPCTSTR subKey)
{
	HKEY hKey;
	long ret0 = RegOpenKeyEx(key, subKey, 0, KEY_READ, &hKey);
	if ( ret0 == ERROR_SUCCESS )
	{
		RegCloseKey(hKey);
		return true;
	}

	RegCloseKey(hKey);
	return false;
}



//-----------------------------------------------------------------------------
// ��ָ����ֵд��ע�����ָ��·��,���ָ��·��û��,�򴴽���д��
//-----------------------------------------------------------------------------
INLINE BOOL Write(LPCTSTR KEY_ROOT, LPCTSTR KEY_NAME, LPCTSTR sz)
{
	if( KEY_ROOT == NULL || KEY_NAME == NULL ) 
		return FALSE;

	HKEY hKey;
	DWORD dwDisposition = REG_CREATED_NEW_KEY;

	long ret0 = 0;
	TCHAR buf[256]={0};
	if ( !Check(HKEY_LOCAL_MACHINE, KEY_ROOT) )
	{
		ret0 = RegCreateKeyEx(HKEY_LOCAL_MACHINE, KEY_ROOT, 0, NULL,
			REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
		if ( ret0 != ERROR_SUCCESS )
			return FALSE;

		RegCloseKey(hKey);
	}

	// -- д��
	RegOpenKeyEx( HKEY_LOCAL_MACHINE, KEY_ROOT, 0, KEY_WRITE, &hKey );
	ret0 = RegSetValueEx(hKey, KEY_NAME, NULL, REG_SZ, (const BYTE*)sz, lstrlen(sz)*sizeof(sz[0])+sizeof(sz[0]));
	if ( ret0 != ERROR_SUCCESS )
		return FALSE;

	RegCloseKey(hKey);
	return TRUE;
}



//-----------------------------------------------------------------------------
// ��ȡע�����ָ��·���ļ�ֵ
//-----------------------------------------------------------------------------
INLINE BOOL Read(LPCTSTR KEY_ROOT, LPCTSTR KEY_NAME, DWORD& key_value)
{
	HKEY hKey;
	long ret0 = RegOpenKeyEx( HKEY_LOCAL_MACHINE, KEY_ROOT, 0, KEY_QUERY_VALUE, &hKey );
	if( ret0 != ERROR_SUCCESS )
		return FALSE;

	DWORD dwBufLen=32;
	RegQueryValueEx( hKey, KEY_NAME, NULL, NULL, (LPBYTE)&key_value, &dwBufLen);
	RegCloseKey(hKey);
	return TRUE;
}


//-----------------------------------------------------------------------------
// ��ע���
//-----------------------------------------------------------------------------
INLINE BOOL Read(LPCTSTR KEY_ROOT, LPCTSTR KEY_NAME, LPTSTR outstr)
{
	HKEY hKey;
	long ret0 = RegOpenKeyEx(HKEY_LOCAL_MACHINE, KEY_ROOT, 0, KEY_READ, &hKey);
	if ( ret0 != ERROR_SUCCESS )
	{
		outstr[0] = 0;
		return FALSE;
	}
	DWORD dwLen = MAX_PATH, dwType = REG_SZ;
	RegQueryValueEx(hKey, KEY_NAME, NULL, &dwType, (LPBYTE)outstr, &dwLen);
	RegCloseKey(hKey);
	return TRUE;
}


}
} // namespace ECore{