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
    public partial class RemoteControl : DevComponents.DotNetBar.Office2007Form
    {
        public RemoteControl()
        {
            InitializeComponent();
        }

        int counter = 1;//计数器
        bool choose = false;//全选\全消
        private static IPAddress GroupAddress = IPAddress.Parse("239.255.0.2");  //服务器给客户端代理发送指令时的组播地址
        private static int GroupPort2 = 13268;      //服务器给客户端代理发送指令时的组播端口号  //20100504 Ding Yiming改为13268

        private void RenoteControl_Load(object sender, EventArgs e)
        {
            try
            {
                for (int i = 0; i < MainForm.hostInfo.Count; i++)
                {
                    if (MainForm.hostInfo[i].bActive == true)
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
                    if (MainForm.serverInfo[i].bActive == true)
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
            catch (Exception ee)
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

        /*部署
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
                        string str = "RS#";
                        string m_hostIPAddr = listViewEx1.Items[i].SubItems[2].Text.ToString();
                        SendInstruct(str, m_hostIPAddr);
                    }
                }
            }
            catch (Exception ee)
            {
                MessageBoxEx.Show(ee.ToString());
            }
        }

        /*向客户端发送命令,发送的数据格式为instruct#destIP。在代理程序中RecvInfoFromSvr()中解析此IP是否为本地主机IP
        */
        private void SendInstruct(String instruct, String destIP)
        {
            try
            {
                IPEndPoint localEP = new IPEndPoint(IPAddress.Parse(MainForm.strLocalAddrIP), 0);
                UdpClient sender = new UdpClient(localEP);
                sender.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, 1);
                sender.Client.SetSocketOption(SocketOptionLevel.IP, SocketOptionName.MulticastTimeToLive, 16);

                IPEndPoint groupEP = new IPEndPoint(GroupAddress, GroupPort2);
                String message = instruct + '#' + destIP;
                byte[] bytes = Encoding.Default.GetBytes(message);

                sender.Send(bytes, bytes.Length, groupEP);
                sender.Close();
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }
    }
}
