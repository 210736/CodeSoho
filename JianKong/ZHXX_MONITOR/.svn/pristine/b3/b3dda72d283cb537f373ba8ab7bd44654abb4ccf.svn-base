// LocalInfo.cpp: implementation of the CLocalInfo class.
//
//////////////////////////////////////////////////////////////////////
//
#include "stdafx.h"
#include "MoniClient.h"
#include "LocalInfo.h"
//
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLocalInfo::CLocalInfo() : m_hostName(_T("")), m_ipAddress(_T("")), m_ipMask(_T(""))
{

}

CLocalInfo::~CLocalInfo()
{

}


//获取本地信息
void CLocalInfo::GetInfo()
{
	

#define BUFSIZE 1024

	LPTSTR lpszSystemInfo;      // pointer to system information string 
	DWORD cchBuff = BUFSIZE;    // size of computer
	TCHAR tchBuffer[BUFSIZE];   // buffer for string

	lpszSystemInfo = tchBuffer; 

	WSADATA  WSAData;
	int WSAReturn;

	//在使用WinSock API之前，必须调用WSAStartup函数，
	//只有该函数成功返回（表示与WinSock库成功地建立起连接），
	//才可以调用其他Windows Sockets DLL中的函数
	WSAReturn = WSAStartup( 0x0101, &WSAData );
	if( WSAReturn == 0 )
	{
		if(gethostname(lpszSystemInfo, 128) == 0) //获取主机名
		{
			//tchBuffer = lpszSystemInfo;
			m_hostName.Format("%s", lpszSystemInfo);
		}
		struct hostent *p_HostEnt;
		p_HostEnt = gethostbyname(lpszSystemInfo);
		if(p_HostEnt != NULL)
		{
			char  Hostaddress[20]; // 主机IP地址
			wsprintf( Hostaddress, "%d.%d.%d.%d",
			( p_HostEnt->h_addr_list[0][0] & 0x00ff ),
			( p_HostEnt->h_addr_list[0][1] & 0x00ff ),
			( p_HostEnt->h_addr_list[0][2] & 0x00ff ),
			( p_HostEnt->h_addr_list[0][3] & 0x00ff ) );
			m_ipAddress.Format(Hostaddress);
			TRACE("%s\t\t\t%d", m_ipAddress, m_ipAddress.GetLength());
			//调用WSACleanup函数进行WinSock的清理工作，以便释放其占用的资源
			WSACleanup();
		}
	}

	////获取子网掩码
	//PIP_ADAPTER_INFO pinfo=NULL;
	//unsigned long len=0;

	//if (pinfo!=NULL)
	//	delete (pinfo);
	//unsigned  long nError;

	////获得本地计算机的网络信息
	//nError	=	GetAdaptersInfo(pinfo,&len);
	//if (nError==0)
	//{
	//	m_ipMask.Format("%s",pinfo->IpAddressList.IpMask.String);
	//}
	//if (nError==ERROR_NO_DATA)
	//{
	//	AfxMessageBox("No adapter information exists for the local computer");
	//}
	//if (nError==ERROR_NOT_SUPPORTED)
	//{
	//	AfxMessageBox("GetAdaptersInfo is not supported by the operating system running on the local computer");
	//}
	//if (nError==ERROR_BUFFER_OVERFLOW)
	//{
	//	pinfo= (PIP_ADAPTER_INFO)malloc(len);
	//	nError	=	GetAdaptersInfo(pinfo,&len);
	//	if (nError==0)
	//	{
	//		m_ipMask.Format("%s",pinfo->IpAddressList.IpMask.String);
	//		TRACE("m_ipMask = %s\n", m_ipMask);
	//	}
	//}

	////获取本机MAC地址
	//pinfo=NULL;
	//len=0;

	////提取网卡信息
	//nError	=	::GetAdaptersInfo(pinfo,&len);
	//if (nError==0)
	//{
	//	do
	//	{
	//		m_macAddress.Format("%.2X.%.2X.%.2X.%.2X.%.2X.%.2X",
	//			pinfo->Address[0],
	//			pinfo->Address[1],
	//			pinfo->Address[2],
	//			pinfo->Address[3],
	//			pinfo->Address[4],
	//			pinfo->Address[5]);
	//	}
	//	while(pinfo->Next != NULL);
	//}
	//	if (nError==ERROR_NO_DATA)
	//	{
	//		AfxMessageBox("No adapter information exists for the local computer");
	//	}
	//	if (nError==ERROR_NOT_SUPPORTED)
	//	{
	//		AfxMessageBox("GetAdaptersInfo is not supported by the operating system running on the local computer");
	//	}
	//	if (nError==ERROR_BUFFER_OVERFLOW)
	//	{
	//		pinfo= (PIP_ADAPTER_INFO)malloc(len);
	//		nError	=	GetAdaptersInfo(pinfo,&len);
	//		if (nError==0)
	//		{
	//			do
	//			{
	//				m_macAddress.Format("%.2X.%.2X.%.2X.%.2X.%.2X.%.2X",
	//					pinfo->Address[0],
	//					pinfo->Address[1],
	//					pinfo->Address[2],
	//					pinfo->Address[3],
	//					pinfo->Address[4],
	//					pinfo->Address[5]);
	//				TRACE("macAddress = %s\n", m_macAddress);
	//			}
	//			while(pinfo->Next != NULL);
	//		}
	//}
	//if (pinfo!=NULL)
	//	delete (pinfo);	
		
}

