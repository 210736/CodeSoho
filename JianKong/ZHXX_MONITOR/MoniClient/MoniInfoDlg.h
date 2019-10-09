#pragma once

#if !defined(AFX_MONICLIENTDLG_H__773F8BCF_07D9_4079_B277_B66B619752ADE__INCLUDED_)
#define AFX_MONICLIENTDLG_H__773F8BCF_07D9_4079_B277_B66B619752ADE__INCLUDED_

#include "afxcmn.h"
#include <vector>
#include <Psapi.h>
#include <BCGCBProInc.h>
typedef struct 
{
	CString processPath;
	CString processName;
	ULARGE_INTEGER uKernelTime;
	ULARGE_INTEGER uUserTime;
	LARGE_INTEGER cpuTime;
	double processCPURate;
	PROCESS_MEMORY_COUNTERS psmemCounters;    //PROCESS_MEMORY_COUNTERS 需要包含头文件Psapi.h
	bool status;		//工作状态，正常工作为true, 停止工作为false;
	DWORD dwProcessID;
	DWORD sdwTickCountOld;
} PROCESSINFO;

typedef struct 
{
	CString serviceName;  //服务显示名称
	CString srvName;      //服务名称 
	DWORD status;         //服务状态,4表示服务正在运行,1表示服务安装但未运行,0表示没有安装此服务. 获取服务信息之前都赋值为0
} SERVICEINFO;


// MoniInfoDlg 对话框

class CMoniInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CMoniInfoDlg)

public:
	CMoniInfoDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMoniInfoDlg();

// 对话框数据
	enum { IDD = IDD_INFO_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);         //一秒计时器处理函数

	DECLARE_MESSAGE_MAP()
public:
	CBCGPListCtrl m_infolist;

	CString ipaddress;          //IP地址  
	CString hostname;           //机器名 

	CString cpuRate;		    //CPU使用率
	CString diskInfo;			//硬盘信息,格式为:可用剩余硬盘空间/硬盘总空间
	CString memoryInfo;		    //内存信息,格式为:可用物理内存/总的内存大小

	CString ifs;                //接口
	CString bandwides;          //带宽
	CString intraffic;          //输入流量
	CString outtraffic;         //输出流量
	CString alltraffic;         //总流量

	std::vector<PROCESSINFO> processInfo;		//要求监视的进程信息    vector需要#include <vector>
	std::vector<SERVICEINFO> serviceInfo;		//要求监视的服务信息

	CString tempstr;      //临时字符串
	CString outstr;       //输出字符串
};
#endif // !defined(AFX_MONICLIENTDLG_H__773F8BCF_07D9_4079_B277_B66B619752ADE__INCLUDED_)