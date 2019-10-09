namespace MonitorAndSet
{
    partial class MainForm
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
            try
            {
                //关闭线程
                recvInfoFromClient.Abort();
                ping_host.Abort();
                receive_data.Abort();
                //清除资源
                server.Close();
            }
            catch
            {
            };
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.tabControl_Menu = new DevComponents.DotNetBar.TabControl();
            this.tabControlPanel_Menu = new DevComponents.DotNetBar.TabControlPanel();
            this.ribbonBar_voice = new DevComponents.DotNetBar.RibbonBar();
            this.btnItem_voice = new DevComponents.DotNetBar.ButtonItem();
            this.ribbonBar_Exit = new DevComponents.DotNetBar.RibbonBar();
            this.btnItem_Exit = new DevComponents.DotNetBar.ButtonItem();
            this.ribbonBar_Interface = new DevComponents.DotNetBar.RibbonBar();
            this.btn_start = new DevComponents.DotNetBar.ButtonItem();
            this.btn_shuntdown = new DevComponents.DotNetBar.ButtonItem();
            this.ribbonBar_Logoperate = new DevComponents.DotNetBar.RibbonBar();
            this.btnItem_History = new DevComponents.DotNetBar.ButtonItem();
            this.btnItem_LogDelete = new DevComponents.DotNetBar.ButtonItem();
            this.ribbonBar_Paraset = new DevComponents.DotNetBar.RibbonBar();
            this.btnItem_AlarmPara = new DevComponents.DotNetBar.ButtonItem();
            this.btnItem_LogPara = new DevComponents.DotNetBar.ButtonItem();
            this.tabItem_Menu = new DevComponents.DotNetBar.TabItem(this.components);
            this.statusStrip_Bottom = new System.Windows.Forms.StatusStrip();
            this.label_tipleft = new System.Windows.Forms.ToolStripStatusLabel();
            this.expandPanel_Left = new DevComponents.DotNetBar.ExpandablePanel();
            this.tableLayoutPanel_LeftSys = new System.Windows.Forms.TableLayoutPanel();
            this.groupPanel_AlarmInfo = new DevComponents.DotNetBar.Controls.GroupPanel();
            this.listViewAlarmInfo = new DevComponents.DotNetBar.Controls.ListViewEx();
            this.chAlarmContent = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.chAlarmTime = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.chAllID = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.groupPanel_SystemInfo = new DevComponents.DotNetBar.Controls.GroupPanel();
            this.listViewSysInfo = new DevComponents.DotNetBar.Controls.ListViewEx();
            this.chSysName = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.chSysContent = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.colorDialog1 = new System.Windows.Forms.ColorDialog();
            this.chOutputContent = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.chOutputTime = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.mytimer = new System.Windows.Forms.Timer(this.components);
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.contextMenuStrip_Server = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.toolStripMenuItem_Server = new System.Windows.Forms.ToolStripMenuItem();
            this.panelEx1 = new DevComponents.DotNetBar.PanelEx();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.menustrip_item_clearAlarm = new System.Windows.Forms.ToolStripMenuItem();
            ((System.ComponentModel.ISupportInitialize)(this.tabControl_Menu)).BeginInit();
            this.tabControl_Menu.SuspendLayout();
            this.tabControlPanel_Menu.SuspendLayout();
            this.statusStrip_Bottom.SuspendLayout();
            this.expandPanel_Left.SuspendLayout();
            this.tableLayoutPanel_LeftSys.SuspendLayout();
            this.groupPanel_AlarmInfo.SuspendLayout();
            this.groupPanel_SystemInfo.SuspendLayout();
            this.contextMenuStrip_Server.SuspendLayout();
            this.contextMenuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabControl_Menu
            // 
            this.tabControl_Menu.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(194)))), ((int)(((byte)(217)))), ((int)(((byte)(247)))));
            this.tabControl_Menu.CanReorderTabs = true;
            this.tabControl_Menu.Controls.Add(this.tabControlPanel_Menu);
            this.tabControl_Menu.Dock = System.Windows.Forms.DockStyle.Top;
            this.tabControl_Menu.Location = new System.Drawing.Point(0, 0);
            this.tabControl_Menu.Name = "tabControl_Menu";
            this.tabControl_Menu.SelectedTabFont = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Bold);
            this.tabControl_Menu.SelectedTabIndex = 0;
            this.tabControl_Menu.Size = new System.Drawing.Size(1133, 104);
            this.tabControl_Menu.TabIndex = 0;
            this.tabControl_Menu.TabLayoutType = DevComponents.DotNetBar.eTabLayoutType.FixedWithNavigationBox;
            this.tabControl_Menu.Tabs.Add(this.tabItem_Menu);
            this.tabControl_Menu.Text = "tabControlMenu";
            // 
            // tabControlPanel_Menu
            // 
            this.tabControlPanel_Menu.Controls.Add(this.ribbonBar_voice);
            this.tabControlPanel_Menu.Controls.Add(this.ribbonBar_Exit);
            this.tabControlPanel_Menu.Controls.Add(this.ribbonBar_Interface);
            this.tabControlPanel_Menu.Controls.Add(this.ribbonBar_Logoperate);
            this.tabControlPanel_Menu.Controls.Add(this.ribbonBar_Paraset);
            this.tabControlPanel_Menu.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControlPanel_Menu.Location = new System.Drawing.Point(0, 26);
            this.tabControlPanel_Menu.Name = "tabControlPanel_Menu";
            this.tabControlPanel_Menu.Padding = new System.Windows.Forms.Padding(1, 1, 10, 1);
            this.tabControlPanel_Menu.Size = new System.Drawing.Size(1133, 78);
            this.tabControlPanel_Menu.Style.BackColor1.Color = System.Drawing.Color.FromArgb(((int)(((byte)(142)))), ((int)(((byte)(179)))), ((int)(((byte)(231)))));
            this.tabControlPanel_Menu.Style.BackColor2.Color = System.Drawing.Color.FromArgb(((int)(((byte)(223)))), ((int)(((byte)(237)))), ((int)(((byte)(254)))));
            this.tabControlPanel_Menu.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.tabControlPanel_Menu.Style.BorderColor.Color = System.Drawing.Color.FromArgb(((int)(((byte)(59)))), ((int)(((byte)(97)))), ((int)(((byte)(156)))));
            this.tabControlPanel_Menu.Style.BorderSide = ((DevComponents.DotNetBar.eBorderSide)(((DevComponents.DotNetBar.eBorderSide.Left | DevComponents.DotNetBar.eBorderSide.Right)
                        | DevComponents.DotNetBar.eBorderSide.Bottom)));
            this.tabControlPanel_Menu.Style.GradientAngle = 90;
            this.tabControlPanel_Menu.TabIndex = 1;
            this.tabControlPanel_Menu.TabItem = this.tabItem_Menu;
            // 
            // ribbonBar_voice
            // 
            this.ribbonBar_voice.AutoOverflowEnabled = true;
            // 
            // 
            // 
            this.ribbonBar_voice.BackgroundMouseOverStyle.Class = "";
            this.ribbonBar_voice.BackgroundMouseOverStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            // 
            // 
            // 
            this.ribbonBar_voice.BackgroundStyle.Class = "";
            this.ribbonBar_voice.BackgroundStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.ribbonBar_voice.ContainerControlProcessDialogKey = true;
            this.ribbonBar_voice.Dock = System.Windows.Forms.DockStyle.Left;
            this.ribbonBar_voice.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.btnItem_voice});
            this.ribbonBar_voice.Location = new System.Drawing.Point(406, 1);
            this.ribbonBar_voice.Name = "ribbonBar_voice";
            this.ribbonBar_voice.Size = new System.Drawing.Size(73, 76);
            this.ribbonBar_voice.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.ribbonBar_voice.TabIndex = 6;
            this.ribbonBar_voice.Text = "声音控制";
            // 
            // 
            // 
            this.ribbonBar_voice.TitleStyle.Class = "";
            this.ribbonBar_voice.TitleStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            // 
            // 
            // 
            this.ribbonBar_voice.TitleStyleMouseOver.Class = "";
            this.ribbonBar_voice.TitleStyleMouseOver.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            // 
            // btnItem_voice
            // 
            this.btnItem_voice.Image = global::MonitorAndSet.Properties.Resources.透明3_fw;
            this.btnItem_voice.ImagePosition = DevComponents.DotNetBar.eImagePosition.Top;
            this.btnItem_voice.Name = "btnItem_voice";
            this.btnItem_voice.SubItemsExpandWidth = 14;
            this.btnItem_voice.Text = "关闭声音";
            this.btnItem_voice.Click += new System.EventHandler(this.btnItem_voice_Click);
            // 
            // ribbonBar_Exit
            // 
            this.ribbonBar_Exit.AutoOverflowEnabled = true;
            // 
            // 
            // 
            this.ribbonBar_Exit.BackgroundMouseOverStyle.Class = "";
            this.ribbonBar_Exit.BackgroundMouseOverStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            // 
            // 
            // 
            this.ribbonBar_Exit.BackgroundStyle.Class = "";
            this.ribbonBar_Exit.BackgroundStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.ribbonBar_Exit.ContainerControlProcessDialogKey = true;
            this.ribbonBar_Exit.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.btnItem_Exit});
            this.ribbonBar_Exit.Location = new System.Drawing.Point(478, 1);
            this.ribbonBar_Exit.Name = "ribbonBar_Exit";
            this.ribbonBar_Exit.Size = new System.Drawing.Size(73, 76);
            this.ribbonBar_Exit.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.ribbonBar_Exit.TabIndex = 5;
            this.ribbonBar_Exit.Text = "系统操作";
            // 
            // 
            // 
            this.ribbonBar_Exit.TitleStyle.Class = "";
            this.ribbonBar_Exit.TitleStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            // 
            // 
            // 
            this.ribbonBar_Exit.TitleStyleMouseOver.Class = "";
            this.ribbonBar_Exit.TitleStyleMouseOver.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            // 
            // btnItem_Exit
            // 
            this.btnItem_Exit.Image = ((System.Drawing.Image)(resources.GetObject("btnItem_Exit.Image")));
            this.btnItem_Exit.ImagePosition = DevComponents.DotNetBar.eImagePosition.Top;
            this.btnItem_Exit.Name = "btnItem_Exit";
            this.btnItem_Exit.SubItemsExpandWidth = 14;
            this.btnItem_Exit.Text = "退出系统";
            this.btnItem_Exit.Click += new System.EventHandler(this.btnItem_Exit_Click);
            this.btnItem_Exit.MouseEnter += new System.EventHandler(this.btnItem_Exit_MouseEnter);
            this.btnItem_Exit.MouseLeave += new System.EventHandler(this.btnItem_Exit_MouseLeave);
            // 
            // ribbonBar_Interface
            // 
            this.ribbonBar_Interface.AutoOverflowEnabled = true;
            // 
            // 
            // 
            this.ribbonBar_Interface.BackgroundMouseOverStyle.Class = "";
            this.ribbonBar_Interface.BackgroundMouseOverStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            // 
            // 
            // 
            this.ribbonBar_Interface.BackgroundStyle.Class = "";
            this.ribbonBar_Interface.BackgroundStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.ribbonBar_Interface.ContainerControlProcessDialogKey = true;
            this.ribbonBar_Interface.Dock = System.Windows.Forms.DockStyle.Left;
            this.ribbonBar_Interface.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.btn_start,
            this.btn_shuntdown});
            this.ribbonBar_Interface.Location = new System.Drawing.Point(271, 1);
            this.ribbonBar_Interface.Name = "ribbonBar_Interface";
            this.ribbonBar_Interface.Size = new System.Drawing.Size(135, 76);
            this.ribbonBar_Interface.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.ribbonBar_Interface.TabIndex = 3;
            this.ribbonBar_Interface.Text = "远程控制";
            // 
            // 
            // 
            this.ribbonBar_Interface.TitleStyle.Class = "";
            this.ribbonBar_Interface.TitleStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            // 
            // 
            // 
            this.ribbonBar_Interface.TitleStyleMouseOver.Class = "";
            this.ribbonBar_Interface.TitleStyleMouseOver.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            // 
            // btn_start
            // 
            this.btn_start.Image = global::MonitorAndSet.Properties.Resources.control_play_32;
            this.btn_start.ImagePosition = DevComponents.DotNetBar.eImagePosition.Top;
            this.btn_start.Name = "btn_start";
            this.btn_start.SubItemsExpandWidth = 14;
            this.btn_start.Text = "远程开机";
            this.btn_start.Click += new System.EventHandler(this.btn_start_Click);
            this.btn_start.MouseEnter += new System.EventHandler(this.btn_start_MouseEnter);
            this.btn_start.MouseLeave += new System.EventHandler(this.btn_start_MouseLeave);
            // 
            // btn_shuntdown
            // 
            this.btn_shuntdown.Image = global::MonitorAndSet.Properties.Resources.control_stop_32;
            this.btn_shuntdown.ImagePosition = DevComponents.DotNetBar.eImagePosition.Top;
            this.btn_shuntdown.Name = "btn_shuntdown";
            this.btn_shuntdown.SubItemsExpandWidth = 14;
            this.btn_shuntdown.Text = "远程关机";
            this.btn_shuntdown.Click += new System.EventHandler(this.btnItem_ShutDown_Click);
            this.btn_shuntdown.MouseEnter += new System.EventHandler(this.btnItem_ShutDown_MouseEnter);
            this.btn_shuntdown.MouseLeave += new System.EventHandler(this.btnItem_ShutDown_MouseLeave);
            // 
            // ribbonBar_Logoperate
            // 
            this.ribbonBar_Logoperate.AutoOverflowEnabled = true;
            // 
            // 
            // 
            this.ribbonBar_Logoperate.BackgroundMouseOverStyle.Class = "";
            this.ribbonBar_Logoperate.BackgroundMouseOverStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            // 
            // 
            // 
            this.ribbonBar_Logoperate.BackgroundStyle.Class = "";
            this.ribbonBar_Logoperate.BackgroundStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.ribbonBar_Logoperate.ContainerControlProcessDialogKey = true;
            this.ribbonBar_Logoperate.Dock = System.Windows.Forms.DockStyle.Left;
            this.ribbonBar_Logoperate.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.btnItem_History,
            this.btnItem_LogDelete});
            this.ribbonBar_Logoperate.Location = new System.Drawing.Point(136, 1);
            this.ribbonBar_Logoperate.Name = "ribbonBar_Logoperate";
            this.ribbonBar_Logoperate.Size = new System.Drawing.Size(135, 76);
            this.ribbonBar_Logoperate.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.ribbonBar_Logoperate.TabIndex = 4;
            this.ribbonBar_Logoperate.Text = "日志操作";
            // 
            // 
            // 
            this.ribbonBar_Logoperate.TitleStyle.Class = "";
            this.ribbonBar_Logoperate.TitleStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            // 
            // 
            // 
            this.ribbonBar_Logoperate.TitleStyleMouseOver.Class = "";
            this.ribbonBar_Logoperate.TitleStyleMouseOver.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            // 
            // btnItem_History
            // 
            this.btnItem_History.Image = ((System.Drawing.Image)(resources.GetObject("btnItem_History.Image")));
            this.btnItem_History.ImagePosition = DevComponents.DotNetBar.eImagePosition.Top;
            this.btnItem_History.Name = "btnItem_History";
            this.btnItem_History.SubItemsExpandWidth = 14;
            this.btnItem_History.Text = "历史信息";
            this.btnItem_History.Click += new System.EventHandler(this.btnItem_History_Click);
            this.btnItem_History.MouseEnter += new System.EventHandler(this.btnItem_History_MouseEnter);
            this.btnItem_History.MouseLeave += new System.EventHandler(this.btnItem_History_MouseLeave);
            // 
            // btnItem_LogDelete
            // 
            this.btnItem_LogDelete.Image = ((System.Drawing.Image)(resources.GetObject("btnItem_LogDelete.Image")));
            this.btnItem_LogDelete.ImagePosition = DevComponents.DotNetBar.eImagePosition.Top;
            this.btnItem_LogDelete.Name = "btnItem_LogDelete";
            this.btnItem_LogDelete.SubItemsExpandWidth = 14;
            this.btnItem_LogDelete.Text = "日志清空";
            this.btnItem_LogDelete.Click += new System.EventHandler(this.btnItem_LogDelete_Click);
            this.btnItem_LogDelete.MouseEnter += new System.EventHandler(this.btnItem_LogDelete_MouseEnter);
            this.btnItem_LogDelete.MouseLeave += new System.EventHandler(this.btnItem_LogDelete_MouseLeave);
            // 
            // ribbonBar_Paraset
            // 
            this.ribbonBar_Paraset.AutoOverflowEnabled = true;
            // 
            // 
            // 
            this.ribbonBar_Paraset.BackgroundMouseOverStyle.Class = "";
            this.ribbonBar_Paraset.BackgroundMouseOverStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            // 
            // 
            // 
            this.ribbonBar_Paraset.BackgroundStyle.Class = "";
            this.ribbonBar_Paraset.BackgroundStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.ribbonBar_Paraset.ContainerControlProcessDialogKey = true;
            this.ribbonBar_Paraset.Dock = System.Windows.Forms.DockStyle.Left;
            this.ribbonBar_Paraset.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.btnItem_AlarmPara,
            this.btnItem_LogPara});
            this.ribbonBar_Paraset.Location = new System.Drawing.Point(1, 1);
            this.ribbonBar_Paraset.Name = "ribbonBar_Paraset";
            this.ribbonBar_Paraset.Size = new System.Drawing.Size(135, 76);
            this.ribbonBar_Paraset.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.ribbonBar_Paraset.TabIndex = 2;
            this.ribbonBar_Paraset.Text = "参数设置";
            // 
            // 
            // 
            this.ribbonBar_Paraset.TitleStyle.Class = "";
            this.ribbonBar_Paraset.TitleStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            // 
            // 
            // 
            this.ribbonBar_Paraset.TitleStyleMouseOver.Class = "";
            this.ribbonBar_Paraset.TitleStyleMouseOver.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            // 
            // btnItem_AlarmPara
            // 
            this.btnItem_AlarmPara.Image = ((System.Drawing.Image)(resources.GetObject("btnItem_AlarmPara.Image")));
            this.btnItem_AlarmPara.ImagePosition = DevComponents.DotNetBar.eImagePosition.Top;
            this.btnItem_AlarmPara.Name = "btnItem_AlarmPara";
            this.btnItem_AlarmPara.SubItemsExpandWidth = 14;
            this.btnItem_AlarmPara.Text = "告警参数";
            this.btnItem_AlarmPara.Click += new System.EventHandler(this.btnItem_AlarmPara_Click);
            this.btnItem_AlarmPara.MouseEnter += new System.EventHandler(this.btnItem_AlarmPara_MouseEnter);
            this.btnItem_AlarmPara.MouseLeave += new System.EventHandler(this.btnItem_AlarmPara_MouseLeave);
            // 
            // btnItem_LogPara
            // 
            this.btnItem_LogPara.Image = ((System.Drawing.Image)(resources.GetObject("btnItem_LogPara.Image")));
            this.btnItem_LogPara.ImagePosition = DevComponents.DotNetBar.eImagePosition.Top;
            this.btnItem_LogPara.Name = "btnItem_LogPara";
            this.btnItem_LogPara.SubItemsExpandWidth = 14;
            this.btnItem_LogPara.Text = "日志参数";
            this.btnItem_LogPara.Click += new System.EventHandler(this.btnItem_LogPara_Click);
            this.btnItem_LogPara.MouseEnter += new System.EventHandler(this.btnItem_LogPara_MouseEnter);
            this.btnItem_LogPara.MouseLeave += new System.EventHandler(this.btnItem_LogPara_MouseLeave);
            // 
            // tabItem_Menu
            // 
            this.tabItem_Menu.AttachedControl = this.tabControlPanel_Menu;
            this.tabItem_Menu.Name = "tabItem_Menu";
            this.tabItem_Menu.Text = "系统菜单";
            // 
            // statusStrip_Bottom
            // 
            this.statusStrip_Bottom.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.label_tipleft});
            this.statusStrip_Bottom.Location = new System.Drawing.Point(0, 514);
            this.statusStrip_Bottom.Name = "statusStrip_Bottom";
            this.statusStrip_Bottom.RenderMode = System.Windows.Forms.ToolStripRenderMode.Professional;
            this.statusStrip_Bottom.Size = new System.Drawing.Size(1133, 22);
            this.statusStrip_Bottom.TabIndex = 0;
            // 
            // label_tipleft
            // 
            this.label_tipleft.Name = "label_tipleft";
            this.label_tipleft.Size = new System.Drawing.Size(0, 17);
            // 
            // expandPanel_Left
            // 
            this.expandPanel_Left.CanvasColor = System.Drawing.SystemColors.Control;
            this.expandPanel_Left.CollapseDirection = DevComponents.DotNetBar.eCollapseDirection.RightToLeft;
            this.expandPanel_Left.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.expandPanel_Left.Controls.Add(this.tableLayoutPanel_LeftSys);
            this.expandPanel_Left.Dock = System.Windows.Forms.DockStyle.Left;
            this.expandPanel_Left.Location = new System.Drawing.Point(0, 104);
            this.expandPanel_Left.Margin = new System.Windows.Forms.Padding(0);
            this.expandPanel_Left.Name = "expandPanel_Left";
            this.expandPanel_Left.Size = new System.Drawing.Size(203, 410);
            this.expandPanel_Left.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.expandPanel_Left.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.expandPanel_Left.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.expandPanel_Left.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.expandPanel_Left.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder;
            this.expandPanel_Left.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.expandPanel_Left.Style.GradientAngle = 90;
            this.expandPanel_Left.TabIndex = 1;
            this.expandPanel_Left.TitleStyle.Alignment = System.Drawing.StringAlignment.Center;
            this.expandPanel_Left.TitleStyle.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.expandPanel_Left.TitleStyle.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.expandPanel_Left.TitleStyle.Border = DevComponents.DotNetBar.eBorderType.RaisedInner;
            this.expandPanel_Left.TitleStyle.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.expandPanel_Left.TitleStyle.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.expandPanel_Left.TitleStyle.GradientAngle = 90;
            this.expandPanel_Left.TitleText = "信息列表";
            this.expandPanel_Left.ExpandedChanged += new DevComponents.DotNetBar.ExpandChangeEventHandler(this.expandPanel_Left_ExpandedChanged);
            // 
            // tableLayoutPanel_LeftSys
            // 
            this.tableLayoutPanel_LeftSys.ColumnCount = 1;
            this.tableLayoutPanel_LeftSys.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel_LeftSys.Controls.Add(this.groupPanel_AlarmInfo, 0, 1);
            this.tableLayoutPanel_LeftSys.Controls.Add(this.groupPanel_SystemInfo, 0, 0);
            this.tableLayoutPanel_LeftSys.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel_LeftSys.Location = new System.Drawing.Point(0, 26);
            this.tableLayoutPanel_LeftSys.Margin = new System.Windows.Forms.Padding(0);
            this.tableLayoutPanel_LeftSys.Name = "tableLayoutPanel_LeftSys";
            this.tableLayoutPanel_LeftSys.RowCount = 3;
            this.tableLayoutPanel_LeftSys.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 33.33333F));
            this.tableLayoutPanel_LeftSys.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 64.84375F));
            this.tableLayoutPanel_LeftSys.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 2.083333F));
            this.tableLayoutPanel_LeftSys.Size = new System.Drawing.Size(203, 384);
            this.tableLayoutPanel_LeftSys.TabIndex = 3;
            // 
            // groupPanel_AlarmInfo
            // 
            this.groupPanel_AlarmInfo.CanvasColor = System.Drawing.SystemColors.Control;
            this.groupPanel_AlarmInfo.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.groupPanel_AlarmInfo.Controls.Add(this.listViewAlarmInfo);
            this.groupPanel_AlarmInfo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupPanel_AlarmInfo.Location = new System.Drawing.Point(3, 130);
            this.groupPanel_AlarmInfo.Margin = new System.Windows.Forms.Padding(3, 3, 0, 3);
            this.groupPanel_AlarmInfo.Name = "groupPanel_AlarmInfo";
            this.groupPanel_AlarmInfo.Size = new System.Drawing.Size(200, 242);
            // 
            // 
            // 
            this.groupPanel_AlarmInfo.Style.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.groupPanel_AlarmInfo.Style.BackColorGradientAngle = 90;
            this.groupPanel_AlarmInfo.Style.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.groupPanel_AlarmInfo.Style.BorderBottom = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel_AlarmInfo.Style.BorderBottomWidth = 1;
            this.groupPanel_AlarmInfo.Style.BorderColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.groupPanel_AlarmInfo.Style.BorderLeft = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel_AlarmInfo.Style.BorderLeftWidth = 1;
            this.groupPanel_AlarmInfo.Style.BorderRight = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel_AlarmInfo.Style.BorderRightWidth = 1;
            this.groupPanel_AlarmInfo.Style.BorderTop = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel_AlarmInfo.Style.BorderTopWidth = 1;
            this.groupPanel_AlarmInfo.Style.Class = "";
            this.groupPanel_AlarmInfo.Style.CornerDiameter = 4;
            this.groupPanel_AlarmInfo.Style.CornerType = DevComponents.DotNetBar.eCornerType.Rounded;
            this.groupPanel_AlarmInfo.Style.TextAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Center;
            this.groupPanel_AlarmInfo.Style.TextColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.groupPanel_AlarmInfo.Style.TextLineAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Near;
            // 
            // 
            // 
            this.groupPanel_AlarmInfo.StyleMouseDown.Class = "";
            this.groupPanel_AlarmInfo.StyleMouseDown.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            // 
            // 
            // 
            this.groupPanel_AlarmInfo.StyleMouseOver.Class = "";
            this.groupPanel_AlarmInfo.StyleMouseOver.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.groupPanel_AlarmInfo.TabIndex = 1;
            this.groupPanel_AlarmInfo.Text = "告警信息";
            // 
            // listViewAlarmInfo
            // 
            // 
            // 
            // 
            this.listViewAlarmInfo.Border.Class = "ListViewBorder";
            this.listViewAlarmInfo.Border.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.listViewAlarmInfo.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.chAlarmContent,
            this.chAlarmTime,
            this.chAllID});
            this.listViewAlarmInfo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listViewAlarmInfo.Location = new System.Drawing.Point(0, 0);
            this.listViewAlarmInfo.Margin = new System.Windows.Forms.Padding(0);
            this.listViewAlarmInfo.MinimumSize = new System.Drawing.Size(100, 16);
            this.listViewAlarmInfo.Name = "listViewAlarmInfo";
            this.listViewAlarmInfo.Size = new System.Drawing.Size(194, 218);
            this.listViewAlarmInfo.TabIndex = 0;
            this.listViewAlarmInfo.UseCompatibleStateImageBehavior = false;
            this.listViewAlarmInfo.View = System.Windows.Forms.View.Details;
            this.listViewAlarmInfo.ColumnWidthChanged += new System.Windows.Forms.ColumnWidthChangedEventHandler(this.listViewAlarmInfo_ColumnWidthChanged);
            this.listViewAlarmInfo.DoubleClick += new System.EventHandler(this.listViewAlarmInfo_DoubleClick);
            // 
            // chAlarmContent
            // 
            this.chAlarmContent.Text = "内容";
            this.chAlarmContent.Width = 280;
            // 
            // chAlarmTime
            // 
            this.chAlarmTime.Text = "时间";
            this.chAlarmTime.Width = 200;
            // 
            // chAllID
            // 
            this.chAllID.Text = "ID";
            // 
            // groupPanel_SystemInfo
            // 
            this.groupPanel_SystemInfo.CanvasColor = System.Drawing.SystemColors.Control;
            this.groupPanel_SystemInfo.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.groupPanel_SystemInfo.Controls.Add(this.listViewSysInfo);
            this.groupPanel_SystemInfo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupPanel_SystemInfo.Location = new System.Drawing.Point(3, 3);
            this.groupPanel_SystemInfo.Name = "groupPanel_SystemInfo";
            this.groupPanel_SystemInfo.Size = new System.Drawing.Size(197, 121);
            // 
            // 
            // 
            this.groupPanel_SystemInfo.Style.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.groupPanel_SystemInfo.Style.BackColorGradientAngle = 90;
            this.groupPanel_SystemInfo.Style.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.groupPanel_SystemInfo.Style.BorderBottom = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel_SystemInfo.Style.BorderBottomWidth = 1;
            this.groupPanel_SystemInfo.Style.BorderColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.groupPanel_SystemInfo.Style.BorderLeft = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel_SystemInfo.Style.BorderLeftWidth = 1;
            this.groupPanel_SystemInfo.Style.BorderRight = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel_SystemInfo.Style.BorderRightWidth = 1;
            this.groupPanel_SystemInfo.Style.BorderTop = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel_SystemInfo.Style.BorderTopWidth = 1;
            this.groupPanel_SystemInfo.Style.Class = "";
            this.groupPanel_SystemInfo.Style.CornerDiameter = 4;
            this.groupPanel_SystemInfo.Style.CornerType = DevComponents.DotNetBar.eCornerType.Rounded;
            this.groupPanel_SystemInfo.Style.TextAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Center;
            this.groupPanel_SystemInfo.Style.TextColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.groupPanel_SystemInfo.Style.TextLineAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Near;
            // 
            // 
            // 
            this.groupPanel_SystemInfo.StyleMouseDown.Class = "";
            this.groupPanel_SystemInfo.StyleMouseDown.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            // 
            // 
            // 
            this.groupPanel_SystemInfo.StyleMouseOver.Class = "";
            this.groupPanel_SystemInfo.StyleMouseOver.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.groupPanel_SystemInfo.TabIndex = 0;
            this.groupPanel_SystemInfo.Text = "系统信息";
            // 
            // listViewSysInfo
            // 
            // 
            // 
            // 
            this.listViewSysInfo.Border.Class = "ListViewBorder";
            this.listViewSysInfo.Border.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.listViewSysInfo.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.chSysName,
            this.chSysContent});
            this.listViewSysInfo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listViewSysInfo.Location = new System.Drawing.Point(0, 0);
            this.listViewSysInfo.MinimumSize = new System.Drawing.Size(100, 16);
            this.listViewSysInfo.Name = "listViewSysInfo";
            this.listViewSysInfo.Size = new System.Drawing.Size(191, 97);
            this.listViewSysInfo.TabIndex = 0;
            this.listViewSysInfo.UseCompatibleStateImageBehavior = false;
            this.listViewSysInfo.View = System.Windows.Forms.View.Details;
            this.listViewSysInfo.ColumnWidthChanged += new System.Windows.Forms.ColumnWidthChangedEventHandler(this.listViewSysInfo_ColumnWidthChanged);
            // 
            // chSysName
            // 
            this.chSysName.Text = "名称";
            this.chSysName.Width = 160;
            // 
            // chSysContent
            // 
            this.chSysContent.Text = "内容";
            this.chSysContent.Width = 250;
            // 
            // chOutputContent
            // 
            this.chOutputContent.Text = "内容";
            this.chOutputContent.Width = 240;
            // 
            // chOutputTime
            // 
            this.chOutputTime.Text = "时间";
            this.chOutputTime.Width = 200;
            // 
            // mytimer
            // 
            this.mytimer.Interval = 1;
            this.mytimer.Tick += new System.EventHandler(this.mytimer_Tick);
            // 
            // timer1
            // 
            this.timer1.Interval = 1000;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // contextMenuStrip_Server
            // 
            this.contextMenuStrip_Server.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripMenuItem_Server});
            this.contextMenuStrip_Server.Name = "contextMenuStrip_Server";
            this.contextMenuStrip_Server.Size = new System.Drawing.Size(101, 26);
            // 
            // toolStripMenuItem_Server
            // 
            this.toolStripMenuItem_Server.BackColor = System.Drawing.Color.Transparent;
            this.toolStripMenuItem_Server.ForeColor = System.Drawing.SystemColors.ControlText;
            this.toolStripMenuItem_Server.Name = "toolStripMenuItem_Server";
            this.toolStripMenuItem_Server.Size = new System.Drawing.Size(100, 22);
            this.toolStripMenuItem_Server.Text = "登陆";
            this.toolStripMenuItem_Server.Click += new System.EventHandler(this.toolStripMenuItem_Server_Click);
            // 
            // panelEx1
            // 
            this.panelEx1.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx1.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.StyleManagerControlled;
            this.panelEx1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx1.Location = new System.Drawing.Point(203, 104);
            this.panelEx1.Name = "panelEx1";
            this.panelEx1.Size = new System.Drawing.Size(930, 410);
            this.panelEx1.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx1.Style.BackColor1.Color = System.Drawing.Color.White;
            this.panelEx1.Style.BackColor2.Color = System.Drawing.Color.White;
            this.panelEx1.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.panelEx1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.panelEx1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.panelEx1.Style.GradientAngle = 90;
            this.panelEx1.TabIndex = 2;
            this.panelEx1.Paint += new System.Windows.Forms.PaintEventHandler(this.Inner_Paint);
            this.panelEx1.MouseClick += new System.Windows.Forms.MouseEventHandler(this.tabControlPanel_Inner_MouseClick);
            this.panelEx1.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.InnerDevice_MouseDoubleClick);
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menustrip_item_clearAlarm});
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(153, 48);
            // 
            // menustrip_item_clearAlarm
            // 
            this.menustrip_item_clearAlarm.Name = "menustrip_item_clearAlarm";
            this.menustrip_item_clearAlarm.Size = new System.Drawing.Size(152, 22);
            this.menustrip_item_clearAlarm.Text = "清空列表";
            this.menustrip_item_clearAlarm.Click += new System.EventHandler(this.menustrip_item_clearAlarm_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1133, 536);
            this.Controls.Add(this.panelEx1);
            this.Controls.Add(this.expandPanel_Left);
            this.Controls.Add(this.statusStrip_Bottom);
            this.Controls.Add(this.tabControl_Menu);
            this.DoubleBuffered = true;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "MainForm";
            this.Text = "信息监控与参数配置软件";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            this.Load += new System.EventHandler(this.MainForm_Load);
            ((System.ComponentModel.ISupportInitialize)(this.tabControl_Menu)).EndInit();
            this.tabControl_Menu.ResumeLayout(false);
            this.tabControlPanel_Menu.ResumeLayout(false);
            this.statusStrip_Bottom.ResumeLayout(false);
            this.statusStrip_Bottom.PerformLayout();
            this.expandPanel_Left.ResumeLayout(false);
            this.tableLayoutPanel_LeftSys.ResumeLayout(false);
            this.groupPanel_AlarmInfo.ResumeLayout(false);
            this.groupPanel_SystemInfo.ResumeLayout(false);
            this.contextMenuStrip_Server.ResumeLayout(false);
            this.contextMenuStrip1.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private DevComponents.DotNetBar.TabControl tabControl_Menu;
        private DevComponents.DotNetBar.TabControlPanel tabControlPanel_Menu;
        private DevComponents.DotNetBar.RibbonBar ribbonBar_Logoperate;
        private DevComponents.DotNetBar.ButtonItem btnItem_History;
        private DevComponents.DotNetBar.ButtonItem btnItem_LogDelete;
        private DevComponents.DotNetBar.RibbonBar ribbonBar_Paraset;
        private DevComponents.DotNetBar.ButtonItem btnItem_AlarmPara;
        private DevComponents.DotNetBar.ButtonItem btnItem_LogPara;
        private DevComponents.DotNetBar.TabItem tabItem_Menu;
        private DevComponents.DotNetBar.ButtonItem btn_start;
        private DevComponents.DotNetBar.RibbonBar ribbonBar_Exit;
        private DevComponents.DotNetBar.ButtonItem btnItem_Exit;
        private DevComponents.DotNetBar.RibbonBar ribbonBar_Interface;
        private System.Windows.Forms.StatusStrip statusStrip_Bottom;
        private System.Windows.Forms.ToolStripStatusLabel label_tipleft;
        private DevComponents.DotNetBar.ExpandablePanel expandPanel_Left;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel_LeftSys;
        //public static DevComponents.DotNetBar.Controls.ListViewEx listViewOutputInfo;
        private System.Windows.Forms.ColumnHeader chOutputContent;
        private System.Windows.Forms.ColumnHeader chOutputTime;
        private DevComponents.DotNetBar.Controls.GroupPanel groupPanel_AlarmInfo;
        private DevComponents.DotNetBar.Controls.ListViewEx listViewAlarmInfo;
        private System.Windows.Forms.ColumnHeader chAlarmContent;
        private System.Windows.Forms.ColumnHeader chAlarmTime;
        private DevComponents.DotNetBar.Controls.GroupPanel groupPanel_SystemInfo;
        private DevComponents.DotNetBar.Controls.ListViewEx listViewSysInfo;
        private System.Windows.Forms.ColumnHeader chSysName;
        private System.Windows.Forms.ColumnHeader chSysContent;
        private System.Windows.Forms.Timer mytimer;
        private System.Windows.Forms.ColorDialog colorDialog1;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip_Server;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem_Server;
        private DevComponents.DotNetBar.PanelEx panelEx1;
        private DevComponents.DotNetBar.ButtonItem btn_shuntdown;
        private DevComponents.DotNetBar.RibbonBar ribbonBar_voice;
        private DevComponents.DotNetBar.ButtonItem btnItem_voice;
        private System.Windows.Forms.ColumnHeader chAllID;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem menustrip_item_clearAlarm;

    }
}

