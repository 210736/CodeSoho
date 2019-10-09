#include "StdAfx.h"
#include "InitFile.h"


CInitFile::CInitFile(void)
{
}


CInitFile::~CInitFile(void)
{
}

void CInitFile::GetGroupCastIP( TCHAR* ip )
{
	GetPrivateProfileString(_T("GroupCast"), _T("groupIP"), _T("224.224.224.224"), ip, 256, _T(".\\MonitorAgent.ini"));  //ÈÎÎñ±í

}

int CInitFile::GetGroupCastPort()
{
	int port;
	port= GetPrivateProfileInt(_T("GroupCast"), _T("groupPort"), 8000, _T(".\\MonitorAgent.ini"));
	return port;

}

int CInitFile::GetTimeInterval()
{
	int interval;
	interval= GetPrivateProfileInt(_T("GroupCast"), _T("timeinterval"), 60, _T(".\\MonitorAgent.ini"));
	return interval;

}

