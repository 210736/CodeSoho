//
//  main.cpp
//  test_CHOL1
//
//  Created by 杨家源 on 2017/7/11.
//  Copyright © 2017年 TNA. All rights reserved.
//

#include "stdio.h"

#include "AppHeaders.h"

INT32_MAIN_INT32_argC_PCWCHAR_argV()
{
    printf("hello, world\n");
    for(int i = 0 ; i < argC; ++i)
    {
        wprintf(L"%ls\n",argV[i]);
		MessageBox(NULL, argV[i], L"1111", MB_ICONINFORMATION | MB_YESNO);
    }
    if (!argV[0]) return -1;
    
    return 0;
}
