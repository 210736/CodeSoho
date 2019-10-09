using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace MonitorAndSet.TopoInfo
{
    class MulticastServer
    {
        public static IPAddress mcastAddress;
        public static int mcastPort;
        public static Socket mcastSocket;

        public static void JoinMulticastGroup()
        {
            try
            {
                // Create a multicast socket.
                mcastSocket = new Socket(AddressFamily.InterNetwork,
                                         SocketType.Dgram,
                                         ProtocolType.Udp);

                // Get the local IP address used by the listener and the sender to
                // exchange multicast messages. 
                //Console.Write("\nEnter local IPAddress for sending multicast packets: ");
                //IPAddress localIPAddr = IPAddress.Parse(Console.ReadLine());

                //获取本机IP
                String HostName = Dns.GetHostName();
                IPHostEntry IpEntry = Dns.GetHostEntry(HostName);
                String localIP = IpEntry.AddressList[0].ToString();
                IPAddress localIPAddr = IPAddress.Parse(localIP);

                // Create an IPEndPoint object. 
                IPEndPoint IPlocal = new IPEndPoint(localIPAddr, 0);

                // Bind this endpoint to the multicast socket.
                mcastSocket.Bind(IPlocal);

                // Define a MulticastOption object specifying the multicast group 
                // address and the local IP address.
                // The multicast group address is the same as the address used by the listener.
                MulticastOption mcastOption;
                mcastOption = new MulticastOption(mcastAddress, localIPAddr);

                mcastSocket.SetSocketOption(SocketOptionLevel.IP,
                                            SocketOptionName.AddMembership,
                                            mcastOption);

            }
            catch (Exception e)
            {
                Console.WriteLine("\n" + e.ToString());
            }
        }

        public static void BroadcastMessage(string message)
        {
            IPEndPoint endPoint;

            try
            {
                //Send multicast packets to the listener.
                endPoint = new IPEndPoint(mcastAddress, mcastPort);
                mcastSocket.SendTo(ASCIIEncoding.ASCII.GetBytes(message), endPoint);
                Console.WriteLine("Multicast data sent.....");
            }
            catch (Exception e)
            {
                Console.WriteLine("\n" + e.ToString());
            }

            mcastSocket.Close();
        }


        public static void Init(string groupIP , int groupPort)
        {
         //   mcastAddress = IPAddress.Parse("224.168.100.2");
         //   mcastPort = 11000;
            mcastAddress = IPAddress.Parse(groupIP);
            mcastPort = groupPort;

            // Join the listener multicast group.
            JoinMulticastGroup();

            // Broadcast the message to the listener.
         //   BroadcastMessage("Hello multicast listener.");
        }
    }
}
