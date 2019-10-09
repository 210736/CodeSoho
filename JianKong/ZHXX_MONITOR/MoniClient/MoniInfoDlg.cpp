// MoniInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MoniClient.h"
#include "MoniInfoDlg.h"


// MoniInfoDlg 对话框

IMPLEMENT_DYNAMIC(CMoniInfoDlg, CDialog)

CMoniInfoDlg::CMoniInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMoniInfoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
}

CMoniInfoDlg::~CMoniInfoDlg()
{
	
}

void CMoniInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INFOLIST, m_infolist);
}


BEGIN_MESSAGE_MAP(CMoniInfoDlg, CDialog)
		ON_WM_TIMER()
END_MESSAGE_MAP()


// MoniInfoDlg 消息处理程序

BOOL CMoniInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here
	CRect rect;
	m_infolist.GetClientRect(&rect);

	m_infolist.InsertColumn(0, "监控项", LVCFMT_CENTER, rect.Width()*1/3);
	m_infolist.InsertColumn(1, "信息", LVCFMT_CENTER, rect.Width()*2/3);

	m_infolist.InsertItem(0,"IP地址");
	m_infolist.InsertItem(1,"机器名");
	m_infolist.InsertItem(2,"");

	m_infolist.InsertItem(3,"CPU使用率(%)");
	m_infolist.InsertItem(4,"硬盘空间(M)");
	m_infolist.InsertItem(5,"内存大小(M)");
	m_infolist.InsertItem(6,"");

	m_infolist.InsertItem(7,"接口");
	m_infolist.InsertItem(8,"带宽");
	m_infolist.InsertItem(9,"输入流量");
	m_infolist.InsertItem(10,"输出流量");
	m_infolist.InsertItem(11,"总流量");
	m_infolist.InsertItem(12,"");

	m_infolist.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0,
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_infolist.EnableMarkSortedColumn ();

	int j = 13;
	for(int i = 0; i < (int)processInfo.size();i++)
	{
		m_infolist.InsertItem(j,processInfo[i].processName);
		j++;
	}

	j++;
	m_infolist.InsertItem(j,"");
	j++;

	for(int i = 0; i < (int)serviceInfo.size();i++)
	{
		m_infolist.InsertItem(j,serviceInfo[i].serviceName);
		j++;
	}

	SetTimer(1, 1000, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMoniInfoDlg::OnTimer(UINT nIDEvent) 
{
	if (m_infolist.GetItemText(0,1) != ipaddress)
	{
		m_infolist.SetItemText(0,1,ipaddress);
	}

	if (m_infolist.GetItemText(1,1) != hostname)
	{
		m_infolist.SetItemText(1,1,hostname);
	}

	if (m_infolist.GetItemText(3,1) != cpuRate)
	{
		m_infolist.SetItemText(3,1,cpuRate);
	}

	if (m_infolist.GetItemText(4,1) != diskInfo)
	{
		m_infolist.SetItemText(4,1,diskInfo);
	}

	if (m_infolist.GetItemText(5,1) != memoryInfo)
	{
		m_infolist.SetItemText(5,1,memoryInfo);
	}

	if (m_infolist.GetItemText(7,1) != ifs)
	{
		m_infolist.SetItemText(7,1,ifs);
	}

	if (m_infolist.GetItemText(8,1) != bandwides)
	{
		m_infolist.SetItemText(8,1,bandwides);
	}

	if (m_infolist.GetItemText(9,1) != intraffic)
	{
		m_infolist.SetItemText(9,1,intraffic);
	}

	if (m_infolist.GetItemText(10,1) != outtraffic)
	{
		m_infolist.SetItemText(10,1,outtraffic);
	}

	if (m_infolist.GetItemText(11,1) != alltraffic)
	{
		m_infolist.SetItemText(11,1,alltraffic);
	}

	for(int i = 13; i < m_infolist.GetItemCount(); i++)
	{
		for(int j = 0; j < (int)processInfo.size(); j++)
		{
			if (m_infolist.GetItemText(i,0) == processInfo[j].processName)
			{
				if(processInfo[j].status == true)
				{
					outstr = "";
					tempstr.Format("%d", processInfo[j].psmemCounters.WorkingSetSize / 1024);
					outstr = "内存占用(K):" + tempstr;
					tempstr.Format("%d", (int)(processInfo[j].processCPURate * 100.0));
					outstr += " -- CPU(%):" + tempstr;
					m_infolist.SetItemText(i,1,outstr);
				}
				else
				{
					m_infolist.SetItemText(i,1,"该进程未运行");
				}
			}
		}

		for(int j = 0; j < (int)serviceInfo.size(); j++)
		{
			if (m_infolist.GetItemText(i,0) == serviceInfo[j].serviceName)
			{
				if (serviceInfo[j].status == 0)
				{
					m_infolist.SetItemText(i,1,"该服务未安装");
				}
				else if (serviceInfo[j].status == 4)
				{
					m_infolist.SetItemText(i,1,"该服务正在运行");
				}
				else if (serviceInfo[j].status == 1)
				{
					m_infolist.SetItemText(i,1,"该服务停止运行");
				}
			}
		}
	}

	CDialog::OnTimer(nIDEvent);
}