using System;
using System.Collections.Generic;
using System.Text;

using System.ComponentModel;
using System.Collections;
using System.Diagnostics;
using System.Data;
using System.Windows.Forms;
using System.Data.OracleClient;
using System.Configuration;
using DevComponents.DotNetBar;

namespace MonitorAndSet.CommonClass
{
    /// <summary>
    /// 类，用于数据访问的类。
    /// </summary>
    public class Database : IDisposable
    {
        /// <summary>
        /// 保护变量，数据库连接。
        /// </summary>
        protected OracleConnection Connection;

        /// <summary>
        /// 保护变量，数据库连接串。
        /// </summary>
        protected String ConnectionString;

        /// <summary>
        /// 系统日志对象，日志来源为“ajaxtest.Database”
        /// </summary>
//        protected MyEventsLog log = new MyEventsLog("ajaxtest.Database");

        /// <summary>
        /// 错误信息，添加
        /// </summary>
        public string wronginfo = "";

        /// <summary>
        /// 构造函数。
        /// </summary>
        /// <param name="DatabaseConnectionString">数据库连接串</param>
        public Database()
        {
            //2011-08-05  BYG
            ConnectionString = MainForm.ConnectionStr;
            //ConnectionString = ConfigurationManager.AppSettings["ConnectionString"].ToString();
            //    WebConfigurationManager.ConnectionStrings["ConnectionString"].ToString();
            //ConnectionString = "Data Source=db051;Persist Security Info=True;User ID=db051user;Password=db051;Unicode=True;";
        }

        /// <summary>
        /// 析构函数，释放非托管资源
        /// </summary>
        ~Database()
        {
            wronginfo = "";   //自己加的
            try
            {
                if (Connection != null)
                    Connection.Close();
            }
            catch (Exception e)
            {
//                log.WriteLog(EventLogEntryType.Warning, "Close失败，系统异常信息：" + e.Message);
                wronginfo = "警告——Close失败，系统异常信息：" + e.Message.ToString();
            }
            try
            {
                Dispose();
            }
            catch { }
        }

        /// <summary>
        /// 保护方法，打开数据库连接。 
        /// </summary>
        protected void Open()
        {
            wronginfo = "";   //自己加的
            if (Connection == null)
            {
                try
                {
                    Connection = new OracleConnection(ConnectionString);
                }
                catch (Exception e)
                {
  //                  log.WriteLog(EventLogEntryType.Error, "创建数据库连接失败，系统异常信息：" + e.Message);
                    wronginfo = "错误——创建数据库连接失败，系统异常信息：" + e.Message.ToString();
                }
            }
            if (Connection.State.Equals(ConnectionState.Closed))
            {
                try
                {
                    Connection.Open();
                }
                catch (Exception e)
                {
 //                   log.WriteLog(EventLogEntryType.Error, "打开数据库连接失败，系统异常信息：" + e.Message);
                    wronginfo = "错误——打开数据库连接失败，系统异常信息：" + e.Message.ToString();
                }
            }
        }

        /// <summary>
        /// 公有方法，关闭数据库连接。
        /// </summary>
        public void Close()
        {
            wronginfo = "";   //自己加的
            try
            {
                if (Connection != null)
                    Connection.Close();
            }
            catch (Exception e)
            {
 //               log.WriteLog(EventLogEntryType.Warning, "Close失败，系统异常信息：" + e.Message);
                wronginfo = "警告——Close失败，系统异常信息：" + e.Message.ToString();
            }
        }

        /// <summary>
        /// 公有方法，释放资源。
        /// </summary>
        public void Dispose()
        {
            wronginfo = "";   //自己加的
            // 确保连接被关闭
            try
            {
                if (Connection != null)
                {
                    Connection.Dispose();
                    Connection = null;
                }
            }
            catch (Exception e)
            {
   //             log.WriteLog(EventLogEntryType.Warning, "Dispose失败，系统异常信息：" + e.Message);
                wronginfo = "警告——Dispose失败，系统异常信息：" + e.Message.ToString();
            }
        }

        /// <summary>
        /// 公有方法，获取数据，返回一个OracleDataReader （调用后主意调用OracleDataReader.Close()）。
        /// </summary>
        /// <param name="SqlString">Sql语句</param>
        /// <returns>OracleDataReader</returns>
        public OracleDataReader GetDataReader(String SqlString)
        {
            Open();
            try
            {
                OracleCommand cmd = new OracleCommand(SqlString, Connection);
                return cmd.ExecuteReader(System.Data.CommandBehavior.CloseConnection);
            }
            catch (Exception e)
            {
   //             log.WriteLog(EventLogEntryType.Error, "GetDataReader失败，SqlString=" + SqlString + ",系统异常信息：" + e.Message);
                wronginfo = "错误——GetDataReader失败，SqlString=" + SqlString + ",系统异常信息：" + e.Message.ToString();
                return null;
            }
        }

        /// <summary>
        /// 公有方法，获取数据，返回一个DataSet。
        /// </summary>
        /// <param name="SqlString">Sql语句</param>
        /// <returns>DataSet</returns>
        public DataSet GetDataSet(String SqlString)
        {
            DataSet dataset = new DataSet();
            Open();
            try
            {
                OracleDataAdapter adapter = new OracleDataAdapter(SqlString, Connection);
                adapter.Fill(dataset);
            }
            catch (Exception e)
            {
   //             log.WriteLog(EventLogEntryType.Warning, "GetDataSet失败，SqlString=" + SqlString + ",系统异常信息：" + e.Message);
                wronginfo = "警告——GetDataSet失败，SqlString=" + SqlString + ",系统异常信息：" + e.Message.ToString();
            }
            finally
            {
                Close();
            }
            return dataset;
        }

        /// <summary>
        /// 公有方法，获取数据，返回一个DataTable。
        /// </summary>
        /// <param name="SqlString">Sql语句</param>
        /// <returns>DataTable</returns>
        public DataTable GetDataTable(String SqlString)
        {
            DataSet dataset = GetDataSet(SqlString);
            dataset.CaseSensitive = false;
            return dataset.Tables[0];
        }

        /// <summary>
        /// 公有方法，获取数据，返回一个DataRow。
        /// </summary>
        /// <param name="SqlString">Sql语句</param>
        /// <returns>DataRow</returns>
        public DataRow GetDataRow(String SqlString)
        {
            DataSet dataset = GetDataSet(SqlString);
            dataset.CaseSensitive = false;
            if (dataset.Tables[0].Rows.Count > 0)
            {
                return dataset.Tables[0].Rows[0];
            }
            else
            {
                return null;
            }
        }

        /// <summary>
        /// 公有方法，执行Sql语句。
        /// </summary>
        /// <param name="SqlString">Sql语句</param>
        /// <returns>对Update、Insert、Delete为影响到的行数，其他情况为-1</returns>
        public int ExecuteSQL(String SqlString)
        {
            int count = -1;
            Open();
            try
            {
                OracleCommand cmd = new OracleCommand(SqlString, Connection);
                count = cmd.ExecuteNonQuery();
            }
            catch (Exception e)
            {
  //              log.WriteLog(EventLogEntryType.Error, "ExecuteSQL失败，SqlString=" + SqlString + ",系统异常信息：" + e.Message);
                count = -1;
                MessageBoxEx.UseSystemLocalizedString = true;
                MessageBoxEx.Show(e.ToString());
            }
            finally
            {
                Close();
            }
            return count;
        }

        /// <summary>
        /// 公有方法，执行一组Sql语句。
        /// </summary>
        /// <param name="SqlStrings">Sql语句组</param>
        /// <returns>是否成功</returns>
        public bool ExecuteSQL(String[] SqlStrings)
        {
            bool success = true;
            Open();
            OracleCommand cmd = new OracleCommand();
            OracleTransaction trans = Connection.BeginTransaction();
            cmd.Connection = Connection;
            cmd.Transaction = trans;

            int i = 0;
            try
            {
                foreach (String str in SqlStrings)
                {
                    cmd.CommandText = str;
                    cmd.ExecuteNonQuery();
                    i++;
                }
                trans.Commit();
            }
            catch (Exception e)
            {
    //            log.WriteLog(EventLogEntryType.Error, "ExecuteSQL失败，SqlString=" + SqlStrings[i] + ",系统异常信息：" + e.Message);
                success = false;
                trans.Rollback();
                MessageBoxEx.UseSystemLocalizedString = true;
                MessageBoxEx.Show(e.ToString());
            }
            finally
            {
                Close();
            }
            return success;
        }

        /// <summary>
        /// 公有方法，在一个数据表中插入一条记录。
        /// </summary>
        /// <param name="TableName">表名</param>
        /// <param name="Cols">哈西表，键值为字段名，值为字段值</param>
        /// <returns>是否成功</returns>
        public bool Insert(String TableName, Hashtable Cols)
        {


            String SqlString = "Insert into " + TableName + SqlStringConstructor.GetAddValues(Cols);

            String[] Sqls = { SqlString };
            return ExecuteSQL(Sqls);
        }


        /// <summary>
        /// 公有方法，更新一个数据表。
        /// </summary>
        /// <param name="TableName">表名</param>
        /// <param name="Cols">哈西表，键值为字段名，值为字段值</param>
        /// <param name="Where">Where子句</param>
        /// <returns>是否成功</returns>
        public bool Update(String TableName, Hashtable Cols, String RowID)
        {
            String SqlString = "Update " + TableName
                                + SqlStringConstructor.GetUpdateSetClause(Cols) + ", ID =" + SqlStringConstructor.GetQuotedString(RowID)
                                + " Where Rowid=" + SqlStringConstructor.GetQuotedString(RowID);

            String[] Sqls = { SqlString };
            return ExecuteSQL(Sqls);
        }
        public bool Delete(String TableName, String RowID)
        {
            String SqlString = "delete " + TableName + " Where Rowid=" + SqlStringConstructor.GetQuotedString(RowID);
            String[] Sqls = { SqlString };
            return ExecuteSQL(Sqls);

        }
        public DataTable Query(String TableName, Hashtable queryItems)
        {
            string where = SqlStringConstructor.GetConditionClause(queryItems);
            string sql = "Select " + TableName + ".* , ROWID From " + TableName + " " + where;
            Database db = new Database();
            return db.GetDataTable(sql);
        }
    }
}
