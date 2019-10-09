using System;
using System.Collections.Generic;
using System.Text;

namespace MonitorAndSet.TopoInfo
{
    /// <summary>
    /// TABLE枚举用于表示从哪个表中读取数据,1表示client表，2表示server表
    /// </summary>
    public enum TABLE
    {
        TAB_CLIENT = 1,
        TAB_SERVER = 2
    };

    /// <summary>
    /// TYPEOFDEVICE枚举用于表示设备的类型
    /// </summary>

    public enum TYPEOFDEVICE
    {
        NONE = 0,
        HOST = 1,
        SWITCH = 2,
        HUB = 3,
        SERVER = 4,
        SAN = 5,
        PLANE = 6,
        RADAR = 7,
        ELEC = 8,
        COMUNI= 9
    }

    /// <summary>
    /// HostInfo表示主机信息
    /// </summary>
    public class HostInfo
    {
        /// <summary>
        /// 无参数构造函数
        /// </summary> 
        public HostInfo()
        {
            this.m_mProcInfo = new Dictionary<string, bool>();
            this.m_mServInfo = new Dictionary<string, bool>();
            this.forbiddenPro = new Dictionary<string, bool>();//禁用进程列表
        }

        /// <summary>
        /// 席位主机信息的构造函数，读取数据库表T_CLIENT时所用
        /// </summary> 
        public HostInfo(string m_sIPInDatabase, int seatID, int clientID, string hostName, bool bActive, long lCurRecvLen,
            long lPreRecvLen, int cpuValue, int diskValue, int memValue, double inComingTraffic, double outGoingTraffic,
            double totalTraffic, TYPEOFDEVICE type, TABLE table)
        {
            this.m_sIPInDatabase = m_sIPInDatabase;
            this.seatID = seatID;
            this.clientID = clientID;
            this.hostName = hostName;
            this.bActive = bActive;
            this.lCurRecvLen = lCurRecvLen;
            this.lPreRecvLen = lPreRecvLen;
            this.cpuValue = cpuValue;
            this.diskValue = diskValue;
            this.memValue = memValue;
            this.inComingTraffic = inComingTraffic;
            this.outGoingTraffic = outGoingTraffic;
            this.totalTraffic = totalTraffic;
            this.type = type;
            this.table = table;
            this.m_mProcInfo = new Dictionary<string, bool>();
            this.m_mServInfo = new Dictionary<string, bool>();
        }

        /// <summary>
        /// 服务器信息的构造函数，读取数据库表T_SERVER时所用
        /// </summary> 
        public HostInfo(string m_sIPInDatabase, int clientID, string seatName, string hostName, bool bActive, long lCurRecvLen,
            long lPreRecvLen, int cpuValue, int diskValue, int memValue, TYPEOFDEVICE type, TABLE table)
        {
            this.m_sIPInDatabase = m_sIPInDatabase;
            this.clientID = clientID;
            this.seatName = seatName;
            this.hostName = hostName;
            this.bActive = bActive;
            this.lCurRecvLen = lCurRecvLen;
            this.lPreRecvLen = lPreRecvLen;
            this.cpuValue = cpuValue;
            this.diskValue = diskValue;
            this.memValue = memValue;
            this.type = type;
            this.table = table;
            this.m_mProcInfo = new Dictionary<string, bool>();
            this.m_mServInfo = new Dictionary<string, bool>();
        }

        private String sIPInDatabase;
        /// <summary>
        /// 数据库中的机器IP地址
        /// </summary>
        public String m_sIPInDatabase
        {
            get { return sIPInDatabase; }
            set { sIPInDatabase = value; }
        }

        private int m_seatID;
        /// <summary>
        /// 席位ID
        /// </summary> 
        public int seatID
        {
            get { return m_seatID; }
            set { m_seatID = value; }
        }

        private String m_seatName;
        /// <summary>
        /// 席位名称
        /// </summary>
        public String seatName
        {
            get { return m_seatName; }
            set { m_seatName = value; }
        }

        private int m_clientID;
        /// <summary>
        /// 客户机ID
        /// </summary>
        public int clientID
        {
            get { return m_clientID; }
            set { m_clientID = value; }
        }

        private String m_hostName;
        /// <summary>
        /// 客户机名称
        /// </summary>
        public String hostName
        {
            get { return m_hostName; }
            set { m_hostName = value; }
        }

        private bool m_bActive;
        /// <summary>
        /// 如果为true则说明这台主机是开机的，否则说明已经关机
        /// </summary>
        public bool bActive
        {
            get { return m_bActive; }
            set { m_bActive = value; }
        }

        private bool m_bSNMP;
        /// <summary>
        /// SNMP服务是否启动
        /// </summary>
        public bool bSNMP
        {
            get { return m_bSNMP; }
            set { m_bSNMP = value; }
        }

        private int m_nCount;
        /// <summary>
        /// 连续没有收到数据的次数
        /// </summary>
        public int nCount
        {
            get { return m_nCount; }
            set { m_nCount = value; }
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

        private String m_sInfo;
        /// <summary>
        /// 刚刚收到的数据
        /// </summary> 
        public String sInfo
        {
            get { return m_sInfo; }
            set { m_sInfo = value; }
        }

        private int m_cpuValue;
        /// <summary>
        /// CPU使用率
        /// </summary>
        public int cpuValue
        {
            get { return m_cpuValue; }
            set { m_cpuValue = value; }
        }

        private int m_diskValue;
        /// <summary>
        /// 硬盘剩余情况
        /// </summary>
        public int diskValue
        {
            get { return m_diskValue; }
            set { m_diskValue = value; }
        }

        private int m_memValue;
        /// <summary>
        /// 内存剩余情况
        /// </summary>
        public int memValue
        {
            get { return m_memValue; }
            set { m_memValue = value; }
        }

        private String m_netPortName;
        /// <summary>
        /// 网络接口名称
        /// </summary>
        public String netPortName
        {
            get { return m_netPortName; }
            set { m_netPortName = value; }
        }

        private long m_bandWidth;
        /// <summary>
        /// 带宽
        /// </summary>
        public long bandWidth
        {
            get { return m_bandWidth; }
            set { m_bandWidth = value; }
        }

        private double m_inComingTraffic;
        /// <summary>
        /// 输入流量
        /// </summary>
        public double inComingTraffic
        {
            get { return m_inComingTraffic; }
            set { m_inComingTraffic = value; }
        }

        private double m_outGoingTraffic;
        /// <summary>
        /// 输出流量
        /// </summary>
        public double outGoingTraffic
        {
            get { return m_outGoingTraffic; }
            set { m_outGoingTraffic = value; }
        }

        private double m_totalTraffic;
        /// <summary>
        /// 总流量
        /// </summary>
        public double totalTraffic
        {
            get { return m_totalTraffic; }
            set { m_totalTraffic = value; }
        }

        private TYPEOFDEVICE m_type;
        /// <summary>
        /// 确定是席位上的主机还是服务器，这里只能是SERVER或者是HOST
        /// </summary>
        public TYPEOFDEVICE type
        {
            get { return m_type; }
            set { m_type = value; }
        }

        private TABLE m_table;
        /// <summary>
        /// 确定数据是从哪个数据表中读取的,1表示client表，2表示server表
        /// </summary> 
        public TABLE table
        {
            get { return m_table; }
            set { m_table = value; }
        }
        public Dictionary<String, bool> forbiddenPro;  //禁用进程列表
        private Dictionary<String, bool> m_mProcInfo;
        /// <summary>
        /// 表示进程的运行与否的情况
        /// </summary>
        public Dictionary<String, bool> mProcInfo
        {
            get { return m_mProcInfo; }
            set { m_mProcInfo = value; }
        }

        private Dictionary<String, bool> m_mServInfo;
        /// <summary>
        /// 表示服务运行与否的情况
        /// </summary>
        public Dictionary<String, bool> mServInfo
        {
            get { return m_mServInfo; }
            set { m_mServInfo = value; }
        }

        private bool m_bNormal;
        /// <summary>
        /// bActive为true时该属性表示主机或服务器的状态，为true表示状态正常;否则表示有指标超过阈值
        /// </summary>
        public bool bNormal
        {
            get { return m_bNormal; }
            set { m_bNormal = value; }
        }

        private int m_cpuTH;
        /// <summary>
        /// 具体机器的CPU使用率门限值
        /// </summary>
        public int cpuTH
        {
            get { return m_cpuTH; }
            set { m_cpuTH = value; }
        }

        private int m_diskTH;
        /// <summary>
        /// 具体机器的硬盘剩余情况门限值
        /// </summary>
        public int diskTH
        {
            get { return m_diskTH; }
            set { m_diskTH = value; }
        }

        private int m_memTH;
        /// <summary>
        /// 具体机器的内存剩余情况门限值
        /// </summary>
        public int memTH
        {
            get { return m_memTH; }
            set { m_memTH = value; }
        }
        private int m_trafficTH;
        /// <summary>
        /// 具体机器的内存剩余情况门限值
        /// </summary>
        public int trafficTH
        {
            get { return m_trafficTH; }
            set { m_trafficTH = value; }
        }
        private int m_XLocation;
        /// <summary>
        /// 拓扑图横坐标
        /// </summary>
        public int XLocation
        {
            get { return m_XLocation; }
            set { m_XLocation = value; }
        }

        private int m_YLocation;
        /// <summary>
        /// 拓扑图纵坐标
        /// </summary>
        public int YLocation
        {
            get { return m_YLocation; }
            set { m_YLocation = value; }
        }

        private string m_url;
        /// <summary>
        /// 服务器URL
        /// </summary> 
        public string url
        {
            get { return m_url; }
            set { m_url = value; }
        }

        private string  m_hostMac;
        /// <summary>
        /// 机器MAC
        /// </summary>
        public string hostMac
        {
            get { return m_hostMac; }
            set { m_hostMac = value; }
        }

    //   public bool l { get; set; }

    }
}
