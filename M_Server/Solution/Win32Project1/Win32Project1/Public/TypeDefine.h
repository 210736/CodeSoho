//
//  TypeDefine.h
//  test_CHOL1
//
//  Created by 杨家源 on 2017/7/16.
//  Copyright © 2017年 TNA. All rights reserved.
//

#ifndef TypeDefine_h
#define TypeDefine_h

#if (TARGET_PLATFORM != PLATFORM_WINDOWS)

#ifndef VOID
#define VOID void
typedef char CHAR;
typedef short SHORT;
typedef long LONG;
#endif

typedef signed char         INT8, *PINT8;
typedef signed short        INT16, *PINT16;
typedef signed int          INT32, *PINT32;
typedef signed long long    INT64, *PINT64;
typedef unsigned char       UINT8, *PUINT8;
typedef unsigned short      UINT16, *PUINT16;
typedef unsigned int        UINT32, *PUINT32;
typedef unsigned long long    UINT64, *PUINT64;

typedef signed char         INT8, *PINT8;
typedef signed short        INT16, *PINT16;
typedef signed int          INT32, *PINT32;
typedef signed long long    INT64, *PINT64;
typedef unsigned char       UINT8, *PUINT8;
typedef unsigned short      UINT16, *PUINT16;
typedef unsigned int        UINT32, *PUINT32;
typedef unsigned long long  UINT64, *PUINT64;

typedef signed int LONG32, *PLONG32;

#define CONST const;

typedef wchar_t WCHAR, *PWCHAR; // 类型, 指针
typedef const wchar_t *PCWCHAR; // 指向常量的指针
typedef const PWCHAR CPWCHAR;   // 常指针
typedef const PCWCHAR CPCWCHAR; // 指向常量的常指针

#endif


#endif /* TypeDefine_h */
