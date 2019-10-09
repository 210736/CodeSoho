using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using DevComponents.DotNetBar;
using System.Net;
using System.Net.Sockets;

namespace MonitorAndSet.UILayer
{
    public partial class Start : DevComponents.DotNetBar.Office2007Form
    {

        int counter = 1;//计数器
        bool choose = false;//全选\全消
        private static IPAddress GroupAddress = IPAddress.Parse("239.255.0.2");  //服务器给客户端代理发送指令时的组播地址
        private static int GroupPort2 = 13268;      //服务器给客户端代理发送指令时的组播端口号  //20100504 Ding Yiming改为13268

        public Start()
        {
            InitializeComponent();
        }

        
        private void Start_Load(object sender, EventArgs e)
        {
            try 
            {
                for (int i = 0; i < MainForm.hostInfo.Count; i++)
                {
                    if (MainForm.hostInfo[i].bActive == false)
                    {
                        string[] data = new string[4];
                        data[0] = counter.ToString();
                        data[1] = MainForm.hostInfo[i].hostName;
                        data[2] = MainForm.hostInfo[i].m_sIPInDatabase;
                        data[3] = MainForm.hostInfo[i].hostMac;
                        ListViewItem item = new ListViewItem(data);
                        item.Checked = true;
                        this.listViewEx1.Items.Add(item);
                        counter++;
                    }
                }
                for (int i = 0; i < MainForm.serverInfo.Count; i++)
                {
                    if (MainForm.serverInfo[i].bActive == false)
                    {
                        string[] data = new string[4];
                        data[0] = counter.ToString();
                        data[1] = MainForm.serverInfo[i].hostName;
                        data[2] = MainForm.serverInfo[i].m_sIPInDatabase;
                        data[3] = MainForm.serverInfo[i].hostMac;
                        ListViewItem item = new ListViewItem(data);
                        item.Checked = true;
                        this.listViewEx1.Items.Add(item);
                        counter++;
                    }
                }
            }
            catch(Exception ee)
            {
                MessageBoxEx.Show(ee.ToString());
            }
        }
        /*全选
         */
        private void buttonX2_Click(object sender, EventArgs e)
        {
            try
            {
                int count = listViewEx1.Items.Count;
                for (int i = 0; i < count; i++)
                {
                    if (listViewEx1.Items[i].Checked == false)
                    {
                        choose = true;
                        break;
                    }
                }

                if (choose)
                {
                    for (int i = 0; i < count; i++)
                    {
                        listViewEx1.Items[i].Checked = true;
                    }
                    choose = false;
                }
                else
                {
                    for (int i = 0; i < count; i++)
                    {
                        listViewEx1.Items[i].Checked = false;
                    }
                    choose = true;
                }
            }
            catch (Exception ee)
            {
                MessageBoxEx.Show(ee.ToString());
            }
        }
        /*开机事件
         */
        private void buttonX1_Click(object sender, EventArgs e)
        {
            try
            {
                int count = listViewEx1.Items.Count;
                for (int i = 0; i < count; i++)
                {
                    if (listViewEx1.Items[i].Checked == true)
                    {
                        byte[] mac = new byte[6];
                        string str_mac = listViewEx1.Items[i].SubItems[3].Text.ToString();
                        if (str_mac != null && str_mac != "")
                        {
                            for (int j = 0; j < 6; j++)
                            {
                                mac[j] = byte.Parse(Convert.ToString(int.Parse(str_mac.Substring(2 * j, 2), System.Globalization.NumberStyles.HexNumber)));
                            }
                            WakeUp(mac);
                        }
                        else 
                        {
                            MessageBoxEx.Show("没有MAC地址无法远程启动！");
                        }
                    }
                }
                MessageBox.Show("远程开机命令发送成功", "通知", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            catch (Exception ee)
            {
                MessageBoxEx.Show(ee.ToString());
            }
        }
        /*开机函数
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
    }
}
