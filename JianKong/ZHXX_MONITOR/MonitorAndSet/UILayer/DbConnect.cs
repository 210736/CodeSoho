using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using DevComponents.DotNetBar;
using System.Configuration;
using MonitorAndSet.CommonClass;
using System.Xml;
namespace MonitorAndSet
{
    public partial class DbConnect : DevComponents.DotNetBar.Office2007Form
    {
        public DbConnect()
        {
            InitializeComponent();
        }

        private void DbConnect_Load(object sender, EventArgs e)
        {
            MessageBoxEx.UseSystemLocalizedString = true;
            try 
            {
                string conn = ConfigurationManager.AppSettings["ConnectionString"].ToString();
                string[] a = conn.Split(';');
                string datasource = a[0].Substring(a[0].IndexOf('=') + 1);
                string dbuser = a[1].Substring(a[1].IndexOf('=') + 1);
                string dbpwd = a[2].Substring(a[2].IndexOf('=') + 1);

                this.tBox_DataSource.Text = datasource;
                this.tBox_dbUser.Text = dbuser;
                this.tBox_dbPassword.Text = dbpwd;
                this.tBox_dbPassword1.Text = dbpwd; 
            }
            catch(Exception ex)
            {
                MessageBoxEx.Show(ex.ToString());
            }
            
        }

        private void btn_dbModified_Click(object sender, EventArgs e)
        {
            MessageBoxEx.UseSystemLocalizedString = true;
            try
            {
                if(this.tBox_dbPassword1.Text!=this.tBox_dbPassword.Text)
                {
                    MessageBoxEx.Show("两次输入密码不一致！请重新输入！");
                    return;
                }
                
                if (MessageBoxEx.Show("确定修改数据库连接参数配置？", "确认修改", MessageBoxButtons.OKCancel) == DialogResult.OK)
                {
                    string conn = "Data Source=";
                    string key = "ConnectionString";
                    conn = conn + this.tBox_DataSource.Text.Trim() + ";User ID=" + this.tBox_dbUser.Text.Trim() + ";Password=" + this.tBox_dbPassword.Text.Trim() + ";Persist Security Info=True;Unicode=True;";

                    MainForm.ConnectionStr = conn;

                    String path = MainForm.nowPath + @"Config.xml";
                    XmlDocument doc = new XmlDocument();
                    doc.Load(path);
                    XmlElement root = doc.DocumentElement;
                    foreach (XmlNode node in root)
                    {
                        if (node.Name == "ConnectionString")
                        {
                            node.InnerText = conn;
                        }
                    }
                    doc.Save(path);

                    MessageBoxEx.Show("数据库连接参数修改成功！");
                    Close();
                }
            }
            catch (Exception ex)
            {
                MessageBoxEx.Show(ex.ToString());
            }
            
        }

        private string trim(string p)
        {
            throw new NotImplementedException();
        }

        private string trim(DevComponents.DotNetBar.Controls.TextBoxX textBoxX)
        {
            throw new NotImplementedException();
        }

        private void btn_dbCancel_Click(object sender, EventArgs e)
        {
            Close();
        }
    }
}
