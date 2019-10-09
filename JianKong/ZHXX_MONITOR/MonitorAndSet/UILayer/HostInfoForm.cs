using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using System.Net;
using System.Net.Sockets;
using DevComponents.DotNetBar;

namespace MonitorAndSet
{
    public delegate void UpdateDialogInfo(string outInfo); //定义委托更新左侧的输出信息窗口
    public partial class HostInfoForm : DevComponents.DotNetBar.Office2007Form
    {
        private bool bChanged = false;     //标志显示机器的IP地址是否被修改了
        public String m_hostIPAddr = "";   //表示显示的是哪个主机的信息
        public string host_mac = "";        //机器MAC

        private static IPAddress GroupAddress = IPAddress.Parse("239.255.0.2");  //服务器给客户端代理发送指令时的组播地址
        private static int GroupPort2 = 13268;      //服务器给客户端代理发送指令时的组播端口号  //20100504 Ding Yiming改为13268

        private THChange _dlgthChange = null;
        private THChange thChange
        {
            get { return _dlgthChange; }
            set { _dlgthChange = value; }
        }

        public HostInfoForm()
        {
            InitializeComponent();
            //UpdateDialogInfo udinfo = Program.mymf.mfUpdataOutput;
        }

        private void HostInfo_Load(object sender, EventArgs e)
        {
            try
            {
                for (int i = 0; i < MainForm.allInfo.Count; i++)
                {
                    if (MainForm.allInfo[i].m_sIPInDatabase == m_hostIPAddr)
                    {
                        if (MainForm.allInfo[i].bActive == false)
                        {
                            host_mac = MainForm.allInfo[i].hostMac;
                            this.btn.Text = "开机";
                            
                        }
                        else
                        {
                            this.btn.Text = "关机";
                        }
                    }
                }
                timer1.Start();

                #region 对话框及控件的背景色设置
                this.BackColor = MainForm.DlgBackColor;
                this.groupBox1.BackColor = Color.Transparent;

                this.label1.BackColor = Color.Transparent;
                this.label2.BackColor = Color.Transparent;
                this.label3.BackColor = Color.Transparent;
                this.label4.BackColor = Color.Transparent;
                this.label5.BackColor = Color.Transparent;
                this.label6.BackColor = Color.Transparent;
                this.label7.BackColor = Color.Transparent;
                this.label8.BackColor = Color.Transparent;
                this.label9.BackColor = Color.Transparent;
                this.label10.BackColor = Color.Transparent;
                this.label11.BackColor = Color.Transparent;
                this.label12.BackColor = Color.Transparent;
                this.label13.BackColor = Color.Transparent;
                this.label14.BackColor = Color.Transparent;
                this.label15.BackColor = Color.Transparent;
                this.label16.BackColor = Color.Transparent;
                this.label17.BackColor = Color.Transparent;
                #endregion
            }
            catch (Exception ex)
            {
                String str = ex.ToString(); 
            }
        }

        private void HostInfoForm_Activated(object sender, EventArgs e)
        {
            try
            {
                this.tBox_IP.Text = m_hostIPAddr;
                this.tBox_name.Text = "";
                this.tBox_CPUValue.Text = "";
                this.tBox_DISKValue.Text = "";
                this.tBox_MEMValue.Text = "";
                this.tBox_TotalTraffic.Text = "";
                this.tBox_IncomingTraffic.Text = "";
                this.tBox_OutgoingTraffic.Text = "";
                this.pgBar_TotalTraffic.Value = 0;
                this.pgBar_InTraffic.Value = 0;
                this.pgBar_OutTraffic.Value = 0;
                this.listView_Process.Items.Clear();
                this.listView_Service.Items.Clear();
            }
            catch (Exception ex)
            { 
                String str = ex.ToString(); 
            }
        }

        private void btn_cancel_Click(object sender, EventArgs e)
        {
            Close();
        }

        #region 1秒计时器 更新主机的信息
        //1秒计时器 更新主机的信息
        private void timer1_Tick(object sender, EventArgs e)
        {
            try
            {
                String str;
                for (int i = 0; i < MainForm.allInfo.Count; i++)
                {
                    if(MainForm.allInfo[i].m_sIPInDatabase == m_hostIPAddr)
                    {
                        if(MainForm.allInfo[i].bActive ==false)
                        {
                            this.btn.Text ="开机";
                        }
                        else 
                        {
                            this.btn.Text = "关机";
                        }

                        if (!bChanged)
                        {
                            this.tBox_CPUTHValue.Text = MainForm.allInfo[i].cpuTH.ToString();
                            this.tBox_DISKTHValue.Text = MainForm.allInfo[i].diskTH.ToString();
                            this.tBox_MEMTHValue.Text = MainForm.allInfo[i].memTH.ToString();
                            str = MainForm.allInfo[i].sInfo;
                            if (MainForm.allInfo[i].lCurRecvLen != 0)
                            {
                                //主机名
                                this.tBox_name.Text = str.Substring(0, str.IndexOf('$'));
                                str = str.Substring(str.IndexOf('$') + 1);     //将接收到的数据字符串去掉最前面的主机名

                                //CPU使用率
                                this.tBox_CPUValue.Text = str.Substring(0, str.IndexOf('$'));
                                str = str.Substring(str.IndexOf('$') + 1);     //将接收到的数据字符串再去掉 CPU使用率

                                //硬盘情况
                                string diskInfo = str.Substring(0, str.IndexOf('$'));
                                if (diskInfo.IndexOf("/") > -1)
                                {
                                    string[] left_total = diskInfo.Split('/');
                                    if (left_total.Length == 2)
                                    {
                                        int left = Convert.ToInt32(left_total[0]) / 1024;
                                        int total = Convert.ToInt32(left_total[1]) / 1024;
                                        this.tBox_DISKValue.Text = left + "GB/共" + total + "GB";
                                    }
                                }
                                else
                                {
                                    this.tBox_DISKValue.Text = diskInfo;
                                }
                                //this.tBox_DISKValue.Text = str.Substring(0, str.IndexOf('$'));
                                str = str.Substring(str.IndexOf('$') + 1);     //将接收到的数据字符串再去掉 硬盘情况

                                //内存情况

                                string memInfo = str.Substring(0, str.IndexOf('$'));
                                if (memInfo.IndexOf("/") > -1)
                                {
                                    string[] left_total = memInfo.Split('/');
                                    if (left_total.Length == 2)
                                    {
                                        double left = Math.Round(Convert.ToDouble(left_total[0]) / 1024, 1);
                                        double total = Math.Round(Convert.ToDouble(left_total[1]) / 1024, 1);
                                        this.tBox_MEMValue.Text = left + "GB/共" + total + "GB";
                                    }
                                }
                                else
                                {
                                    this.tBox_MEMValue.Text = memInfo;
                                }
                                //this.tBox_MEMValue.Text = str.Substring(0, str.IndexOf('$'));
                                str = str.Substring(str.IndexOf('$') + 1);     //将接收到的数据字符串再去掉 内存情况

                                //被监视的进程情况
                                while (!str.StartsWith("$"))
                                {
                                    String strPro;
                                    strPro = str.Substring(0, str.IndexOf('&'));

                                    int pos;
                                    pos = strPro.IndexOf('#');   //进程名称#进程内存占用(K)#进程CPU使用率或者进程名称#NOT EXECUTED 查找第一个#
                                    int nIndex;
                                    for (nIndex = 0; nIndex < this.listView_Process.Items.Count; nIndex++)
                                    {
                                        if (strPro.Substring(0, pos) == this.listView_Process.Items[nIndex].SubItems[0].Text)
                                        {
                                            if (strPro.Substring(pos + 1) == "NOT EXECUTED")
                                            {
                                                if (this.listView_Process.Items[nIndex].SubItems[1].Text != "未运行")
                                                {
                                                    this.listView_Process.Items[nIndex].SubItems[1].Text = "未运行";
                                                    this.listView_Process.Items[nIndex].SubItems[2].Text = "";
                                                    this.listView_Process.Items[nIndex].SubItems[3].Text = "";
                                                }
                                            }
                                            else
                                            {
                                                if (this.listView_Process.Items[nIndex].SubItems[1].Text != "正在运行")
                                                {
                                                    this.listView_Process.Items[nIndex].SubItems[1].Text = "正在运行";
                                                }
                                                strPro = strPro.Substring(pos + 1);  //去掉 进程名称
                                                this.listView_Process.Items[nIndex].SubItems[3].Text = strPro.Substring(0, strPro.IndexOf('#'));  //进程内存占用
                                                strPro = strPro.Substring(strPro.IndexOf('#') + 1);  //再去掉 进程内存占用
                                                this.listView_Process.Items[nIndex].SubItems[2].Text = strPro;       //进程CPU使用率
                                            }
                                            break;
                                        }
                                    }
                                    if (nIndex == this.listView_Process.Items.Count)
                                    {
                                        ListViewItem lt = new ListViewItem(strPro.Substring(0, pos));
                                        lt.SubItems.Add("未运行");
                                        lt.SubItems.Add("");
                                        lt.SubItems.Add("");
                                        this.listView_Process.Items.Insert(nIndex, lt);
                                    }
                                    str = str.Substring(str.IndexOf('&') + 1);
                                }
                                str = str.Substring(1);

                                //被监视的服务情况
                                while (!str.StartsWith("$"))
                                {
                                    String strSer;
                                    strSer = str.Substring(0, str.IndexOf('&'));

                                    int pos;
                                    pos = strSer.IndexOf('#');   //服务名称#服务状态 查找该#
                                    int nIndex;
                                    for (nIndex = 0; nIndex < this.listView_Service.Items.Count; nIndex++)
                                    {
                                        if (strSer.Substring(0, pos) == this.listView_Service.Items[nIndex].SubItems[0].Text)
                                        {
                                            if (strSer.Substring(pos + 1) == "0")
                                            {
                                                if (this.listView_Service.Items[nIndex].SubItems[1].Text != "该服务未安装")
                                                {
                                                    this.listView_Service.Items[nIndex].SubItems[1].Text = "该服务未安装";
                                                }
                                            }
                                            else if (strSer.Substring(pos + 1) == "4")
                                            {
                                                if (this.listView_Service.Items[nIndex].SubItems[1].Text != "正在运行")
                                                {
                                                    this.listView_Service.Items[nIndex].SubItems[1].Text = "正在运行";
                                                }
                                            }
                                            else if (strSer.Substring(pos + 1) == "1")
                                            {
                                                if (this.listView_Service.Items[nIndex].SubItems[1].Text != "停止运行")
                                                {
                                                    this.listView_Service.Items[nIndex].SubItems[1].Text = "停止运行";
                                                }
                                            }
                                            break;
                                        }
                                    }
                                    if (nIndex == this.listView_Service.Items.Count)
                                    {
                                        ListViewItem lt = new ListViewItem(strSer.Substring(0, pos));
                                        lt.SubItems.Add("");
                                        this.listView_Service.Items.Insert(nIndex, lt);
                                    }
                                    str = str.Substring(str.IndexOf('&') + 1);
                                }

                                //提取流量信息      $接口#带宽#输入流量#输出流量#总流量#
                                String strTraffic;
                                strTraffic = MainForm.allInfo[i].sInfo;
                                strTraffic = strTraffic.Substring(strTraffic.LastIndexOf('$') + 1);    //提取 接口#带宽#输入流量#输出流量#总流量#

                                //m_topo.m_hostInfo[i].netPortName = strTraffic.Substring(0, strTraffic.IndexOf('#'));    //提取 接口
                                strTraffic = strTraffic.Substring(strTraffic.IndexOf('#') + 1);                           //去掉 接口#

                                //m_topo.m_hostInfo[i].bandWidth = Convert.ToInt32(strTraffic.Substring(0, strTraffic.IndexOf('#')));    //提取 带宽
                                strTraffic = strTraffic.Substring(strTraffic.IndexOf('#') + 1);                                        //去掉 带宽#
                                if (strTraffic.IndexOf("-") > -1) //异常数据 不变
                                {
                                    //若流量大于1000,进度条设为100
                                    this.tBox_IncomingTraffic.Text = this.tBox_IncomingTraffic.Text;
                                    this.pgBar_InTraffic.Value = this.pgBar_InTraffic.Value;

                                    this.tBox_OutgoingTraffic.Text = this.tBox_OutgoingTraffic.Text;
                                    this.pgBar_OutTraffic.Value = this.pgBar_OutTraffic.Value;

                                    this.tBox_TotalTraffic.Text = this.tBox_TotalTraffic.Text;
                                    this.pgBar_TotalTraffic.Value = this.pgBar_TotalTraffic.Value;
                                }
                                else
                                {
                                    //若流量大于1000,进度条设为100
                                    this.tBox_IncomingTraffic.Text = strTraffic.Substring(0, strTraffic.IndexOf('#')) + "KB/sec";  //提取 输入流量
                                    this.pgBar_InTraffic.Value = (int)(Convert.ToDouble(strTraffic.Substring(0, strTraffic.IndexOf('#'))) / 1280.0);//kB
                                    strTraffic = strTraffic.Substring(strTraffic.IndexOf('#') + 1);                    //去掉 输入流量#

                                    this.tBox_OutgoingTraffic.Text = strTraffic.Substring(0, strTraffic.IndexOf('#')) + "KB/sec";  //提取 输出流量
                                    this.pgBar_OutTraffic.Value = (int)(Convert.ToDouble(strTraffic.Substring(0, strTraffic.IndexOf('#'))) / 1280.0);//kB
                                    strTraffic = strTraffic.Substring(strTraffic.IndexOf('#') + 1);                     //去掉 输出流量#

                                    this.tBox_TotalTraffic.Text = strTraffic.Substring(0, strTraffic.IndexOf('#')) + "KB/sec";     //提取 总流量

                                    int totalValue = this.pgBar_InTraffic.Value + this.pgBar_OutTraffic.Value;
                                    if (totalValue > 100) totalValue = 100;
                                    this.pgBar_TotalTraffic.Value = totalValue;
                                }
                            }
                            else
                            {
                                if (this.tBox_CPUValue.Text != "")
                                {
                                    this.tBox_CPUValue.Text = "";
                                }
                                if (this.tBox_DISKValue.Text != "")
                                {
                                    this.tBox_DISKValue.Text = "";
                                }
                                if (this.tBox_MEMValue.Text != "")
                                {
                                    this.tBox_MEMValue.Text = "";
                                }
                                if (this.tBox_TotalTraffic.Text != "")
                                {
                                    this.tBox_TotalTraffic.Text = "";
                                    this.pgBar_TotalTraffic.Value = 0;
                                }
                                if (this.tBox_IncomingTraffic.Text != "")
                                {
                                    this.tBox_IncomingTraffic.Text = "";
                                    this.pgBar_InTraffic.Value = 0;
                                }
                                if (this.tBox_OutgoingTraffic.Text != "")
                                {
                                    this.tBox_OutgoingTraffic.Text = "";
                                    this.pgBar_OutTraffic.Value = 0;
                                }
                                for (int k = 0; k < this.listView_Process.Items.Count; k++)
                                {
                                    if (this.listView_Process.Items[k].SubItems[1].Text != "未运行")
                                    {
                                        this.listView_Process.Items[k].SubItems[1].Text = "未运行";
                                        this.listView_Process.Items[k].SubItems[2].Text = "";
                                        this.listView_Process.Items[k].SubItems[3].Text = "";
                                    }
                                }
                                for (int j = 0; j < this.listView_Service.Items.Count; j++)
                                {
                                    if (this.listView_Service.Items[j].SubItems[1].Text != "未运行")
                                    {
                                        this.listView_Service.Items[j].SubItems[1].Text = "未运行";
                                    }
                                }
                            }
                        }
                        else if (bChanged)
                        {
                            bChanged = false;
                            this.listView_Process.Clear();
                            this.listView_Service.Clear();
                        }
                    }
                    
                }
            }
            catch(Exception ex)
            {
                String str = ex.ToString();
            }
        }
        #endregion

        #region 启动和关闭 选中的进程或服务
        //注意:开启停止服务时 服务的启动类型不能是"已禁用"
        //启动 选中的进程或服务
        private void btn_StartProSer_Click(object sender, EventArgs e)
        {
            MessageBoxEx.UseSystemLocalizedString = true;
            try
            {
                string timeStr = DateTime.Now.Year.ToString() + "-" + DateTime.Now.Month.ToString() + "-" + DateTime.Now.Day.ToString() + "/" + DateTime.Now.Hour.ToString() + ":" + DateTime.Now.Minute.ToString() + ":" + DateTime.Now.Second.ToString();
                if (this.listView_Process.SelectedIndices.Count == 0 && this.listView_Service.SelectedIndices.Count == 0)
                {
                    MessageBoxEx.Show("请在列表中选择要启动的进程或服务！");
                }
                else
                {
                    for (int i = 0; i < this.listView_Process.Items.Count; i++)
                    {
                        if (this.listView_Process.SelectedIndices.Count != 0)
                        {
                            if (this.listView_Process.SelectedIndices[0] == i &&
                            this.listView_Process.Items[i].SubItems[1].Text == "未运行")
                            {
                                String str = this.listView_Process.Items[i].SubItems[0].Text;

                                MainForm.mfUpdataOutput("启动[" + m_hostIPAddr + "]进程：" + str);
                                str = "PS#" + str;        						//P代表进程，S代表启动，这将在客户端得到分析
                                SendInstruct(str, m_hostIPAddr);
                                break;
                            }
                            if (this.listView_Process.SelectedIndices[0] == i &&
                            this.listView_Process.Items[i].SubItems[1].Text == "正在运行")
                            {
                                MessageBoxEx.Show("该进程已经在运行！");
                                break;
                            }
                        }
                    }
                    for (int j = 0; j < this.listView_Service.Items.Count; j++)
                    {
                        if (this.listView_Service.SelectedIndices.Count != 0)
                        {
                            if (this.listView_Service.SelectedIndices[0] == j &&
                            this.listView_Service.Items[j].SubItems[1].Text == "停止运行")
                            {
                                String str = this.listView_Service.Items[j].SubItems[0].Text;
                                MainForm.mfUpdataOutput("启动[" + m_hostIPAddr + "]服务：" + str);
                                str = "SS#" + str;        						//第一个S代表服务，第二个S代表启动，这将在客户端得到分析
                                SendInstruct(str, m_hostIPAddr);
                                break;
                            }
                            if (this.listView_Service.SelectedIndices[0] == j &&
                            this.listView_Service.Items[j].SubItems[1].Text == "正在运行")
                            {
                                MessageBoxEx.Show("该服务已经在运行！");
                                break;
                            }
                            if (this.listView_Service.SelectedIndices[0] == j &&
                            this.listView_Service.Items[j].SubItems[1].Text == "该服务未安装")
                            {
                                MessageBoxEx.Show("该服务未安装，无法启动！");
                                break;
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

        //关闭 选中的进程或服务
        private void btn_CloseProSer_Click(object sender, EventArgs e)
        {
            MessageBoxEx.UseSystemLocalizedString = true;
            try
            {
                string timeStr = DateTime.Now.Year.ToString() + "-" + DateTime.Now.Month.ToString() + "-" + DateTime.Now.Day.ToString() + "/" + DateTime.Now.Hour.ToString() + ":" + DateTime.Now.Minute.ToString() + ":" + DateTime.Now.Second.ToString();
                if (this.listView_Process.SelectedIndices.Count == 0 && this.listView_Service.SelectedIndices.Count == 0)
                {
                    MessageBoxEx.Show("请在列表中选择要关闭的进程或服务！");
                }
                else
                {
                    for (int i = 0; i < this.listView_Process.Items.Count; i++)
                    {
                        if (this.listView_Process.SelectedIndices.Count != 0)
                        {
                            if (this.listView_Process.SelectedIndices[0] == i &&
                            this.listView_Process.Items[i].SubItems[1].Text == "正在运行")
                            {
                                String str = this.listView_Process.Items[i].SubItems[0].Text;
                                MainForm.mfUpdataOutput("关闭[" + m_hostIPAddr + "]进程：" + str);
                                str = "PC#" + str;        						//P代表进程，C代表关闭，这将在客户端得到分析
                                SendInstruct(str, m_hostIPAddr);
                                break;
                            }
                            if (this.listView_Process.SelectedIndices[0] == i &&
                            this.listView_Process.Items[i].SubItems[1].Text == "未运行")
                            {
                                MessageBoxEx.Show("该进程未运行！");
                                break;
                            }
                        }
                    }
                    for (int j = 0; j < this.listView_Service.Items.Count; j++)
                    {
                        if (this.listView_Service.SelectedIndices.Count != 0)
                        {
                            if (this.listView_Service.SelectedIndices[0] == j &&
                            this.listView_Service.Items[j].SubItems[1].Text == "正在运行")
                            {
                                String str = this.listView_Service.Items[j].SubItems[0].Text;
                                MainForm.mfUpdataOutput("关闭[" + m_hostIPAddr + "]服务：" + str);
                                str = "SC#" + str;        						//S代表服务，C代表关闭，这将在客户端得到分析
                                SendInstruct(str, m_hostIPAddr);
                                break;
                            }
                            if (this.listView_Service.SelectedIndices[0] == j &&
                            this.listView_Service.Items[j].SubItems[1].Text == "停止运行")
                            {
                                MessageBoxEx.Show("该服务已经停止运行！");
                                break;
                            }
                            if (this.listView_Service.SelectedIndices[0] == j &&
                            this.listView_Service.Items[j].SubItems[1].Text == "该服务未安装")
                            {
                                MessageBoxEx.Show("该服务未安装，关闭无效！");
                                break;
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

        #region 向客户端发送命令
        //向客户端发送命令,发送的数据格式为instruct#destIP。在代理程序中RecvInfoFromSvr()中解析此IP是否为本地主机IP
        private void SendInstruct(String instruct, String destIP)
        {
            try
            {
                IPEndPoint localEP = new IPEndPoint(IPAddress.Parse(MainForm.strLocalAddrIP), 0); //--ding 20090603+
                UdpClient sender = new UdpClient(localEP);

                sender.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, 1);
                //byte[] ipbyte = IPAddress.Parse(MainForm.strLocalAddrIP).GetAddressBytes();
                //sender.Client.SetSocketOption(SocketOptionLevel.IP, SocketOptionName.MulticastInterface, ipbyte);
                sender.Client.SetSocketOption(SocketOptionLevel.IP, SocketOptionName.MulticastTimeToLive, 16);

                IPEndPoint groupEP = new IPEndPoint(GroupAddress, GroupPort2);
                String message = instruct + '#' + destIP;
                byte[] bytes = Encoding.Default.GetBytes(message);    //20090609gai
               
                sender.Send(bytes, bytes.Length, groupEP);//20100408
                sender.Close();
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }
        #endregion

        #region 修改具体机器的告警门限值
        private void btn_THchange_Click(object sender, EventArgs e)
        {
            try
            {
                if (thChange == null || thChange.IsDisposed)
                {
                    thChange = new THChange();
                    thChange.Owner = this;
                }
                thChange.m_hostIPAddr = m_hostIPAddr;
                thChange.ShowDialog();
                thChange.WindowState = FormWindowState.Normal;
                thChange.Activate();
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }
        #endregion

        //下面的代码避免"关闭/启动 服务/进程"时出现 表面单选实际双选 的问题
        #region 点击除两个列表框、启动/停止按钮以外的其他可编辑控件时 删除两个列表框中已选择的项
        private void tBox_IP_Enter(object sender, EventArgs e)
        {
            try
            {
                if (this.listView_Process.SelectedIndices.Count != 0)
                {
                    this.listView_Process.SelectedIndices.Clear();
                }
                if (this.listView_Service.SelectedIndices.Count != 0)
                {
                    this.listView_Service.SelectedIndices.Clear();
                }
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }

        private void tBox_CPUValue_Enter(object sender, EventArgs e)
        {
            try
            {
                if (this.listView_Process.SelectedIndices.Count != 0)
                {
                    this.listView_Process.SelectedIndices.Clear();
                }
                if (this.listView_Service.SelectedIndices.Count != 0)
                {
                    this.listView_Service.SelectedIndices.Clear();
                }
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }

        private void tBox_DISKValue_Enter(object sender, EventArgs e)
        {
            try
            {
                if (this.listView_Process.SelectedIndices.Count != 0)
                {
                    this.listView_Process.SelectedIndices.Clear();
                }
                if (this.listView_Service.SelectedIndices.Count != 0)
                {
                    this.listView_Service.SelectedIndices.Clear();
                }
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }

        private void tBox_MEMValue_Enter(object sender, EventArgs e)
        {
            try
            {
                if (this.listView_Process.SelectedIndices.Count != 0)
                {
                    this.listView_Process.SelectedIndices.Clear();
                }
                if (this.listView_Service.SelectedIndices.Count != 0)
                {
                    this.listView_Service.SelectedIndices.Clear();
                }
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }

        private void tBox_name_Enter(object sender, EventArgs e)
        {
            try
            {
                if (this.listView_Process.SelectedIndices.Count != 0)
                {
                    this.listView_Process.SelectedIndices.Clear();
                }
                if (this.listView_Service.SelectedIndices.Count != 0)
                {
                    this.listView_Service.SelectedIndices.Clear();
                }
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }

        private void tBox_CPUTHValue_Enter(object sender, EventArgs e)
        {
            try
            {
                if (this.listView_Process.SelectedIndices.Count != 0)
                {
                    this.listView_Process.SelectedIndices.Clear();
                }
                if (this.listView_Service.SelectedIndices.Count != 0)
                {
                    this.listView_Service.SelectedIndices.Clear();
                }
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }

        private void tBox_DISKTHValue_Enter(object sender, EventArgs e)
        {
            try
            {
                if (this.listView_Process.SelectedIndices.Count != 0)
                {
                    this.listView_Process.SelectedIndices.Clear();
                }
                if (this.listView_Service.SelectedIndices.Count != 0)
                {
                    this.listView_Service.SelectedIndices.Clear();
                }
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }

        private void tBox_MEMTHValue_Enter(object sender, EventArgs e)
        {
            try
            {
                if (this.listView_Process.SelectedIndices.Count != 0)
                {
                    this.listView_Process.SelectedIndices.Clear();
                }
                if (this.listView_Service.SelectedIndices.Count != 0)
                {
                    this.listView_Service.SelectedIndices.Clear();
                }
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }

        private void tBox_TotalTraffic_Enter(object sender, EventArgs e)
        {
            try
            {
                if (this.listView_Process.SelectedIndices.Count != 0)
                {
                    this.listView_Process.SelectedIndices.Clear();
                }
                if (this.listView_Service.SelectedIndices.Count != 0)
                {
                    this.listView_Service.SelectedIndices.Clear();
                }
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }

        private void tBox_IncomingTraffic_Enter(object sender, EventArgs e)
        {
            try
            {
                if (this.listView_Process.SelectedIndices.Count != 0)
                {
                    this.listView_Process.SelectedIndices.Clear();
                }
                if (this.listView_Service.SelectedIndices.Count != 0)
                {
                    this.listView_Service.SelectedIndices.Clear();
                }
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }

        private void tBox_OutgoingTraffic_Enter(object sender, EventArgs e)
        {
            try
            {
                if (this.listView_Process.SelectedIndices.Count != 0)
                {
                    this.listView_Process.SelectedIndices.Clear();
                }
                if (this.listView_Service.SelectedIndices.Count != 0)
                {
                    this.listView_Service.SelectedIndices.Clear();
                }
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }

        private void btn_THchange_Enter(object sender, EventArgs e)
        {
            try
            {
                if (this.listView_Process.SelectedIndices.Count != 0)
                {
                    this.listView_Process.SelectedIndices.Clear();
                }
                if (this.listView_Service.SelectedIndices.Count != 0)
                {
                    this.listView_Service.SelectedIndices.Clear();
                }
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }
        #endregion

        #region 点击进程列表框时 删除服务列表框中已选择的项
        private void listView_Process_Enter(object sender, EventArgs e)
        {
            try
            {
                if (this.listView_Service.SelectedIndices.Count != 0)
                {
                    this.listView_Service.SelectedIndices.Clear();
                }
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }
        #endregion

        #region 点击服务列表框时 删除进程列表框中已选择的项
        private void listView_Service_Enter(object sender, EventArgs e)
        {
            try
            {
                if (this.listView_Process.SelectedIndices.Count != 0)
                {
                    this.listView_Process.SelectedIndices.Clear();
                }
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }
        #endregion

        #region 限制列宽的最小值
        private void listView_Process_ColumnWidthChanged(object sender, ColumnWidthChangedEventArgs e)
        {
            if(this.listView_Process.Columns[e.ColumnIndex].Width<90)
            {
                this.listView_Process.Columns[e.ColumnIndex].Width = 90;
            }
        }

        private void listView_Service_ColumnWidthChanged(object sender, ColumnWidthChangedEventArgs e)
        {
            if (this.listView_Service.Columns[e.ColumnIndex].Width < 90)
            {
                this.listView_Service.Columns[e.ColumnIndex].Width = 90;
            }
        }

        #endregion

        #region 远程开机/关机
        private void btn_Click(object sender, EventArgs e)
        {
            try
            {
                if(this.btn.Text=="开机")
                {
                    byte[] mac = new byte[6];
                    string str_mac = host_mac;
                    if (str_mac!=null && str_mac!="") 
                    {
                        for (int i = 0; i < 6; i++)
                        {
                            mac[i] = byte.Parse(Convert.ToString(int.Parse(str_mac.Substring(2 * i, 2), System.Globalization.NumberStyles.HexNumber)));
                        }
                        WakeUp(mac);
                        MessageBox.Show("远程开机命令发送成功", "通知", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }
                   
                }
                else if(this.btn.Text =="关机")
                {
                    if (MessageBoxEx.Show("确定进行关机操作？请确认已保存该机器正在编辑的文件", "警告！", MessageBoxButtons.OKCancel, MessageBoxIcon.Warning) == DialogResult.OK)
                    {
                        shutdown();
                    }
                }
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }
        /*开机
         */
        public static void WakeUp(byte[] mac)
        {
            try
            {
                IPEndPoint localEP = new IPEndPoint(IPAddress.Parse(MainForm.strLocalAddrIP), 0);
                UdpClient udp = new UdpClient(localEP);
                udp.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, 1);
                udp.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.Broadcast, 1);
                IPEndPoint groupEP = new IPEndPoint(IPAddress.Broadcast, GroupPort2);
                byte[] bytes = new byte[17 * 6];
                for (int i = 0; i < 6; i++)
                {
                    bytes[i] = 0xFF;
                }

                for (int i = 1; i <= 16; i++)
                {
                    for (int j = 0; j < 6; j++)
                    {
                        bytes[6 * i + j] = mac[j];
                    }
                }
                udp.Send(bytes, bytes.Length, groupEP);
                udp.Close();
            }
            catch (Exception e)
            {
                MessageBox.Show(e.ToString());
            }
        }
        /*关机
         */
        private void shutdown()
        {  
            try
            {
                string str = "CS#" ;
                SendInstruct(str, m_hostIPAddr);
            }
            catch (Exception e)
            {
                MessageBoxEx.Show(e.ToString());
            }
        }
        #endregion
    }
}