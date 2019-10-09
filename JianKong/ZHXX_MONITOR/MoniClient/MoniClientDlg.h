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
#include "Dupapi.h"        //˫���ȱ� ͷ�ļ�
#include "MoniInfoDlg.h"



#define MAXSIZE 1024
#define WM_GETINFO WM_USER + 200

#define SystemBasicInformation       0
#define SystemPerformanceInformation 2
#define SystemTimeInformation        3

#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))

#define DEFAULT_PORT  5150
#define DEFAULT_BUFFER_LENGTH  2048

#define DEFAULT_PORT2  7500       //��ӵ�,��������Ϣʱ�Ķ˿ں�

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

UINT RecvInfoFromSvr(LPVOID pParam);				//���ܷ����������������߹ر�ĳ�����̻�����������߳�
UINT RecvSvrStruct(LPVOID pParam);				   //���ܷ��������͵�˫���ȱ����������л���������߳�

class CMoniClientDlg : public CDialog
{
// Construction
public:
	void ReadFile(void);	 //��ȡ�����ļ���ͨ����ȡ��Ϣ��m_processInfo��m_serviceInfo��ʼ��
	void ReadFile(CString);	 //��ȡ�����ļ���ͨ����ȡ��Ϣ��m_processInfo��m_serviceInfo��ʼ��
	void StartApplication();

	void CpuInfo();          //���CPU���ܵ�ʹ����
	void GetCpuUsage();          //���CPU���ܵ�ʹ����
	void DiskInfo();         //��ñ�����Ӳ��ʹ�������Ϣ
	void MemoryInfo();	     //���ϵͳ�ڴ����
	void ProcessInfo();      //��ñ����ӵĽ�����Ϣ
	void ServiceInfo();       //��÷�����Ϣ
	void GetMac(char* mac);				//��ȡmac��ַ
	int Get_cpu_usage(int pid);  //����pid��ȡ����cpuʹ����

	void StartProcess(LPCSTR processName);			//����δ���е���Ҫ�����ӵĽ���
	void StartServices(LPCSTR serviceName);			//����δ���е���Ҫ�����ӵķ���
	void CloseProcess(LPCSTR processName);			//�ر��������е���Ҫ�����ӵĽ���
	void CloseService(LPCSTR serviceName);			//�ر��������е���Ҫ�����ӵķ���

	void GetServerStatus();         //���˫���ȱ�״̬��Ϣ
	void BegSwitch();               //���������л��ӿں���
	void ForceZhu();                //����ǿ��������/�����ӿں���(ǿ��Ϊ����)
	void AutoMode();                //���ûָ��Զ���ʽ�ӿں���
	
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
	afx_msg void OnTimer(UINT nIDEvent);                        //һ���ʱ��������
	afx_msg void OnPopupMenu2();     //�޸ļ�ش��������ļ�
	afx_msg void OnPopupMenu3();     //�˳���ش���
	afx_msg LRESULT OnGetInfo(WPARAM wParam, LPARAM lParam);	//�ӷ��������յ����ݺ�Ĵ�����
	afx_msg LRESULT OnTrayNotification(WPARAM wparam, LPARAM lparam);
	//
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	//CStdioFile w_file;  //д��¼�ļ�(�鿴�Ƿ��յ�����) //20100407 ding+
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
	SOCKET sk;              //SOCKET��Ҫ����ͷ�ļ�afxsock.h
	char *sendbuf;
	SOCKADDR_IN recipient;  //���������ն˷�������ʱ�Ľ��շ�,�鲥IPΪ239.255.0.11,�˿�Ϊ5150
	bool started;           //Ӧ���Ƿ����ı�־
	bool bsent;             //˫���ȱ��������Ƿ��͵ı�־

	ULONGLONG nAllFreeBytes;      //���д��̵Ŀ������ɿռ���ܺ�
	ULONGLONG nAllBytes;          //����Ӳ�̵��ܵĿռ��ܺ�

	FILETIME kernelTime;
	FILETIME userTime;

	FILETIME m_preIdleTime;  //����CPU��ʹ����
	FILETIME m_preKernelTime; //����CPU��ʹ����
	FILETIME m_preUserTime; //����CPU��ʹ����

	DWORD idProcesses[1024];
	DWORD dwNumProcesses;
	//ENUM_SERVICE_STATUS enumServiceStatus[500];      //ENUM_SERVICE_STATUS��Ҫ����ͷ�ļ�#include <Winsvc.h>
	ENUM_SERVICE_STATUS enumServiceStatus[1000];      //ENUM_SERVICE_STATUS��Ҫ����ͷ�ļ�#include <Winsvc.h>
	DWORD servicesReturned;

	//���һ��Ļ����ǰ����������ļ���IP���Ⱥ�˳��
	CString m_serverAIP;          //˫���ȱ� �׻���IP
	CString m_serverBIP;          //˫���ȱ� �һ���IP

	/******FTP��½**********/
	std::string m_ftpIP;
	std::string m_ftpUser;
	std::string m_ftpPassword;
	std::string m_ServerPath;
	std::string m_LocalPath;
	//˫���ȱ��������ĸ���״̬��Ϣ
	unsigned char StatusOfA;       //�׻�����/��״̬��1-������2-������0-״̬δȷ��������ֵδ����
	unsigned char StatusOfB;       //�һ�����/��״̬��1-������2-������0-״̬δȷ��������ֵδ����
	unsigned char Line1Status;     //ר������1��״̬��0-������1-���ϣ�2-״̬δȷ��������ֵδ����
	unsigned char Line2Status;     //ר������2��״̬��0-������1-���ϣ�2-״̬δȷ��������ֵδ����
	unsigned char SoftStatusOfA;   //�׻��ؼ������״̬��0-������1-�쳣��2-״̬δȷ��������ֵδ����
	unsigned char NetStatusOfA;    //�׻��������ӵ�״̬��0-������1-���ϣ�2-״̬δȷ��������ֵδ����
	unsigned char Mode;            //˫��������ʽ��0-ǿ�Ʒ�ʽ��1-�Զ���ʽ��2-��ʽδȷ��������ֵδ���塣
	unsigned char SoftStatusOfB;   //�һ��ؼ������״̬��0-������1-�쳣��2-״̬δȷ��������ֵδ����
	unsigned char NetStatusOfB;    //�һ��������ӵ�״̬��0-������1-���ϣ�2-״̬δȷ��������ֵδ����

	DUP_STATUS DupStatus;   //˫��״̬�ṹ��

	//����Ҫ�����Ϣ
	CString m_cpuRate;				//CPUʹ����
	CString m_diskInfo;				//Ӳ����Ϣ,��ʽΪ:����ʣ��Ӳ�̿ռ�/Ӳ���ܿռ�
	CString m_memoryInfo;		    //�ڴ���Ϣ,��ʽΪ:���������ڴ�/�ܵ��ڴ��С

	MFNetTraffic m_cTrafficInComing;
	MFNetTraffic m_cTrafficOutGoing;
	MFNetTraffic m_cTrafficTotal;
	MFNetTraffic m_cTrafficClass;
	std::vector<PROCESSINFO> m_processInfo;		//Ҫ����ӵĽ�����Ϣ    vector��Ҫ#include <vector>
	std::vector<SERVICEINFO> m_serviceInfo;		//Ҫ����ӵķ�����Ϣ

	CMoniInfoDlg InfoDlg;      //��ʾ�����Ϣ�ĶԻ���

private:
	HANDLE m_threadHand;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MONICLIENTDLG_H__773F8BCF_07D9_4079_B277_B66B619752AD__INCLUDED_)
