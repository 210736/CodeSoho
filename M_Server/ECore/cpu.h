//-----------------------------------------------------------------------------
// author Lyp
// date 2003-08-03
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {

class CUDelay
{
public:
    void    Mark(){ m_mark = ::GetTickCount(); };
    int     MSec(){ return( (::GetTickCount()-m_mark)&0x7FFFFFFF); };
private:
    DWORD   m_mark;
};


class ECORE_API CpuUsage
{
public:
    CpuUsage( void );
    ~CpuUsage( void );
    
    // return :
    // % of cpu usage for this process 
    // % cpu systemUsage 
    int GetUsage( int* pSystemUsage);

private:
	typedef BOOL ( __stdcall * pfnGetSystemTimes)( LPFILETIME lpIdleTime, LPFILETIME lpKernelTime, LPFILETIME lpUserTime );

    static CUDelay	s_delay;
    static __int64	s_time;
    static __int64	s_idleTime;
    static __int64	s_kernelTime;
    static __int64	s_userTime;
    static int		s_lastCpu;
    static int		s_cpu[5];
    static __int64	s_kernelTimeProcess;
    static __int64	s_userTimeProcess;
    static int		s_lastCpuProcess;
    static int		s_cpuProcess[5];
    static int		s_count;
    static int		s_index;
    static __int64	s_lastUpTime;

    static HINSTANCE			s_hKernel;
    static pfnGetSystemTimes	s_pfnGetSystemTimes;

    CRITICAL_SECTION			m_lock;
};

} // namespace ECore {
