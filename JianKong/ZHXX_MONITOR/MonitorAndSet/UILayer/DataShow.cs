using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using System.Data.OracleClient;
using MonitorAndSet.CommonClass;
using MonitorAndSet.TopoInfo;
using DevComponents.DotNetBar;

namespace MonitorAndSet
{
    public partial class DataShow : DevComponents.DotNetBar.Office2007Form
    {
        public DataShow()
        {
            InitializeComponent();
        }

        private void DataShow_Load(object sender, EventArgs e)
        {
            try
            {
                this.BackColor = MainForm.DlgBackColor;

                //ShowDeviceInfo();
                timer1.Start();
            }
            catch (Exception ex)
            {
                String str = ex.ToString();
            }
        }

        private void btn_cancel_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void DataShow_Activated(object sender, EventArgs e)
        {
           // ShowDeviceInfo();
        }

        //public void ShowDeviceInfo()
        //{
        //    try
        //    {
        //        this.label_detail.Text = "任务" + MainForm.tctaskname + "的测量设备：";
        //        this.DeviceListView.Items.Clear();
        //        String strtrue = "有数据";
        //        String strfalse = "无数据";
        //        System.Globalization.NumberFormatInfo GN = new System.Globalization.CultureInfo("zh-CN", false).NumberFormat;
        //        GN.NumberDecimalDigits = 3;

        //        for (int i = 0; i < MainForm.temp.Count; i++)
        //        {
        //            ListViewItem lt = new ListViewItem(MainForm.temp[i].msdevid.ToString());
        //            if (MainForm.temp[i].state)
        //            {
        //                lt.SubItems.Add(strtrue);
        //            }
        //            else
        //            {
        //                lt.SubItems.Add(strfalse);
        //            }
        //            lt.SubItems.Add(MainForm.temp[i].Traffic.ToString("N", GN) + "KB/sec");
        //            lt.SubItems.Add(MainForm.temp[i].msdevcode.ToString());
        //            lt.SubItems.Add(MainForm.temp[i].msdevtypename);
        //            lt.SubItems.Add(MainForm.temp[i].devIP);
        //            if (MainForm.temp[i].devPort == 0)
        //            {
        //                lt.SubItems.Add("");
        //            }
        //            else
        //            {
        //                lt.SubItems.Add(MainForm.temp[i].devPort.ToString());
        //            }
        //            this.DeviceListView.Items.Add(lt);
        //        }

        //        //将有数据的数据源信息背景设为粉红色
        //        for (int i = 0; i < this.DeviceListView.Items.Count; i++)
        //        {
        //            if (this.DeviceListView.Items[i].SubItems[1].Text == "有数据")
        //            {
        //                this.DeviceListView.Items[i].BackColor = Color.Pink;
        //            }
        //        }
        //    }
        //    catch (Exception ex)
        //    {
        //        String str = ex.ToString();
        //    }
        //}

        private void timer1_Tick(object sender, EventArgs e)
        {
           // ShowDeviceInfo();
        }
        #region 限制LIST列的最小宽度
        private void DeviceListView_ColumnWidthChanged(object sender, ColumnWidthChangedEventArgs e)
        {
            if(this.DeviceListView.Columns[e.ColumnIndex].Width<100)
            {
                this.DeviceListView.Columns[e.ColumnIndex].Width = 100;
            }
        }
        #endregion
    }
}