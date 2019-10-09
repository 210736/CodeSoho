namespace MonitorAndSet
{
    partial class HostInfoForm
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(HostInfoForm));
            this.tBox_MEMTHValue = new System.Windows.Forms.TextBox();
            this.tBox_DISKTHValue = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.tBox_CPUTHValue = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.tBox_MEMValue = new System.Windows.Forms.TextBox();
            this.tBox_DISKValue = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.tBox_CPUValue = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.tBox_IP = new System.Windows.Forms.TextBox();
            this.listView_Process = new System.Windows.Forms.ListView();
            this.chprocessname = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.chprocessstate = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.chprocessCPU = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.chprocessmemory = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.listView_Service = new System.Windows.Forms.ListView();
            this.chservicename = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.chservicestate = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.tBox_OutgoingTraffic = new System.Windows.Forms.TextBox();
            this.tBox_IncomingTraffic = new System.Windows.Forms.TextBox();
            this.tBox_TotalTraffic = new System.Windows.Forms.TextBox();
            this.label16 = new System.Windows.Forms.Label();
            this.label17 = new System.Windows.Forms.Label();
            this.label14 = new System.Windows.Forms.Label();
            this.label15 = new System.Windows.Forms.Label();
            this.label13 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.pgBar_OutTraffic = new System.Windows.Forms.ProgressBar();
            this.pgBar_InTraffic = new System.Windows.Forms.ProgressBar();
            this.pgBar_TotalTraffic = new System.Windows.Forms.ProgressBar();
            this.label11 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.btn_StartProSer = new DevComponents.DotNetBar.ButtonX();
            this.btn_CloseProSer = new DevComponents.DotNetBar.ButtonX();
            this.btn_THchange = new DevComponents.DotNetBar.ButtonX();
            this.btn_cancel = new DevComponents.DotNetBar.ButtonX();
            this.tBox_name = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.btn = new DevComponents.DotNetBar.ButtonX();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // tBox_MEMTHValue
            // 
            this.tBox_MEMTHValue.Location = new System.Drawing.Point(362, 75);
            this.tBox_MEMTHValue.Name = "tBox_MEMTHValue";
            this.tBox_MEMTHValue.ReadOnly = true;
            this.tBox_MEMTHValue.Size = new System.Drawing.Size(100, 21);
            this.tBox_MEMTHValue.TabIndex = 11;
            this.tBox_MEMTHValue.Enter += new System.EventHandler(this.tBox_MEMTHValue_Enter);
            // 
            // tBox_DISKTHValue
            // 
            this.tBox_DISKTHValue.Location = new System.Drawing.Point(362, 52);
            this.tBox_DISKTHValue.Name = "tBox_DISKTHValue";
            this.tBox_DISKTHValue.ReadOnly = true;
            this.tBox_DISKTHValue.Size = new System.Drawing.Size(100, 21);
            this.tBox_DISKTHValue.TabIndex = 10;
            this.tBox_DISKTHValue.Enter += new System.EventHandler(this.tBox_DISKTHValue_Enter);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(238, 78);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(113, 12);
            this.label3.TabIndex = 9;
            this.label3.Text = "内存剩余门限值(M):";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(238, 55);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(113, 12);
            this.label2.TabIndex = 8;
            this.label2.Text = "硬盘剩余门限值(G):";
            // 
            // tBox_CPUTHValue
            // 
            this.tBox_CPUTHValue.Location = new System.Drawing.Point(362, 29);
            this.tBox_CPUTHValue.Name = "tBox_CPUTHValue";
            this.tBox_CPUTHValue.ReadOnly = true;
            this.tBox_CPUTHValue.Size = new System.Drawing.Size(100, 21);
            this.tBox_CPUTHValue.TabIndex = 7;
            this.tBox_CPUTHValue.Enter += new System.EventHandler(this.tBox_CPUTHValue_Enter);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(238, 32);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(119, 12);
            this.label1.TabIndex = 6;
            this.label1.Text = "CPU占用率门限值(%):";
            // 
            // tBox_MEMValue
            // 
            this.tBox_MEMValue.Location = new System.Drawing.Point(119, 75);
            this.tBox_MEMValue.Name = "tBox_MEMValue";
            this.tBox_MEMValue.ReadOnly = true;
            this.tBox_MEMValue.Size = new System.Drawing.Size(100, 21);
            this.tBox_MEMValue.TabIndex = 17;
            this.tBox_MEMValue.Enter += new System.EventHandler(this.tBox_MEMValue_Enter);
            // 
            // tBox_DISKValue
            // 
            this.tBox_DISKValue.Location = new System.Drawing.Point(119, 52);
            this.tBox_DISKValue.Name = "tBox_DISKValue";
            this.tBox_DISKValue.ReadOnly = true;
            this.tBox_DISKValue.Size = new System.Drawing.Size(100, 21);
            this.tBox_DISKValue.TabIndex = 16;
            this.tBox_DISKValue.Enter += new System.EventHandler(this.tBox_DISKValue_Enter);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(12, 78);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(101, 12);
            this.label4.TabIndex = 15;
            this.label4.Text = "内存剩余情况(M):";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(12, 55);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(101, 12);
            this.label5.TabIndex = 14;
            this.label5.Text = "硬盘剩余情况(G):";
            // 
            // tBox_CPUValue
            // 
            this.tBox_CPUValue.Location = new System.Drawing.Point(119, 29);
            this.tBox_CPUValue.Name = "tBox_CPUValue";
            this.tBox_CPUValue.ReadOnly = true;
            this.tBox_CPUValue.Size = new System.Drawing.Size(100, 21);
            this.tBox_CPUValue.TabIndex = 13;
            this.tBox_CPUValue.Enter += new System.EventHandler(this.tBox_CPUValue_Enter);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(12, 32);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(83, 12);
            this.label6.TabIndex = 12;
            this.label6.Text = "CPU占用率(%):";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(12, 9);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(71, 12);
            this.label7.TabIndex = 18;
            this.label7.Text = "主机IP地址:";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(238, 9);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(83, 12);
            this.label8.TabIndex = 18;
            this.label8.Text = "主机计算机名:";
            // 
            // tBox_IP
            // 
            this.tBox_IP.Location = new System.Drawing.Point(119, 6);
            this.tBox_IP.Name = "tBox_IP";
            this.tBox_IP.ReadOnly = true;
            this.tBox_IP.Size = new System.Drawing.Size(100, 21);
            this.tBox_IP.TabIndex = 13;
            this.tBox_IP.Enter += new System.EventHandler(this.tBox_IP_Enter);
            // 
            // listView_Process
            // 
            this.listView_Process.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.listView_Process.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.chprocessname,
            this.chprocessstate,
            this.chprocessCPU,
            this.chprocessmemory});
            this.listView_Process.FullRowSelect = true;
            this.listView_Process.GridLines = true;
            this.listView_Process.Location = new System.Drawing.Point(14, 103);
            this.listView_Process.MultiSelect = false;
            this.listView_Process.Name = "listView_Process";
            this.listView_Process.Size = new System.Drawing.Size(448, 167);
            this.listView_Process.TabIndex = 19;
            this.listView_Process.UseCompatibleStateImageBehavior = false;
            this.listView_Process.View = System.Windows.Forms.View.Details;
            this.listView_Process.ColumnWidthChanged += new System.Windows.Forms.ColumnWidthChangedEventHandler(this.listView_Process_ColumnWidthChanged);
            this.listView_Process.Enter += new System.EventHandler(this.listView_Process_Enter);
            // 
            // chprocessname
            // 
            this.chprocessname.Text = "进程名称";
            this.chprocessname.Width = 140;
            // 
            // chprocessstate
            // 
            this.chprocessstate.Text = "状态";
            this.chprocessstate.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.chprocessstate.Width = 90;
            // 
            // chprocessCPU
            // 
            this.chprocessCPU.Text = "CPU(%)";
            this.chprocessCPU.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.chprocessCPU.Width = 90;
            // 
            // chprocessmemory
            // 
            this.chprocessmemory.Text = "内存占用(K)";
            this.chprocessmemory.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.chprocessmemory.Width = 128;
            // 
            // listView_Service
            // 
            this.listView_Service.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.listView_Service.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.chservicename,
            this.chservicestate});
            this.listView_Service.FullRowSelect = true;
            this.listView_Service.GridLines = true;
            this.listView_Service.Location = new System.Drawing.Point(14, 276);
            this.listView_Service.MultiSelect = false;
            this.listView_Service.Name = "listView_Service";
            this.listView_Service.Size = new System.Drawing.Size(448, 167);
            this.listView_Service.TabIndex = 19;
            this.listView_Service.UseCompatibleStateImageBehavior = false;
            this.listView_Service.View = System.Windows.Forms.View.Details;
            this.listView_Service.ColumnWidthChanged += new System.Windows.Forms.ColumnWidthChangedEventHandler(this.listView_Service_ColumnWidthChanged);
            this.listView_Service.Enter += new System.EventHandler(this.listView_Service_Enter);
            // 
            // chservicename
            // 
            this.chservicename.Text = "服务名称";
            this.chservicename.Width = 220;
            // 
            // chservicestate
            // 
            this.chservicestate.Text = "状态";
            this.chservicestate.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.chservicestate.Width = 228;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.tBox_OutgoingTraffic);
            this.groupBox1.Controls.Add(this.tBox_IncomingTraffic);
            this.groupBox1.Controls.Add(this.tBox_TotalTraffic);
            this.groupBox1.Controls.Add(this.label16);
            this.groupBox1.Controls.Add(this.label17);
            this.groupBox1.Controls.Add(this.label14);
            this.groupBox1.Controls.Add(this.label15);
            this.groupBox1.Controls.Add(this.label13);
            this.groupBox1.Controls.Add(this.label12);
            this.groupBox1.Controls.Add(this.pgBar_OutTraffic);
            this.groupBox1.Controls.Add(this.pgBar_InTraffic);
            this.groupBox1.Controls.Add(this.pgBar_TotalTraffic);
            this.groupBox1.Controls.Add(this.label11);
            this.groupBox1.Controls.Add(this.label10);
            this.groupBox1.Controls.Add(this.label9);
            this.groupBox1.Location = new System.Drawing.Point(480, 6);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(200, 437);
            this.groupBox1.TabIndex = 21;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "流量信息";
            // 
            // tBox_OutgoingTraffic
            // 
            this.tBox_OutgoingTraffic.BackColor = System.Drawing.SystemColors.Control;
            this.tBox_OutgoingTraffic.Location = new System.Drawing.Point(71, 304);
            this.tBox_OutgoingTraffic.Name = "tBox_OutgoingTraffic";
            this.tBox_OutgoingTraffic.ReadOnly = true;
            this.tBox_OutgoingTraffic.Size = new System.Drawing.Size(117, 21);
            this.tBox_OutgoingTraffic.TabIndex = 27;
            this.tBox_OutgoingTraffic.Enter += new System.EventHandler(this.tBox_OutgoingTraffic_Enter);
            // 
            // tBox_IncomingTraffic
            // 
            this.tBox_IncomingTraffic.BackColor = System.Drawing.SystemColors.Control;
            this.tBox_IncomingTraffic.Location = new System.Drawing.Point(71, 157);
            this.tBox_IncomingTraffic.Name = "tBox_IncomingTraffic";
            this.tBox_IncomingTraffic.ReadOnly = true;
            this.tBox_IncomingTraffic.Size = new System.Drawing.Size(117, 21);
            this.tBox_IncomingTraffic.TabIndex = 27;
            this.tBox_IncomingTraffic.Enter += new System.EventHandler(this.tBox_IncomingTraffic_Enter);
            // 
            // tBox_TotalTraffic
            // 
            this.tBox_TotalTraffic.BackColor = System.Drawing.SystemColors.Control;
            this.tBox_TotalTraffic.Location = new System.Drawing.Point(72, 19);
            this.tBox_TotalTraffic.Name = "tBox_TotalTraffic";
            this.tBox_TotalTraffic.ReadOnly = true;
            this.tBox_TotalTraffic.Size = new System.Drawing.Size(117, 21);
            this.tBox_TotalTraffic.TabIndex = 27;
            this.tBox_TotalTraffic.Enter += new System.EventHandler(this.tBox_TotalTraffic_Enter);
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Location = new System.Drawing.Point(172, 416);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(23, 12);
            this.label16.TabIndex = 26;
            this.label16.Text = "1Gb";
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Location = new System.Drawing.Point(6, 416);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(11, 12);
            this.label17.TabIndex = 25;
            this.label17.Text = "0";
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(172, 270);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(23, 12);
            this.label14.TabIndex = 24;
            this.label14.Text = "1Gb";
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(6, 270);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(11, 12);
            this.label15.TabIndex = 23;
            this.label15.Text = "0";
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(172, 132);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(23, 12);
            this.label13.TabIndex = 22;
            this.label13.Text = "1Gb";
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(6, 132);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(11, 12);
            this.label12.TabIndex = 21;
            this.label12.Text = "0";
            // 
            // pgBar_OutTraffic
            // 
            this.pgBar_OutTraffic.Location = new System.Drawing.Point(12, 332);
            this.pgBar_OutTraffic.Name = "pgBar_OutTraffic";
            this.pgBar_OutTraffic.Size = new System.Drawing.Size(177, 81);
            this.pgBar_OutTraffic.TabIndex = 20;
            // 
            // pgBar_InTraffic
            // 
            this.pgBar_InTraffic.Location = new System.Drawing.Point(12, 185);
            this.pgBar_InTraffic.Name = "pgBar_InTraffic";
            this.pgBar_InTraffic.Size = new System.Drawing.Size(177, 81);
            this.pgBar_InTraffic.TabIndex = 20;
            // 
            // pgBar_TotalTraffic
            // 
            this.pgBar_TotalTraffic.Location = new System.Drawing.Point(12, 48);
            this.pgBar_TotalTraffic.Name = "pgBar_TotalTraffic";
            this.pgBar_TotalTraffic.Size = new System.Drawing.Size(177, 81);
            this.pgBar_TotalTraffic.TabIndex = 20;
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(6, 307);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(59, 12);
            this.label11.TabIndex = 19;
            this.label11.Text = "输出流量:";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(6, 160);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(59, 12);
            this.label10.TabIndex = 19;
            this.label10.Text = "输入流量:";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(6, 23);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(47, 12);
            this.label9.TabIndex = 19;
            this.label9.Text = "总流量:";
            // 
            // timer1
            // 
            this.timer1.Interval = 1000;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // btn_StartProSer
            // 
            this.btn_StartProSer.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btn_StartProSer.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.btn_StartProSer.Location = new System.Drawing.Point(14, 449);
            this.btn_StartProSer.Name = "btn_StartProSer";
            this.btn_StartProSer.Size = new System.Drawing.Size(149, 23);
            this.btn_StartProSer.TabIndex = 24;
            this.btn_StartProSer.Text = "启动指定进程或服务";
            this.btn_StartProSer.Click += new System.EventHandler(this.btn_StartProSer_Click);
            // 
            // btn_CloseProSer
            // 
            this.btn_CloseProSer.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btn_CloseProSer.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.btn_CloseProSer.Location = new System.Drawing.Point(190, 449);
            this.btn_CloseProSer.Name = "btn_CloseProSer";
            this.btn_CloseProSer.Size = new System.Drawing.Size(155, 23);
            this.btn_CloseProSer.TabIndex = 25;
            this.btn_CloseProSer.Text = "关闭指定进程或服务";
            this.btn_CloseProSer.Click += new System.EventHandler(this.btn_CloseProSer_Click);
            // 
            // btn_THchange
            // 
            this.btn_THchange.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btn_THchange.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.btn_THchange.Location = new System.Drawing.Point(480, 449);
            this.btn_THchange.Name = "btn_THchange";
            this.btn_THchange.Size = new System.Drawing.Size(101, 23);
            this.btn_THchange.TabIndex = 26;
            this.btn_THchange.Text = "更改告警门限值";
            this.btn_THchange.Click += new System.EventHandler(this.btn_THchange_Click);
            this.btn_THchange.Enter += new System.EventHandler(this.btn_THchange_Enter);
            // 
            // btn_cancel
            // 
            this.btn_cancel.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btn_cancel.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.btn_cancel.Location = new System.Drawing.Point(603, 449);
            this.btn_cancel.Name = "btn_cancel";
            this.btn_cancel.Size = new System.Drawing.Size(75, 23);
            this.btn_cancel.TabIndex = 27;
            this.btn_cancel.Text = "关闭";
            this.btn_cancel.Click += new System.EventHandler(this.btn_cancel_Click);
            // 
            // tBox_name
            // 
            this.tBox_name.BackColor = System.Drawing.SystemColors.Control;
            // 
            // 
            // 
            this.tBox_name.Border.Class = "TextBoxBorder";
            this.tBox_name.Border.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.tBox_name.Location = new System.Drawing.Point(362, 6);
            this.tBox_name.Name = "tBox_name";
            this.tBox_name.ReadOnly = true;
            this.tBox_name.Size = new System.Drawing.Size(100, 21);
            this.tBox_name.TabIndex = 28;
            this.tBox_name.Enter += new System.EventHandler(this.tBox_name_Enter);
            // 
            // btn
            // 
            this.btn.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btn.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.btn.Location = new System.Drawing.Point(375, 449);
            this.btn.Name = "btn";
            this.btn.Size = new System.Drawing.Size(87, 23);
            this.btn.Style = DevComponents.DotNetBar.eDotNetBarStyle.StyleManagerControlled;
            this.btn.TabIndex = 29;
            this.btn.Text = "开机";
            this.btn.Click += new System.EventHandler(this.btn_Click);
            // 
            // HostInfoForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(194)))), ((int)(((byte)(217)))), ((int)(((byte)(247)))));
            this.ClientSize = new System.Drawing.Size(689, 482);
            this.Controls.Add(this.btn);
            this.Controls.Add(this.tBox_name);
            this.Controls.Add(this.btn_cancel);
            this.Controls.Add(this.btn_THchange);
            this.Controls.Add(this.btn_CloseProSer);
            this.Controls.Add(this.btn_StartProSer);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.listView_Service);
            this.Controls.Add(this.listView_Process);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.tBox_MEMValue);
            this.Controls.Add(this.tBox_DISKValue);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.tBox_IP);
            this.Controls.Add(this.tBox_CPUValue);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.tBox_MEMTHValue);
            this.Controls.Add(this.tBox_DISKTHValue);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.tBox_CPUTHValue);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.Name = "HostInfoForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "主机信息";
            this.Activated += new System.EventHandler(this.HostInfoForm_Activated);
            this.Load += new System.EventHandler(this.HostInfo_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox tBox_MEMTHValue;
        private System.Windows.Forms.TextBox tBox_DISKTHValue;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox tBox_CPUTHValue;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox tBox_MEMValue;
        private System.Windows.Forms.TextBox tBox_DISKValue;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox tBox_CPUValue;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TextBox tBox_IP;
        private System.Windows.Forms.ListView listView_Process;
        private System.Windows.Forms.ListView listView_Service;
        private System.Windows.Forms.ColumnHeader chprocessname;
        private System.Windows.Forms.ColumnHeader chprocessstate;
        private System.Windows.Forms.ColumnHeader chprocessCPU;
        private System.Windows.Forms.ColumnHeader chprocessmemory;
        private System.Windows.Forms.ColumnHeader chservicename;
        private System.Windows.Forms.ColumnHeader chservicestate;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.ProgressBar pgBar_OutTraffic;
        private System.Windows.Forms.ProgressBar pgBar_InTraffic;
        private System.Windows.Forms.ProgressBar pgBar_TotalTraffic;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.TextBox tBox_TotalTraffic;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.TextBox tBox_OutgoingTraffic;
        private System.Windows.Forms.TextBox tBox_IncomingTraffic;
        private DevComponents.DotNetBar.ButtonX btn_StartProSer;
        private DevComponents.DotNetBar.ButtonX btn_CloseProSer;
        private DevComponents.DotNetBar.ButtonX btn_THchange;
        private DevComponents.DotNetBar.ButtonX btn_cancel;
        private DevComponents.DotNetBar.Controls.TextBoxX tBox_name;
        private DevComponents.DotNetBar.ButtonX btn;
    }
}