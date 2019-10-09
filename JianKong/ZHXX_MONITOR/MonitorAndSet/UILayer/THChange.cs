using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Linq;
using System.Xml;
using System.Xml.Linq;
using System.Windows.Forms;
using DevComponents.DotNetBar;
using DevComponents.DotNetBar.Controls;
using System.IO;


namespace MonitorAndSet
{
    public partial class THChange : DevComponents.DotNetBar.Office2007Form
    {
        public String m_hostIPAddr = "";  //表示显示的是哪个主机的具体的告警门限值信息
        private int m_cpu;
        private int m_disk;
        private int m_mem;
        private int m_traffic;
        string filepath = "systemData";
        public THChange()
        {
            InitializeComponent();
        }

        private void THChange_Load(object sender, EventArgs e)
        {
            try
            {
                this.BackColor = MainForm.DlgBackColor;
                label5.Text = "限制启动的进程服务\r\n(如有多个，一行填写一个)";
                for (int i = 0; i < MainForm.allInfo.Count; i++)
                {
                    if (MainForm.allInfo[i].m_sIPInDatabase == m_hostIPAddr)
                    {
                        m_cpu = MainForm.allInfo[i].cpuTH;
                        m_disk = MainForm.allInfo[i].diskTH;
                        m_mem = MainForm.allInfo[i].memTH;
                        m_traffic = MainForm.allInfo[i].trafficTH;
                        ReadProcess(i);
                        break;
                    }
                }
                ShowTHValue();
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }

        private void btn_ChangeOk_Click(object sender, EventArgs e)
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
                ShowTHValue();//20100601 Ding Yiming+
                return;
            }
            try
            {
                int.Parse(this.tBox_MEMTHValue.Text);
            }
            catch
            {
                MessageBoxEx.Show("内存剩余门限值错误！\n请输入0-30000之间的整数！", "提示", MessageBoxButtons.OK, MessageBoxIcon.Information);
                ShowTHValue();//20100601 Ding Yiming+
                return;
            }
            try
            {
                int.Parse(this.tBox_DISKTHValue.Text);
            }
            catch
            {
                MessageBoxEx.Show("硬盘剩余门限值错误！\n请输入0-5000之间的整数！", "提示", MessageBoxButtons.OK, MessageBoxIcon.Information);
                ShowTHValue();//20100601 Ding Yiming+
                return;
            }
            try
            {
                //2009-5-18+
                if (GetCPUValue() <= 0 || GetCPUValue() >= 100) //20100601 Ding Yiming 将">"改为">=" 
                {
                    MessageBoxEx.Show("CPU门限值错误！\n请输入0-100之间的整数");
                    ShowTHValue();
                    return;
                }
                if (GetMemValue() <= 0 || GetMemValue() >= 30000) //20100601 Ding Yiming 将">"改为">=" 
                {
                    MessageBoxEx.Show("内存剩余门限值错误！\n请输入0-30000之间的整数");
                    ShowTHValue();
                    return;
                }
                if (GetDiskValue() <= 0 || GetDiskValue() >= 5000) //20100601 Ding Yiming 将">"改为">=" 
                {
                    MessageBoxEx.Show("硬盘剩余门限值错误！\n请输入0-5000之间的整数");
                    ShowTHValue();
                    return;
                }
                if (GetTrafficValue() <= 0) //20100601 Ding Yiming 将">"改为">=" 
                {
                    MessageBoxEx.Show("流量门限值设置错误！\n请输入大于0的整数");
                    ShowTHValue();
                    return;
                }

                if (MessageBoxEx.Show("确定修改该设备的告警门限值？", "确认修改", MessageBoxButtons.OKCancel) == DialogResult.OK)
                {
                    GetCPUValue();
                    GetDiskValue();
                    GetMemValue();
                    GetTrafficValue();
                    for (int i = 0; i < MainForm.allInfo.Count; i++)
                    {
                        if (MainForm.allInfo[i].m_sIPInDatabase == m_hostIPAddr)
                        {
                            MainForm.allInfo[i].cpuTH = m_cpu;
                            MainForm.allInfo[i].diskTH = m_disk;
                            MainForm.allInfo[i].memTH = m_mem;
                            MainForm.allInfo[i].trafficTH = m_traffic;
                            setForbiddenPro(i, tBox_forbiddenPro.Text);
                            updatexmlfile(filepath, MainForm.allInfo[i].hostName, tBox_forbiddenPro.Text);
                            break;
                        }
                    }
                    MessageBoxEx.Show("该设备的告警门限值更改成功！");
                    Close();
                }
                else
                {
                    ShowTHValue();
                }
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }

        }

        private void setForbiddenPro(int id, string prosData)
        {
            MainForm.allInfo[id].forbiddenPro.Clear();
            string pros = prosData.Replace("\r\n",";");
            string[] data = pros.Split(';');
            for (int j = 0; j < data.Length; j++)
            {
                if ( !MainForm.allInfo[id].forbiddenPro.ContainsKey(data[j])  && data[j] != "")
                   MainForm.allInfo[id].forbiddenPro.Add(data[j].Trim(), true);
            }
        }
        private void btn_ChangeCancle_Click(object sender, EventArgs e)
        {
            Close();
        }
        private void ShowTHValue()
        {
            try
            {
                this.Text = m_hostIPAddr + "门限值更改";
                for (int i = 0; i < MainForm.allInfo.Count; i++)
                {
                    if (MainForm.allInfo[i].m_sIPInDatabase == m_hostIPAddr)
                    {
                        this.tBox_CPUTHValue.Text = MainForm.allInfo[i].cpuTH.ToString();
                        this.tBox_DISKTHValue.Text = MainForm.allInfo[i].diskTH.ToString();
                        this.tBox_MEMTHValue.Text = MainForm.allInfo[i].memTH.ToString();
                        this.tBox_TRAFFICValue.Text = MainForm.allInfo[i].trafficTH.ToString();
                    }
                }
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }
        public int GetCPUValue()
        {
            m_cpu = Convert.ToInt32(this.tBox_CPUTHValue.Text);
            return Convert.ToInt32(this.tBox_CPUTHValue.Text);
        }
        public int GetDiskValue()
        {
            m_disk = Convert.ToInt32(this.tBox_DISKTHValue.Text);
            return Convert.ToInt32(this.tBox_DISKTHValue.Text);
        }
        public int GetMemValue()
        {
            m_mem = Convert.ToInt32(this.tBox_MEMTHValue.Text);
            return Convert.ToInt32(this.tBox_MEMTHValue.Text);
        }
        public int GetTrafficValue()
        {
            m_traffic = Convert.ToInt32(this.tBox_TRAFFICValue.Text);
            return Convert.ToInt32(this.tBox_TRAFFICValue.Text);
        }
        private void ReadProcess( int id)
        {
            if (!File.Exists(filepath))
            {
                createfile(filepath);
                tBox_TRAFFICValue.Text = "无限制进程";
                return;
            }
            try
            {
                XDocument xdoc2 = XDocument.Load(filepath);
                var querResult = from c in xdoc2.Descendants("client")
                                 where c.Attribute("name").Value == MainForm.allInfo[id].hostName
                                 select c.Value;
                foreach (var item in querResult)
                {
                    tBox_forbiddenPro.Text = item.Replace(";", "\r\n");
                    return;
                }
                tBox_forbiddenPro.Text = "无进程限制";
            }
            catch (Exception)
            {
            }
        }

        private void createfile(string filepath)
        {
            if (!File.Exists(filepath))
            {
                XDocument xdoc = new XDocument(
                    new XElement("Prcocess"));
                xdoc.Save(filepath);
            }
        }

        private void updatexmlfile(string filepath, string clientname, string pronamedata)
        {
            string proname = pronamedata.Replace("\r\n", ";");
            createfile(filepath);
            try
            {
                XmlDocument doc = new XmlDocument();
                doc.Load(filepath);
                XmlElement root = doc.DocumentElement;
                foreach (XmlNode node in root)
                {
                    if (node.Name == "client")
                    {
                        foreach (XmlAttribute xattr in node.Attributes)
                        {
                            if (xattr.Name == "name")
                            {
                                if (xattr.Value == clientname)
                                {
                                    node.InnerText = proname;
                                    doc.Save(filepath);
                                    return;
                                }
                            }
                        }
                    }
                }
                XmlNode node2 = doc.CreateElement("client");
                XmlNode attrTime = doc.CreateNode(XmlNodeType.Attribute, "name", null);
                attrTime.Value = clientname;
                node2.Attributes.SetNamedItem(attrTime);
                node2.InnerText = proname;
                root.AppendChild(node2);
                doc.Save(filepath);
            }
            catch (Exception)
            {
            }
        }
    }
}