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
	PROCESS_MEMORY_COUNTERS psmemCounters;    //PROCESS_MEMORY_COUNTERS ��Ҫ����ͷ�ļ�Psapi.h
	bool status;		//����״̬����������Ϊtrue, ֹͣ����Ϊfalse;
	DWORD dwProcessID;
	DWORD sdwTickCountOld;
} PROCESSINFO;

typedef struct 
{
	CString serviceName;  //������ʾ����
	CString srvName;      //�������� 
	DWORD status;         //����״̬,4��ʾ������������,1��ʾ����װ��δ����,0��ʾû�а�װ�˷���. ��ȡ������Ϣ֮ǰ����ֵΪ0
} SERVICEINFO;


// MoniInfoDlg �Ի���

class CMoniInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CMoniInfoDlg)

public:
	CMoniInfoDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMoniInfoDlg();

// �Ի�������
	enum { IDD = IDD_INFO_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);         //һ���ʱ��������

	DECLARE_MESSAGE_MAP()
public:
	CBCGPListCtrl m_infolist;

	CString ipaddress;          //IP��ַ  
	CString hostname;           //������ 

	CString cpuRate;		    //CPUʹ����
	CString diskInfo;			//Ӳ����Ϣ,��ʽΪ:����ʣ��Ӳ�̿ռ�/Ӳ���ܿռ�
	CString memoryInfo;		    //�ڴ���Ϣ,��ʽΪ:���������ڴ�/�ܵ��ڴ��С

	CString ifs;                //�ӿ�
	CString bandwides;          //����
	CString intraffic;          //��������
	CString outtraffic;         //�������
	CString alltraffic;         //������

	std::vector<PROCESSINFO> processInfo;		//Ҫ����ӵĽ�����Ϣ    vector��Ҫ#include <vector>
	std::vector<SERVICEINFO> serviceInfo;		//Ҫ����ӵķ�����Ϣ

	CString tempstr;      //��ʱ�ַ���
	CString outstr;       //����ַ���
};
#endif // !defined(AFX_MONICLIENTDLG_H__773F8BCF_07D9_4079_B277_B66B619752ADE__INCLUDED_)