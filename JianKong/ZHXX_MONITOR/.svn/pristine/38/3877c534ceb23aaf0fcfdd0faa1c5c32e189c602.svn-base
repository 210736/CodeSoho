#pragma once
//#include "..\Include\CSLScope.h"
#include "StdAfx.h";
#include "UDPReceiver.h"
//#include "CurveDrawMainFrame.h"

class CCurveDrawMainFrame;

struct RecvBroadCastPort
{
	//RecvBroadCastPort(CCurveDrawMainFrame* mainFrame)
	//{
	//	this->mainFrame= mainFrame;
	//}
	//CCurveDrawMainFrame* mainFrame;
	//CTSLScope* pTslScope;
    int HNO;   //fequency NO 
    int year;
    int day; 
    int hour;  
    int min; 
    int sec; 
    int us;
};

class CThreadRecvBroadcastData
{
public:
	CThreadRecvBroadcastData(void);
	~CThreadRecvBroadcastData(void);

	static  unsigned int _stdcall ThreadRecvBroadcastData(void* pdata);
	static bool InitRecvSocket();
	static void GetLocalIP(CString &strLocalIP);
	static void SetTimeFromData(TIMEDATA *);
	static void TerminateThread();
private:
	static CUDPReceiver* m_pUDPRecver;
	static HANDLE m_hRecvStopEvent;
};

