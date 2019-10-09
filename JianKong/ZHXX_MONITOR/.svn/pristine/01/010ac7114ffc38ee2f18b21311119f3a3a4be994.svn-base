using System;
using System.Collections.Generic;
using System.Text;

namespace MonitorAndSet.TopoInfo
{
    /// <summary>
    /// AlarmDetail表示告警的具体信息,用于告警信息对话框中告警信息的显示
    /// </summary>
    public class AlarmDetail
    {
        public AlarmDetail()
        {

        }

        public AlarmDetail(bool flag, String dtime, String type, String content)
        {
            this.flag = flag;
            this.dtime = dtime;
            this.type = type;
            this.content = content;
        }

        private bool m_flag;
        /// <summary>
        /// 告警状态标志,为真表示已解决,为假表示未解决
        /// </summary>
        public bool flag
        {
            get { return m_flag; }
            set { m_flag = value; }
        }

        private String m_dtime;
        /// <summary>
        /// 告警日期时间
        /// </summary>
        public String dtime
        {
            get { return m_dtime; }
            set { m_dtime = value; }
        }

        private String m_type;
        /// <summary>
        /// 告警类型,即日志信息中的次类型,包括"主机"和"网络"
        /// </summary>
        public String type
        {
            get { return m_type; }
            set { m_type = value; }
        }

        private String m_content;
        /// <summary>
        /// 告警内容,包括 主机ID号*主机名*日志内容
        /// </summary>
        public String content
        {
            get { return m_content; }
            set { m_content = value; }
        }
    }
}
