using System;
using System.Collections.Generic;
using System.Text;

using System.Net;
using System.Net.Sockets;

namespace MonitorAndSet.TopoInfo
{
    /// <summary>
    /// MulticastSocket表示 接收数据的组播SOCKET
    /// </summary>
    public class MulticastSocket
    {
        public MulticastSocket(String GroupIP, int GroupPort)
        {
            this.lPreRecvLen = 0;
            this.lCurRecvLen = 0;
            this.sock = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
            IPEndPoint ipep = new IPEndPoint(IPAddress.Any, GroupPort);
            this.sock.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress,true); 
            this.sock.Bind(ipep); 
            IPAddress ip = IPAddress.Parse(GroupIP);
            this.sock.SetSocketOption(SocketOptionLevel.IP, SocketOptionName.AddMembership,
                    new MulticastOption(ip, IPAddress.Any));            
        }

        public MulticastSocket()
        {
            this.lPreRecvLen = 0;
            this.lCurRecvLen = 0;
        }

        private Socket m_sock;
        /// <summary>
        /// SOCKET套接字
        /// </summary> 
        public Socket sock
        {
            get { return m_sock; }
            set { m_sock = value; }
        }

        private long m_lCurRecvLen;
        /// <summary>
        /// 到当前收到的数据的总长度
        /// </summary> 
        public long lCurRecvLen
        {
            get { return m_lCurRecvLen; }
            set { m_lCurRecvLen = value; }
        }

        private long m_lPreRecvLen;
        /// <summary>
        /// 之前收到的数据的总长度
        /// </summary> 
        public long lPreRecvLen
        {
            get { return m_lPreRecvLen; }
            set { m_lPreRecvLen = value; }
        }

        public long ReceiveData()
        {
            try
            {
                byte[] b = new byte[20480];
                this.sock.ReceiveTimeout = 1000;    //设置超时间隔,该时间内收不到数据将抛出异常,返回0
                this.sock.Receive(b);
                String str = System.Text.Encoding.UTF8.GetString(b, 0, b.GetLength(0));
                //String str = b.ToString();
                str = str.Substring(0, str.IndexOf("\0"));
                
                int n = str.Trim().Length;
                return n;
                //return str.Trim().Length;
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
                return 0;
            }
        }
        // add by lixiaohu 
        public long ReceiveData(ref string timestr)
        {
            try
            {
                byte[] b = new byte[2048];
                this.sock.ReceiveTimeout = 1000;    //设置超时间隔,该时间内收不到数据将抛出异常,返回0
                this.sock.Receive(b);
                timestr = System.Text.Encoding.UTF8.GetString(b, 0, b.GetLength(0));
                //String str = b.ToString();
                timestr = timestr.Substring(0, timestr.IndexOf("\0"));
                int n = timestr.Trim().Length;
                return n;
                //return str.Trim().Length;
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
                return 0;
            }
        }
    }
}
