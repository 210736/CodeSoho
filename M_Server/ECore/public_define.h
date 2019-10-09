//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2003/12/1
// Last: 2007/12/4
//-----------------------------------------------------------------------------
#pragma once

//------------------------------------------------------------------------------
// INLINE定义
//------------------------------------------------------------------------------
#ifndef INLINE
#define INLINE __forceinline
#endif

#define STATIC static

//------------------------------------------------------------------------------
// 基本字符串定义
//------------------------------------------------------------------------------
const INT32	EXTREME_STRING = 128*1024;
const INT32	HUGE_STRING	 = 512;
const INT32	LONG_STRING	 = 256;
const INT32	MID_STRING	 = 128;
const INT32 SHORT_STRING = 32;
const INT32 NAME_STRING	 = 8;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
#ifdef ECORE_EXPORTS
#define ECORE_API __declspec(dllexport)
#else
#define ECORE_API __declspec(dllimport)
#endif


//-----------------------------------------------------------------------------
// VALID定义
//-----------------------------------------------------------------------------
#define INVALID				(-1)
#define INVALID_POINTER		((LPVOID)(LONG_PTR)-1)
#define INVALID_HANDLE		(INVALID_HANDLE_VALUE)

INLINE ECORE_API bool VALID(BYTE n)		{ return n != BYTE(-1); }
INLINE ECORE_API bool VALID(WORD n)		{ return n != WORD(-1); }
INLINE ECORE_API bool VALID(INT32 n)	{ return n != (-1); }
INLINE ECORE_API bool VALID(DWORD n)	{ return INT32(n) != (-1); }
INLINE ECORE_API bool VALID(INT64 n)	{ return n != (-1); }
INLINE ECORE_API bool VALID(LPVOID n)	{ return (INVALID_POINTER != n) && (NULL != n); }
INLINE ECORE_API bool VALID(LPCVOID n)	{ return (INVALID_POINTER != n) && (NULL != n); }


//-----------------------------------------------------------------------------
// 文件系统
//-----------------------------------------------------------------------------
#define SEEK_SET				0
#define SEEK_CUR				1
#define SEEK_END				2


//-----------------------------------------------------------------------------
// 宽字节的__FILE__
//-----------------------------------------------------------------------------
#ifndef __TFILE__
#ifdef _UNICODE
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __TFILE__ WIDEN(__FILE__)
#else
#define __TFILE__ __FILE__
#endif
#endif

#ifndef __TDATE__
#ifdef _UNICODE
#define __TDATE__ WIDEN(__DATE__)
#else
#define __TDATE__ __DATE__
#endif
#endif

//-----------------------------------------------------------------------------
// 一些常用的宏
//-----------------------------------------------------------------------------
#define GET_DWORD(p) *((DWORD*)p)
