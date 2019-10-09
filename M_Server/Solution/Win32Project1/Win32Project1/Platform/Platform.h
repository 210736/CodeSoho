//
//  Platform.h
//  test_CHOL1
//
//  Created by 杨家源 on 2017/7/16.
//  Copyright © 2017年 TNA. All rights reserved.
//

#ifndef Platform_h
#define Platform_h

#define PLATFORM_UNKNOW     0
#define PLATFORM_LINUX      1
#define PLATFORM_MAC        2
#define PLATFORM_WINDOWS    3
#define PLATFORM_IOS        4
#define PLATFORM_ANDROID    5

#define TARGET_PLATFORM PLATFORM_WINDOWS

#define INT32_MAIN_INT32_argC_PCWCHAR_argV()                                             \
INT32 wchar_main(INT32 argC, PCWCHAR argV[]);                                            \
GENERAL_MAIN()                                                                           \
INT32 wchar_main(INT32 argC, PCWCHAR argV[])

#if   (TARGET_PLATFORM == PLATFORM_LINUX)
#define GENERAL_MAIN() int main(int argc, const char * argv[])                           \
{                                                                                        \
    PWCHAR* wArgv = (WCHAR**)malloc(argc * sizeof(PWCHAR));                              \
    memset(wArgv,0 , argc * sizeof(WCHAR*));                                             \
    for (int i = 0; i < argc;++i)                                                        \
    {                                                                                    \
        wArgv[i] = (PWCHAR)malloc(strlen(argv[i])*sizeof(WCHAR) + sizeof(WCHAR));        \
        memset(wArgv[i], 0, strlen(argv[i])*sizeof(WCHAR) + sizeof(WCHAR));              \
        mbstowcs(wArgv[i], argv[i], strlen(argv[i]));                                    \
    }                                                                                    \
    return wchar_main(argc, (PCWCHAR*)wArgv);											 \
}

#elif (TARGET_PLATFORM == PLATFORM_MAC)
#define GENERAL_MAIN() int main(int argc, const char * argv[])                            \
{                                                                                         \
    PWCHAR* wArgv = (WCHAR**)malloc(argc * sizeof(PWCHAR));                               \
    memset(wArgv,0 , argc * sizeof(WCHAR*));                                              \
    for (int i = 0; i < argc;++i)                                                         \
    {                                                                                     \
        wArgv[i] = (PWCHAR)malloc(strlen(argv[i])*sizeof(WCHAR) + sizeof(WCHAR));         \
        memset(wArgv[i], 0, strlen(argv[i])*sizeof(WCHAR) + sizeof(WCHAR));               \
        mbstowcs(wArgv[i], argv[i], strlen(argv[i]));                                     \
    }                                                                                     \
    return wchar_main(argc, (PCWCHAR*)wArgv);                                             \
}



#elif (TARGET_PLATFORM == PLATFORM_WINDOWS)
#define GENERAL_MAIN() int APIENTRY _tWinMain( HINSTANCE hInstance,                       \
                        HINSTANCE hPrevInstance,                                          \
                        LPTSTR    lpCmdLine,                                              \
                        int       nCmdShow)                                               \
{                                                                                         \
    WCHAR* curry_Pos = lpCmdLine;                                                         \
    WCHAR* argV[512];                                                                     \
    memset(argV, 0, sizeof(argV));                                                        \
    WCHAR** startPos = argV;                                                              \
    INT32 argC = 0;                                                                       \
    for (WCHAR* tempChar = lpCmdLine; TRUE; ++tempChar)                                   \
    {                                                                                     \
        if (*tempChar == L' ' || *tempChar == L'\0')                                      \
        {                                                                                 \
            if (tempChar == curry_Pos && *curry_Pos == L' ')                              \
            {                                                                             \
                ++curry_Pos;                                                              \
                continue;                                                                 \
            }                                                                             \
            size_t char_lenth = tempChar - curry_Pos;                                     \
            *startPos = (WCHAR*)malloc(sizeof(WCHAR)*char_lenth + sizeof(WCHAR));         \
            memset(*startPos, 0, sizeof(WCHAR)*char_lenth);                               \
            memcpy(*startPos, curry_Pos, sizeof(WCHAR)*char_lenth);                       \
            (*startPos)[char_lenth] = L'\0';                                              \
            curry_Pos += char_lenth + 1;                                                  \
            ++startPos;                                                                   \
            ++argC;                                                                       \
            if (*tempChar == L'\0')                                                       \
            {                                                                             \
                break;                                                                    \
            }                                                                             \
        }                                                                                 \
    }                                                                                     \
    return wchar_main(argC, (CONST WCHAR**)argV);                                         \
}

#elif (TARGET_PLATFORM == PLATFORM_IOS)


#elif (TARGET_PLATFORM == PLATFORM_ANDROID)

#else


#endif



#endif /* Platform_h */

