using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using MonitorAndSet.TopoInfo;
using SnmpSharpNet;
using DevComponents.DotNetBar;
using MonitorAndSet.CommonClass;
using System.Threading;
using System.Collections;

namespace MonitorAndSet
{
    public partial class SwitchInfoForm : DevComponents.DotNetBar.Office2007Form
    {
        public String SwitchIP = "";   //表示交换机的IP
        public int SwitchID = -1;   //表示交换机的ID
        private  int switchtype;
        public bool start = true;
        public int eportNum; //电口数目；
        public int gportNum; //光口数目；
        private int setdataOK = 0;
        public Thread recvSNMP;
        #region 定义oid信息
        //系统描述
        public string sysDescr = "1.3.6.1.2.1.1.1.0";
        //系统运行时间
        public string sysUpTime = "1.3.6.1.2.1.1.3.0";
        //端口描述
        public string ifDescr = ".1.3.6.1.2.1.2.2.1.2";
        //端口类型
        public string ifType = ".1.3.6.1.2.1.2.2.1.3";
        //端口状态
        public string ifOperState = ".1.3.6.1.2.1.2.2.1.8";
        //连接时长
        public string ifLastChange = ".1.3.6.1.2.1.2.2.1.9";
        //接收字节数
        public string ifInOctets = ".1.3.6.1.2.1.2.2.1.10";
        //发送字节数
        public string ifOutOctets = ".1.3.6.1.2.1.2.2.1.16";
        #endregion

        public ifInformation[] ifEP = new ifInformation[48];      //电口
        public ifInformation[] ifGP = new ifInformation[48];     //光口
      

        #region 交换机型号名称
        private const string EX8208 = "ex8208";
        private const string EX3200 = "ex3200";
        private const string EX4200 = "ex4200";
        private const string SSR2000 = "SSR 2000";
        private const string ISPIRATE = "iSpirit";
        private const string CISCO = "Cisco";
        #endregion

        /* 建立LABEL列表为显示交换机端口 2013-10-11 BYG
         */
      //  List<LabelX> ports = new List<LabelX>();
        private LabelX[] ports = new LabelX[48];
        private LabelX[] portG = new LabelX[48];
        //   List<Hashtable> connInfo = new List<Hashtable>();
        private Hashtable conIndex = new Hashtable();
        private Hashtable conGIndex = new Hashtable();

        public class ifInformation
        {
            public string type;
            public string operState;
            public string lastChange;
            public string inOctets;
            public string outOctets;
            public string Descr;
        }
        //定义字体  
        public Font MyFont = new Font("宋体", 10.0f, GraphicsUnit.Pixel);

        public List<ifInformation> ifList = new List<ifInformation>();
        /// <summary>
        /// 构造器
        /// </summary>
        public SwitchInfoForm()
        {
            InitializeComponent();
            // reduce flicker
            SetStyle(ControlStyles.AllPaintingInWmPaint, true);
            SetStyle(ControlStyles.UserPaint, true);
            SetStyle(ControlStyles.DoubleBuffer, true);
        }
        /// <summary>
        /// form加载事件
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void SwitchInfo_Load(object sender, EventArgs e)
        {
            try 
            {
                PortsInitial();
                //LabelTextClear();
                getConnectDevices();
                this.switchip.Text = SwitchIP;
                this.switchmac.Text = "";
                this.labelX4.Text = "";
                this.labelX6.Text = "";
                getType();
                recvSnmpRun();
                this.timer1.Start();
            }
            catch(Exception ee)
            {
                MessageBoxEx.Show(ee.ToString());
            }
        }


        /// <summary>
        /// 1秒计时器
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void timer1_tick(object sender, EventArgs e)
        {

            if (setdataOK == 1)
            {
                for (int i = 0; i < 48; i++)
                {
                    if (ports[i] != null)
                    {
                        groupPanel1.Controls.Add(ports[i]);
                        ports[i].MouseMove += new MouseEventHandler(control_MouseMove);
                        balloonTip1.SetBalloonText(ports[i], ports[i].Name);
                    }
                }
                //for (int i = 0; i < 48; i++)
                //{
                //    if (portG[i] != null)
                //    {
                //        groupPanel1.Controls.Add(portG[i]);
                //        portG[i].MouseMove += new MouseEventHandler(controlG_MouseMove);
                //        balloonTip1.SetBalloonText(portG[i], portG[i].Name);
                //    }

                //}
                setdataOK++;
            }

            if (setdataOK == 2)
            {

                for (int i = 0; i < 48; i++)
                {
                    if (ifEP[i] != null)
                    {
                        setLable(i, ifEP[i].operState);

                    }
                }
                //for (int i = 0; i < 48; i++)
                //{
                //    if (ifGP[i] != null)
                //    {
                //        setGLable(i, ifGP[i].operState);
                //    }
                //}
                this.labelX6.Text = (gportNum > 0) ? (eportNum.ToString() + " + " + gportNum.ToString()) : eportNum.ToString();

            }
        }

        private void recvSnmpRun()
        {
            recvSNMP = new Thread(new ThreadStart(this.recSnmp));
            recvSNMP.IsBackground = true;
            recvSNMP.Start();
        }

        private void recSnmp()
        {
            while (start)
            {
                try
                {
                    SimpleSnmp ssnmp = new SimpleSnmp(SwitchIP, MainForm.Community);//"7""5"  community改为从配置文件内读取
                    Pdu pdu_get = new Pdu(PduType.Get);
                    pdu_get.VbList.Add(this.sysDescr);
                    pdu_get.VbList.Add(this.sysUpTime);
                    Dictionary<Oid, AsnType> result = ssnmp.Get(SnmpVersion.Ver2, pdu_get);
                    if (result != null)
                    {
                        //    this.switchmac.Text = result.ElementAt(0).Value.ToString();
                        this.labelX4.Text = result.ElementAt(1).Value.ToString();
                        Dictionary<Oid, AsnType> ifTypeDic = ssnmp.Walk(SnmpVersion.Ver2, ifType);
                        Dictionary<Oid, AsnType> ifOperStateDic = ssnmp.Walk(SnmpVersion.Ver2, ifOperState);
                        Dictionary<Oid, AsnType> ifLastChangeDic = ssnmp.Walk(SnmpVersion.Ver2, ifLastChange);
                        Dictionary<Oid, AsnType> ifInOctetsDic = ssnmp.Walk(SnmpVersion.Ver2, ifInOctets);
                        Dictionary<Oid, AsnType> ifOutOctetsDic = ssnmp.Walk(SnmpVersion.Ver2, ifOutOctets);
                        Dictionary<Oid, AsnType> ifDescrDic = ssnmp.Walk(SnmpVersion.Ver2, ifDescr);
                        //Dictionary<Oid, AsnType> redDic = ssnmp.Walk(SnmpVersion.Ver2, alamred);
                        //Dictionary<Oid, AsnType> yellowDic = ssnmp.Walk(SnmpVersion.Ver2, alamyellow);
                        //Dictionary<Oid, AsnType> ledIndexDic = ssnmp.Walk(SnmpVersion.Ver2, ledIndex);
                        //leddescDic = ssnmp.Walk(SnmpVersion.Ver2, leddesc);
                        //ledstateDic = ssnmp.Walk(SnmpVersion.Ver2, ledstate);
                        //   Dictionary<Oid, AsnType> ledstateorderDic = ssnmp.Walk(SnmpVersion.Ver2, ledstateorder);
                        int index = 0;
                        foreach (KeyValuePair<Oid, AsnType> kvp in ifTypeDic)
                        {
                            if (index >= ifInOctetsDic.Count || index >= ifOutOctetsDic.Count)
                                continue;
                            string Descr = (ifDescrDic.Values.ElementAt(index)).ToString();
                            string typedic = (ifTypeDic.Values.ElementAt(index)).ToString();
                            string operstate = (ifOperStateDic.Values.ElementAt(index)).ToString();
                            string lastchange = (ifLastChangeDic.Values.ElementAt(index)).ToString();
                            string inoctets = (ifInOctetsDic.Values.ElementAt(index)).ToString();
                            string outoctets = (ifOutOctetsDic.Values.ElementAt(index)).ToString();

                            switch (switchtype)
                            {
                                case 0: getType0Data(Descr, typedic, operstate, lastchange, inoctets, outoctets); break;
                                case 1: getType1Data(Descr, typedic, operstate, lastchange, inoctets, outoctets); break;
                                case 2: getType2Data(Descr, typedic, operstate, lastchange, inoctets, outoctets); break;
                                case 3: getType3Data(Descr, typedic, operstate, lastchange, inoctets, outoctets); break;
                                case 4: getType4Data(Descr, typedic, operstate, lastchange, inoctets, outoctets); break;
                                case 5: getType5Data(Descr, typedic, operstate, lastchange, inoctets, outoctets); break;
                                default: return;
                            }
                            index++;
                        }
                        if (setdataOK == 0)
                            setdataOK++;
                    }
                    else
                    {
                        return;
                    }
                }
                catch (Exception EE)
                {
                    MessageBoxEx.Show(EE.ToString());
                }
            }
        }
        /// <summary>
        /// 画交换机端口连接情况，绿灯表示连接状态，红灯表示断开状态
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void portstate_paint(object sender, PaintEventArgs e)
        {
            //try 
            //{
            //    if(ifList.Count>0)
            //    {
            //        for (int i = 0; i < ifList.Count; i++)
            //        {
            //            ifInformation ifInf = ifList[i];
            //            if (ifInf.operState == "1")
            //            {
            //                ports[i].Image = global::MonitorAndSet.Properties.Resources.port_green;

            //            }
            //            else if (ifInf.operState == "2")
            //            {
            //                ports[i].Image = global::MonitorAndSet.Properties.Resources.port_red;
            //            }

            //            this.superTooltip1.SetSuperTooltip(ports[i],
            //                new DevComponents.DotNetBar.SuperTooltipInfo("端口" + (i + 1), "",
            //                    "连接设备：" + connInfo[i]["name"] + "\n\r"
            //                    + "设备  IP：" + connInfo[i]["ip"] + "\n\r"
            //                    + "输    入：" + ifInf.inOctets + "\n\r"
            //                    + "输    出：" + ifInf.outOctets, null, null,
            //                DevComponents.DotNetBar.eTooltipColor.Lemon));
            //        }
            //    }

            //}
            //catch(Exception EE)
            //{
            //    MessageBoxEx.Show(EE.ToString());
            //}
        }


        private void setLable(int Lableid, string operState)
        {
            if (operState == "1")
            {
                ports[Lableid].Image = global::MonitorAndSet.Properties.Resources.port_green;
                //    ports[Lableid].ImagePosition = eImagePosition.Top;

            }
            else if (operState == "2")
            {
                ports[Lableid].Image = global::MonitorAndSet.Properties.Resources.port_red;
                //  ports[Lableid].ImagePosition = eImagePosition.Top;
            }
        }



        /*去除TEXT显示文字
         */
        private void LabelTextClear() 
        {
            try 
            {
                foreach(LabelX label in ports)
                {
                    label.Text = "";

                }
            }
            catch(Exception e)
            {
                MessageBoxEx.Show(e.ToString());
            }
        }

        /*初始化PORTS
         */
        private void PortsInitial() 
        {
            //ports.Add(p1);
            //ports.Add(p2);
            //ports.Add(p3);
            //ports.Add(p4);
            //ports.Add(p5);
            //ports.Add(p6);
            //ports.Add(p7);
            //ports.Add(p8);
            //ports.Add(p9);
            //ports.Add(p10);
            //ports.Add(p11);
            //ports.Add(p12);
            //ports.Add(p13);
            //ports.Add(p14);
            //ports.Add(p15);
            //ports.Add(p16);
            //ports.Add(p17);
            //ports.Add(p18);
            //ports.Add(p19);
            //ports.Add(p20);
            //ports.Add(p21);
            //ports.Add(p22);
            //ports.Add(p23);
            //ports.Add(p24);
        }

        private void switchmac_MouseEnter(object sender, EventArgs e)
        {
            this.superTooltip1.SetSuperTooltip(this.switchmac, new DevComponents.DotNetBar.SuperTooltipInfo("系统描述", "",
               this.switchmac.Text , null, null, DevComponents.DotNetBar.eTooltipColor.Lemon));
        }

        /*获取交换机各端口连接设备
         */

        private void getConnectDevices() 
        {
            try 
            {
                Database db = new Database();
                string sql = "SELECT ROUTER_CONNID, ROUTER_PORT FROM T_ROUTER WHERE ROUTER_SERVERID = " + SwitchID +" ORDER BY ROUTER_PORT ";
                DataSet ds = db.GetDataSet(sql);
                int count = ds.Tables[0].Rows.Count;
                if (count > 0)
                {
                    //DataTable dt = ds.Tables[0];
                    for (int i = 0; i < count; i++ )
                    {
                        DataRow dr =ds.Tables[0].Rows[i];
                        int connid = int.Parse(dr["ROUTER_CONNID"].ToString());
                        int port = int.Parse(dr["ROUTER_PORT"].ToString());
                        if(connid == SwitchID)
                            continue;
                        for (int j = 0; j < MainForm.allInfo.Count;j++ )
                        {
                            if(MainForm.allInfo[j].clientID==connid)
                            {
                                conIndex.Add(port, j);
                                //Hashtable table = new Hashtable();
                                //table.Add("name",MainForm.allInfo[j].hostName);
                                //table.Add("ip",MainForm.allInfo[j].m_sIPInDatabase);
                                //connInfo.Add(table);
                            }
                        }
                    }
                }
            }
            catch(Exception ee)
            {
                MessageBoxEx.Show(ee.ToString());
            }
        }

        private void getType()
        {
            string typename = string.Empty;
            SimpleSnmp ssnmp = new SimpleSnmp(SwitchIP, MainForm.Community);//"7""5"  community改为从配置文件内读取
            Pdu pdu_get = new Pdu(PduType.Get);
            pdu_get.VbList.Add(this.sysDescr);
            //  pdu_get.VbList.Add(this.sysUpTime);
            Dictionary<Oid, AsnType> result = ssnmp.Get(SnmpVersion.Ver2, pdu_get);
            if (result != null)
            {
                typename = result.ElementAt(0).Value.ToString();
                if (typename.IndexOf(EX8208) != -1)
                {
                    switchtype = 0;
                    switchmac.Text = "EX8208";
                    return;
                }
                else if (typename.IndexOf(EX3200) != -1)
                {
                    switchtype = 1;
                    switchmac.Text = "EX3200";
                    return;
                }
                else if (typename.IndexOf(SSR2000) != -1)
                {
                    switchtype = 2;
                    switchmac.Text = "SSR-2000";
                }
                else if (typename.IndexOf(ISPIRATE) != -1)
                {
                    switchtype = 3;
                    switchmac.Text = typename;
                }
                else if (typename.IndexOf(CISCO) != -1)
                {
                    switchtype = 5;
                    switchmac.Text = typename;
                }
                else if (typename.IndexOf(EX4200) != -1)
                {
                    switchtype = 4;
                    switchmac.Text = "ex4200";
                }
                else
                {
                    switchtype = 5;
                    switchmac.Text = typename.Replace("\r\n"," ");
                }
            }
        }

        private void getType5Data(string Descr, string typedic, string operstate, string lastchange, string inoctets, string outoctets)
        {
            if (typedic == "6")
            {
                if (Descr.IndexOf("GigabitEthernet") != -1)
                {
                    string[] str = Descr.ToString().Split('/');

                    int port = int.Parse(str[2]);
                    if (ifEP[port - 1] == null)
                    {
                        ifInformation ifInf = new ifInformation();
                        ifInf.Descr = Descr;
                        ifInf.type = typedic;
                        ifInf.operState = operstate;
                        ifInf.lastChange = lastchange;
                        ifInf.inOctets = inoctets;
                        ifInf.outOctets = outoctets;
                        ifEP[port - 1] = ifInf;
                        setPortLabel(1, port, port - 1);
                        eportNum++;
                    }
                    else
                    {
                        ifEP[port - 1].operState = operstate;
                        ifEP[port - 1].lastChange = lastchange;
                        ifEP[port - 1].inOctets = inoctets;
                        ifEP[port - 1].outOctets = outoctets;
                    }

                }
            }
        }

        private void setPortLabel(int switchTypeid, int port, int pid)
        {
            LabelX lb = new LabelX();
            Point center = new Point(groupPanel1.Location.X + groupPanel1.Width / 2, 20);
            lb.Name = pid.ToString();
            lb.BackgroundStyle.Class = "";
            lb.BackgroundStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            lb.ImagePosition = DevComponents.DotNetBar.eImagePosition.Top;
            lb.Size = new System.Drawing.Size(34, 47);
            lb.Text = port.ToString();
            lb.TextAlignment = System.Drawing.StringAlignment.Center;
            lb.BackColor = Color.Transparent;
            lb.TextLineAlignment = System.Drawing.StringAlignment.Far;

            if (switchTypeid == 0)
            {
                int xindex = port / 2;
                int yindex = (port % 2 == 0) ? 0 : 1;
                lb.Location = new System.Drawing.Point(center.X - 540 + xindex * 40, center.Y + yindex * 50);
            }
            if (switchTypeid == 1)
            {
                int xindex = port / 2;
                int yindex = (port % 2 == 0) ? 0 : 1;
                lb.Location = new System.Drawing.Point(center.X - 250 + xindex * 40, center.Y + yindex * 50);
            }
            if (switchTypeid == 3)
            {
                int xindex = (port - 1) / 2;
                int yindex = (port % 2 == 0) ? 0 : 1;
                lb.Location = new System.Drawing.Point(center.X - 250 + xindex * 40, center.Y + yindex * 50);
            }
            if (switchTypeid == 2)
            {
                int xindex = pid / 2;
                int yindex = (pid % 2 == 0) ? 0 : 1;
                lb.Location = new System.Drawing.Point(center.X - 165 + xindex * 40, center.Y + yindex * 50);
            }
            if (switchtype == 5)
            {
                int xindex = (port -1) / 2;
                int yindex = (port % 2 == 0) ? 0 : 1;
                lb.Location = new System.Drawing.Point(center.X - 510 + xindex * 40, center.Y + yindex * 50);
            }
            ports[pid] = lb;
        }

        private void getType0Data(string Descr, string typedic, string operstate, string lastchange,string inoctets, string outoctets)
        {

        }

        private void getType1Data(string Descr, string typedic, string operstate, string lastchange, string inoctets, string outoctets)
        {

        }

        private void getType2Data(string Descr, string typedic, string operstate, string lastchange, string inoctets, string outoctets)
        {

        }


        private void getType3Data(string Descr, string typedic, string operstate, string lastchange, string inoctets, string outoctets)
        {

        }

        private void getType4Data(string Descr, string typedic, string operstate, string lastchange, string inoctets, string outoctets)
        {

        }

        private void control_MouseMove(object sender, MouseEventArgs e)
        {
            LabelX la = (LabelX)sender;
            int id = int.Parse(la.Name);
            if (id != -1 && id < eportNum)           //端口数可能是24口，或者是48口
            {
                if (conIndex.ContainsKey(id))
                {
                    string status = (ifEP[id].operState == "1") ? "UP" : "DOWN";
                    string text = "端口号: " + id.ToString() + "  设备名称 :" + MainForm.allInfo[(int)conIndex[id]].hostName + "\r\n状态：" + status;
                    string text2 = text + "\r\n设备IP：" + MainForm.allInfo[(int)conIndex[id]].m_sIPInDatabase + "\n\r" + "输入：" + ifEP[id].inOctets + "\n\r" + "输出：" + ifEP[id].outOctets;
                    balloonTip1.SetBalloonText(la, text2);
                }
                else
                {
                    balloonTip1.SetBalloonText(la, "端口未连接任何设备");
                }
            }
        }

        private void SwitchInfoForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            start = false;
        }

    }


}
