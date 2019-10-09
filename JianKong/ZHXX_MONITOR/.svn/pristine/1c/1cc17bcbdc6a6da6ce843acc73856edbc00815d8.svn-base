using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using DevComponents.DotNetBar;

namespace MonitorAndSet
{
    static class Program
    {
        /// <summary>
        /// 应用程序的主入口点。
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            try
            {
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);
                if (args.Length > 0)
                {
                    Application.Run(new MainForm(args[0]));
                }
                else
                {
                    Application.Run(new MainForm());
                }
            }
            catch (Exception ex)
            {
                MessageBoxEx.UseSystemLocalizedString = true;
                MessageBoxEx.Show(ex.ToString());
            }
        }
    }
}
