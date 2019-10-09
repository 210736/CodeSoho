using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using DevComponents.DotNetBar;
using DevComponents.AdvTree;
using System.IO;

namespace MonitorAndSet
{
    public partial class FolderBrowserDialog : DevComponents.DotNetBar.Office2007Form
    {
        public FolderBrowserDialog()
        {
            InitializeComponent();
        }
        public string newlogpath = "";// 浏览获得新日志保存路径 2011-08-10 

        #region File Tree

        private ElementStyle _RightAlignFileSizeStyle = null;
        private void FolderBrowserDialog_Load(object sender, EventArgs e)
        {
            advTree1.Nodes.Clear();
            DriveInfo[] drives = DriveInfo.GetDrives();
            // Disables tree layout and display updates to optimize update
            advTree1.BeginUpdate();
            foreach (DriveInfo driveInfo in drives)
            {
                if (driveInfo.DriveType != DriveType.Fixed) continue;
                Node node = new Node();
                node.Tag = driveInfo;
                node.Text = driveInfo.Name.Replace(@"\", "");
                node.Image = global::MonitorAndSet.Properties.Resources.Harddrive;
                //node.Cells.Add(new Cell("Local Disk"));
                //node.Cells.Add(new Cell());
                advTree1.Nodes.Add(node);
                // We will load drive content on demand
                node.ExpandVisibility = eNodeExpandVisibility.Visible;
            }
            // Enable tree layout and display updates, performs any pending layout and display updates
            advTree1.EndUpdate();
            _RightAlignFileSizeStyle = new ElementStyle();
            _RightAlignFileSizeStyle.TextAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Far;
        }

        private void advTree1_BeforeExpand(object sender, AdvTreeNodeCancelEventArgs e)
        {
            Node parent = e.Node;
            if (parent.Nodes.Count > 0) return;

            if (parent.Tag is DriveInfo)
            {
                advTree1.BeginUpdate();
                DriveInfo driveInfo = (DriveInfo)parent.Tag;
                LoadDirectories(parent, driveInfo.RootDirectory);
                parent.ExpandVisibility = eNodeExpandVisibility.Auto;
                advTree1.EndUpdate(true);
            }
            else if (parent.Tag is DirectoryInfo)
            {
                LoadDirectories(parent, (DirectoryInfo)parent.Tag);
            }
        }

        private void LoadDirectories(Node parent, DirectoryInfo directoryInfo)
        {
            DirectoryInfo[] directories = directoryInfo.GetDirectories();
            foreach (DirectoryInfo dir in directories)
            {
                if ((dir.Attributes & FileAttributes.Hidden) == FileAttributes.Hidden) continue;
                Node node = new Node();
                node.Tag = dir;
                node.Text = dir.Name;
                node.Image = global::MonitorAndSet.Properties.Resources.FolderClosed;
                node.ImageExpanded = global::MonitorAndSet.Properties.Resources.FolderOpen;
                //node.Cells.Add(new Cell("Local Folder"));
                //node.Cells.Add(new Cell());
                node.ExpandVisibility = eNodeExpandVisibility.Visible;
                parent.Nodes.Add(node);
            }
            ///文件浏览
            //FileInfo[] files = directoryInfo.GetFiles();
            //foreach (FileInfo file in files)
            //{
            //    Node node = new Node();
            //    node.Text = file.Name;
            //    node.Image = global::MonitorAndSet.Properties.Resources.Document;
            //    node.Cells.Add(new Cell("File"));
            //    Cell cell = new Cell(file.Length.ToString("N0"));
            //    cell.StyleNormal = _RightAlignFileSizeStyle;
            //    node.Cells.Add(cell);

            //    parent.Nodes.Add(node);
            //}
        }
        #endregion
        //新建文件夹
        private void btn_FBDFNew_Click(object sender, EventArgs e)
        {
            MessageBoxEx.UseSystemLocalizedString = true;
            try 
            {
                Node node1 = advTree1.SelectedNode;
                
                if (node1 == null)
                {
                    MessageBoxEx.Show("请先选定新建文件夹所在位置！");
                }
                else
                {
                    string path="";
                    if (node1.Parent != null)
                    {
                        path = (node1.FullPath).Replace(';', '\\').Substring(1);
                        path = path.Remove(path.Length - node1.TagString.Length) + "\\" + node1.TagString;
                    }
                    else if(node1.Tag is DriveInfo)
                    {
                        path = node1.Tag.ToString() ;
                    }
                    DirectoryInfo d1 = new DirectoryInfo(path);
                    ///标示树节点是否存在  false 表示不存在 
                    bool flag = false;
                    foreach (DirectoryInfo d3 in d1.GetDirectories())
                    {
                        int lengthd1 = d1.FullName.Length;
                        int lengthd3 = d3.FullName.Length;
                        string d3text = d3.FullName.Substring(lengthd1);
                        if (d3text == "LogFile")
                        {
                            MessageBoxEx.Show("日志文件夹已存在！");
                            flag = true;
                            break;
                        }
                    }
                    DirectoryInfo d2 = d1.CreateSubdirectory("LogFile");
                    advTree1.BeginUpdate();
                    Node node = new Node();
                    node.Tag = d2;
                    node.Text = d2.Name;
                    node.Image = global::MonitorAndSet.Properties.Resources.FolderClosed;
                    node.ImageExpanded = global::MonitorAndSet.Properties.Resources.FolderOpen;
                    //node.Cells.Add(new Cell("Local Folder"));
                    //node.Cells.Add(new Cell());
                    node.ExpandVisibility = eNodeExpandVisibility.Visible;
                    
                    if (flag == false)
                    {
                        node1.Nodes.Add(node);
                    }              
                    advTree1.EndUpdate();
                }
                
            }
            catch(Exception ex)
            {
                MessageBoxEx.Show(ex.ToString());
            }
            
        }
        
        /// 选定新建文件夹路径
        private void btn_FBDFOk_Click(object sender, EventArgs e)
        {
            MessageBoxEx.UseSystemLocalizedString = true;
            try
            {
                Node node1 = advTree1.SelectedNode;
                if (node1 == null)
                {
                    MessageBoxEx.Show("请先选定文件夹所在位置！");
                }
                else
                {
                    string path = (node1.FullPath).Replace(';', '\\').Substring(1);
                    newlogpath = path.Remove(path.Length - node1.TagString.Length) + "\\" + node1.TagString + "\\";
                    this.Close();
                }

            }
            catch (Exception ex)
            {
                MessageBoxEx.Show(ex.ToString());
            }
        }
        
        /// 取消按钮        
        private void btn_FBDFCancel_Click(object sender, EventArgs e)
        {
            this.Close();
        }
        
        //删除文件夹按钮
        private void btn_FBDFDelete_Click(object sender, EventArgs e)
        {
            MessageBoxEx.UseSystemLocalizedString = true;
            try
            {
                Node node1 = advTree1.SelectedNode;

                //不能删除根目录
                if (node1.Tag is DriveInfo)
                {
                    MessageBoxEx.Show("磁盘根目录不能删除！");
                }
                else
                {
                    ///选定要删除文件夹
                    if (node1 == null)
                    {
                        MessageBoxEx.Show("请先选定要删除的文件夹！");
                    }
                    else
                    {
                        if (MessageBoxEx.Show("确定删除 " + node1.TagString + "？", "文件夹删除", MessageBoxButtons.OKCancel) == DialogResult.OK)
                        {
                            string path = (node1.FullPath).Replace(';', '\\').Substring(1);
                            path = path.Remove(path.Length - node1.Text.Length) + "\\" + node1.Text;
                            Node nodep = node1.Parent;
                            string fname = node1.TagString;
                            DeleteFolder(path);
                            nodep.Nodes.Remove(node1,eTreeAction.Code);
                            MessageBoxEx.Show("文件夹" + fname + "删除成功！");
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBoxEx.Show(ex.ToString());
            }
        }
        
        //删除文件夹及所有子文件函数
        public static void DeleteFolder(string directorypath)
        {
            MessageBoxEx.UseSystemLocalizedString = true;
            try
            {
                foreach (string d in Directory.GetFileSystemEntries(directorypath))
                {
                    if (File.Exists(d))
                    {
                        FileInfo fi = new FileInfo(d);
                        if (fi.Attributes.ToString().IndexOf("ReadOnly") != -1)
                        {
                            fi.Attributes = FileAttributes.Normal;
                        }
                        File.Delete(d);
                    }
                    else
                        DeleteFolder(d);
                }
                Directory.Delete(directorypath);
            }
            catch(Exception ex)
            {
                MessageBoxEx.Show(ex.ToString());
            }
            
        }

        ///重命名文件夹
        private void btn_FBDFReName_Click(object sender, EventArgs e)
        {

        }
    }
}
