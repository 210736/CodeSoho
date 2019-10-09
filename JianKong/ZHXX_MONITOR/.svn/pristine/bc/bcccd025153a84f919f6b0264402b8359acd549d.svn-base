using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using MonitorAndSet.CommonClass;
using System.IO;
using DevComponents.DotNetBar;
using DevComponents.DotNetBar.Rendering;
using DevComponents.Editors.DateTimeAdv;

namespace MonitorAndSet
{
    public partial class HistroyQuery : DevComponents.DotNetBar.Office2007Form
    {
        List<string> iplistCLPT = new List<string>();
        List<string> iplistGLPT = new List<string>();
        private const string CLPTDBNAME="tdm_task_log";
        private const string GLPTDBNAME = "dps_syslognew";
        public HistroyQuery()
        {
            InitializeComponent();
        }
        public class DbLogData
        {
            string logType = string.Empty;
            string IP = string.Empty;
            string logData = string.Empty;
        }
        private void HistroyQuery_Load(object sender, EventArgs e)
        {
            try
            {
                this.BackColor = MainForm.DlgBackColor;
                this.dateTimePicker1.Value = System.DateTime.Today;
                this.dateTimePicker2.Value = System.DateTime.Today;
                this.dateTimePicker1.Enabled = false;
                this.dateTimePicker2.Enabled = false;
                this.cBox_MainStyle.Enabled = false;
                this.cBox_SecondStyle.Enabled = false;
                getiplist();
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }


        private void btn_HQuery_Click(object sender, EventArgs e)
        {
            if (tabControl1.SelectedTab == tabPage1)
            {
                searchDevices();
            }
            else if (tabControl1.SelectedTab == tabPage2)
            {
                searchProPlatform();
            }
            else if (tabControl1.SelectedTab == tabPage3)
            {
                searchManagePlatform();
            }
    
        }

        private void getiplist()
        {
            Database db = new Database();
            string sql = " select distinct f_handleip from " + CLPTDBNAME;
            string sql2 = " select distinct IPADDRESS from " + GLPTDBNAME;
            //
            DataSet ds = db.GetDataSet(sql);
            int hostcount = ds.Tables[0].Rows.Count;
            if (hostcount > 0)
            {
                for (int i = 0; i < hostcount; i++)
                {
                    DataRow dr = ds.Tables[0].Rows[i];
                    string ip =dr["f_handleip"].ToString();
                    cBox_CLPT_IP.Items.Add(ip);
                }
            }
            //           
            ds = db.GetDataSet(sql2);
            hostcount = ds.Tables[0].Rows.Count;
            if (hostcount > 0)
            {
                for (int i = 0; i < hostcount; i++)
                {
                    DataRow dr = ds.Tables[0].Rows[i];
                    string ip = dr["IPADDRESS"].ToString();
                    cBox_GLPT_IP.Items.Add(ip);
                }
            }


        }
        /// <summary>
        /// 处理平台日志
        /// </summary>
        private void searchProPlatform()
        {
             MessageBoxEx.UseSystemLocalizedString = true;
             try
             {              
                if (dateTimeStart_CLPT.Value > dateTimeEnd_CLPT.Value)
                {
                    MessageBoxEx.Show("开始时间应早于结束时间！");
                    return;
                } 
                listView_clpt.Items.Clear();
                string ip = cBox_CLPT_IP.Text == "全部" ? "" : cBox_CLPT_IP.Text;
                searchDB(CLPTDBNAME, ip, dateTimeStart_CLPT.Value, dateTimeEnd_CLPT.Value);               
             }
             catch (Exception e)
             {

             }
        }

        private void searchDB(string DBName, string IP = "", DateTime startTime=new DateTime(),DateTime endTime = new DateTime() )
        {
            List<DbLogData> logs = new List<DbLogData>();
            try
            {
                Database db = new Database();
                string sql = "";
                string ipstr = "";
                string timestr="";
                if (DBName == GLPTDBNAME)
                {
                    sql = "SELECT  to_char(createtime,'yyyy-mm-dd hh24:mi:ss'),  IPADDRESS, opcontent from "+ GLPTDBNAME;
                    if (IP != "")
                        ipstr = String.Format(" IPADDRESS='{0}'", IP);
                    if (startTime != new DateTime() && endTime != new DateTime())
                    {
                        string starttime = startTime.ToString("yyyy-MM-dd");
                        string entime = endTime.ToString("yyyy-MM-dd");
                        timestr = string.Format("to_timestamp(createtime)>=timestamp'{0} 00:00:00' and to_timestamp(createtime)<=timestamp'{1} 23:59:59'", starttime, entime);
                    }
                    if (ipstr != "" &&timestr != "")
                    {
                        sql += string.Format(" where {0} and {1}", ipstr, timestr);
                    }
                    else if (ipstr != "")
                    {
                        sql += string.Format(" where {0}", ipstr);
                    }
                    else if (timestr != "")
                    {
                        sql += string.Format(" where {0}", timestr);
                    }
                }
                else if (DBName == CLPTDBNAME)
                {
                    sql = "SELECT  to_char(F_HANDLETIME,'yyyy-mm-dd hh24:mi:ss'), f_handleip, F_content from "+CLPTDBNAME;
                    if (IP != "")
                        ipstr = String.Format(" f_handleip='{0}'", IP);
                    if (startTime != new DateTime() && endTime != new DateTime())
                    {
                        string starttime = startTime.ToString("yyyy-MM-dd");
                        string entime = endTime.ToString("yyyy-MM-dd");
                        timestr = string.Format("to_timestamp(F_HANDLETIME)>=timestamp'{0} 00:00:00' and to_timestamp(F_HANDLETIME)<=timestamp'{1} 23:59:59'", starttime, entime);
                    }
                    if (ipstr != "" && timestr != "")
                    {
                        sql += string.Format(" where {0} and {1}", ipstr, timestr);
                    }
                    else if (ipstr != "")
                    {
                        sql += string.Format(" where {0}", ipstr);
                    }
                    else if (timestr != "")
                    {
                        sql += string.Format(" where {0}", timestr);
                    }
                }
                DataSet ds = db.GetDataSet(sql);
                int hostcount = ds.Tables[0].Rows.Count;
                if (hostcount > 0)
                {
                    for (int i = 0; i < hostcount; i++)
                    {
                        DataRow dr = ds.Tables[0].Rows[i];
                        string time = dr[0].ToString();
                        string ip = dr[1].ToString();
                        string content = dr[2].ToString();
                        ListViewItem it = new ListViewItem(time);
                        it.SubItems.Add(ip);
                        it.SubItems.Add(content);
                        if (DBName == CLPTDBNAME)
                            listView_clpt.Items.Add(it);
                        else
                            listView_glpt.Items.Add(it);
                    }
                }
            }
            catch (Exception e) { }

        }
        /// <summary>
        /// 管理平台日志
        /// </summary>
        private void searchManagePlatform()
        {
            MessageBoxEx.UseSystemLocalizedString = true;
            try
            {            
                if (dateTimeStart_GLPT.Value > dateTimeEnd_GLPT.Value)
                {
                    MessageBoxEx.Show("开始时间应早于结束时间！");
                    return;
                }
                listView_glpt.Items.Clear();
                string ip = cBox_GLPT_IP.Text == "全部" ? "" : cBox_GLPT_IP.Text;
                searchDB(GLPTDBNAME, ip, dateTimeStart_GLPT.Value, dateTimeEnd_GLPT.Value);
            }
            catch (Exception e)
            {

            }
        }

        private void searchDevices()
        {
            MessageBoxEx.UseSystemLocalizedString = true;
            try
            {

                bool flag = true;
                if (cBox_QueryStyle.Text == "")
                {
                    flag = false;
                    MessageBoxEx.Show("请选择具体的查询类型！");
                }
                else if (cBox_QueryStyle.Text == "类型" || cBox_QueryStyle.Text == "类型和时间")
                {
                    if (cBox_MainStyle.Text == "")
                    {
                        flag = false;
                        MessageBoxEx.Show("此时主类型不可为空，请选择主类型！");
                    }
                }
                if (flag)   //根据查询条件显示查询结果
                {
                    this.listView1.Items.Clear();  //清空之前的查询结果

                    String[] yearfoldEntries;      //日志文件夹下的每个年文件夹
                    String[] monthfoldEntries;     //年文件夹下的每个月文件夹

                    String[] fileEntries;          //月文件夹下的每个日志文件 
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

                    #region 查询类型为"全部"
                    if (cBox_QueryStyle.Text == "全部")    //显示所有日志内容
                    {
                        if (Directory.Exists(MainForm.m_LogPath + "LogFile"))//+ "LogFile"
                        {
                            yearfoldEntries = Directory.GetDirectories(MainForm.m_LogPath + "LogFile");//+ "LogFile"
                            foreach (String yearfold in yearfoldEntries)                   //获取每个年文件夹
                            {
                                monthfoldEntries = Directory.GetDirectories(yearfold);
                                foreach (String monthfold in monthfoldEntries)             //获取每个月文件夹
                                {
                                    fileEntries = Directory.GetFiles(monthfold);           //获取月文件夹中的每一个日志文件
                                    foreach (String fileName in fileEntries)
                                    {
                                        try
                                        {
                                            using (StreamReader sr = File.OpenText(fileName))
                                            {
                                                while (sr.Peek() >= 0)
                                                {
                                                    str = sr.ReadLine();
                                                    int n = str.IndexOf('-');
                                                    ListViewItem lt = new ListViewItem(str.Substring(0, n));  //日期时间

                                                    str = str.Substring(n + 5);
                                                    n = str.IndexOf('-');
                                                    lt.SubItems.Add(str.Substring(0, n));  //主类型

                                                    str = str.Substring(n + 5);
                                                    n = str.IndexOf('-');
                                                    lt.SubItems.Add(str.Substring(0, n));  //次类型

                                                    str = str.Substring(n + 6);
                                                    lt.SubItems.Add(str);  //日志内容

                                                    this.listView1.Items.Add(lt);
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
                        if (this.listView1.Items.Count == 0)
                        {
                            MessageBoxEx.Show("没有符合查询条件的历史记录！");
                        }
                    }
                    #endregion

                    #region 查询类型为"类型"
                    //根据主类型和次类型显示日志内容
                    else if (cBox_QueryStyle.Text == "类型")
                    {
                        if (Directory.Exists(MainForm.m_LogPath + "LogFile"))//+ "LogFile"
                        {
                            yearfoldEntries = Directory.GetDirectories(MainForm.m_LogPath + "LogFile");//+ "LogFile"
                            foreach (String yearfold in yearfoldEntries)                   //获取每个年文件夹
                            {
                                monthfoldEntries = Directory.GetDirectories(yearfold);
                                foreach (String monthfold in monthfoldEntries)             //获取每个月文件夹
                                {
                                    fileEntries = Directory.GetFiles(monthfold);           //获取月文件夹中的每一个日志文件
                                    foreach (String fileName in fileEntries)
                                    {
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

                                                    if (mainstr == this.cBox_MainStyle.Text)
                                                    {
                                                        if (this.cBox_SecondStyle.Text == "")
                                                        {
                                                            ListViewItem lt = new ListViewItem(dt);
                                                            lt.SubItems.Add(mainstr);
                                                            lt.SubItems.Add(sencstr);
                                                            lt.SubItems.Add(logevent);
                                                            this.listView1.Items.Add(lt);
                                                        }
                                                        else if (this.cBox_SecondStyle.Text == sencstr)
                                                        {
                                                            ListViewItem lt = new ListViewItem(dt);
                                                            lt.SubItems.Add(mainstr);
                                                            lt.SubItems.Add(sencstr);
                                                            lt.SubItems.Add(logevent);
                                                            this.listView1.Items.Add(lt);
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
                        if (this.listView1.Items.Count == 0)
                        {
                            MessageBoxEx.Show("没有符合查询条件的历史记录！");
                        }
                    }
                    #endregion

                    #region 查询类型为"时间"
                    //根据起止时间显示日志内容
                    else if (cBox_QueryStyle.Text == "时间")
                    {
                        if (Directory.Exists(MainForm.m_LogPath + "LogFile"))//+ "LogFile"
                        {
                            int startyear = dateTimePicker1.Value.Year;    //开始时间的年
                            int startmonth = dateTimePicker1.Value.Month;  //开始时间的月
                            int startday = dateTimePicker1.Value.Day;      //开始时间的日

                            int endyear = dateTimePicker2.Value.Year;    //终止时间的年
                            int endmonth = dateTimePicker2.Value.Month;  //终止时间的月
                            int endday = dateTimePicker2.Value.Day;      //终止时间的日


                            DateTime dt1 = dateTimePicker1.Value;
                            DateTime dt2 = dateTimePicker2.Value;



                            if (dt1 > dt2)
                            {
                                MessageBoxEx.Show("起始时间不能晚于截至时间！");
                            }
                            else
                            {
                                int year = 0;   //年
                                int month = 0;  //月
                                int day = 0;

                                String yearstr = "";    //年文件夹名
                                String monthstr = "";   //月文件夹名
                                String filestr = "";    //日志文件名

                                yearfoldEntries = Directory.GetDirectories(MainForm.m_LogPath + "LogFile");// + "LogFile"
                                foreach (String yearfold in yearfoldEntries)                   //获取每个年文件夹
                                {
                                    yearstr = yearfold.Substring(yearfold.LastIndexOf('\\') + 1);
                                    year = Convert.ToInt32(yearstr.Substring(0, yearstr.IndexOf('年')));

                                    if (year < startyear || year > endyear)          //年文件夹的年 在起止年之外
                                    {
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
                                                if (month < startmonth || month > endmonth)
                                                {
                                                    continue;
                                                }
                                            }
                                            else
                                            {
                                                if (month < startmonth)
                                                {
                                                    continue;
                                                }
                                            }
                                        }
                                        else if (year == endyear)
                                        {
                                            if (year == startyear)
                                            {
                                                if (month < startmonth || month > endmonth)
                                                {
                                                    continue;
                                                }
                                            }
                                            else
                                            {
                                                if (month > endmonth)
                                                {
                                                    continue;
                                                }
                                            }
                                        }

                                        fileEntries = Directory.GetFiles(monthfold);           //获取月文件夹中的每一个日志文件
                                        foreach (String fileName in fileEntries)
                                        {
                                            filestr = fileName.Substring(fileName.LastIndexOf('\\') + 1);
                                            //day = Convert.ToInt32(filestr.Substring(0, filestr.IndexOf('日')));
                                            //int interIndex =filestr.IndexOf('日') + 2;
                                            //interval = Convert.ToInt32(filestr.Substring(interIndex, filestr.IndexOf('天')));
                                            //filestr = filestr.Substring(filestr.IndexOf('日') + 4);

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

                                            if (endlogdt.CompareTo(dateTimePicker1.Value.Date) < 0)
                                            {
                                                continue;
                                            }
                                            if (startlogdt.CompareTo(dateTimePicker2.Value.Date) > 0)
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

                                                        if (logdt.CompareTo(dateTimePicker1.Value.Date) >= 0 && logdt.CompareTo(dateTimePicker2.Value.Date) <= 0)
                                                        {
                                                            ListViewItem lt = new ListViewItem(dt);
                                                            lt.SubItems.Add(mainstr);
                                                            lt.SubItems.Add(sencstr);
                                                            lt.SubItems.Add(logevent);
                                                            this.listView1.Items.Add(lt);
                                                        }
                                                    }
                                                }
                                            }
                                            catch (Exception ex)
                                            {
                                                MessageBoxEx.UseSystemLocalizedString = true;
                                                MessageBoxEx.Show("读取的日志文件出现错误，请检查！" + Environment.NewLine +
                                                    "异常:" + ex.Message);
                                            }

                                        }

                                    }
                                }
                                if (this.listView1.Items.Count == 0)
                                {
                                    MessageBoxEx.UseSystemLocalizedString = true;
                                    MessageBoxEx.Show("没有符合查询条件的历史记录！");
                                }
                            }
                        }

                    }
                    #endregion

                    #region 查询类型为"类型&时间"
                    //根据主次类型和起始时间显示日志内容
                    else if (cBox_QueryStyle.Text == "类型和时间")
                    {
                        if (Directory.Exists(MainForm.m_LogPath + "LogFile"))//+ "LogFile"
                        {
                            int startyear = dateTimePicker1.Value.Year;    //开始时间的年
                            int startmonth = dateTimePicker1.Value.Month;  //开始时间的月
                            int startday = dateTimePicker1.Value.Day;      //开始时间的日

                            int endyear = dateTimePicker2.Value.Year;    //终止时间的年
                            int endmonth = dateTimePicker2.Value.Month;  //终止时间的月
                            int endday = dateTimePicker2.Value.Day;      //终止时间的日


                            DateTime dt1 = dateTimePicker1.Value;
                            DateTime dt2 = dateTimePicker2.Value;

                            if (dt1 > dt2)
                            {
                                MessageBoxEx.Show("起始时间不能晚于截至时间！");
                            }
                            else
                            {
                                int year = 0;   //年
                                int month = 0;  //月
                                int day = 0;

                                String yearstr = "";    //年文件夹名
                                String monthstr = "";   //月文件夹名
                                String filestr = "";    //日志文件名

                                yearfoldEntries = Directory.GetDirectories(MainForm.m_LogPath + "LogFile");//+ "LogFile"
                                foreach (String yearfold in yearfoldEntries)                   //获取每个年文件夹
                                {
                                    yearstr = yearfold.Substring(yearfold.LastIndexOf('\\') + 1);
                                    year = Convert.ToInt32(yearstr.Substring(0, yearstr.IndexOf('年')));

                                    if (year < startyear || year > endyear)          //年文件夹的年 在起止年之外
                                    {
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
                                                if (month < startmonth || month > endmonth)
                                                {
                                                    continue;
                                                }
                                            }
                                            else
                                            {
                                                if (month < startmonth)
                                                {
                                                    continue;
                                                }
                                            }
                                        }
                                        else if (year == endyear)
                                        {
                                            if (year == startyear)
                                            {
                                                if (month < startmonth || month > endmonth)
                                                {
                                                    continue;
                                                }
                                            }
                                            else
                                            {
                                                if (month > endmonth)
                                                {
                                                    continue;
                                                }
                                            }
                                        }

                                        fileEntries = Directory.GetFiles(monthfold);           //获取月文件夹中的每一个日志文件
                                        foreach (String fileName in fileEntries)
                                        {
                                            filestr = fileName.Substring(fileName.LastIndexOf('\\') + 1);
                                            //day = Convert.ToInt32(filestr.Substring(0, filestr.IndexOf('日'))); 
                                            //interval = Convert.ToInt32(filestr.Substring(0, filestr.IndexOf('天')));
                                            //filestr = filestr.Substring(filestr.IndexOf('日') + 4);

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

                                            if (endlogdt.CompareTo(dateTimePicker1.Value.Date) < 0)
                                            {
                                                continue;
                                            }
                                            if (startlogdt.CompareTo(dateTimePicker2.Value.Date) > 0)
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

                                                        if (logdt.CompareTo(dateTimePicker1.Value.Date) >= 0 && logdt.CompareTo(dateTimePicker2.Value.Date) <= 0)
                                                        {
                                                            if (mainstr == this.cBox_MainStyle.Text)
                                                            {
                                                                if (this.cBox_SecondStyle.Text == "")
                                                                {
                                                                    ListViewItem lt = new ListViewItem(dt);
                                                                    lt.SubItems.Add(mainstr);
                                                                    lt.SubItems.Add(sencstr);
                                                                    lt.SubItems.Add(logevent);
                                                                    this.listView1.Items.Add(lt);
                                                                }
                                                                else if (this.cBox_SecondStyle.Text == sencstr)
                                                                {
                                                                    ListViewItem lt = new ListViewItem(dt);
                                                                    lt.SubItems.Add(mainstr);
                                                                    lt.SubItems.Add(sencstr);
                                                                    lt.SubItems.Add(logevent);
                                                                    this.listView1.Items.Add(lt);
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
                                if (this.listView1.Items.Count == 0)
                                {
                                    MessageBoxEx.Show("没有符合查询条件的历史记录！");
                                }
                            }
                        }
                    }
                    #endregion
                }
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }
        /// <summary>
        /// 根据查询类型来确定查询条件的可用性
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void cBox_QueryStyle_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (this.cBox_QueryStyle.Text == "全部")
            {
                this.dateTimePicker1.Enabled = false;
                this.dateTimePicker2.Enabled = false;
                this.cBox_MainStyle.Enabled = false;
                this.cBox_SecondStyle.Enabled = false;
            }
            else if (this.cBox_QueryStyle.Text == "类型")
            {
                this.dateTimePicker1.Enabled = false;
                this.dateTimePicker2.Enabled = false;
                this.cBox_MainStyle.Enabled = true;
                this.cBox_SecondStyle.Enabled = true;
            }
            else if (this.cBox_QueryStyle.Text == "时间")
            {
                this.dateTimePicker1.Enabled = true;
                this.dateTimePicker2.Enabled = true;
                this.cBox_MainStyle.Enabled = false;
                this.cBox_SecondStyle.Enabled = false;
            }
            else if (this.cBox_QueryStyle.Text == "类型和时间")
            {
                this.dateTimePicker1.Enabled = true;
                this.dateTimePicker2.Enabled = true;
                this.cBox_MainStyle.Enabled = true;
                this.cBox_SecondStyle.Enabled = true;
            }
        }





    }
}