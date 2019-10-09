using System;
using System.Collections.Generic;
using System.Text;

using adventnet.snmp.snmp2;

namespace MonitorAndSet.TopoInfo
{
    public class Snmp
    {
        #region MIB定义
        public static string mib = ".1.3.6.1.2.1";	//

        //系统组
        public static string sysName = ".1.3.6.1.2.1.1.5";	     //
        public static string sysServices = ".1.3.6.1.2.1.1.7";	//判断PC or switch or router

        //ip地址表
        public static string ipAddrTable = ".1.3.6.1.2.1.4.20";   //SEQUENCE OF数据类型结构
        public static string ipAddrEntry = ".1.3.6.1.2.1.4.20.1";  //SEQUENCE数据类型结构，共由下面5个对象构成
        public static string ipAdEntAddr = ".1.3.6.1.2.1.4.20.1.1";            //     //这一行的ip地址
        public static string ipAdEntIfIndex = ".1.3.6.1.2.1.4.20.1.2";	            //条目的索引值  //对应的接口数
        public static string ipAdEntNetMask = ".1.3.6.1.2.1.4.20.1.3.128.1.1.51";	//ip地址的掩码

        //ip路由表
        public static string ipRouteTable = ".1.3.6.1.2.1.4.21";		//
        public static string ipRouteEntry = ".1.3.6.1.2.1.4.21.1";	      //到一个特定目的地的路由
        public static string ipRouteDest = ".1.3.6.1.2.1.4.21.1.1";	      //这个路由的目标ip
        public static string ipRouteIfIndex = ".1.3.6.1.2.1.4.21.1.2";	  //接口的索引
        public static string ipRouteNextHop = ".1.3.6.1.2.1.4.21.1.7";	  //下一跳的ip地址
        public static string ipRouteType = ".1.3.6.1.2.1.4.21.1.8";	      //路由类型
        public static string ipRouteMask = ".1.3.6.1.2.1.4.21.1.11";	  //同ipRouteDest进行对比前，与目的地址and的掩码

        public static string Gateway = ".1.3.6.1.2.1.4.21.1.7";        //网关，注意和ipRouteNextHop值一样
        public static string ifPhysAddress = ".1.3.6.1.2.1.2.2.1.6.1"; //物理地址
        public static string ipForwarding = ".1.3.6.1.2.1.4.1.0";     //是否转发

        //交换机相关
        //交换机基本信息
        public static string dot1dBase = ".1.3.6.1.2.1.17.1";
        public static string dot1dBaseBridgeAddress = ".1.3.6.1.2.1.17.1.1";       //交换机的MAC地址   
        public static string dot1dBaseNumPorts = ".1.3.6.1.2.1.17.1.2";           //交换机的端口数

        //交换机地址转发表
        public static string dot1dTpFdbTable = ".1.3.6.1.2.1.17.4.3";  //SEQUENCE OF数据类型结构
        public static string dot1dTpFdbEntry = ".1.3.6.1.2.1.17.4.3.1"; //SEQUENCE数据类型结构，由下面3个对象构成
        public static string dot1dTpFdbAddress = ".1.3.6.1.2.1.17.4.3.1.1";      //交换机地址表中的源MAC地址
        public static string dot1dTpFdbPort = ".1.3.6.1.2.1.17.4.3.1.2";          //查看交换机中地址表中的源地址所在的端口
        public static string dot1dTpFdbStatus = ".1.3.6.1.2.1.17.4.3.1.3";

        //交换机转发端口表,端口流量
        public static string dot1dTpPortTable = ".1.3.6.1.2.1.17.4.4";  //SEQUENCE OF数据类型结构
        public static string dot1dTpPortEntry = ".1.3.6.1.2.1.17.4.4.1"; //SEQUENCE数据类型结构，由下面5个对象构成
        public static string dot1dTpPort = ".1.3.6.1.2.1.17.4.4.1.1";
        public static string dot1dTpPortMaxInfo = ".1.3.6.1.2.1.17.4.4.1.2";
        public static string dot1dTpPortInFrames = ".1.3.6.1.2.1.17.4.4.1.3";    //与交换机流量相关的
        public static string dot1dTpPortOutFrames = ".1.3.6.1.2.1.17.4.4.1.4";   //与交换机流量相关的
        public static string dot1dTpPortInDiscards = ".1.3.6.1.2.1.17.4.4.1.5";
        #endregion

        public String outstring = "";
        public String wronginfo = "";

        public SnmpAPI api;
        public SnmpSession session;

        public String m_sDestIp;	   //目的主机IP地址

        public Snmp()
        {

        }

        #region 初始化及启动SNMP StartUp()
        /// <summary>
        /// 初始化及启动SNMP
        /// </summary>
        public bool StartUp()
        {
            // Start SNMP API
            api = new SnmpAPI();

            // Open session
            session = new SnmpSession(api);

            try
            {
                session.Open();
            }
            catch (SnmpException e)
            {
                wronginfo += "Error opening socket: " + e.Message.ToString();
                return false;
            }

            return true;
        }
        #endregion

        #region 设置目标主机IP
        /// <summary>
        /// 设置目标主机IP
        /// </summary>
        public void SetDestIP(String destIP)
        {
            this.m_sDestIp = destIP;
        }
        #endregion

        #region 获取一个表的内容SnmpGetTable()
        /// <summary>
        /// 获取一个表的内容
        /// </summary>
        public bool SnmpGetTable(System.String OID, ref System.String result)       //获取一个表的内容
        {

            SnmpPDU pdu = new SnmpPDU();

            //Build GET Request PDU
            pdu.Community = "public";
            //get the value from the command line
            UDPProtocolOptions option = new UDPProtocolOptions(m_sDestIp);
            pdu.ProtocolOptions = option;
            //pdu.Timeout = 1;
            pdu.Retries = 2;
            pdu.Command = SnmpAPI.GETNEXT_REQ_MSG;     //注意：获取值是用GETNEXT

            // add OIDs
            SnmpOID rootoid = new SnmpOID(OID);
            String root = rootoid.ToString();
            pdu.AddNull(rootoid);

            // loop for each PDU in the walk
            while (true)
            // until received OID isn't in sub-tree
            {
                try
                {
                    // Send PDU and receive response PDU
                    pdu = session.SyncSend(pdu);
                }
                catch (SnmpException e)
                {
                    wronginfo += "Sending PDU" + e.Message.ToString();
                    return false;
                }

                if (pdu == null)
                {
                    // timeout
                    result += "Request timed out to " + m_sDestIp;
                    return false;
                }

                // stop if outside sub-tree
                if (!(pdu.GetObjectID(0).ToString().StartsWith(root)))
                {
                    break;
                }

                int version = pdu.Version;

                if (version == SnmpAPI.SNMP_VERSION_1)
                {
                    // check for error 
                    if (pdu.Errstat != 0)
                    {
                        wronginfo += "Error Indication in response: " + SnmpException.ExceptionString((sbyte)pdu.Errstat).ToString() + "\nErrindex: " + pdu.Errindex.ToString();
                        return false;
                    }
                    // print response pdu variable-bindings                    
                    result += pdu.PrintVarBinds().ToString();

                }
                else if (version == SnmpAPI.SNMP_VERSION_2C)
                {

                    System.Collections.IEnumerator e = pdu.VariableBindings.GetEnumerator();

                    while (e.MoveNext())
                    {
                        int error = 0;
                        SnmpVarBind varbind = (SnmpVarBind)e.Current;
                        // check for error
                        if ((error = varbind.Errindex) != 0)
                        {
                            wronginfo += "Error Indication in response: " + SnmpException.ExceptionString((sbyte)error);
                            return false;
                        }
                        // print response pdu variable-bindings
                        result += pdu.PrintVarBinds().ToString();

                    }
                }
                else
                {
                    wronginfo += "Invalid Version Number";
                    return false;
                }

                // set GETNEXT_REQ_MSG to do walk
                // Don't forget to set request id to 0 otherwise next request will fail
                pdu.Reqid = 0;
                pdu.Command = adventnet.snmp.snmp2.SnmpAPI.GETNEXT_REQ_MSG;
            } // end of while true


            return true;
        }
        #endregion

        #region 获取多个简单变量的值SnmpGetMultiValue()
        /// <summary>
        /// 获取多个简单变量的值
        /// </summary>
        public bool SnmpGetMultiValue(List<String> oidlist, ref List<String> resultlist)        //获取多个简单变量的值
        {
            //Build GET Request PDU
            SnmpPDU pdu = new SnmpPDU();
            pdu.Community = "public";
            //get the value from the command line
            UDPProtocolOptions option = new UDPProtocolOptions(m_sDestIp);
            pdu.ProtocolOptions = option;
            pdu.Timeout = 1;
            pdu.Retries = 2;
            pdu.Command = SnmpAPI.GETNEXT_REQ_MSG;     //注意：获取值是用GETNEXT

            // add OIDs
            SnmpOID oid;
            for (int i = 0; i < oidlist.Count; i++)
            {
                oid = new SnmpOID(oidlist[i]);
                pdu.AddNull(oid);
            }


            SnmpPDU result = null;
            // Send PDU and receive response PDU
            try
            {
                result = session.SyncSend(pdu);
            }
            catch (SnmpException e)
            {

                wronginfo += "Error sending SNMP request: " + e.Message.ToString();
                return false;
            }

            if (result == null)
            {
                outstring += "Request timed out!";
                return false;
            }
            else
            {
                if (result.Errstat == 0)
                {
                    for (int i = 0; i < result.VariableBindings.Count; i++)
                    {
                        resultlist.Add(result.VariableBindings[i].ToString());
                    }
                }
                else
                {
                    wronginfo += result.Error;
                    return false;
                }
            }
            return true;

        }
        #endregion

        #region 获取一个简单变量的值SnmpGetValue()
        /// <summary>
        /// 获取一个简单变量的值
        /// </summary>
        public bool SnmpGetValue(String OID, ref String outstr)        //获取一个简单变量的值
        {
            //Build GET Request PDU
            SnmpPDU pdu = new SnmpPDU();
            pdu.Community = "public";
            //get the value from the command line
            UDPProtocolOptions option = new UDPProtocolOptions(m_sDestIp);
            pdu.ProtocolOptions = option;
            //pdu.Timeout = 1;
            pdu.Retries = 2;
            pdu.Command = SnmpAPI.GETNEXT_REQ_MSG;     //注意：获取值是用GETNEXT

            // add OIDs
            SnmpOID oid = new SnmpOID(OID);
            pdu.AddNull(oid);

            SnmpPDU result = null;
            // Send PDU and receive response PDU
            try
            {
                result = session.SyncSend(pdu);
            }
            catch (SnmpException e)
            {

                wronginfo += "Error sending SNMP request: " + e.Message.ToString();
                return false;
            }

            if (result == null)
            {
                outstring += "Request timed out!";
                return false;
            }
            else
            {
                if (result.Errstat == 0)
                {
                    outstr = result.PrintVarBinds().ToString(); ;
                }
                else
                {
                    wronginfo += result.Error;
                    return false;
                }
            }
            return true;

        }
        #endregion

        #region 关闭SNMP
        /// <summary>
        /// 关闭SNMP
        /// </summary>
        public void SnmpClose()
        {
            // close session
            session.Close();
            // stop api thread
            api.Close();
        }
        #endregion

        #region 获取表内容后计算得到的总变量数目
        /// <summary>
        /// 获取表内容后计算得到的总变量数目
        /// </summary>
        public int GetSnmpCount(System.String result)
        {
            int num = 0;
            for (int i = 0; i < result.Length; i++)
            {
                if (result[i] == 'O' && result[i + 1] == 'b' && result[i + 2] == 'j' && result[i + 3] == 'e' && result[i + 4] == 'c'
                    && result[i + 5] == 't' && result[i + 6] == ' ' && result[i + 7] == 'I' && result[i + 8] == 'D' && result[i + 9] == ':')
                {
                    num++;
                }
            }
            return num;
        }
        #endregion

        #region 从简单变量的PDU中获取信息
        /// <summary>
        /// 从简单变量的PDU中获取信息
        /// </summary>
        public String GetInfoFromValuePDU(System.String result)
        {
            String resultstr = result.Trim();
            if(resultstr == "")
            {
                return resultstr;
            }
            String str = "";
            str = resultstr.Substring(resultstr.LastIndexOf(": ") + 2);
            return str;
        }
        #endregion

        #region 从表变量的PDU中获取两列变量的信息
        /// <summary>
        /// 从表变量的PDU中获取两列变量的信息,numOfCol是该表包含的列变量的数目,col1,col2是获取列的序列号(从0开始的,最大为numOfCol-1),获取内容分别存入outList1和outList2
        /// </summary>
        //从表变量的PDU中获取信息,numOfCol是该表包含的列变量的数目,outList1和outList2分别是输出的内容
        //返回值若为空,表示没有取到信息;不为空,Convert.ToInt32(返回值)表示获得的每个列变量的个数
        public String GetInfoFromTablePDU(System.String result, int numOfCol, int col1, int col2, ref List<String> outList1, ref List<String> outList2)
        {
            String resultstr = result.Trim();
            if (resultstr == "")
            {
                return resultstr;
            }

            int nCount = GetSnmpCount(result) / numOfCol;     //获得的每个列变量的数目

            String str = "";
            for (int k = 0; k < numOfCol; k++)
            {
                for (int i = 0; i < nCount; i++)
                {
                    if ((k == numOfCol - 1) && (i == nCount - 1))  //判断是最后的一个变量
                    {
                        str = resultstr;
                    }
                    else
                    {
                        str = resultstr.Substring(0, resultstr.IndexOf("\nO"));
                    }
                    str = str.Substring(str.LastIndexOf(": ") + 2);
                    if (k == col1)        //获取表中第col1列变量的内容,放入outList1中
                    {
                        outList1.Add(str);
                    }
                    if (k == col2)       //获取表中第col2列变量的内容,放入outList2中
                    {
                        outList2.Add(str);
                    }
                    resultstr = resultstr.Substring(resultstr.IndexOf("\nO") + 1);  //去掉已获得变量的内容
                }
            }
            return nCount.ToString();
        }
        #endregion
    }
}
