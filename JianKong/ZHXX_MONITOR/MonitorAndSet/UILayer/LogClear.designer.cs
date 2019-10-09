namespace MonitorAndSet
{
    partial class LogClear
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(LogClear));
            this.labelX1 = new DevComponents.DotNetBar.LabelX();
            this.labelX2 = new DevComponents.DotNetBar.LabelX();
            this.btn_AllClear = new DevComponents.DotNetBar.ButtonX();
            this.btn_LClear = new DevComponents.DotNetBar.ButtonX();
            this.btn_LClearCancle = new DevComponents.DotNetBar.ButtonX();
            this.label2 = new System.Windows.Forms.Label();
            this.dateTimePicker1 = new DevComponents.Editors.DateTimeAdv.DateTimeInput();
            this.dateTimePicker2 = new DevComponents.Editors.DateTimeAdv.DateTimeInput();
            ((System.ComponentModel.ISupportInitialize)(this.dateTimePicker1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.dateTimePicker2)).BeginInit();
            this.SuspendLayout();
            // 
            // labelX1
            // 
            // 
            // 
            // 
            this.labelX1.BackgroundStyle.Class = "";
            this.labelX1.BackgroundStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.labelX1.Location = new System.Drawing.Point(12, 31);
            this.labelX1.Name = "labelX1";
            this.labelX1.Size = new System.Drawing.Size(19, 23);
            this.labelX1.TabIndex = 7;
            this.labelX1.Text = "从";
            // 
            // labelX2
            // 
            // 
            // 
            // 
            this.labelX2.BackgroundStyle.Class = "";
            this.labelX2.BackgroundStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.labelX2.Location = new System.Drawing.Point(144, 31);
            this.labelX2.Name = "labelX2";
            this.labelX2.Size = new System.Drawing.Size(19, 23);
            this.labelX2.TabIndex = 8;
            this.labelX2.Text = "至";
            // 
            // btn_AllClear
            // 
            this.btn_AllClear.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btn_AllClear.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.btn_AllClear.Location = new System.Drawing.Point(35, 65);
            this.btn_AllClear.Name = "btn_AllClear";
            this.btn_AllClear.Size = new System.Drawing.Size(66, 23);
            this.btn_AllClear.TabIndex = 9;
            this.btn_AllClear.Text = "全部清空";
            this.btn_AllClear.Click += new System.EventHandler(this.btn_AllClear_Click);
            // 
            // btn_LClear
            // 
            this.btn_LClear.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btn_LClear.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.btn_LClear.Location = new System.Drawing.Point(122, 65);
            this.btn_LClear.Name = "btn_LClear";
            this.btn_LClear.Size = new System.Drawing.Size(51, 23);
            this.btn_LClear.TabIndex = 10;
            this.btn_LClear.Text = "清空";
            this.btn_LClear.Click += new System.EventHandler(this.btn_LClear_Click);
            // 
            // btn_LClearCancle
            // 
            this.btn_LClearCancle.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btn_LClearCancle.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.btn_LClearCancle.Location = new System.Drawing.Point(194, 65);
            this.btn_LClearCancle.Name = "btn_LClearCancle";
            this.btn_LClearCancle.Size = new System.Drawing.Size(51, 23);
            this.btn_LClearCancle.TabIndex = 11;
            this.btn_LClearCancle.Text = "取消";
            this.btn_LClearCancle.Click += new System.EventHandler(this.btn_LClearCancle_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 10);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(89, 12);
            this.label2.TabIndex = 12;
            this.label2.Text = "日志清空范围：";
            // 
            // dateTimePicker1
            // 
            // 
            // 
            // 
            this.dateTimePicker1.BackgroundStyle.Class = "DateTimeInputBackground";
            this.dateTimePicker1.BackgroundStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.dateTimePicker1.ButtonDropDown.Shortcut = DevComponents.DotNetBar.eShortcut.AltDown;
            this.dateTimePicker1.ButtonDropDown.Visible = true;
            this.dateTimePicker1.Location = new System.Drawing.Point(28, 31);
            // 
            // 
            // 
            this.dateTimePicker1.MonthCalendar.AnnuallyMarkedDates = new System.DateTime[0];
            // 
            // 
            // 
            this.dateTimePicker1.MonthCalendar.BackgroundStyle.BackColor = System.Drawing.SystemColors.Window;
            this.dateTimePicker1.MonthCalendar.BackgroundStyle.Class = "";
            this.dateTimePicker1.MonthCalendar.BackgroundStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.dateTimePicker1.MonthCalendar.ClearButtonVisible = true;
            // 
            // 
            // 
            this.dateTimePicker1.MonthCalendar.CommandsBackgroundStyle.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.dateTimePicker1.MonthCalendar.CommandsBackgroundStyle.BackColorGradientAngle = 90;
            this.dateTimePicker1.MonthCalendar.CommandsBackgroundStyle.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.dateTimePicker1.MonthCalendar.CommandsBackgroundStyle.BorderTop = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.dateTimePicker1.MonthCalendar.CommandsBackgroundStyle.BorderTopColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder;
            this.dateTimePicker1.MonthCalendar.CommandsBackgroundStyle.BorderTopWidth = 1;
            this.dateTimePicker1.MonthCalendar.CommandsBackgroundStyle.Class = "";
            this.dateTimePicker1.MonthCalendar.CommandsBackgroundStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.dateTimePicker1.MonthCalendar.DisplayMonth = new System.DateTime(2011, 12, 1, 0, 0, 0, 0);
            this.dateTimePicker1.MonthCalendar.MarkedDates = new System.DateTime[0];
            this.dateTimePicker1.MonthCalendar.MonthlyMarkedDates = new System.DateTime[0];
            // 
            // 
            // 
            this.dateTimePicker1.MonthCalendar.NavigationBackgroundStyle.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.dateTimePicker1.MonthCalendar.NavigationBackgroundStyle.BackColorGradientAngle = 90;
            this.dateTimePicker1.MonthCalendar.NavigationBackgroundStyle.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.dateTimePicker1.MonthCalendar.NavigationBackgroundStyle.Class = "";
            this.dateTimePicker1.MonthCalendar.NavigationBackgroundStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.dateTimePicker1.MonthCalendar.TodayButtonVisible = true;
            this.dateTimePicker1.MonthCalendar.WeeklyMarkedDays = new System.DayOfWeek[0];
            this.dateTimePicker1.Name = "dateTimePicker1";
            this.dateTimePicker1.Size = new System.Drawing.Size(110, 21);
            this.dateTimePicker1.Style = DevComponents.DotNetBar.eDotNetBarStyle.StyleManagerControlled;
            this.dateTimePicker1.TabIndex = 17;
            // 
            // dateTimePicker2
            // 
            // 
            // 
            // 
            this.dateTimePicker2.BackgroundStyle.Class = "DateTimeInputBackground";
            this.dateTimePicker2.BackgroundStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.dateTimePicker2.ButtonDropDown.Shortcut = DevComponents.DotNetBar.eShortcut.AltDown;
            this.dateTimePicker2.ButtonDropDown.Visible = true;
            this.dateTimePicker2.Location = new System.Drawing.Point(167, 31);
            // 
            // 
            // 
            this.dateTimePicker2.MonthCalendar.AnnuallyMarkedDates = new System.DateTime[0];
            // 
            // 
            // 
            this.dateTimePicker2.MonthCalendar.BackgroundStyle.BackColor = System.Drawing.SystemColors.Window;
            this.dateTimePicker2.MonthCalendar.BackgroundStyle.Class = "";
            this.dateTimePicker2.MonthCalendar.BackgroundStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.dateTimePicker2.MonthCalendar.ClearButtonVisible = true;
            // 
            // 
            // 
            this.dateTimePicker2.MonthCalendar.CommandsBackgroundStyle.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.dateTimePicker2.MonthCalendar.CommandsBackgroundStyle.BackColorGradientAngle = 90;
            this.dateTimePicker2.MonthCalendar.CommandsBackgroundStyle.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.dateTimePicker2.MonthCalendar.CommandsBackgroundStyle.BorderTop = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.dateTimePicker2.MonthCalendar.CommandsBackgroundStyle.BorderTopColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder;
            this.dateTimePicker2.MonthCalendar.CommandsBackgroundStyle.BorderTopWidth = 1;
            this.dateTimePicker2.MonthCalendar.CommandsBackgroundStyle.Class = "";
            this.dateTimePicker2.MonthCalendar.CommandsBackgroundStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.dateTimePicker2.MonthCalendar.DisplayMonth = new System.DateTime(2011, 12, 1, 0, 0, 0, 0);
            this.dateTimePicker2.MonthCalendar.MarkedDates = new System.DateTime[0];
            this.dateTimePicker2.MonthCalendar.MonthlyMarkedDates = new System.DateTime[0];
            // 
            // 
            // 
            this.dateTimePicker2.MonthCalendar.NavigationBackgroundStyle.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.dateTimePicker2.MonthCalendar.NavigationBackgroundStyle.BackColorGradientAngle = 90;
            this.dateTimePicker2.MonthCalendar.NavigationBackgroundStyle.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.dateTimePicker2.MonthCalendar.NavigationBackgroundStyle.Class = "";
            this.dateTimePicker2.MonthCalendar.NavigationBackgroundStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.dateTimePicker2.MonthCalendar.TodayButtonVisible = true;
            this.dateTimePicker2.MonthCalendar.WeeklyMarkedDays = new System.DayOfWeek[0];
            this.dateTimePicker2.Name = "dateTimePicker2";
            this.dateTimePicker2.Size = new System.Drawing.Size(110, 21);
            this.dateTimePicker2.Style = DevComponents.DotNetBar.eDotNetBarStyle.StyleManagerControlled;
            this.dateTimePicker2.TabIndex = 18;
            // 
            // LogClear
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(194)))), ((int)(((byte)(217)))), ((int)(((byte)(247)))));
            this.ClientSize = new System.Drawing.Size(289, 105);
            this.Controls.Add(this.dateTimePicker2);
            this.Controls.Add(this.dateTimePicker1);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.btn_LClearCancle);
            this.Controls.Add(this.btn_LClear);
            this.Controls.Add(this.btn_AllClear);
            this.Controls.Add(this.labelX2);
            this.Controls.Add(this.labelX1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.Name = "LogClear";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "日志清空";
            this.Load += new System.EventHandler(this.LogClear_Load);
            ((System.ComponentModel.ISupportInitialize)(this.dateTimePicker1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.dateTimePicker2)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private DevComponents.DotNetBar.LabelX labelX1;
        private DevComponents.DotNetBar.LabelX labelX2;
        private DevComponents.DotNetBar.ButtonX btn_AllClear;
        private DevComponents.DotNetBar.ButtonX btn_LClear;
        private DevComponents.DotNetBar.ButtonX btn_LClearCancle;
        private System.Windows.Forms.Label label2;
        private DevComponents.Editors.DateTimeAdv.DateTimeInput dateTimePicker1;
        private DevComponents.Editors.DateTimeAdv.DateTimeInput dateTimePicker2;
    }
}