using System;
using System.Collections.Generic;
using System.Text;

using System.IO;

namespace MonitorAndSet.TopoInfo
{
    /// <summary>
    /// LogInfo表示日志信息
    /// </summary>
    public class LogInfo
    {
        public String yearfold = "";     //写日志时的 年日志文件夹名  2008年
        public int year = 0;
        public String monthfold = "";    //写日志时的 月日志文件夹名  12月
        public int month = 0;
        public String datefile = "";     //写日志时的 日文件名        25日
        public int date = 0;

        public LogInfo()
        {

        }

        private int m_hostid;
        /// <summary>
        /// 主机ID号
        /// </summary>
        public int hostid
        {
            get { return m_hostid; }
            set { m_hostid = value; }
        }

        private string m_ipaddr;
        /// <summary>
        /// 主机ID号
        /// </summary>
        public string ipaddr
        {
            get { return m_ipaddr; }
            set { m_ipaddr = value; }
        }

        private String m_hostname;
        /// <summary>
        /// 主机名
        /// </summary>
        public String hostname
        {
            get { return m_hostname; }
            set { m_hostname = value; }
        }

        private String m_maintype;
        /// <summary>
        /// 主类型,包括"异常"和"操作"
        /// </summary>
        public String maintype
        {
            get { return m_maintype; }
            set { m_maintype = value; }
        }

        private String m_senctype;
        /// <summary>
        /// 次类型,包括"主机"和"网络"
        /// </summary>
        public String senctype
        {
            get { return m_senctype; }
            set { m_senctype = value; }
        }

        private String m_logcontent;
        /// <summary>
        /// 日志内容
        /// </summary>
        public String logcontent
        {
            get { return m_logcontent; }
            set { m_logcontent = value; }
        }

        #region 写日志文件
        public void WriteLog()
        {
            String sysdate = GetSysDate();

            //09-4-10加  针对获取的系统时间格式为 "星期五 2009年04月10日"
            if(sysdate.IndexOf("年")== 9)
            {
                sysdate = sysdate.Substring(4);
            }

            String str = sysdate;
            year = Convert.ToInt32(str.Substring(0, str.IndexOf("年")));
            yearfold = str.Substring(0, str.IndexOf("年") + 1);
            str = str.Substring(str.IndexOf("年") + 1);

            month = Convert.ToInt32(str.Substring(0, str.IndexOf("月")));
            monthfold = str.Substring(0, str.IndexOf("月") + 1);
            str = str.Substring(str.IndexOf("月") + 1);

            date = Convert.ToInt32(str.Substring(0, str.IndexOf("日")));
            datefile = str;

            StreamWriter sw;
            if(!FindLogFolder())
            {
                Directory.CreateDirectory(MainForm.m_LogPath + "LogFile\\");
            }

            //判断当年日志文件夹是否已经存在
            if (!Directory.Exists(MainForm.m_LogPath + "LogFile\\" + yearfold))
            {
                Directory.CreateDirectory(MainForm.m_LogPath + "LogFile\\" + yearfold);
            }

            //判断当月日志文件夹是否已经存在
            if (!Directory.Exists(MainForm.m_LogPath + "LogFile\\" + yearfold + "\\" + monthfold))
            {
                Directory.CreateDirectory(MainForm.m_LogPath + "LogFile\\" + yearfold + "\\" + monthfold);
            }

            //判断当天应记录的日志文件是否已经存在
            if (MainForm.loginterval == 1)       //每天一个文件
            {
                if (!File.Exists(MainForm.m_LogPath + "LogFile\\" + yearfold + "\\" + monthfold + "\\" + datefile + " 1天 日志" + ".txt"))
                {
                    sw = File.CreateText(MainForm.m_LogPath + "LogFile\\" + yearfold + "\\" + monthfold + "\\" + datefile + " 1天 日志" + ".txt");
                }
                else
                {
                    sw = File.AppendText(MainForm.m_LogPath + "LogFile\\" + yearfold + "\\" + monthfold + "\\" + datefile + " 1天 日志" + ".txt");
                }
                String time = GetSysTime();
                try
                {
                    sw.WriteLine(sysdate + time + "-主类型:" + m_maintype + "-次类型:" + m_senctype + "-日志内容:"
                        + ipaddr + "*" + m_hostname + "*" + m_logcontent);
                    sw.Close();
                }
                catch (Exception e)
                {
                    String stri = e.ToString();
                }
            }
            else  //按照记录周期每隔几天一个文件 文件名格式为: 5日 4天 日志.txt, 表示记录了5日至9日,共4天的日志信息
            {
                int daysinmonth = System.DateTime.DaysInMonth(year, month);
                int groupCount = 0;  //按照当前的记录周期 该条日志应保存在第几组日志文件内

                if (date % MainForm.loginterval == 0)
                {
                    groupCount = date / MainForm.loginterval;
                }
                else
                {
                    groupCount = date / MainForm.loginterval + 1;
                }

                int filedate = 1 + MainForm.loginterval * (groupCount - 1);   //记录周期大于1时日志文件所显示的日期为该组日期中的第一天日期

                if (!File.Exists(MainForm.m_LogPath + "LogFile\\" + yearfold + "\\" + monthfold + "\\" + filedate.ToString() + "日 " + MainForm.loginterval.ToString() + "天 " + "日志" + ".txt"))
                {
                    sw = File.CreateText(MainForm.m_LogPath + "LogFile\\" + yearfold + "\\" + monthfold + "\\" + filedate.ToString() + "日 " + MainForm.loginterval.ToString() + "天 " + "日志" + ".txt");
                }
                else
                {
                    sw = File.AppendText(MainForm.m_LogPath + "LogFile\\" + yearfold + "\\" + monthfold + "\\" + filedate.ToString() + "日 " + MainForm.loginterval.ToString() + "天 " + "日志" + ".txt");
                }
                String time = GetSysTime();
                try
                {
                    sw.WriteLine(sysdate + time + "-主类型:" + m_maintype + "-次类型:" + m_senctype + "-日志内容:"
                        + ipaddr + "*" + m_hostname + "*" + m_logcontent);
                    sw.Close();
                }
                catch (Exception e)
                {
                    String stri = e.ToString();
                }
                
            }

        }

        public void WriteStaLog()
        {
            String sysdate = GetSysDate();

            //09-4-10加  针对获取的系统时间格式为 "星期五 2009年04月10日"
            if (sysdate.IndexOf("年") == 9)
            {
                sysdate = sysdate.Substring(4);
            }

            String str = sysdate;
            year = Convert.ToInt32(str.Substring(0, str.IndexOf("年")));
            yearfold = str.Substring(0, str.IndexOf("年") + 1);
            str = str.Substring(str.IndexOf("年") + 1);

            month = Convert.ToInt32(str.Substring(0, str.IndexOf("月")));
            monthfold = str.Substring(0, str.IndexOf("月") + 1);
            str = str.Substring(str.IndexOf("月") + 1);

            date = Convert.ToInt32(str.Substring(0, str.IndexOf("日")));
            datefile = str;

            StreamWriter sw;
            if (!FindLogFolder())
            {
                Directory.CreateDirectory(MainForm.m_LogPath + "LogFile\\");
            }

            //判断当年日志文件夹是否已经存在
            if (!Directory.Exists(MainForm.m_LogPath + "LogFile\\" + yearfold))
            {
                Directory.CreateDirectory(MainForm.m_LogPath + "LogFile\\" + yearfold);
            }

            //判断当月日志文件夹是否已经存在
            if (!Directory.Exists(MainForm.m_LogPath + "LogFile\\" + yearfold + "\\" + monthfold))
            {
                Directory.CreateDirectory(MainForm.m_LogPath + "LogFile\\" + yearfold + "\\" + monthfold);
            }

            //判断当天应记录的日志文件是否已经存在
            if (MainForm.loginterval == 1)       //每天一个文件
            {
                if (!File.Exists(MainForm.m_LogPath + "LogFile\\" + yearfold + "\\" + monthfold + "\\" + datefile + " 1天 日志" + ".txt"))
                {
                    sw = File.CreateText(MainForm.m_LogPath + "LogFile\\" + yearfold + "\\" + monthfold + "\\" + datefile + " 1天 日志" + ".txt");
                }
                else
                {
                    sw = File.AppendText(MainForm.m_LogPath + "LogFile\\" + yearfold + "\\" + monthfold + "\\" + datefile + " 1天 日志" + ".txt");
                }
                String time = GetSysTime();
                try
                {
                    sw.WriteLine(sysdate + time + "-主类型:" + m_maintype + "-次类型:" + m_senctype + "-日志内容:"
                        + m_logcontent);
                    sw.Close();
                }
                catch (Exception e)
                {
                    String stri = e.ToString();
                }
            }
            else  //按照记录周期每隔几天一个文件 文件名格式为: 5日 4天 日志.txt, 表示记录了5日至9日,共4天的日志信息
            {
                int daysinmonth = System.DateTime.DaysInMonth(year, month);
                int groupCount = 0;  //按照当前的记录周期 该条日志应保存在第几组日志文件内

                if (date % MainForm.loginterval == 0)
                {
                    groupCount = date / MainForm.loginterval;
                }
                else
                {
                    groupCount = date / MainForm.loginterval + 1;
                }

                int filedate = 1 + MainForm.loginterval * (groupCount - 1);   //记录周期大于1时日志文件所显示的日期为该组日期中的第一天日期

                if (!File.Exists(MainForm.m_LogPath + "LogFile\\" + yearfold + "\\" + monthfold + "\\" + filedate.ToString() + "日 " + MainForm.loginterval.ToString() + "天 " + "日志" + ".txt"))
                {
                    sw = File.CreateText(MainForm.m_LogPath + "LogFile\\" + yearfold + "\\" + monthfold + "\\" + filedate.ToString() + "日 " + MainForm.loginterval.ToString() + "天 " + "日志" + ".txt");
                }
                else
                {
                    sw = File.AppendText(MainForm.m_LogPath + "LogFile\\" + yearfold + "\\" + monthfold + "\\" + filedate.ToString() + "日 " + MainForm.loginterval.ToString() + "天 " + "日志" + ".txt");
                }
                String time = GetSysTime();
                try
                {
                    sw.WriteLine(sysdate + time + "-主类型:" + m_maintype + "-次类型:" + m_senctype + "-日志内容:"
                        + m_logcontent);
                    sw.Close();
                }
                catch (Exception e)
                {
                    String stri = e.ToString();
                }

            }

        }
        #endregion

        #region 获得系统时间
        //获得系统时间，返回格式为8时11分10秒
        public String GetSysTime()
        {
            String strSysTime,strhour,strminite,strsecond;
            strSysTime = DateTime.Now.ToLongTimeString();     //此时格式为8:11:10
            strhour = strSysTime.Substring(0, strSysTime.IndexOf(':'));
            strSysTime = strSysTime.Substring(strSysTime.IndexOf(':') + 1);
            strminite = strSysTime.Substring(0, strSysTime.IndexOf(':'));
            strSysTime = strSysTime.Substring(strSysTime.IndexOf(':') + 1);
            strsecond = strSysTime.Substring(strSysTime.IndexOf(':') + 1);
            strSysTime = strhour + "时" + strminite + "分" + strsecond + "秒";
            return strSysTime;
        }
        #endregion

        #region 获得系统日期
        //获得系统日期，返回格式为2008年11月10日
        public String GetSysDate()
        {
            String strSysDate;
            strSysDate = DateTime.Now.ToLongDateString();
            return strSysDate;
        }
        #endregion

        #region 判断日志保存路径下LogFile文件夹是否已经存在
        //判断日志保存路径下LogFile文件夹是否已经存在
        public bool FindLogFolder()
        {
            if (Directory.Exists(MainForm.m_LogPath + "LogFile\\"))
            {
                return true;
            }
            return false;
        }
        #endregion
    }
}
