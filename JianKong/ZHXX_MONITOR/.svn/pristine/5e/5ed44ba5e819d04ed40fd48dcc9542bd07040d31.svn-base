using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net.Sockets;
using System.Net;
using DevComponents.DotNetBar;

namespace MonitorAndSet.TopoInfo
{
    public  class TcpConnect
    {
        public static void SendMessage(String software, String server, String message)
        {
            try
            {
                // Create a TcpClient.
                // Note, for this client to work you need to have a TcpServer 
                // connected to the same address as specified by the server, port
                // combination.
                
                Int32 port = Int32.Parse(MainForm.GroupPort2.ToString());
                TcpClient client = new TcpClient(server, port);

                // Translate the passed message into ASCII and store it as a Byte array.
                Byte[] data;
                if (software == "实时讲评")
                {
                    data = new Byte[20];
                    data[0] = 1;
                    data[1] = 20;                       //表示数组大小
                    for (int i = 0; i < 15; i++)
                    {
                        data[i + 2] = 0;
                    }
                    data[5] = 16;                       //命令类型，16表示开始命令
                    data[13] = 3;                       //从这开始是命令的内容，用4位代表字符串长度
                    data[17] = 97;                      //a
                    data[18] = 98;                      //b
                    data[19] = 99;                      //c

                }
                else
                {
                    data = System.Text.Encoding.ASCII.GetBytes(message);
                }
                // Get a client stream for reading and writing.
                //  Stream stream = client.GetStream();

                NetworkStream stream = client.GetStream();

                // Send the message to the connected TcpServer. 
                stream.Write(data, 0, data.Length);

                //Console.WriteLine("Sent: {0}", message);
                
                // Receive the TcpServer.response.

                // Buffer to store the response bytes.
                data = new Byte[256];

                // String to store the response ASCII representation.
                String responseData = String.Empty;

                // Read the first batch of the TcpServer response bytes.
                Int32 bytes = stream.Read(data, 0, data.Length);
                responseData = System.Text.Encoding.ASCII.GetString(data, 0, bytes);
                //Console.WriteLine("Received: {0}", responseData);
                //MessageBox.Show(responseData);
                // Close everything.
                stream.Close();
                client.Close();
                string timeStr = DateTime.Now.Year.ToString() + "-" + DateTime.Now.Month.ToString() + "-" + DateTime.Now.Day.ToString() + "/" + DateTime.Now.Hour.ToString() + ":" + DateTime.Now.Minute.ToString() + ":" + DateTime.Now.Second.ToString();
                MainForm.AddListViewItem(MainForm.listViewOutputInfo, "向" + software + "服务器[" + server + "]发送训练任务开始消息！", timeStr, System.Drawing.Color.GreenYellow);
                
            }
            catch (ArgumentNullException e)
            {
                MessageBoxEx.Show(e.ToString());
            }
            catch (SocketException e)
            {
                MessageBoxEx.Show(e.ToString());
            }
            
        }

        public static void ReceiveMessage()
        {
            TcpListener server = null;
            try
            {
                // Set the TcpListener on port 13000.
                Int32 port = 13000;
                IPAddress localAddr = IPAddress.Parse("192.168.27.48");

                // TcpListener server = new TcpListener(port);
                server = new TcpListener(localAddr, port);

                // Start listening for client requests.
                server.Start();

                // Buffer for reading data
                Byte[] bytes = new Byte[256];
                String data = null;

                // Enter the listening loop.
                while (true)
                {
                    Console.Write("Waiting for a connection... ");

                    // Perform a blocking call to accept requests.
                    // You could also user server.AcceptSocket() here.
                    TcpClient client = server.AcceptTcpClient();
                    Console.WriteLine("Connected!");

                    data = null;

                    // Get a stream object for reading and writing
                    NetworkStream stream = client.GetStream();

                    int i;

                    // Loop to receive all the data sent by the client.
                    while ((i = stream.Read(bytes, 0, bytes.Length)) != 0)
                    {
                        // Translate data bytes to a ASCII string.
                        data = System.Text.Encoding.ASCII.GetString(bytes, 0, i);
                        Console.WriteLine("Received: {0}", data);

                        // Process the data sent by the client.
                        data = data.ToUpper();

                        byte[] msg = System.Text.Encoding.ASCII.GetBytes(data);

                        // Send back a response.
                        stream.Write(msg, 0, msg.Length);
                        Console.WriteLine("Sent: {0}", data);
                        //MessageBox.Show("shoudao" + data);
                    }

                    // Shutdown and end connection
                    client.Close();
                }
            }
            catch (SocketException e)
            {
                Console.WriteLine("SocketException: {0}", e);
            }
            finally
            {
                // Stop listening for new clients.
                server.Stop();
            }

            
            Console.WriteLine("\nHit enter to continue...");
            Console.Read();
        }  
    }
}
