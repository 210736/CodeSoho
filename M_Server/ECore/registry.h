//-----------------------------------------------------------------------------
// File: crc.h
// Desc: crc算法
// Auth: Lyp
// Date: 2003/11/13
//-----------------------------------------------------------------------------
#pragma once

namespace ECore{
namespace Reg{
//-----------------------------------------------------------------------------
// 检测注册表中指定键值是否存在
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
// 将指定键值写入注册表中指定路径,如果指定路径没有,则创建并写入
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

	// -- 写入
	RegOpenKeyEx( HKEY_LOCAL_MACHINE, KEY_ROOT, 0, KEY_WRITE, &hKey );
	ret0 = RegSetValueEx(hKey, KEY_NAME, NULL, REG_SZ, (const BYTE*)sz, lstrlen(sz)*sizeof(sz[0])+sizeof(sz[0]));
	if ( ret0 != ERROR_SUCCESS )
		return FALSE;

	RegCloseKey(hKey);
	return TRUE;
}



//-----------------------------------------------------------------------------
// 读取注册表中指定路径的键值
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
// 读注册表
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