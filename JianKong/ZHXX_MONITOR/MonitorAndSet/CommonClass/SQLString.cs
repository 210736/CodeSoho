using System;
using System.Collections.Generic;
using System.Text;

using System.Collections;

namespace MonitorAndSet.CommonClass
{
    /// <summary>
    /// SQLString 的摘要说明。
    /// </summary>
    public class SqlStringConstructor
    {
        /// <summary>
        /// 公有静态方法，将文本转换成适合在Sql语句里使用的字符串。
        /// </summary>
        /// <returns>转换后文本</returns>	
        public static String GetQuotedString(String pStr)
        {
            return ("'" + pStr.Replace("'", "''") + "'");
        }

        public static String GetAddValues(Hashtable Cols)
        {
            int Count = 0;

            if (Cols.Count <= 0)
            {
                return "";
            }

            String Fields = " (";
            String Values = " Values(";
            foreach (DictionaryEntry item in Cols)
            {
                if (Count != 0)
                {
                    Fields += ",";
                    Values += ",";
                }
                Fields += item.Key.ToString();
                Values += GetAddValue(item.Value);
                Count++;
            }
            Fields += ")";
            Values += ")";
            return Fields + Values;
        }

        public static String GetAddValue(object Value)
        {
            if (Value.GetType().ToString() == "System.String")
            {
                return SqlStringConstructor.GetQuotedString(Value.ToString());
            }
            else if (Value.GetType().ToString() == "System.DateTime")
            {
                return SqlStringConstructor.GetFormatedDateString((DateTime)Value);
            }
            else
            {
                return Value.ToString();
            }
        }

        /// <summary>
        /// 根据条件哈希表,构造SQL语句中的AND条件子句
        /// </summary>
        /// <param name="conditionHash">条件哈希表</param>
        /// <returns>AND关系条件子句</returns>
        public static String GetConditionClause(Hashtable queryItems)
        {

            int Count = 0;
            String Where = "";

            //根据哈希表，循环生成条件子句
            foreach (DictionaryEntry item in queryItems)
            {
                if (Count == 0)
                    Where = " Where ";
                else
                    Where += " And ";

                //根据查询列的数据类型，决定是否加单引号
                if (item.Value.GetType().ToString() == "System.String")
                {
                    Where += item.Key.ToString()
                        + " = "//" Like " 
                        + SqlStringConstructor.GetQuotedString(item.Value.ToString());//"%");
                }
                else if (item.Value.GetType().ToString() == "System.DateTime")
                {
                    Where += item.Key.ToString()
                        + " = "//" Like " 
                        + SqlStringConstructor.GetFormatedDateString((DateTime)item.Value);
                }
                else
                {
                    Where += item.Key.ToString() + "= " + item.Value.ToString();
                }
                Count++;
            }
            return Where;
        }

        public static String GetUpdateSetClause(Hashtable queryItems)
        {

            int Count = 0;
            String Where = "";

            //根据哈希表，循环生成条件子句
            foreach (DictionaryEntry item in queryItems)
            {
                if (Count == 0)
                    Where = " Set ";
                else
                    Where += " , ";

                //根据查询列的数据类型，决定是否加单引号
                if (item.Value.GetType().ToString() == "System.String")
                {
                    Where += item.Key.ToString()
                        + " = "//" Like " 
                        + SqlStringConstructor.GetQuotedString(item.Value.ToString());//"%");
                }
                else if (item.Value.GetType().ToString() == "System.DateTime")
                {
                    Where += item.Key.ToString()
                        + " = "//" Like " 
                        + SqlStringConstructor.GetFormatedDateString((DateTime)item.Value);
                }
                else
                {
                    Where += item.Key.ToString() + "= " + item.Value.ToString();
                }
                Count++;
            }
            return Where;
        }

        private static String GetFormatedDateString(DateTime pDT)
        {
            return (" TO_DATE('" + pDT.Day.ToString() + '/' + pDT.Month.ToString() + '/' + pDT.Year.ToString() + "','DD/MM/YYYY')");
        }

        /// <summary>
        /// 根据条件哈希表,构造SQL语句中的条件子句
        /// </summary>
        /// <param name="conditionHash">条件哈希表</param>
        /// <param name="type">与还是或查询，取值={"AND","OR"}</param>
        /// <returns>AND关系条件子句</returns>
        public static String GetConditionClause(Hashtable queryItems, string type)
        {

            int Count = 0;
            String Where = "";

            //根据哈希表，循环生成条件子句
            foreach (DictionaryEntry item in queryItems)
            {
                if (Count == 0)
                    Where = " Where ";
                else
                    Where += " " + type + " ";

                //根据查询列的数据类型，决定是否加单引号
                if (item.Value.GetType().ToString() == "System.String" || item.Value.GetType().ToString() == "System.DateTime")
                {
                    Where += item.Key.ToString()
                        + " Like "
                        + SqlStringConstructor.GetQuotedString("%"
                        + item.Value.ToString()
                        + "%");
                }
                else
                {
                    Where += item.Key.ToString() + "= " + item.Value.ToString();
                }
                Count++;
            }
            return Where;
        }
    }
}
