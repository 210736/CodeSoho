#ifndef _STDAFX_MCONSOLE_H
#define _STDAFX_MCONSOLE_H

#include "../ECore/ECore.h"

#if defined(MCONSOLE_LIBRARY)
#  define MCONSOLE_API __declspec(dllexport)
#else
#  define MCONSOLE_API __declspec(dllimport)
#endif

#endif // _STDAFX_MCONSOLE_H