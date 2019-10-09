namespace MonitorAndSet
{
    partial class AlarmInfo
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(AlarmInfo));
            this.listView1 = new System.Windows.Forms.ListView();
            this.chAlarmNO = new System.Windows.Forms.ColumnHeader();
            this.chAlarmState = new System.Windows.Forms.ColumnHeader();
            this.chAlarmDateTime = new System.Windows.Forms.ColumnHeader();
            this.chAlarmType = new System.Windows.Forms.ColumnHeader();
            this.chAlarmDetail = new System.Windows.Forms.ColumnHeader();
            this.btn_AInfoRefresh = new System.Windows.Forms.Button();
            this.btn_AInfoCancle = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.label_TotalCount = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label_UnsolveCount = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // listView1
            // 
            this.listView1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.listView1.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.chAlarmNO,
            this.chAlarmState,
            this.chAlarmDateTime,
            this.chAlarmType,
            this.chAlarmDetail});
            this.listView1.FullRowSelect = true;
            this.listView1.GridLines = true;
            this.listView1.Location = new System.Drawing.Point(7, 8);
            this.listView1.Name = "listView1";
            this.listView1.Size = new System.Drawing.Size(550, 175);
            this.listView1.TabIndex = 0;
            this.listView1.UseCompatibleStateImageBehavior = false;
            this.listView1.View = System.Windows.Forms.View.Details;
            this.listView1.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.listView1_MouseDoubleClick);
            // 
            // chAlarmNO
            // 
            this.chAlarmNO.Text = "序号";
            this.chAlarmNO.Width = 40;
            // 
            // chAlarmState
            // 
            this.chAlarmState.Text = "告警状态";
            this.chAlarmState.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.chAlarmState.Width = 70;
            // 
            // chAlarmDateTime
            // 
            this.chAlarmDateTime.Text = "告警时间";
            this.chAlarmDateTime.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.chAlarmDateTime.Width = 170;
            // 
            // chAlarmType
            // 
            this.chAlarmType.Text = "告警类型";
            this.chAlarmType.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.chAlarmType.Width = 70;
            // 
            // chAlarmDetail
            // 
            this.chAlarmDetail.Text = "告警描述";
            this.chAlarmDetail.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.chAlarmDetail.Width = 200;
            // 
            // btn_AInfoRefresh
            // 
            this.btn_AInfoRefresh.Location = new System.Drawing.Point(442, 184);
            this.btn_AInfoRefresh.Name = "btn_AInfoRefresh";
            this.btn_AInfoRefresh.Size = new System.Drawing.Size(50, 23);
            this.btn_AInfoRefresh.TabIndex = 1;
            this.btn_AInfoRefresh.Text = "刷新";
            this.btn_AInfoRefresh.UseVisualStyleBackColor = true;
            this.btn_AInfoRefresh.Click += new System.EventHandler(this.btn_AInfoRefresh_Click);
            // 
            // btn_AInfoCancle
            // 
            this.btn_AInfoCancle.Location = new System.Drawing.Point(507, 184);
            this.btn_AInfoCancle.Name = "btn_AInfoCancle";
            this.btn_AInfoCancle.Size = new System.Drawing.Size(50, 23);
            this.btn_AInfoCancle.TabIndex = 1;
            this.btn_AInfoCancle.Text = "取消";
            this.btn_AInfoCancle.UseVisualStyleBackColor = true;
            this.btn_AInfoCancle.Click += new System.EventHandler(this.btn_AInfoCancle_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(10, 190);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(77, 12);
            this.label1.TabIndex = 2;
            this.label1.Text = "告警总数为：";
            // 
            // label_TotalCount
            // 
            this.label_TotalCount.AutoSize = true;
            this.label_TotalCount.Location = new System.Drawing.Point(86, 190);
            this.label_TotalCount.Name = "label_TotalCount";
            this.label_TotalCount.Size = new System.Drawing.Size(11, 12);
            this.label_TotalCount.TabIndex = 3;
            this.label_TotalCount.Text = "0";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(152, 190);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(101, 12);
            this.label2.TabIndex = 4;
            this.label2.Text = "未解除告警数为：";
            // 
            // label_UnsolveCount
            // 
            this.label_UnsolveCount.AutoSize = true;
            this.label_UnsolveCount.Location = new System.Drawing.Point(254, 190);
            this.label_UnsolveCount.Name = "label_UnsolveCount";
            this.label_UnsolveCount.Size = new System.Drawing.Size(11, 12);
            this.label_UnsolveCount.TabIndex = 5;
            this.label_UnsolveCount.Text = "0";
            // 
            // AlarmInfo
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(562, 210);
            this.Controls.Add(this.label_UnsolveCount);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label_TotalCount);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.btn_AInfoCancle);
            this.Controls.Add(this.btn_AInfoRefresh);
            this.Controls.Add(this.listView1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.Name = "AlarmInfo";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "告警信息";
            this.Load += new System.EventHandler(this.AlarmInfo_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ListView listView1;
        private System.Windows.Forms.Button btn_AInfoRefresh;
        private System.Windows.Forms.ColumnHeader chAlarmState;
        private System.Windows.Forms.ColumnHeader chAlarmDateTime;
        private System.Windows.Forms.ColumnHeader chAlarmType;
        private System.Windows.Forms.ColumnHeader chAlarmDetail;
        private System.Windows.Forms.Button btn_AInfoCancle;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label_TotalCount;
        private System.Windows.Forms.ColumnHeader chAlarmNO;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label_UnsolveCount;
    }
}