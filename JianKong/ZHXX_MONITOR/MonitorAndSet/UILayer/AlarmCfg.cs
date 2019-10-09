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
    public partial class AlarmCfg : DevComponents.DotNetBar.Office2007Form
    {
        public String m_hostIPAddr = "";  //表示显示的是哪个主机的具体的告警门限值信息

        public AlarmCfg()
        {
            InitializeComponent();
        }

        private void AlarmCfg_Load(object sender, EventArgs e)
        {
            try
            {
                this.BackColor = MainForm.DlgBackColor;
                this.tBox_CPUTHValue.Text = MainForm.CPUVALUE.ToString();
                this.tBox_DISKTHValue.Text = MainForm.DISKVALUE.ToString();
                this.tBox_MEMTHValue.Text = MainForm.MEMVALUE.ToString();
                this.tBox_TRAFFICValue.Text = MainForm.TRAFFICVALUE.ToString();
                this.tBox_staInterval.Text = MainForm.statInterval.ToString();
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }

        private void btn_ACfgCancle_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void btn_ACfgOk_Click(object sender, EventArgs e)
        {
            //2009-5-18+
            MessageBoxEx.UseSystemLocalizedString = true;
            try
            {
                int.Parse(this.tBox_CPUTHValue.Text);
            }
            catch
            {
                MessageBoxEx.Show("CPU门限值错误！\n请输入0-100之间的整数！", "提示", MessageBoxButtons.OK, MessageBoxIcon.Information);
                ReadFile();//20100601 Ding Yiming+
                return;
            }
            try
            {
                int.Parse(this.tBox_MEMTHValue.Text);
            }
            catch
            {
                MessageBoxEx.Show("内存剩余门限值错误！\n请输入0-300,000(300GB)之间的整数！", "提示", MessageBoxButtons.OK, MessageBoxIcon.Information);
                ReadFile();//20100601 Ding Yiming+
                return;
            }
            try
            {
                int.Parse(this.tBox_DISKTHValue.Text);
            }
            catch
            {
                MessageBoxEx.Show("硬盘剩余门限值错误！\n请输入0-500,000,000(500TB)之间的整数！", "提示", MessageBoxButtons.OK, MessageBoxIcon.Information);
                ReadFile();//20100601 Ding Yiming+
                return;
            }
            try
            {
                int.Parse(this.tBox_TRAFFICValue.Text);
            }
            catch
            {
                MessageBoxEx.Show("流量最大限值！\n请输入大于零的整数！", "提示", MessageBoxButtons.OK, MessageBoxIcon.Information);
                this.tBox_TRAFFICValue.Text = MainForm.TRAFFICVALUE.ToString();
                return;
            }
            try
            {
                int.Parse(this.tBox_staInterval.Text);
            }
            catch
            {
                MessageBoxEx.Show("统计时间间隔！\n请输入大于零的整数！", "提示", MessageBoxButtons.OK, MessageBoxIcon.Information);
                this.tBox_staInterval.Text = MainForm.statInterval.ToString();
                return;
            }
            //2009-5-18+

            try
            {
                if (GetCPUValue() <= 0 || GetCPUValue() >= 100) //20100601 Ding Yiming 将">"改为">=" 
                {
                    MessageBoxEx.Show("CPU门限值错误！\n请输入0-100之间的整数");
                    ReadFile();
                    return;
                }
                if (GetMemValue() <= 0 || GetMemValue() >= 300000) //20100601 Ding Yiming 将">"改为">=" 
                {
                    MessageBoxEx.Show("内存剩余门限值错误！\n请输入0-300,000(300GB)之间的整数");
                    ReadFile();
                    return;
                }
                if (GetDiskValue() <= 0 || GetDiskValue() >= 500000000) //20100601 Ding Yiming 将">"改为">=" 
                {
                    MessageBoxEx.Show("硬盘剩余门限值错误！\n请输入0-500,000,000(500TB)之间的整数");
                    ReadFile();
                    return;
                }
                if (GetTrafficValue() <= 0) //20100601 Ding Yiming 将">"改为">=" 
                {
                    MessageBoxEx.Show("流量剩余门限值错误！\n请输入大于0整数");
                    ReadFile();
                    return;
                }
                if (GetStaInterValue() <= 0) //20100601 Ding Yiming 将">"改为">=" 
                {
                    MessageBoxEx.Show("时间间隔错误！\n请输入大于0整数");
                    ReadFile();
                    return;
                }
                if (MessageBoxEx.Show("确定修改告警参数配置？", "确认修改", MessageBoxButtons.OKCancel) == DialogResult.OK)
                {
                    WriteFile();
                    SetCPUValue(GetCPUValue());
                    SetDiskValue(GetDiskValue());
                    SetMemValue(GetMemValue());
                    SetTrafficValue(GetTrafficValue());
                    SetStaInterValue(GetStaInterValue());
                    //09-2-16加下面的循环 目的:修改总的告警参数时,具体机器的告警参数也跟着改变成统一的告警参数
                    for (int i = 0; i < MainForm.allInfo.Count; i++)
                    {
                        MainForm.allInfo[i].cpuTH = GetCPUValue();
                        MainForm.allInfo[i].diskTH = GetDiskValue();
                        MainForm.allInfo[i].memTH = GetMemValue();
                        MainForm.allInfo[i].trafficTH = GetTrafficValue();
                    }
                    MessageBoxEx.Show("告警参数修改成功！");
                    Close();
                }
                else
                {
                    this.tBox_CPUTHValue.Text = MainForm.CPUVALUE.ToString();
                    this.tBox_DISKTHValue.Text = MainForm.DISKVALUE.ToString();
                    this.tBox_MEMTHValue.Text = MainForm.MEMVALUE.ToString();
                    this.tBox_TRAFFICValue.Text = MainForm.TRAFFICVALUE.ToString();
                    this.tBox_staInterval.Text = MainForm.statInterval.ToString();
                }
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }

        public void SetCPUValue(int cpuValue)
        {
            try
            {
                this.tBox_CPUTHValue.Text = cpuValue.ToString();
                MainForm.CPUVALUE = cpuValue;
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }

        public void SetDiskValue(int diskValue)
        {
            try
            {
                this.tBox_DISKTHValue.Text = diskValue.ToString();
                MainForm.DISKVALUE = diskValue;
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }

        public void SetMemValue(int memValue)
        {
            try
            {
                this.tBox_MEMTHValue.Text = memValue.ToString();
                MainForm.MEMVALUE = memValue;
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }

        public void SetTrafficValue(int trafficvalue)
        {
            try
            {
                this.tBox_TRAFFICValue.Text = trafficvalue.ToString();
                MainForm.TRAFFICVALUE = trafficvalue;
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }

        public void SetStaInterValue(int staInterval)
        {
            try
            {
                this.tBox_staInterval.Text = staInterval.ToString();
                MainForm.statInterval = staInterval;
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }

        public int GetCPUValue()
        {
            return Convert.ToInt32(this.tBox_CPUTHValue.Text);
        }

        public int GetDiskValue()
        {
            return Convert.ToInt32(this.tBox_DISKTHValue.Text);
        }

        public int GetMemValue()
        {
            return Convert.ToInt32(this.tBox_MEMTHValue.Text);
        }

        public int GetTrafficValue()
        {
            return Convert.ToInt32(this.tBox_TRAFFICValue.Text);
        }
        public int GetStaInterValue()
        {
            return Convert.ToInt32(this.tBox_staInterval.Text);
        }

        public void ReadFile()
        {
            try
            {
                String path = MainForm.nowPath + @"Config.xml";
                XmlDocument doc = new XmlDocument();
                doc.Load(path);
                XmlElement root = doc.DocumentElement;
                foreach (XmlNode node in root)
                {
                    if (node.Name == "CPU")
                    {
                        SetCPUValue(Convert.ToInt32(node.InnerText));
                    }
                    else if (node.Name == "Disk")
                    {
                        SetDiskValue(Convert.ToInt32(node.InnerText));
                    }
                    else if (node.Name == "Memory")
                    {
                        SetMemValue(Convert.ToInt32(node.InnerText));
                    }
                    else if (node.Name == "Traffic")
                    {
                        SetTrafficValue(Convert.ToInt32(node.InnerText));
                    }
                    else if (node.Name == "Statinterval")
                    {
                        SetTrafficValue(Convert.ToInt32(node.InnerText));
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBoxEx.UseSystemLocalizedString = true;
                MessageBoxEx.Show("指定的配置文件不存在或出现错误，请检查！" + Environment.NewLine +
                    "异常:" + ex.Message);
            }
        }

        public void WriteFile()
        {
            try
            {
                String path = MainForm.nowPath + @"Config.xml";
                XmlDocument doc = new XmlDocument();
                doc.Load(path);
                XmlElement root = doc.DocumentElement;
                foreach (XmlNode node in root)
                {
                    if (node.Name == "CPU")
                    {
                        node.InnerText = GetCPUValue().ToString();
                    }
                    else if (node.Name == "Disk")
                    {
                        node.InnerText = GetDiskValue().ToString();
                    }
                    else if (node.Name == "Memory")
                    {
                        node.InnerText = GetMemValue().ToString();
                    }
                    else if (node.Name == "Traffic")
                    {
                        node.InnerText = GetTrafficValue().ToString();
                    }
                    else if (node.Name == "Statinterval")
                    {
                        node.InnerText = GetStaInterValue().ToString();
                    }
                    
                }
                doc.Save(path);
            }
            catch(Exception ex)
            {
                MessageBoxEx.UseSystemLocalizedString = true;
                MessageBoxEx.Show("指定的配置文件不存在或出现错误，请检查！" + Environment.NewLine +
                    "异常:" + ex.Message);
            }
        }

    }
}