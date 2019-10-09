using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using System.IO;
using MonitorAndSet.TopoInfo;
using DevComponents.DotNetBar;


namespace MonitorAndSet
{
    //告警信息对话框,仅显示当前那一天的告警,不显示历史记录
    public partial class AlarmInfo : Form
    {
        public List<AlarmDetail> m_AlarmList = new List<AlarmDetail>();
        public String filename = "";  //读取告警信息的文件名字

        public AlarmInfo()
        {
            InitializeComponent();
        }

        private void AlarmInfo_Load(object sender, EventArgs e)
        {
            try
            {
                this.BackColor = MainForm.DlgBackColor;

                GetAlarmInfo();
                ShowAlarmInfo();
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }

        private void btn_AInfoCancle_Click(object sender, EventArgs e)
        {
            Close();
        }

        //点击刷新按钮显示最新的全部告警信息 09-2-16加
        private void btn_AInfoRefresh_Click(object sender, EventArgs e)
        {
            try
            {
                GetAlarmInfo();
                ShowAlarmInfo();
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }

        #region 获取告警信息
        //将告警信息存储在m_AlarmList中
        private void GetAlarmInfo()
        {
            MessageBoxEx.UseSystemLocalizedString = true;
            try
            {
                m_AlarmList.Clear();

                //首先获得系统当前日期,格式为2008年11月10日
                //String strSysDate = DateTime.Now.ToLongDateString();
                this.listView1.Items.Clear();  //清空之前的查询结果

                String[] yearfoldEntries;      //日志文件夹下的每个年文件夹
                String[] monthfoldEntries;     //年文件夹下的每个月文件夹
                String[] fileEntries;          //日志文件夹下的每个文件 
                String str;                    //日志文件每一行的内容

                DateTime startlogdt;           //日志文件的开始日期时间
                DateTime endlogdt;             //日志文件的终止日期时间
                int interval;                  //日志文件的记录周期

                String dt;                     //日志文件每一行内容的日期时间
                String mainstr;                //主类型
                String sencstr;                //次类型
                String logevent;               //日志事件的内容

                int logyear;                   //日志文件每一行内容 日期中的年
                int logmonth;                  //日志文件每一行内容 日期中的月
                int logday;                    //日志文件每一行内容 日期中的日
                DateTime logdt;                //日志文件每一行内容 的日期

                if (Directory.Exists(MainForm.m_LogPath + "LogFile"))
                {
                    int yearoftoday = DateTime.Now.Year;    //当前时间的年
                    int monthoftoday = DateTime.Now.Month;  //当前时间的月
                    int dayoftoday = DateTime.Now.Day;      //当前时间的日

                    DateTime todaydt = new DateTime(yearoftoday, monthoftoday, dayoftoday);

                    int year = 0;   //年
                    int month = 0;  //月
                    int day = 0;

                    String yearstr = "";    //年文件夹名
                    String monthstr = "";   //月文件夹名
                    String filestr = "";    //日志文件名

                    yearfoldEntries = Directory.GetDirectories(MainForm.m_LogPath + "LogFile");
                    foreach (String yearfold in yearfoldEntries)                   //获取每个年文件夹
                    {
                        yearstr = yearfold.Substring(yearfold.LastIndexOf('\\') + 1);
                        year = Convert.ToInt32(yearstr.Substring(0, yearstr.IndexOf('年')));

                        if (year != yearoftoday)          //年文件夹 不是今年的
                        {
                            continue;
                        }

                        monthfoldEntries = Directory.GetDirectories(yearfold);
                        foreach (String monthfold in monthfoldEntries)             //获取每个月文件夹
                        {
                            monthstr = monthfold.Substring(monthfold.LastIndexOf('\\') + 1);
                            month = Convert.ToInt32(monthstr.Substring(0, monthstr.IndexOf('月')));

                            if (month != monthoftoday)      //月文件夹 不是这个月的
                            {
                                continue;
                            }

                            fileEntries = Directory.GetFiles(monthfold);           //获取月文件夹中的每一个日志文件
                            foreach (String fileName in fileEntries)
                            {
                                filestr = fileName.Substring(fileName.LastIndexOf('\\') + 1);
                                day = Convert.ToInt32(filestr.Substring(0, filestr.IndexOf('日')));
                                filestr = filestr.Substring(filestr.IndexOf('日') + 2);
                                interval = Convert.ToInt32(filestr.Substring(0, filestr.IndexOf('天')));

                                startlogdt = new DateTime(year, month, day);
                                endlogdt = new DateTime(year, month, day + (interval - 1));

                                if (endlogdt.CompareTo(todaydt) < 0)
                                {
                                    continue;
                                }
                                if (startlogdt.CompareTo(todaydt) > 0)
                                {
                                    continue;
                                }

                                try
                                {
                                    using (StreamReader sr = File.OpenText(fileName))
                                    {
                                        while (sr.Peek() >= 0)
                                        {
                                            str = sr.ReadLine();
                                            int n = str.IndexOf('-');
                                            dt = str.Substring(0, n);  //日期时间

                                            str = str.Substring(n + 5);
                                            n = str.IndexOf('-');
                                            mainstr = str.Substring(0, n);  //主类型

                                            str = str.Substring(n + 5);
                                            n = str.IndexOf('-');
                                            sencstr = str.Substring(0, n);  //次类型

                                            str = str.Substring(n + 6);
                                            logevent = str;  //日志内容

                                            String dtstr = dt;
                                            logyear = Convert.ToInt32(dtstr.Substring(0, dtstr.IndexOf('年')));
                                            dtstr = dtstr.Substring(dtstr.IndexOf('年') + 1);

                                            logmonth = Convert.ToInt32(dtstr.Substring(0, dtstr.IndexOf('月')));
                                            dtstr = dtstr.Substring(dtstr.IndexOf('月') + 1);

                                            logday = Convert.ToInt32(dtstr.Substring(0, dtstr.IndexOf('日')));
                                            logdt = new DateTime(logyear, logmonth, logday);

                                            if (logdt.CompareTo(todaydt) == 0)       //为今天的日志内容
                                            {
                                                //判断,若为异常直接添加,若为操作判断是否解决某些异常的操作,若是则将对应告警的状态设为真
                                                if (mainstr == "异常")
                                                {
                                                    m_AlarmList.Add(new AlarmDetail(false, dt, sencstr, logevent));
                                                }
                                                else if (mainstr == "操作")
                                                {
                                                    String opstr = logevent.Substring(0, logevent.Length - 4);  //操作的内容
                                                    for (int i = 0; i < m_AlarmList.Count; i++)
                                                    {
                                                        if (m_AlarmList[i].flag == false)
                                                        {
                                                            String strcont = m_AlarmList[i].content;
                                                            if (sencstr == "主机")
                                                            {
                                                                strcont = strcont.Substring(0, strcont.Length - 5);
                                                            }
                                                            else if (sencstr == "网络")
                                                            {
                                                                strcont = strcont.Substring(0, strcont.Length - 4);
                                                            }

                                                            if (opstr == strcont)
                                                            {
                                                                m_AlarmList[i].flag = true;
                                                                //break;//20100422 Ding Yiming注释
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                catch (Exception ex)
                                {
                                    MessageBoxEx.Show("读取的日志文件出现错误，请检查！" + Environment.NewLine +
                                        "异常:" + ex.Message);
                                }
                            }
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }
        #endregion

        #region 显示告警信息
        //将m_AlarmList中的告警信息显示到窗体控件中
        private void ShowAlarmInfo()
        {
            try
            {
                String strtrue = "已解除";
                String strfalse = "未解除";
                int n = m_AlarmList.Count;   //告警信息总数
                int m = 0;                   //未解除的告警信息数
                for (int i = 0; i < n; i++)
                {
                    ListViewItem lt = new ListViewItem((i + 1).ToString());
                    if (m_AlarmList[i].flag == true)
                    {
                        lt.SubItems.Add(strtrue);
                    }
                    else if (m_AlarmList[i].flag == false)
                    {
                        m++;
                        lt.SubItems.Add(strfalse);
                    }
                    lt.SubItems.Add(m_AlarmList[i].dtime);
                    lt.SubItems.Add(m_AlarmList[i].type);
                    lt.SubItems.Add(m_AlarmList[i].content);
                    this.listView1.Items.Add(lt);
                }
                this.label_TotalCount.Text = n.ToString();
                this.label_UnsolveCount.Text = m.ToString();

                //09-2-4加下面的判断语句
                if (m > 0)
                {
                    MainForm.mflag = true;
                }
                else
                {
                    MainForm.mflag = false;
                }

                //将未解除的告警信息背景设为红色
                for (int i = 0; i < n; i++)
                {
                    if (this.listView1.Items[i].SubItems[1].Text == "未解除")
                    {
                        this.listView1.Items[i].BackColor = Color.Pink;
                    }
                }
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
            
        }
        #endregion

        #region 改变告警信息的状态
        //双击未解决的告警信息,是否更改状态
        private void listView1_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            try
            {
                MessageBoxEx.UseSystemLocalizedString = true;
                if (this.listView1.SelectedItems[0].SubItems[1].Text == "未解除")
                {
                    if (MessageBoxEx.Show("是否更改该告警信息的状态为'已解除'？", "确认更改", MessageBoxButtons.YesNo) == DialogResult.Yes)
                    {
                        //向日志文件中添加一条操作类型的记录,恢复正常,更新告警信息的显示过程

                        LogInfo logInfo = new LogInfo();

                        //需要更改的告警信息次类型为"主机"时的更改方法
                        if (this.listView1.SelectedItems[0].SubItems[3].Text == "主机")
                        {
                            String info = this.listView1.SelectedItems[0].SubItems[4].Text;    //格式为 主机ID号*主机名*日志内容

                            int pos = info.IndexOf('*');
                            logInfo.hostid = Convert.ToInt32(info.Substring(0, pos));    //提取主机ID号
                            info = info.Substring(pos + 1);

                            pos = info.IndexOf('*');
                            logInfo.hostname = info.Substring(0, pos);    //提取主机名
                            info = info.Substring(pos + 1);

                            info = info.Substring(0, info.Length - 5);  //提取日志内容


                            logInfo.logcontent = info + "恢复正常";

                            logInfo.maintype = "操作";
                            logInfo.senctype = this.listView1.SelectedItems[0].SubItems[3].Text;

                            logInfo.WriteLog();

                            GetAlarmInfo();
                            ShowAlarmInfo();
                        }

                        //需要更改的告警信息次类型为"网络"时的更改方法
                        else if (this.listView1.SelectedItems[0].SubItems[3].Text == "网络")
                        {
                            String info = this.listView1.SelectedItems[0].SubItems[4].Text;    //格式为 主机ID号*主机名*日志内容

                            int pos = info.IndexOf('*');
                            logInfo.hostid = Convert.ToInt32(info.Substring(0, pos));    //提取主机ID号
                            info = info.Substring(pos + 1);

                            pos = info.IndexOf('*');
                            logInfo.hostname = info.Substring(0, pos);    //提取主机名
                            info = info.Substring(pos + 1);

                            info = info.Substring(0, info.Length - 4);  //提取日志内容


                            logInfo.logcontent = info + "恢复正常";

                            logInfo.maintype = "操作";
                            logInfo.senctype = this.listView1.SelectedItems[0].SubItems[3].Text;

                            logInfo.WriteLog();

                            GetAlarmInfo();
                            ShowAlarmInfo();
                        }

                    }
                }
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }
        #endregion

    }
}