using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

using DevComponents.DotNetBar;
using System.Xml;



namespace MonitorAndSet
{
    public partial class LogCfg : DevComponents.DotNetBar.Office2007Form
    {
        public String oldlogpath = "";  //修改日志保存路径时用的  //2009-5-14+ 
        
        public LogCfg()
        {
            InitializeComponent();
        }
       
        private FolderBrowserDialog _FBDialog = null;
        private FolderBrowserDialog FBDialogForm
        {
            get { return _FBDialog; }
            set { _FBDialog = value; }
        }
        private void LogCfg_Load(object sender, EventArgs e)
        {
            try
            {
                this.BackColor = MainForm.DlgBackColor;
                tBox_LogPath.Text = MainForm.m_LogPath;
                this.folderBrowserDialog1.SelectedPath = MainForm.m_LogPath;
                this.cBox_loginterval.SelectedIndex = MainForm.loginterval - 1;
                if (MainForm.m_savetime == "Y")
                {
                    this.cBox_savetime.SelectedIndex = 0;
                }
                else if (MainForm.m_savetime == "M")
                {
                    this.cBox_savetime.SelectedIndex = 1;
                }
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }

        private void btn_FoldBrowse_Click(object sender, EventArgs e)
        {
            #region  WINFORM 文件夹浏览

            try
            {
                if (folderBrowserDialog1.ShowDialog() == DialogResult.OK)
                {

                    tBox_LogPath.Text = folderBrowserDialog1.SelectedPath;
                }
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
            #endregion
        }

        private void btn_LCfgCancle_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void btn_LCfgOk_Click(object sender, EventArgs e)
        {
            MessageBoxEx.UseSystemLocalizedString = true;
            try
            {
                String tipstr = "确定修改日志参数配置？";
                if (MainForm.m_savetime == "Y" && this.cBox_savetime.SelectedIndex == 1)
                {
                    tipstr = "将日志保存时间从年改为月会删除前一个月之前的所有日志(如果需要保留请先进行备份),是否确定修改？";
                }

                if (MessageBoxEx.Show(tipstr, "确认修改", MessageBoxButtons.OKCancel) == DialogResult.OK)
                {
                    MainForm.loginterval = this.cBox_loginterval.SelectedIndex + 1;
                    if (this.cBox_savetime.SelectedIndex == 0)
                    {
                        MainForm.m_savetime = "Y";
                    }
                    else if (this.cBox_savetime.SelectedIndex == 1)
                    {
                        MainForm.m_savetime = "M";
                    }

                    String pathstr = this.tBox_LogPath.Text;
                    oldlogpath = MainForm.m_LogPath; //2009-5-14+
                    if (pathstr.Substring(pathstr.Length - 1) == "\\")
                    {
                        MainForm.m_LogPath = this.tBox_LogPath.Text;
                    }
                    else
                    {
                        MainForm.m_LogPath = this.tBox_LogPath.Text + '\\';
                    }

                    //2009-5-14gai
                    String[] yearfoldEntries;      //日志文件夹下的每个年文件夹
                    String[] monthfoldEntries;     //年文件夹下的每个月文件夹
                    String[] fileEntries;          //月文件夹下的每个日志文件

                    try
                    {
                        //2009-5-14+
                        if (oldlogpath != MainForm.m_LogPath) //日志保存路径变化才进行移动
                        {
                            if (oldlogpath.Substring(0, 1) == MainForm.m_LogPath.Substring(0, 1)) //在同一个磁盘卷标内移动
                            {
                                Directory.Move(oldlogpath + "LogFile", MainForm.m_LogPath + "LogFile");
                            }
                            else //不在同一个磁盘卷标内移动
                            {
                                if (Directory.Exists(oldlogpath + "LogFile"))
                                {
                                    yearfoldEntries = Directory.GetDirectories(oldlogpath + "LogFile");
                                    foreach (String yearfold in yearfoldEntries)                  //获取每个年文件夹
                                    {
                                        monthfoldEntries = Directory.GetDirectories(yearfold);
                                        foreach (String monthfold in monthfoldEntries)             //获取每个月文件夹
                                        {
                                            fileEntries = Directory.GetFiles(monthfold);           //获取月文件夹中的每一个日志文件
                                            foreach (String fileName in fileEntries)
                                            {
                                                //fileName为每个日志文件的绝对路径
                                                if (Directory.Exists(MainForm.m_LogPath + monthfold.Substring(oldlogpath.Length)))
                                                {
                                                    File.Move(fileName, MainForm.m_LogPath + fileName.Substring(oldlogpath.Length));
                                                }
                                                else
                                                {
                                                    Directory.CreateDirectory(MainForm.m_LogPath + monthfold.Substring(oldlogpath.Length));
                                                    File.Move(fileName, MainForm.m_LogPath + fileName.Substring(oldlogpath.Length));
                                                }
                                            }
                                            Directory.Delete(monthfold);
                                        }
                                        Directory.Delete(yearfold);
                                    }
                                    Directory.Delete(oldlogpath + "LogFile");
                                }
                            }
                        }

                        //20100601 Ding Yiming 将下面四行代码从前面移到这里
                        
                        String path = MainForm.nowPath + @"Config.xml";
                        XmlDocument doc = new XmlDocument();
                        doc.Load(path);
                        XmlElement root = doc.DocumentElement;
                        foreach (XmlNode node in root)
                        {
                            if (node.Name == "Loginterval")
                            {
                                node.InnerText = MainForm.loginterval.ToString();
                            }
                            else if (node.Name == "LogSavetime")
                            {
                                node.InnerText = MainForm.m_savetime;
                            }
                            else if (node.Name == "LogPath")
                            {
                                node.InnerText = MainForm.m_LogPath;
                            }
                        }
                        doc.Save(path);

                        //end 2009-5-14+
                        MessageBoxEx.Show("日志参数修改成功！");
                    }
                    catch (Exception ex)
                    {
                        this.tBox_LogPath.Text = oldlogpath;
                        MainForm.m_LogPath = oldlogpath;
                        MessageBoxEx.Show("日志文件拷贝移动出现异常，请检查！" + Environment.NewLine + "异常:" + ex.Message);
                    }

                    //监控日志保存时间从"一年之内"改为"一月之内"后,删除前一个月之前的所有日志
                    if (tipstr == "将日志保存时间从年改为月会删除前一个月之前的所有日志(如果需要保留请先进行备份),是否确定修改？")
                    {
                        int year = DateTime.Now.Year;
                        int month = DateTime.Now.Month;

                        String yearstr;    //年文件夹名
                        String monthstr;   //月文件夹名

                        int foldyear;   //文件夹的年
                        int foldmonth;  //文件夹的月

                        if (month == 1)         //删除上一年12月之前的,即删除上一年之前所有的
                        {
                            if (Directory.Exists(MainForm.m_LogPath + "LogFile"))
                            {
                                yearfoldEntries = Directory.GetDirectories(MainForm.m_LogPath + "LogFile");
                                foreach (String yearfold in yearfoldEntries)                  //获取每个年文件夹
                                {
                                    yearstr = yearfold.Substring(yearfold.LastIndexOf('\\') + 1);
                                    foldyear = Convert.ToInt32(yearstr.Substring(0, yearstr.IndexOf('年')));

                                    if (foldyear < year)
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
                            }
                        }
                        else          //删除本年内前一个月之前的
                        {
                            if (Directory.Exists(MainForm.m_LogPath + "LogFile"))
                            {
                                yearfoldEntries = Directory.GetDirectories(MainForm.m_LogPath + "LogFile");
                                foreach (String yearfold in yearfoldEntries)                  //获取每个年文件夹
                                {
                                    yearstr = yearfold.Substring(yearfold.LastIndexOf('\\') + 1);
                                    foldyear = Convert.ToInt32(yearstr.Substring(0, yearstr.IndexOf('年')));

                                    if (foldyear < year)
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

                                    if (foldyear == year)
                                    {
                                        monthfoldEntries = Directory.GetDirectories(yearfold);
                                        foreach (String monthfold in monthfoldEntries)             //获取每个月文件夹
                                        {
                                            monthstr = monthfold.Substring(monthfold.LastIndexOf('\\') + 1);
                                            foldmonth = Convert.ToInt32(monthstr.Substring(0, monthstr.IndexOf('月')));

                                            if (foldmonth < month) //20100601 Ding Yiming 去掉"-1"
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
                                }
                            }
                        }

                    }
                    Close();
                }
                else
                {
                    tBox_LogPath.Text = MainForm.m_LogPath;
                    this.cBox_loginterval.SelectedIndex = MainForm.loginterval - 1;
                    if (MainForm.m_savetime == "Y")
                    {
                        this.cBox_savetime.SelectedIndex = 0;
                    }
                    else if (MainForm.m_savetime == "M")
                    {
                        this.cBox_savetime.SelectedIndex = 1;
                    }
                }
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }
    }
}