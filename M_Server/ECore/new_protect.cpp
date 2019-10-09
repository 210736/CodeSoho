//-----------------------------------------------------------------------------
// File: new_protect
// Auth: Lyp
// Date: 2004-2-26	
// Last: 2004-2-26
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "new_protect.h"

namespace ECore {
//-----------------------------------------------------------------------------
// 对new的结果进行检测,如果发现失败,则提示用户选择是否 退出/重试/继续
//-----------------------------------------------------------------------------
INT NewHandle(size_t size)
{
	CONST INT ONE_KILO = 1024;
	CONST INT ONE_MEGA = 1024 * 1024;

	// 检查堆是否被破坏
	int state = _heapchk();

	// 获取系统内存占用情况
	MEMORYSTATUSEX sysMem;
	ZeroMemory( &sysMem, sizeof(sysMem) );
	sysMem.dwLength = sizeof(sysMem);
	GlobalMemoryStatusEx(&sysMem);

	// 获取进程内存占用
	PROCESS_MEMORY_COUNTERS appMem;
	ZeroMemory( &appMem, sizeof(appMem) );
	appMem.cb = sizeof(appMem);
	GetProcessMemoryInfo( GetCurrentProcess(), &appMem, sizeof(appMem) );

	// 提示用户,让用户处理
	TObj<Error>()->Msg(_T("Get memory failed, MALLOC(or NEW) for %lu bytes failed, Workset:%dK, Pagefile:%dK, TotalPhys:%dMB, AvailPhys:%dMB, TotalPageFile:%dMB, AvailPageFile:%dMB, HeapCheck:%d\r\n"), 
		size, 
		INT(appMem.WorkingSetSize / ONE_KILO),
		INT(appMem.PagefileUsage / ONE_KILO),
		INT(sysMem.ullTotalPhys / ONE_MEGA),
		INT(sysMem.ullAvailPhys / ONE_MEGA),
		INT(sysMem.ullTotalPageFile / ONE_MEGA),
		INT(sysMem.ullAvailPageFile / ONE_MEGA),
		state );

	return 1;
}

} // namespace ECore {