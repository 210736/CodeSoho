using System;
using System.Collections.Generic;
using System.Text;

using System.Drawing;
using System.Management;
using System.Windows.Forms;
using System.Windows;
using System.Data;
using MonitorAndSet.CommonClass;
using DevComponents.DotNetBar;


namespace MonitorAndSet.TopoInfo
{
    #region 各结构体和枚举的定义
    //各结构体和枚举的定义

    //VIDEOATTRIBUTE结构体表示视频源属性
    public struct VIDEOATTRIBUTE
    {
        public string videoID;				//网络视频ID
        public string videoName;			//网络视频名称
        public string videoDesc;			//视频描述
        public string cpostID;
        public string senderID;
        public string videoIP;				//组播IP
        public string videoPort;			//组播端口号
        public int videoValid;				//是否有组播数据
        public int playStat;
        public string playPort;
        public string videoBPS;				//数据流量
        public string playBPS;				//重放数据流量
    };

    //AUDIOATTRIBUTE结构体表示音频源属性
    public struct AUDIOATTRIBUTE
    {
        public string audioID;				//音频ID
        public string branch;
        public string branckDesc;
        public string port;					//端口号
        public string audioIP;				//组播IP
        public int valid;					//是否有组播数据
        public int playValid;				//是否有重放数据
        public string audioBPS;				//数据流量
    };

    //SCREENCLIENT结构体表示
    public struct SCREENCLIENT
    {
        public string clientID;				//客户机ID
        public string clientName;			//客户机名称
        public string clientIP;				//客户机IP
        public string multiPort;			//组播端口号
        public string multiIP;				//组播IP
        public int clientValid;				//是否有组播数据
        public string seatName;				//席位名称
        public string cPostID;				//所在指挥所名称
        public string playPort;				//重放端口号
        public int playValid;				//是否有重放数据
        public string clientBPS;			//数据流量
        public string playBPS;				//重放数据流量
    };

    //RADARDATA结构体表示雷达数据
    public struct RADARDATA
    {
        public string ID;					//雷达ID
        public string multicastAddr;		//组播地址
        public string port;					//组播端口
        public int valid;					//是否有组播数据
        public string playPort;				//重放端口
        public int playValid;				//是否有重放数据
        public string radarBPS;				//数据流量
        public string playBPS;				//重放数据流量
    };

    //MonitorCondition枚举表示监控状态：实时 重放
    public enum MonitorCondition
    {
        realTime,
        replay
    };

    //traffictype枚举表示网络流量类型：总流量、输入流量、输出流量
    public enum traffictype
    {
        Traffic_Total = 0,
        Traffic_Incoming = 1,
        Traffic_Outgoing = 2
    };
    #endregion

    public class Topology
    {
        #region 定义的变量和所用的资源\

        public List<String> m_sIPActiveDevices = new List<String>();    //活动设备的IP地址列表
        public List<int> m_indexOfSwitch = new List<int>();		   //记录交换机在m_sIPActiveDevices中的索引
        public List<String> m_sNameOfDevices = new List<String>();      //活动设备的名字
        public List<String> m_sMacOfDevices = new List<String>();       //活动设备的硬件地址

        public Snmp m_snmp = new Snmp();
        public int flagnodeindex = -1;       //本机在m_sIPActiveDevices中的索引号,若-1则表示没有

        //通过在主窗体程序中读取数据库的表将主机、服务器、交换机信息保存在下面的列表中
        public List<HostInfo> m_hostInfo;// = new List<HostInfo>();

        
        //绘图所用的Image文件
        public static Image imgv = Image.FromFile(@"Picture\buleline_v.bmp");
        public static Image imgh = Image.FromFile(@"Picture\buleline_h.bmp");
        public static Image image_HostGray = Image.FromFile(@"Picture\SeatGray.bmp");  //灰色 关机
        public static Image image_HostBlue = Image.FromFile(@"Picture\SeatBlue.bmp");  //蓝色 开机且状态正常 
        public static Image image_HostYellow = Image.FromFile(@"Picture\SeatYellow.bmp");  //黄色开机但收不到代理数据
        public static Image image_HostRed = Image.FromFile(@"Picture\SeatRed.bmp");  // 红色 开机,状态超过阈值
        public static Image image_sanBackground = Image.FromFile(@"Picture\GrayBackground.bmp");   //灰色背景
        public static Image image_switchBlue = Image.FromFile(@"Picture\switchBlue.bmp");  //蓝色 开机且状态正常
        public static Image image_switchGray = Image.FromFile(@"Picture\switchGray.bmp");  //灰色 关机
        public static Image image_switchRed = Image.FromFile(@"Picture\switchRed.bmp");  //红色 开机,状态超过阈值
        public static Image image_switchYellow = Image.FromFile(@"Picture\switchYellow.bmp");  //黄色 开机但收不到代理数据

        public static Image image_serverBlue = Image.FromFile(@"Picture\switchBlue.bmp");  //蓝色 开机且状态正常
        public static Image image_serverGray = Image.FromFile(@"Picture\switchGray.bmp");  //灰色 关机
        public static Image image_serverRed = Image.FromFile(@"Picture\switchRed.bmp");  //红色 开机,状态超过阈值
        public static Image image_serverYellow = Image.FromFile(@"Picture\switchYellow.bmp");  //黄色 开机但收不到代理数据

        public static Image image_DataLineTrue_v = Image.FromFile(@"Picture\grennline_v.bmp");
        public static Image image_DataLineTrue_h = Image.FromFile(@"Picture\grennline_h.bmp");
        public static Image image_DataLineFalse_v = Image.FromFile(@"Picture\grayline_v.bmp");
        public static Image image_DataLineFalse_h = Image.FromFile(@"Picture\grayline_h.bmp");

        
        public int scount = 0; //网络拓扑图中交换机的个数  20090820+ 
        #endregion

        public Topology()
        {
            m_hostInfo = new List<HostInfo>();
        }

        #region 获得某IP指定主机的基本信息（包括主机名和硬件地址）
        //获得某IP指定主机的基本信息（包括主机名和硬件地址） 初始化m_sNameOfDevices和m_sMacOfDevices
        public bool GetBasicInfo(String sIP)
        {
            string resultname = "";
            string resultmac = "";
            m_snmp.StartUp();
            m_snmp.SetDestIP(sIP);
            m_snmp.SnmpGetValue(Snmp.sysName, ref resultname);
            m_snmp.SnmpGetValue(Snmp.ifPhysAddress, ref resultmac);
            String strname = m_snmp.GetInfoFromValuePDU(resultname);
            String strmac = m_snmp.GetInfoFromValuePDU(resultmac);
            m_snmp.SnmpClose();
            if (strname == "" && strmac == "")
            {
                for (int i = 0; i < m_sIPActiveDevices.Count; i++)
                {
                    if (sIP == m_sIPActiveDevices[i])
                    {
                        m_sIPActiveDevices.RemoveAt(i);
                        break;
                    }
                }
                return false;
            }
            for (int i = 0; i < m_hostInfo.Count; i++)
            {
                if (m_hostInfo[i].m_sIPInDatabase == sIP)
                {
                    m_hostInfo[i].bSNMP = true;
                }
            }
            m_sNameOfDevices.Add(strname);
            m_sMacOfDevices.Add(strmac);
            return true;
        }
        #endregion


        /***************************2013-08-20 BYG**********************************/
        #region 读取数据库中内部设备信息
        //读取席位机
        public static void setHost()
        {
            try
            {
                Database db = new Database();
                string sql = "SELECT * FROM T_SEAT order by seat_typeid desc";
                DataSet ds = db.GetDataSet(sql);

                int hostcount = ds.Tables[0].Rows.Count;
                if (hostcount > 0)
                {
                    for (int i = 0; i < hostcount; i++)
                    {
                        DataRow dr = ds.Tables[0].Rows[i];
                        HostInfo host = new HostInfo();
                        host.bActive = false;
                        host.clientID = int.Parse(dr["SEAT_ID"].ToString());
                        host.hostName = dr["SEAT_HOSTNAME"].ToString();
                        host.m_sIPInDatabase = dr["SEAT_IPA"].ToString();
                        //host.seatID = int.Parse(dr["CLIENT_SEATID"].ToString());
                        host.seatName = dr["SEAT_NAME"].ToString();
                        host.cpuTH = MainForm.CPUVALUE;
                        host.diskTH = MainForm.DISKVALUE;
                        host.memTH = MainForm.MEMVALUE;
                        host.trafficTH = MainForm.TRAFFICVALUE;
                        host.lCurRecvLen = 0;
                        host.lPreRecvLen = 0;
                        host.memValue = 0;
                        host.cpuValue = 0;
                        host.diskValue = 0;
                        host.inComingTraffic = 0.0;
                        host.outGoingTraffic = 0.0;
                        host.totalTraffic = 0.0;
                           
                        host.table = TABLE.TAB_CLIENT;
                        host.hostMac = dr["SEAT_MAC"].ToString();
                        if (dr["SEAT_TYPEID"].ToString() == "2")
                        {
                            host.type = TYPEOFDEVICE.HOST;
                            MainForm.hostInfo.Add(host);
                        }
                        else if (dr["SEAT_TYPEID"].ToString() == "1")
                        {
                            host.type = TYPEOFDEVICE.SERVER;
                            MainForm.serverInfo.Add(host);
                        }
                        else if (dr["SEAT_TYPEID"].ToString() == "0")
                        {
                            host.type = TYPEOFDEVICE.SWITCH;
                            MainForm.switchInfo.Add(host);
                        }

                        MainForm.allInfo.Add(host);
                    }
                }
            }
            catch (Exception exx)
            {
                MessageBoxEx.Show(exx.ToString());
            }
        }
        
        #endregion

        

    }
}
