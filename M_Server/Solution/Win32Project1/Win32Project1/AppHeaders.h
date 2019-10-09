//
//  AppHeaders.h
//  test_CHOL1
//
//  Created by 杨家源 on 2017/7/16.
//  Copyright © 2017年 TNA. All rights reserved.
//

#ifndef AppHeaders_h
#define AppHeaders_h

#include "Platform/Platform.h"

#include <iostream>
#include <stdio.h>
#if (TARGET_PLATFORM == PLATFORM_WINDOWS)
#include <windows.h>
#include <tchar.h>
#elif (TARGET_PLATFORM == PLATFORM_LINUX)

#endif

#include "Public/TypeDefine.h"

#endif /* AppHeaders_h */
