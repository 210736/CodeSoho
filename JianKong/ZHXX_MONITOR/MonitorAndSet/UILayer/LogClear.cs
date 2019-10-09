using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

using DevComponents.DotNetBar;
using DevComponents.DotNetBar.Rendering;
using DevComponents.Editors.DateTimeAdv;

namespace MonitorAndSet
{
    public partial class LogClear : DevComponents.DotNetBar.Office2007Form
    {
        public LogClear()
        {
            InitializeComponent();
        }

        private void LogClear_Load(object sender, EventArgs e)
        {
            try
            {
                this.BackColor = MainForm.DlgBackColor;
                this.dateTimePicker1.Value = System.DateTime.Today;
                this.dateTimePicker2.Value = System.DateTime.Today;
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }

        private void btn_LClearCancle_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void btn_LClear_Click(object sender, EventArgs e)
        {
            MessageBoxEx.UseSystemLocalizedString = true;
            try
            {
                int startyear = dateTimePicker1.Value.Year;    //开始时间的年
                int startmonth = dateTimePicker1.Value.Month;  //开始时间的月
                int startday = dateTimePicker1.Value.Day;      //开始时间的日

                int endyear = dateTimePicker2.Value.Year;    //终止时间的年
                int endmonth = dateTimePicker2.Value.Month;  //终止时间的月
                int endday = dateTimePicker2.Value.Day;      //终止时间的日

                DateTime startdt = new DateTime(startyear, startmonth, startday);
                DateTime enddt = new DateTime(endyear, endmonth, endday);

                if (startdt.CompareTo(enddt) > 0)
                {
                    MessageBoxEx.Show("开始时间不应晚于终止时间!");
                    return;
                }

                if (MessageBoxEx.Show("将要删除所选范围内的日志，是否继续？", "确认清空", MessageBoxButtons.YesNo) == DialogResult.Yes)
                {
                    String[] yearfoldEntries;      //日志文件夹下的每个年文件夹
                    String[] monthfoldEntries;     //年文件夹下的每个月文件夹

                    String[] fileEntries;          //月文件夹下的每个日志文件

                    if (Directory.Exists(MainForm.m_LogPath + "LogFile"))
                    {
                        int year = 0;   //年
                        int month = 0;  //月
                        int day = 0;

                        String yearstr = "";    //年文件夹名
                        String monthstr = "";   //月文件夹名
                        String filestr = "";    //日志文件名

                        DateTime startlogdt;           //日志文件的开始日期时间
                        DateTime endlogdt;             //日志文件的终止日期时间
                        int interval;                  //日志文件的记录周期

                        yearfoldEntries = Directory.GetDirectories(MainForm.m_LogPath + "LogFile");
                        foreach (String yearfold in yearfoldEntries)                  //获取每个年文件夹
                        {
                            yearstr = yearfold.Substring(yearfold.LastIndexOf('\\') + 1);
                            year = Convert.ToInt32(yearstr.Substring(0, yearstr.IndexOf('年')));

                            if (year > startyear && year < endyear)    //在起止年之间的 年文件夹 直接删除
                            {
                                monthfoldEntries = Directory.GetDirectories(yearfold);
                                foreach (String monthfold in monthfoldEntries)             //获取每个月文件夹
                                {
                                    fileEntries = Directory.GetFiles(monthfold);           //获取月文件夹中的每一个日志文件
                                    foreach (String fileName in fileEntries)
                                    {
                                        File.Delete(fileName);
                                    }
                                    Directory.Delete(monthfold);
                                }
                                Directory.Delete(yearfold);
                                continue;
                            }

                            monthfoldEntries = Directory.GetDirectories(yearfold);
                            foreach (String monthfold in monthfoldEntries)             //获取每个月文件夹
                            {
                                monthstr = monthfold.Substring(monthfold.LastIndexOf('\\') + 1);
                                month = Convert.ToInt32(monthstr.Substring(0, monthstr.IndexOf('月')));

                                if (year == startyear)
                                {
                                    if (year == endyear)
                                    {
                                        if (month > startmonth && month < endmonth)
                                        {
                                            fileEntries = Directory.GetFiles(monthfold);           //获取月文件夹中的每一个日志文件
                                            foreach (String fileName in fileEntries)
                                            {
                                                File.Delete(fileName);
                                            }
                                            Directory.Delete(monthfold);
                                            continue;
                                        }
                                    }
                                    else
                                    {
                                        if (month > startmonth)
                                        {
                                            fileEntries = Directory.GetFiles(monthfold);           //获取月文件夹中的每一个日志文件
                                            foreach (String fileName in fileEntries)
                                            {
                                                File.Delete(fileName);
                                            }
                                            Directory.Delete(monthfold);
                                            continue;
                                        }
                                    }
                                }
                                else if (year == endyear)
                                {
                                    if (year == startyear)
                                    {
                                        if (month > startmonth && month < endmonth)
                                        {
                                            fileEntries = Directory.GetFiles(monthfold);           //获取月文件夹中的每一个日志文件
                                            foreach (String fileName in fileEntries)
                                            {
                                                File.Delete(fileName);
                                            }
                                            Directory.Delete(monthfold);
                                            continue;
                                        }
                                    }
                                    else
                                    {
                                        if (month < endmonth)
                                        {
                                            fileEntries = Directory.GetFiles(monthfold);           //获取月文件夹中的每一个日志文件
                                            foreach (String fileName in fileEntries)
                                            {
                                                File.Delete(fileName);
                                            }
                                            Directory.Delete(monthfold);
                                            continue;
                                        }
                                    }
                                }

                                fileEntries = Directory.GetFiles(monthfold);           //获取月文件夹中的每一个日志文件
                                foreach (String fileName in fileEntries)
                                {
                                    filestr = fileName.Substring(fileName.LastIndexOf('\\') + 1);
                                    //day = Convert.ToInt32(filestr.Substring(0, filestr.IndexOf('日')));
                                    //filestr = filestr.Substring(filestr.IndexOf('日') + 4);
                                    //interval = Convert.ToInt32(filestr.Substring(0, filestr.IndexOf('天')));
                                    string[] dli = filestr.Split(' ');
                                    if (dli.Length > 2)
                                    {
                                        day = Convert.ToInt32(dli[0].Substring(0, dli[0].IndexOf('日')));
                                        interval = Convert.ToInt32(dli[1].Substring(0, dli[1].IndexOf('天')));
                                        filestr = dli[2];
                                    }
                                    else
                                    {
                                        continue;
                                    }

                                    startlogdt = new DateTime(year, month, day);
                                    endlogdt = new DateTime(year, month, day + (interval - 1));

                                    if (startlogdt.CompareTo(dateTimePicker1.Value.Date) >= 0 && endlogdt.CompareTo(dateTimePicker2.Value.Date) <= 0)
                                    {
                                        File.Delete(fileName);
                                    }

                                    if(interval>1)
                                    {
                                        String str;
                                        String dt;                     //日志文件每一行内容的日期时间
                                        int logyear;                   //日志文件每一行内容 日期中的年
                                        int logmonth;                  //日志文件每一行内容 日期中的月
                                        int logday;                    //日志文件每一行内容 日期中的日
                                        DateTime logdt;                //日志文件每一行内容 的日期

                                        //针对范围内的每一个日志文件中的每一条记录进行判断和删除
                                        try
                                        {
                                            using (StreamReader sr = File.OpenText(fileName))
                                            {
                                                StreamWriter sw;
                                                while (sr.Peek() >= 0)
                                                {
                                                    str = sr.ReadLine();
                                                    int n = str.IndexOf('-');
                                                    dt = str.Substring(0, n);  //日期时间

                                                    String dtstr = dt;
                                                    logyear = Convert.ToInt32(dtstr.Substring(0, dtstr.IndexOf('年')));
                                                    dtstr = dtstr.Substring(dtstr.IndexOf('年') + 1);

                                                    logmonth = Convert.ToInt32(dtstr.Substring(0, dtstr.IndexOf('月')));
                                                    dtstr = dtstr.Substring(dtstr.IndexOf('月') + 1);

                                                    logday = Convert.ToInt32(dtstr.Substring(0, dtstr.IndexOf('日')));
                                                    logdt = new DateTime(logyear, logmonth, logday);

                                                    if (logdt.CompareTo(dateTimePicker1.Value.Date) >= 0 && logdt.CompareTo(dateTimePicker2.Value.Date) <= 0)
                                                    { }
                                                    else
                                                    {
                                                        if (!File.Exists(fileName + "1.txt"))
                                                        {
                                                            sw = File.CreateText(fileName + "1.txt");
                                                        }
                                                        else
                                                        {
                                                            sw = File.AppendText(fileName + "1.txt");
                                                        }

                                                        sw.WriteLine(str);
                                                        sw.Close();
                                                    }
                                                }
                                            }

                                            File.Delete(fileName);
                                            if (File.Exists(fileName + "1.txt"))
                                            {
                                                File.Move(fileName + "1.txt", fileName);
                                            }
                                        }
                                        catch (Exception ex)
                                        {
                                            MessageBoxEx.Show("删除日志文件出现错误，请检查！" + Environment.NewLine +
                                            "异常:" + ex.Message);
                                        }
                                    }                                 

                                }
                            }
                        }
                    }
                    MessageBoxEx.Show("所选范围内的日志已删除！");
                }
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }

        private void btn_AllClear_Click(object sender, EventArgs e)
        {
            MessageBoxEx.UseSystemLocalizedString = true;
            try
            {
                if (MessageBoxEx.Show("将要删除所有的日志，是否继续？", "确认全部清空", MessageBoxButtons.YesNo) == DialogResult.Yes)
                {
                    String[] yearfoldEntries;      //日志文件夹下的每个年文件夹
                    String[] monthfoldEntries;     //年文件夹下的每个月文件夹

                    String[] fileEntries;          //月文件夹下的每个日志文件
                    if (Directory.Exists(MainForm.m_LogPath + "LogFile"))
                    {
                        yearfoldEntries = Directory.GetDirectories(MainForm.m_LogPath + "LogFile");
                        foreach (String yearfold in yearfoldEntries)
                        {
                            monthfoldEntries = Directory.GetDirectories(yearfold);
                            foreach (String monthfold in monthfoldEntries)             //获取每个月文件夹
                            {
                                fileEntries = Directory.GetFiles(monthfold);           //获取月文件夹中的每一个日志文件
                                foreach (String fileName in fileEntries)
                                {
                                    File.Delete(fileName);
                                }
                                Directory.Delete(monthfold);
                            }
                            Directory.Delete(yearfold);
                        }
                    }
                    MessageBoxEx.Show("全部日志已删除！");
                }
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }
    }
}