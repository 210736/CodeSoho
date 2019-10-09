//-----------------------------------------------------------------------------
// author Lyp
// date 2003-08-03
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "cpu.h"

namespace ECore {

class CUTime
{
public:
	CUTime( void ):m_time(0){}
	CUTime& operator =( __int64 time ){ m_time = time; return *this; }
private:
    __int64 m_time;
};

class Lock
{
public:
    Lock( LPCRITICAL_SECTION sect ){  EnterCriticalSection( pLock=sect ); };
    ~Lock(){ LeaveCriticalSection( pLock ); };
private:
    LPCRITICAL_SECTION      pLock;
};

__int64	CpuUsage::s_time;
CUDelay	CpuUsage::s_delay;
int		CpuUsage::s_count = 0;
int		CpuUsage::s_index = 0;
__int64 CpuUsage::s_idleTime;
__int64 CpuUsage::s_kernelTime;
__int64 CpuUsage::s_userTime;
int		CpuUsage::s_lastCpu = 0;
int		CpuUsage::s_cpu[];
__int64 CpuUsage::s_kernelTimeProcess;
__int64 CpuUsage::s_userTimeProcess;
int		CpuUsage::s_lastCpuProcess;
int		CpuUsage::s_cpuProcess[];
__int64 CpuUsage::s_lastUpTime = 0;
HINSTANCE CpuUsage::s_hKernel = NULL;


typedef BOOL ( __stdcall * pfnGetSystemTimes)( LPFILETIME lpIdleTime, LPFILETIME lpKernelTime, LPFILETIME lpUserTime );
pfnGetSystemTimes CpuUsage::s_pfnGetSystemTimes = NULL;

CpuUsage::CpuUsage()
{
    ::InitializeCriticalSection( &m_lock );
    if( s_hKernel == NULL )
    {   
        s_hKernel = LoadLibrary( _T("Kernel32.dll") );
        if( s_hKernel != NULL )
        {
            s_pfnGetSystemTimes = (pfnGetSystemTimes)GetProcAddress( s_hKernel, "GetSystemTimes" );
            if( s_pfnGetSystemTimes == NULL )
            {
                FreeLibrary( s_hKernel ); s_hKernel = NULL;
            }
        }
    }

    s_delay.Mark();
}

CpuUsage::~CpuUsage()
{
    if( s_hKernel == NULL )
    {
        FreeLibrary( s_hKernel ); s_hKernel = NULL;
    }

    ::DeleteCriticalSection( &m_lock );
}

int CpuUsage::GetUsage( int* pSystemUsage )
{
    __int64 sTime;
    int sLastCpu;
    int sLastCpuProcess;
    CUTime sLastUpTime;

    // lock
    { 
        Lock lock( &m_lock );

        sTime           = s_time;
        sLastCpu        = s_lastCpu;
        sLastCpuProcess = s_lastCpuProcess;
        sLastUpTime     = s_lastUpTime;
    }

    if( s_delay.MSec() <= 200 )
    {
        if( pSystemUsage != NULL )
            *pSystemUsage = sLastCpu;

        return sLastCpuProcess;
    }

    __int64 time;
    
    __int64 idleTime;
    __int64 kernelTime;
    __int64 userTime;
    __int64 kernelTimeProcess;
    __int64 userTimeProcess;

    GetSystemTimeAsFileTime( (LPFILETIME)&time );

    if( sTime == 0 )
    {
        // for the system
        if( s_pfnGetSystemTimes != NULL )
        {
            /*BOOL res = */s_pfnGetSystemTimes( (LPFILETIME)&idleTime, (LPFILETIME)&kernelTime, (LPFILETIME)&userTime );
        }
        else
        {
            idleTime    = 0;
            kernelTime  = 0;
            userTime    = 0;
        }

        // for this process
        {
            FILETIME createTime;
            FILETIME exitTime;
            GetProcessTimes( GetCurrentProcess(), &createTime, &exitTime, 
                             (LPFILETIME)&kernelTimeProcess, 
                             (LPFILETIME)&userTimeProcess );
        }

        // LOCK
        {
            Lock lock( &m_lock );

            s_time              = time;
            
            s_idleTime          = idleTime;
            s_kernelTime        = kernelTime;
            s_userTime          = userTime;

            s_kernelTimeProcess = kernelTimeProcess;
            s_userTimeProcess   = userTimeProcess;

            s_lastCpu           = 0;
            s_lastCpuProcess    = 0;

            s_lastUpTime        = kernelTime + userTime;

            sLastCpu        = s_lastCpu;
            sLastCpuProcess = s_lastCpuProcess;
            sLastUpTime     = s_lastUpTime;
        }

        if( pSystemUsage != NULL )
            *pSystemUsage = sLastCpu;

        s_delay.Mark();
        return sLastCpuProcess;
    }
    /////////////////////////////////////////////////////
    // sTime != 0

    __int64 div = ( time - sTime );
        
    // for the system
    if( s_pfnGetSystemTimes != NULL )
    {
        /*BOOL res = */s_pfnGetSystemTimes( (LPFILETIME)&idleTime, (LPFILETIME)&kernelTime, (LPFILETIME)&userTime );
    }
    else
    {
        idleTime    = 0;
        kernelTime  = 0;
        userTime    = 0;
    }

    // for this process
    {
        FILETIME createTime;
        FILETIME exitTime;
        GetProcessTimes( GetCurrentProcess(), &createTime, &exitTime, 
                         (LPFILETIME)&kernelTimeProcess, 
                         (LPFILETIME)&userTimeProcess );
    }

    int cpu;
    int cpuProcess;
    // LOCK
    {
        Lock lock( &m_lock );

        __int64 usr = userTime   - s_userTime;
        __int64 ker = kernelTime - s_kernelTime;
        __int64 idl = idleTime   - s_idleTime;
        __int64 sys = (usr + ker);

        if( sys == 0 )
            cpu = 0;
        else
            cpu = int( (sys - idl) *100 / sys ); // System Idle take 100 % of cpu :-((
        
        cpuProcess = int( ( ( ( userTimeProcess - s_userTimeProcess ) + ( kernelTimeProcess - s_kernelTimeProcess ) ) *100 ) / div );
        
        s_time              = time;

        s_idleTime          = idleTime;
        s_kernelTime        = kernelTime;
        s_userTime          = userTime;

        s_kernelTimeProcess = kernelTimeProcess;
        s_userTimeProcess   = userTimeProcess;
        
        s_cpu[(s_index++) %5] = cpu;
        s_cpuProcess[(s_index++) %5] = cpuProcess;
        s_count ++;
        if( s_count > 5 ) 
            s_count = 5;
        
        int i;
        cpu = 0;
        for( i = 0; i < s_count; i++ )
            cpu += s_cpu[i];
        
        cpuProcess = 0;
        for( i = 0; i < s_count; i++ )
            cpuProcess += s_cpuProcess[i];

        cpu         /= s_count;
        cpuProcess  /= s_count;
        
        s_lastCpu        = cpu;
        s_lastCpuProcess = cpuProcess;

        s_lastUpTime     = kernelTime + userTime;

        sLastCpu        = s_lastCpu;
        sLastCpuProcess = s_lastCpuProcess;
        sLastUpTime     = s_lastUpTime;
    }

    if( pSystemUsage != NULL )
        *pSystemUsage = sLastCpu;

    s_delay.Mark();
    return sLastCpuProcess;
}



} // namespace ECore {
