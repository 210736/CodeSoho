// MoniClientDlg.h : header file
//

#if !defined(AFX_MONICLIENTDLG_H__773F8BCF_07D9_4079_B277_B66B619752AD__INCLUDED_)
#define AFX_MONICLIENTDLG_H__773F8BCF_07D9_4079_B277_B66B619752AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMoniClientDlg dialog

#include <afxwin.h>
#include <afxsock.h>
#include <Psapi.h>
#include <vector>
#include <fstream>
#include <Winsvc.h>
#include <Winsock2.h>

#include "LocalInfo.h"
#include "MFNetTraffic.h"
#include "Dupapi.h"        //双机热备 头文件
#include "MoniInfoDlg.h"



#define MAXSIZE 1024
#define WM_GETINFO WM_USER + 200

#define SystemBasicInformation       0
#define SystemPerformanceInformation 2
#define SystemTimeInformation        3

#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))

#define DEFAULT_PORT  5150
#define DEFAULT_BUFFER_LENGTH  2048

#define DEFAULT_PORT2  7500       //后加的,代理发送信息时的端口号

typedef struct
{
    DWORD   dwUnknown1;
    ULONG   uKeMaximumIncrement;
    ULONG   uPageSize;
    ULONG   uMmNumberOfPhysicalPages;
    ULONG   uMmLowestPhysicalPage;
    ULONG   uMmHighestPhysicalPage;
    ULONG   uAllocationGranularity;
    PVOID   pLowestUserAddress;
    PVOID   pMmHighestUserAddress;
    ULONG   uKeActiveProcessors;
    BYTE    bKeNumberProcessors;
    BYTE    bUnknown2;
    WORD    wUnknown3;
} SYSTEM_BASIC_INFORMATION;

typedef struct
{
    LARGE_INTEGER   liIdleTime;
   // DWORD           dwSpare[76];
	DWORD           dwSpare[100];
} SYSTEM_PERFORMANCE_INFORMATION;

typedef struct
{
    LARGE_INTEGER liKeBootTime;
    LARGE_INTEGER liKeSystemTime;
    LARGE_INTEGER liExpTimeZoneBias;
    ULONG         uCurrentTimeZoneId;
    DWORD         dwReserved;
} SYSTEM_TIME_INFORMATION;

UINT RecvInfoFromSvr(LPVOID pParam);				//接受服务器发送启动或者关闭某个进程或服务的命令的线程
UINT RecvSvrStruct(LPVOID pParam);				   //接受服务器发送的双机热备服务器的切换等命令的线程

class CMoniClientDlg : public CDialog
{
// Construction
public:
	void ReadFile(void);	 //读取配置文件，通过读取信息对m_processInfo和m_serviceInfo初始化
	void ReadFile(CString);	 //读取配置文件，通过读取信息对m_processInfo和m_serviceInfo初始化
	void StartApplication();

	void CpuInfo();          //获得CPU的总的使用率
	void GetCpuUsage();          //获得CPU的总的使用率
	void DiskInfo();         //获得本机的硬盘使用情况信息
	void MemoryInfo();	     //获得系统内存情况
	void ProcessInfo();      //获得被监视的进程信息
	void ServiceInfo();       //获得服务信息
	void GetMac(char* mac);				//获取mac地址
	int Get_cpu_usage(int pid);  //根据pid获取进程cpu使用率

	void StartProcess(LPCSTR processName);			//启动未运行的需要被监视的进程
	void StartServices(LPCSTR serviceName);			//启动未运行的需要被监视的服务
	void CloseProcess(LPCSTR processName);			//关闭正在运行的需要被监视的进程
	void CloseService(LPCSTR serviceName);			//关闭正在运行的需要被监视的服务

	void GetServerStatus();         //获得双机热备状态信息
	void BegSwitch();               //调用请求切换接口函数
	void ForceZhu();                //调用强制设置主/副机接口函数(强制为主机)
	void AutoMode();                //调用恢复自动方式接口函数
	
	CMoniClientDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CMoniClientDlg();

// Dialog Data
	//{{AFX_DATA(CMoniClientDlg)
	enum { IDD = IDD_MONICLIENT_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMoniClientDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMoniClientDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);                        //一秒计时器处理函数
	afx_msg void OnPopupMenu2();     //修改监控代理配置文件
	afx_msg void OnPopupMenu3();     //退出监控代理
	afx_msg LRESULT OnGetInfo(WPARAM wParam, LPARAM lParam);	//从服务器接收到数据后的处理函数
	afx_msg LRESULT OnTrayNotification(WPARAM wparam, LPARAM lparam);
	//
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	//CStdioFile w_file;  //写记录文件(查看是否收到数据) //20100407 ding+
	//CString txtFile;//20100407 ding+

	SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo;
    SYSTEM_TIME_INFORMATION        SysTimeInfo;
    SYSTEM_BASIC_INFORMATION       SysBaseInfo;
	double                         dbIdleTime;
    double                         dbSystemTime;
    LONG                           status;
    LARGE_INTEGER                  liOldIdleTime;
    LARGE_INTEGER                  liOldSystemTime;

	int iPort;
	DWORD dwLength;
	WSADATA wsd;
	SOCKET sk;              //SOCKET需要包含头文件afxsock.h
	char *sendbuf;
	SOCKADDR_IN recipient;  //代理给监控终端发送数据时的接收方,组播IP为239.255.0.11,端口为5150
	bool started;           //应用是否开启的标志
	bool bsent;             //双机热备的数据是否发送的标志

	ULONGLONG nAllFreeBytes;      //所有磁盘的可用自由空间的总和
	ULONGLONG nAllBytes;          //所有硬盘的总的空间总和

	FILETIME kernelTime;
	FILETIME userTime;

	FILETIME m_preIdleTime;  //计算CPU总使用率
	FILETIME m_preKernelTime; //计算CPU总使用率
	FILETIME m_preUserTime; //计算CPU总使用率

	DWORD idProcesses[1024];
	DWORD dwNumProcesses;
	//ENUM_SERVICE_STATUS enumServiceStatus[500];      //ENUM_SERVICE_STATUS需要包含头文件#include <Winsvc.h>
	ENUM_SERVICE_STATUS enumServiceStatus[1000];      //ENUM_SERVICE_STATUS需要包含头文件#include <Winsvc.h>
	DWORD servicesReturned;

	//甲乙机的划分是按代理配置文件中IP的先后顺序
	CString m_serverAIP;          //双机热备 甲机的IP
	CString m_serverBIP;          //双机热备 乙机的IP

	/******FTP登陆**********/
	std::string m_ftpIP;
	std::string m_ftpUser;
	std::string m_ftpPassword;
	std::string m_ServerPath;
	std::string m_LocalPath;
	//双机热备服务器的各种状态信息
	unsigned char StatusOfA;       //甲机的主/副状态，1-主机，2-副机，0-状态未确定，其它值未定义
	unsigned char StatusOfB;       //乙机的主/副状态，1-主机，2-副机，0-状态未确定，其它值未定义
	unsigned char Line1Status;     //专用连接1的状态，0-正常，1-故障，2-状态未确定，其它值未定义
	unsigned char Line2Status;     //专用连接2的状态，0-正常，1-故障，2-状态未确定，其它值未定义
	unsigned char SoftStatusOfA;   //甲机关键软件的状态，0-正常，1-异常，2-状态未确定，其它值未定义
	unsigned char NetStatusOfA;    //甲机网络连接的状态，0-正常，1-故障，2-状态未确定，其它值未定义
	unsigned char Mode;            //双工工作方式，0-强制方式，1-自动方式，2-方式未确定，其它值未定义。
	unsigned char SoftStatusOfB;   //乙机关键软件的状态，0-正常，1-异常，2-状态未确定，其它值未定义
	unsigned char NetStatusOfB;    //乙机网络连接的状态，0-正常，1-故障，2-状态未确定，其它值未定义

	DUP_STATUS DupStatus;   //双工状态结构体

	//各种要求的信息
	CString m_cpuRate;				//CPU使用率
	CString m_diskInfo;				//硬盘信息,格式为:可用剩余硬盘空间/硬盘总空间
	CString m_memoryInfo;		    //内存信息,格式为:可用物理内存/总的内存大小

	MFNetTraffic m_cTrafficInComing;
	MFNetTraffic m_cTrafficOutGoing;
	MFNetTraffic m_cTrafficTotal;
	MFNetTraffic m_cTrafficClass;
	std::vector<PROCESSINFO> m_processInfo;		//要求监视的进程信息    vector需要#include <vector>
	std::vector<SERVICEINFO> m_serviceInfo;		//要求监视的服务信息

	CMoniInfoDlg InfoDlg;      //显示监控信息的对话框

private:
	HANDLE m_threadHand;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MONICLIENTDLG_H__773F8BCF_07D9_4079_B277_B66B619752AD__INCLUDED_)
