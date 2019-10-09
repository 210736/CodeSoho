namespace MonitorAndSet
{
    partial class THChange
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(THChange));
            this.label1 = new System.Windows.Forms.Label();
            this.tBox_CPUTHValue = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.tBox_DISKTHValue = new System.Windows.Forms.TextBox();
            this.tBox_MEMTHValue = new System.Windows.Forms.TextBox();
            this.btn_ChangeOk = new DevComponents.DotNetBar.ButtonX();
            this.btn_ChangeCancle = new DevComponents.DotNetBar.ButtonX();
            this.label4 = new System.Windows.Forms.Label();
            this.tBox_TRAFFICValue = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.tBox_forbiddenPro = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(8, 11);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(119, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "CPU占用率门限值(%):";
            // 
            // tBox_CPUTHValue
            // 
            this.tBox_CPUTHValue.Location = new System.Drawing.Point(162, 8);
            this.tBox_CPUTHValue.Name = "tBox_CPUTHValue";
            this.tBox_CPUTHValue.Size = new System.Drawing.Size(135, 21);
            this.tBox_CPUTHValue.TabIndex = 1;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(8, 45);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(119, 12);
            this.label2.TabIndex = 2;
            this.label2.Text = "硬盘剩余门限值(GB):";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(8, 79);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(119, 12);
            this.label3.TabIndex = 3;
            this.label3.Text = "内存剩余门限值(MB):";
            // 
            // tBox_DISKTHValue
            // 
            this.tBox_DISKTHValue.Location = new System.Drawing.Point(162, 42);
            this.tBox_DISKTHValue.Name = "tBox_DISKTHValue";
            this.tBox_DISKTHValue.Size = new System.Drawing.Size(135, 21);
            this.tBox_DISKTHValue.TabIndex = 4;
            // 
            // tBox_MEMTHValue
            // 
            this.tBox_MEMTHValue.Location = new System.Drawing.Point(162, 76);
            this.tBox_MEMTHValue.Name = "tBox_MEMTHValue";
            this.tBox_MEMTHValue.Size = new System.Drawing.Size(135, 21);
            this.tBox_MEMTHValue.TabIndex = 5;
            // 
            // btn_ChangeOk
            // 
            this.btn_ChangeOk.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btn_ChangeOk.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.btn_ChangeOk.Location = new System.Drawing.Point(83, 253);
            this.btn_ChangeOk.Name = "btn_ChangeOk";
            this.btn_ChangeOk.Size = new System.Drawing.Size(55, 23);
            this.btn_ChangeOk.TabIndex = 8;
            this.btn_ChangeOk.Text = "确定";
            this.btn_ChangeOk.Click += new System.EventHandler(this.btn_ChangeOk_Click);
            // 
            // btn_ChangeCancle
            // 
            this.btn_ChangeCancle.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btn_ChangeCancle.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.btn_ChangeCancle.Location = new System.Drawing.Point(216, 253);
            this.btn_ChangeCancle.Name = "btn_ChangeCancle";
            this.btn_ChangeCancle.Size = new System.Drawing.Size(55, 23);
            this.btn_ChangeCancle.TabIndex = 9;
            this.btn_ChangeCancle.Text = "取消";
            this.btn_ChangeCancle.Click += new System.EventHandler(this.btn_ChangeCancle_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(8, 116);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(119, 12);
            this.label4.TabIndex = 10;
            this.label4.Text = "流量最大限值(Mb/s):";
            // 
            // tBox_TRAFFICValue
            // 
            this.tBox_TRAFFICValue.Location = new System.Drawing.Point(162, 113);
            this.tBox_TRAFFICValue.Name = "tBox_TRAFFICValue";
            this.tBox_TRAFFICValue.Size = new System.Drawing.Size(135, 21);
            this.tBox_TRAFFICValue.TabIndex = 11;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(12, 149);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(95, 12);
            this.label5.TabIndex = 12;
            this.label5.Text = "限制启动的进程:";
            // 
            // tBox_forbiddenPro
            // 
            this.tBox_forbiddenPro.Location = new System.Drawing.Point(162, 140);
            this.tBox_forbiddenPro.Multiline = true;
            this.tBox_forbiddenPro.Name = "tBox_forbiddenPro";
            this.tBox_forbiddenPro.Size = new System.Drawing.Size(135, 107);
            this.tBox_forbiddenPro.TabIndex = 13;
            // 
            // THChange
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(309, 310);
            this.Controls.Add(this.tBox_forbiddenPro);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.tBox_TRAFFICValue);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.btn_ChangeCancle);
            this.Controls.Add(this.btn_ChangeOk);
            this.Controls.Add(this.tBox_MEMTHValue);
            this.Controls.Add(this.tBox_DISKTHValue);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.tBox_CPUTHValue);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "THChange";
            this.ShowIcon = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "具体机器告警门限值更改";
            this.Load += new System.EventHandler(this.THChange_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox tBox_CPUTHValue;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox tBox_DISKTHValue;
        private System.Windows.Forms.TextBox tBox_MEMTHValue;
        private DevComponents.DotNetBar.ButtonX btn_ChangeOk;
        private DevComponents.DotNetBar.ButtonX btn_ChangeCancle;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox tBox_TRAFFICValue;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox tBox_forbiddenPro;
    }
}