#ifndef _MConsole_h
#define _MConsole_h
#include "stdafx.h"

#ifdef _DEBUG

#ifdef X64
#pragma comment(lib,"..\\vsout\\MConsole\\debug_x64\\MConsole.lib")
#else
#pragma comment(lib,"..\\vsout\\MConsole\\debug\\MConsole.lib")
#endif

#else
#ifdef X64
#pragma comment(lib,"..\\vsout\\MConsole\\release_X64\\MConsole.lib")
#else
#pragma comment(lib,"..\\vsout\\MConsole\\release\\MConsole.lib")
#endif

#endif

// 只需要到处公共接口，所以只包含这一个头文件
#include "mconsole_global.h"

#endif