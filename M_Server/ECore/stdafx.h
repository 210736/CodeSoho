//-----------------------------------------------------------------------------
// File: stdafx
// Desc: ����Ԥ����ͷ�ļ�
// Auth: Lyp
// Date: 2003/11/30
//
//
// include file for standard system include files, or project specific 
// include files that are used frequently, but are changed infrequently
//-----------------------------------------------------------------------------
#pragma once

// ����һЩ����
#pragma warning(disable:4355)	// Level 3: "this": ���ڻ���Ա��ʼֵ�趨���б�
#pragma warning(disable:4251)	// Level 3: need to have dll-interface
#pragma warning(disable:4201)	// Level 4: nonstandard extension used : nameless struct/union
//#pragma warning(disable:4127)	// Level 4: conditional expression is constant
//#pragma warning(disable:4512)	// Level 4: assignment operator could not be generated
//#pragma warning(disable:4100)	// Level 4: unreferenced formal parameter
#pragma warning(disable:4996)

//-----------------------------------------------------------------------------
// ����ϵͳ����
//-----------------------------------------------------------------------------
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define _CRT_SECURE_NO_DEPRECATE
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

//#define X64 1

//#define InterlockedDecrementEx(_param) _InterlockedDecrement64((volatile LONG64*)(_param)) 
//#define InterlockedExchangeEx(_param, _param2) _InterlockedExchange64((volatile LONG64*)(_param), _param2) 
//#define InterlockedIncrementEx(_param) _InterlockedIncrement64((volatile LONG64*)(_param)) 

//#define InterlockedDecrementEx(_param) _InterlockedDecrement((volatile ULONGLONG*)(_param)) 
//#define InterlockedExchangeEx(_param, _param2) _InterlockedExchange((volatile ULONGLONG*)(_param), _param2) 
//#define InterlockedIncrementEx(_param) _InterlockedIncrement((volatile ULONGLONG*)(_param)) 
//-----------------------------------------------------------------------------
// ϵͳͷ�ļ�����
//-----------------------------------------------------------------------------
#define NOMINMAX
#include <windows.h>
#include <ctype.h>
#include <time.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <wininet.h>
#include <shlwapi.h>
#include <process.h>	// for _beginthreadex
#include <mmsystem.h>
#include <tchar.h>
#include <assert.h>
#include <eh.h>			// for "_set_se_translator"
#include <math.h>
#include <new.h>
#include <ShellAPI.h>
#include <DbgHelp.h>
#include <psapi.h>		// for EnumProcessModules
#include <tlhelp32.h>
#include "Shlwapi.h"
#include <Iphlpapi.h>	// for GetAdaptersInfo

#pragma comment(lib,"Iphlpapi.lib")
#pragma comment(lib,"Psapi.lib")

//------------------------------------------------------------------------------
// STLͷ�ļ�����
//------------------------------------------------------------------------------
//#define _STLP_VERBOSE_AUTO_LINK
//#define _STLP_DONT_USE_AUTO_LINK
#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include <list>
#include <algorithm>
#include <map>
#include <set>
#include <queue>
#include <stack>
#include <locale>
#include <limits>
#include <typeinfo>
using namespace std;


//------------------------------------------------------------------------------
// D3D���
//------------------------------------------------------------------------------
#include "../D3d/Include/D3D9.h"
#include "../D3d/Include/d3dx9math.h"
#pragma comment(lib,"E:/M_Server/D3d/Lib/x64/d3dx9.lib")

//-----------------------------------------------------------------------------
// ȫ�ֻ�������
//-----------------------------------------------------------------------------
#include "public_define.h"

//-----------------------------------------------------------------------------
// �������Ͷ���
//-----------------------------------------------------------------------------
#include "public_typedefine.h"	
#include "public_table.h"
#include "net_define.h"
#include "obj_helper.h"

//-----------------------------------------------------------------------------
// �ڴ�й©���
//-----------------------------------------------------------------------------
// #define MEM_TRACE


#include "error.h"
#include "mutex.h"
#include "map_ts.h"
#include "list_ts.h"

#ifdef MEM_TRACE
#include "mem_trace.h"
#endif 


//-----------------------------------------------------------------------------
// �����ͷŲ���
//-----------------------------------------------------------------------------
#ifndef SAFE_DEL
#define SAFE_DEL(p)				{ if(p) { delete(p);		(p) = NULL; } }
#endif
#ifndef SAFE_DEL_ARRAY
#define SAFE_DEL_ARRAY(p)		{ if(p) { delete[](p);		(p) = NULL; } }
#endif
#ifndef SAFE_FREE
#define SAFE_FREE(p)			{ if(p) { free(p);			(p) = NULL; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)			{ if(p) { (p)->Release();	(p) = NULL; } }
#endif
#ifndef SAFE_CLOSE_HANDLE
#define SAFE_CLOSE_HANDLE(h)	{ if(h) { CloseHandle(h);	(h) = NULL; } }
#endif

//-----------------------------------------------------------------------------
// ��ѧ��
//-----------------------------------------------------------------------------
#include "MathBaseDefine.h"
#include "Math.h"


//-----------------------------------------------------------------------------
// ��׼���ݽṹ
//-----------------------------------------------------------------------------
#include "fast_memory.h"
#include "mem_cache.h"
#include "mem_cache_ex.h"
#include "local_lock.h"
#include "condition.h"
#include "msg_queue.h"
#include "simple_map.h"
#include "simple_list.h"


#include "msg_queue_ts.h"
#include "fix_map.h"
#include "emap.h"
#include "binder.h"
#include "tinyXml.h"
#include "kernel_mutex.h"



//-----------------------------------------------------------------------------
// �ӿڹ���
//-----------------------------------------------------------------------------
#include "obj_mgr.h"

//-----------------------------------------------------------------------------
// ȫ�ֺ���
//-----------------------------------------------------------------------------
#include "public_function.h"

//----------------------------------------------------------------------------
// ���ò���
//----------------------------------------------------------------------------
#include "log.h"
#include "crc.h"
#include "md5.h"
#include "unicode.h"
#include "wan_define.h"
#include "FourCC.h"
#include "fast_code.h"
#include "color.h"
#include "super_string.h"
#include "transform.h"
#include "ini_loader.h"
#include "word_filter.h"

//-----------------------------------------------------------------------------
// ���涨��
//-----------------------------------------------------------------------------
extern HINSTANCE g_hInstance;


