using System;
using System.Collections.Generic;
using System.Net;
using System.Web;
using System.Management;
using System.Net.Sockets;
using System.Text;
using System.Windows.Forms;
using DevComponents.DotNetBar;
// author lixiaohu
// date   20110223
// summary uses a UdpClient to send UDP datagrams to the directed broadcast address broadIP, using port btoadPort 


namespace MonitorAndSet.TopoInfo
{
    class BroadcastSocket
    {
        public BroadcastSocket(String broadIP,int broadPort)
        {
            this.lCurSendLen = 0;
            this.lPreSendLen = 0;
         //   string localIP = GetLocalAddress();
            /*获取本机IP
            String HostName = Dns.GetHostName();
            IPHostEntry IpEntry = Dns.GetHostEntry(HostName);
            String localIP = IpEntry.AddressList[0].ToString(); */
           // string localIP = "192.168.27.23";
           // localIP = localIP.Substring(0, localIP.LastIndexOf('.'));
           // broadIP = localIP + "255";
            this.sock = new Socket(AddressFamily.InterNetwork, SocketType.Dgram,ProtocolType.Udp);

            IPAddress broadcast = IPAddress.Parse("255.255.255.255");

            //byte[] sendbuf = Encoding.ASCII.GetBytes(args[0]);
            this.ep = new IPEndPoint(broadcast, broadPort);

        }

        private Socket m_bdsock;
        /// <summary>
        /// SOCKET套接字
        /// </summary> 
        public Socket sock
        {
            get { return m_bdsock; }
            set { m_bdsock = value; }
        }

        private long m_lCurSendLen;
        /// <summary>
        /// 到当前发送的数据的总长度
        /// </summary> 
        public long lCurSendLen
        {
            get { return m_lCurSendLen; }
            set { m_lCurSendLen = value; }
        }

        private long m_lPreSendLen;
        /// <summary>
        /// 之前发送的数据的总长度
        /// </summary> 
        public long lPreSendLen
        {
            get { return m_lPreSendLen; }
            set { m_lPreSendLen = value; }
        }

        private IPEndPoint m_ep;
        /// <summary>
        /// 广播地址及端口
        /// </summary>
        public IPEndPoint ep
        {
            get {return m_ep;}
            set {m_ep = value;}
        }

        public long Boradcast_Send(string buff)
        {
            
            try
            {
                byte[] sendbuf = Encoding.ASCII.GetBytes(buff);
                lCurSendLen = this.sock.SendTo(sendbuf,this.ep);
                lPreSendLen += lCurSendLen;
                return lCurSendLen;
            }
            catch (Exception ex)
            {
                //添加到报警信息中
                String str = ex.ToString();
                MessageBoxEx.UseSystemLocalizedString = true;
                MessageBoxEx.Show(str);
                return 0;
            }
        }
    }
}
