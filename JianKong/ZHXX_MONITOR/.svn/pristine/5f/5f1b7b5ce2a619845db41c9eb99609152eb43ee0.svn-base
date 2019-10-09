using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
// author lixiaohu
// date   20110223
// summery Get or set system time at this instance
namespace MonitorAndSet.CommonClass
{
    class TimeInterface
    {
        public struct SystemTime{
            public ushort wYear;
            public ushort wMonth;
            public ushort wDayOfWeek;
            public ushort wDay;
            public ushort wHour;
            public ushort wMinute;
            public ushort wSecond;
            public ushort wMillSeconds;
        }
        [DllImport("kernel32.dll")]
        public static extern bool SetSystemTime(ref SystemTime sysTime);
        [DllImport("kernel32.dll")]
        public static extern bool SetLocalTime(ref SystemTime sysTime);
        [DllImport("kernel32.dll")]
        public static extern void GetSystemTime(ref SystemTime sysTime);
        [DllImport("kernel32.dll")]
        public static extern void GetLocalTime(ref SystemTime sysTime);

        public static bool SetLocalTimeByStr(string timeInfo)   //timeInfo 格式: YYYY.MM.DD-HH:MM:SS
        {
            bool flag;
            SystemTime systime = new SystemTime();
            systime.wYear = Convert.ToUInt16(timeInfo.Substring(0,timeInfo.IndexOf('.')));
            timeInfo = timeInfo.Substring(timeInfo.IndexOf('.')+1);
            systime.wMonth = Convert.ToUInt16(timeInfo.Substring(0, timeInfo.IndexOf('.')));
            timeInfo = timeInfo.Substring(timeInfo.IndexOf('.') + 1);
            systime.wDay = Convert.ToUInt16(timeInfo.Substring(0, timeInfo.IndexOf('-')));
            timeInfo = timeInfo.Substring(timeInfo.IndexOf('.') + 1);
            systime.wHour = Convert.ToUInt16(timeInfo.Substring(0, timeInfo.IndexOf(':')));
            timeInfo = timeInfo.Substring(timeInfo.IndexOf(':') + 1);
            systime.wMinute = Convert.ToUInt16(timeInfo.Substring(0, timeInfo.IndexOf(':')));
            timeInfo = timeInfo.Substring(timeInfo.IndexOf(':') + 1);
            systime.wSecond = Convert.ToUInt16(timeInfo.Substring(0));
            
            try{
                flag = TimeInterface.SetLocalTime(ref systime);
                return flag;
            }
            catch(Exception ex)
            {
                Console.WriteLine("异常:%s\n", ex.ToString());
                return false; ;
            }
        }

        public static bool SetSystemTimeByStr(string timeInfo)   //timeInfo 格式: YYYY.MM.DD-HH:MM:SS
        {
            bool flag;
            SystemTime systime = new SystemTime();
            systime.wYear = Convert.ToUInt16(timeInfo.Substring(0, timeInfo.IndexOf('.')));
            timeInfo = timeInfo.Substring(timeInfo.IndexOf('.') + 1);
            systime.wMonth = Convert.ToUInt16(timeInfo.Substring(0, timeInfo.IndexOf('.')));
            timeInfo = timeInfo.Substring(timeInfo.IndexOf('.') + 1);
            systime.wDay = Convert.ToUInt16(timeInfo.Substring(0, timeInfo.IndexOf('-')));
            timeInfo = timeInfo.Substring(timeInfo.IndexOf('.') + 1);
            systime.wHour = Convert.ToUInt16(timeInfo.Substring(0, timeInfo.IndexOf(':')));
            timeInfo = timeInfo.Substring(timeInfo.IndexOf(':') + 1);
            systime.wMinute = Convert.ToUInt16(timeInfo.Substring(0, timeInfo.IndexOf(':')));
            timeInfo = timeInfo.Substring(timeInfo.IndexOf(':') + 1);
            systime.wSecond = Convert.ToUInt16(timeInfo.Substring(0));

            try
            {
                flag = TimeInterface.SetSystemTime(ref systime);
                return flag;
            }
            catch (Exception ex)
            {
                Console.WriteLine("异常:%s\n", ex.ToString());
                return false; ;
            }
        }

        public static void GetLocalTimeInCSharp( ref SystemTime Systime)
        {
            try{
                TimeInterface.GetLocalTime(ref Systime);
            }
            catch (Exception ex)
            {
                Console.WriteLine("异常：%s\n",ex.ToString());
            }
        }

        public static void GetSystemTimeInCSharp(ref SystemTime Systime)
        {
            try
            {
                TimeInterface.GetSystemTime(ref Systime);
            }
            catch (Exception ex)
            {
                Console.WriteLine("异常：%s\n", ex.ToString());
            }
        }
    }
}
