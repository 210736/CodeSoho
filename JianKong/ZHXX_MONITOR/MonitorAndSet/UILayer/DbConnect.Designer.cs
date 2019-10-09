namespace MonitorAndSet
{
    partial class DbConnect
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.tBox_DataSource = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.tBox_dbUser = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.btn_dbModified = new DevComponents.DotNetBar.ButtonX();
            this.btn_dbCancel = new DevComponents.DotNetBar.ButtonX();
            this.label4 = new System.Windows.Forms.Label();
            this.tBox_dbPassword1 = new System.Windows.Forms.MaskedTextBox();
            this.tBox_dbPassword = new System.Windows.Forms.MaskedTextBox();
            this.SuspendLayout();
            // 
            // tBox_DataSource
            // 
            // 
            // 
            // 
            this.tBox_DataSource.Border.Class = "TextBoxBorder";
            this.tBox_DataSource.Border.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.tBox_DataSource.Location = new System.Drawing.Point(98, 16);
            this.tBox_DataSource.Name = "tBox_DataSource";
            this.tBox_DataSource.Size = new System.Drawing.Size(127, 21);
            this.tBox_DataSource.TabIndex = 0;
            // 
            // tBox_dbUser
            // 
            // 
            // 
            // 
            this.tBox_dbUser.Border.Class = "TextBoxBorder";
            this.tBox_dbUser.Border.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.tBox_dbUser.Location = new System.Drawing.Point(98, 49);
            this.tBox_dbUser.Name = "tBox_dbUser";
            this.tBox_dbUser.Size = new System.Drawing.Size(127, 21);
            this.tBox_dbUser.TabIndex = 1;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(27, 18);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(53, 12);
            this.label1.TabIndex = 2;
            this.label1.Text = "数据源：";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(27, 51);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(53, 12);
            this.label2.TabIndex = 3;
            this.label2.Text = "用户名：";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(27, 85);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(41, 12);
            this.label3.TabIndex = 4;
            this.label3.Text = "密码：";
            // 
            // btn_dbModified
            // 
            this.btn_dbModified.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btn_dbModified.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.btn_dbModified.Location = new System.Drawing.Point(54, 155);
            this.btn_dbModified.Name = "btn_dbModified";
            this.btn_dbModified.Size = new System.Drawing.Size(57, 23);
            this.btn_dbModified.TabIndex = 6;
            this.btn_dbModified.Text = "修改";
            this.btn_dbModified.Click += new System.EventHandler(this.btn_dbModified_Click);
            // 
            // btn_dbCancel
            // 
            this.btn_dbCancel.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btn_dbCancel.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.btn_dbCancel.Location = new System.Drawing.Point(137, 155);
            this.btn_dbCancel.Name = "btn_dbCancel";
            this.btn_dbCancel.Size = new System.Drawing.Size(59, 23);
            this.btn_dbCancel.TabIndex = 7;
            this.btn_dbCancel.Text = "取消";
            this.btn_dbCancel.Click += new System.EventHandler(this.btn_dbCancel_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(27, 119);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(65, 12);
            this.label4.TabIndex = 9;
            this.label4.Text = "密码确认：";
            // 
            // tBox_dbPassword1
            // 
            this.tBox_dbPassword1.Location = new System.Drawing.Point(98, 116);
            this.tBox_dbPassword1.Name = "tBox_dbPassword1";
            this.tBox_dbPassword1.Size = new System.Drawing.Size(127, 21);
            this.tBox_dbPassword1.TabIndex = 11;
            this.tBox_dbPassword1.UseSystemPasswordChar = true;
            // 
            // tBox_dbPassword
            // 
            this.tBox_dbPassword.Location = new System.Drawing.Point(98, 85);
            this.tBox_dbPassword.Name = "tBox_dbPassword";
            this.tBox_dbPassword.Size = new System.Drawing.Size(127, 21);
            this.tBox_dbPassword.TabIndex = 12;
            this.tBox_dbPassword.UseSystemPasswordChar = true;
            // 
            // DbConnect
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(255, 190);
            this.Controls.Add(this.tBox_dbPassword);
            this.Controls.Add(this.tBox_dbPassword1);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.btn_dbCancel);
            this.Controls.Add(this.btn_dbModified);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.tBox_dbUser);
            this.Controls.Add(this.tBox_DataSource);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "DbConnect";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "数据库连接";
            this.Load += new System.EventHandler(this.DbConnect_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private DevComponents.DotNetBar.Controls.TextBoxX tBox_DataSource;
        private DevComponents.DotNetBar.Controls.TextBoxX tBox_dbUser;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private DevComponents.DotNetBar.ButtonX btn_dbModified;
        private DevComponents.DotNetBar.ButtonX btn_dbCancel;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.MaskedTextBox tBox_dbPassword1;
        private System.Windows.Forms.MaskedTextBox tBox_dbPassword;
    }
}