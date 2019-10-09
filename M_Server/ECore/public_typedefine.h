//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2003/12/11
// Last: 2004/03/14
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// 基本类型定义
//-----------------------------------------------------------------------------
#ifdef _UNICODE
typedef std::wstring		tstring;
typedef std::wstringstream	tstringstream;
typedef std::wstreambuf		tstreambuf;
typedef std::wifstream      tifstream;
typedef std::wofstream      tofstream;
typedef std::wfstream       tfstream;
#else
typedef std::string			tstring;
typedef std::stringstream	tstringstream;
typedef std::streambuf		tstreambuf;
typedef std::ifstream	    tifstream;
typedef std::ofstream		tofstream;
typedef std::fstream		tfstream;
#endif


namespace ECore{

	// 各种类的声明
	class Error;
	class DiskIO;
	class Filter;
	class INIObj;
	class ObjMgr;
	class Log;
	class MsgQueueTS;
	class XmlLoader;
	class VFS;
	class EPK;
	class Executor;
	class ThreadMgr;
	class Task;
	class DummyMutex;
	class Mutex;
	class AtomMutex;
	template<class DummyMutex> class MemCache;
	template<class Mutex> class MemCache;
	template<class AtomMutex> class MemCache;

	namespace Wan
	{
		class Server;
		class Client;
	}
	namespace Lan
	{
		class Server;
		class Client;
	}

}	// namespace ECore{


// 命令集(控制台使用)
//typedef std::vector<std::string> COMMANDS;


// To declare an interlocked function for use as an intrinsic,
// First, the function must be declared with the leading underscore.
// Second, the new function must appear in a #pragma intrinsic statement.
// For convenience, the intrinsic versions of the functions may be
// #define'd to appear in the source code without the leading underscore.
// Declare all of the supported interlocked intrinsic functions:
// compile with: /Oi /MT /D"_X86_"

#ifndef VOLATILE
#define VOLATILE volatile
#endif

#ifndef X64
extern "C"
{
	LONG  __cdecl _InterlockedIncrement(LONG VOLATILE *Addend);
	LONG  __cdecl _InterlockedDecrement(LONG VOLATILE *Addend);
	LONG  __cdecl _InterlockedCompareExchange(LPLONG VOLATILE Dest, LONG Exchange, LONG Comp);
	LONG  __cdecl _InterlockedExchange(LPLONG VOLATILE Target, LONG Value);
	LONG  __cdecl _InterlockedExchangeAdd(LPLONG VOLATILE Addend, LONG Value);

#if _WIN32_WINNT>=0x0502
	LONGLONG __cdecl _InterlockedIncrement64(LONGLONG VOLATILE *Addend);
	LONGLONG __cdecl _InterlockedDecrement64(LONGLONG VOLATILE *Addend);
	LONGLONG __cdecl _InterlockedCompareExchange64(PLONGLONG VOLATILE Dest, LONGLONG Exchange, LONGLONG Comp);
	LONGLONG __cdecl _InterlockedExchange64(PLONGLONG VOLATILE Target, LONGLONG Value);
	LONGLONG __cdecl _InterlockedExchangeAdd64(PLONGLONG VOLATILE Addend, LONGLONG Value);
#endif
}
#endif

#pragma intrinsic(memcpy, memset, strlen, strcpy, strcmp, strcat)

#pragma intrinsic (_InterlockedCompareExchange)
#define InterlockedCompareExchange _InterlockedCompareExchange

#pragma intrinsic (_InterlockedExchange)
#define InterlockedExchangeEx(_param, _param2) _InterlockedExchange((volatile ULONGLONG*)(_param), _param2)

#pragma intrinsic (_InterlockedExchangeAdd)
#define InterlockedExchangeAdd _InterlockedExchangeAdd

#pragma intrinsic (_InterlockedIncrement)
#define InterlockedIncrementEx(_param) _InterlockedIncrement((volatile ULONGLONG*)(_param)) 

#pragma intrinsic (_InterlockedDecrement)
#define InterlockedDecrementEx(_param) _InterlockedDecrement((volatile ULONGLONG*)(_param)) 

#if _WIN32_WINNT>=0x0502
#pragma intrinsic (_InterlockedCompareExchange64)
#define InterlockedCompareExchange64 _InterlockedCompareExchange64

#pragma intrinsic (_InterlockedExchange64)
#define InterlockedExchange _InterlockedExchange64 

#pragma intrinsic (_InterlockedExchangeAdd64)
#define InterlockedExchangeAdd _InterlockedExchangeAdd64

#pragma intrinsic (_InterlockedIncrement64)
#define InterlockedIncrement _InterlockedIncrement64

#pragma intrinsic (_InterlockedDecrement64)
#define InterlockedDecrement _InterlockedDecrement64
#endif