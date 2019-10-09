#include "StdAfx.h"
#include "ThreadRecvBroadcastData.h"
#include "UDPReceiver.h"
#include "InitFile.h"

CUDPReceiver* CThreadRecvBroadcastData::m_pUDPRecver;
 HANDLE CThreadRecvBroadcastData::m_hRecvStopEvent;

void CThreadRecvBroadcastData::GetLocalIP(CString &strLocalIP)
{
	DWORD aryHostIP[5] = {0};
	char tchBuffer[1024] = {0};  

	if (gethostname(tchBuffer, 128) == 0) 
	{
		CStringA strLocalHostName;
		strLocalHostName.Format("%s", tchBuffer);
	}

	struct hostent *p_HostEnt;
	p_HostEnt = gethostbyname(tchBuffer);

	if(p_HostEnt != NULL)
	{
		for (int i=0;p_HostEnt->h_addr_list[i]!=NULL&&(i+1)<=5;i++)
		{
			aryHostIP[i]=ntohl(*((DWORD*)p_HostEnt->h_addr_list[i]));
		}

		unsigned char* pIP;
		pIP=(unsigned char*)&aryHostIP[0];
		strLocalIP.Format(_T("%u.%u.%u.%u"),*(pIP+3),*(pIP+2),*(pIP+1),*(pIP));
	}

}

CThreadRecvBroadcastData::CThreadRecvBroadcastData(void)
{
}


CThreadRecvBroadcastData::~CThreadRecvBroadcastData(void)
{
}

 unsigned int _stdcall CThreadRecvBroadcastData::ThreadRecvBroadcastData( void* pdata )
{
	/*RecvBroadCastPort* pRecvBroadCastPort= (RecvBroadCastPort*)pdata;*/
	//if( !pRecvBroadCastPort )
	//	return 0;
    CInitFile inifile;
	int timeinterval = inifile.GetTimeInterval();
	
	HANDLE handles[2] = {NULL, NULL};
	handles[0] = WSACreateEvent();
	handles[1] = CreateEvent(NULL, TRUE, FALSE, NULL);

	//停止事件
	m_hRecvStopEvent = handles[1];
	InitRecvSocket();

	if (m_pUDPRecver != NULL)
	{
		m_pUDPRecver->SetReadEvent(handles[0]);
	}
	DWORD lasttime =0;
	while(1)
	{		
		//线程退出
		if(m_hRecvStopEvent == NULL)
		{
			m_hRecvStopEvent = CreateEvent(NULL, TRUE ,FALSE, NULL);
			if (m_hRecvStopEvent == NULL)
			{
				TRACE("create stop event failed!");	
				break;
			}
			handles[1] = m_hRecvStopEvent;
		}

		DWORD wait_index = WSAWaitForMultipleEvents(2, handles, FALSE, INFINITE, FALSE);

		if(wait_index == WSA_WAIT_FAILED)
		{
			//关闭两个句柄后，函数返回
			WSACloseEvent(handles[0]);
			WSACloseEvent(handles[1]);

			return 0;
		}

		////便于下次循环等待时正确接收数据；
		//如果线程停止事件被激活，而读事件根本没有被激活，那么将读事件重置为未激活状态也
		//没有错误
		WSAResetEvent(handles[0]);

		if (wait_index == WSA_WAIT_EVENT_0 && m_pUDPRecver != NULL)
		{
			char  buf[28];
			memset(buf,0x00,sizeof(buf));
			int recvbuflen = 28;
			//使用套接字读取数据，并返回读到的数据长度recv_len
			int recv_len = m_pUDPRecver->ReadData(buf, recvbuflen);

			if(recv_len > 0)
			{
				/*pRecvBroadCastPort->mainFrame->ProcessGroupData(buf);*/
				TIMEDATA * timedata = (TIMEDATA *)buf;
				 if(lasttime ==0)
				{
					SetTimeFromData(timedata);
					lasttime = GetTickCount();
				 }
				 else if(GetTickCount() - lasttime >=(timeinterval *1000))
				 {
					 SetTimeFromData(timedata);
					 lasttime = GetTickCount();
				 }

			}
		}
		
		//线程退出 WSASetEvent(m_hRecvStopEvent)被执行
		if (wait_index == WSA_WAIT_EVENT_0 + 1)
		{
			WSACloseEvent(handles[0]);
			WSACloseEvent(handles[1]);
			delete m_pUDPRecver;
			return 0;
		}

	//	Sleep(timeinterval *1000);
	}

	return 0;

} 

 bool CThreadRecvBroadcastData::InitRecvSocket()
 {
	 if (m_pUDPRecver == NULL)
	 {
		 CString localIP;
		 GetLocalIP(localIP);
		 
		 CStringA localIPA(localIP.GetBuffer(0));

		 CInitFile initFile;
		 TCHAR groupIP[256]= {0};
		 initFile.GetGroupCastIP(groupIP);

		 CStringA GroupIPA(groupIP);

		 int GroupPort;
		 GroupPort= initFile.GetGroupCastPort();
		 
		 m_pUDPRecver = new CUDPReceiver(ntohl(inet_addr(localIPA.GetBuffer(0))), GroupPort, ntohl(inet_addr(GroupIPA.GetBuffer(0))));
		 if (m_pUDPRecver == NULL)
		 {
			 return false;
		 }
		 //
		 BOOL initsuc = m_pUDPRecver->init(MULTICAST);
		 if (!initsuc)
		 {
			 return false;
		 }
	 }

	 return true;
 }

 void CThreadRecvBroadcastData::TerminateThread()
 {
    SetEvent(m_hRecvStopEvent); 
 }

 
 void CThreadRecvBroadcastData::SetTimeFromData(TIMEDATA * timedata)
 {
	 WORD year = (short)timedata->year;	
	 WORD days = (short)timedata->day;
	 WORD hour = (short)timedata->hour;
	 WORD min = (short)timedata->min;
	 WORD sec = (short)timedata->sec;
	 WORD ms = (short)timedata->us/1000;
	 WORD day =0;
	 WORD tmpday=0; 
	 WORD month =1;
	 
	  _SYSTEMTIME * ltime = new _SYSTEMTIME; 
	  ltime->wYear = year;
	  ltime->wHour = hour;
	  ltime->wMinute = min;
	  ltime->wSecond = sec;
	  ltime->wMilliseconds = ms;
     if(year < 1601)
	 {
		 LPSYSTEMTIME localtime = new _SYSTEMTIME;
		  GetLocalTime(localtime);
		 ltime->wYear = localtime->wYear;
		 delete localtime;
	 }
	 short leap=0;	 
	 if((year%400==0) || ((year%100!=0)&&(year%4==0)))
		 leap = 1;
	 if( days != 0 )
	 {
		 for( month =1; month <=12; month++)
	       {
				switch( month)
				{
				case 1:
				case 3:
				case 5:
				case 7:
				case 8:
				case 10:
				case 12:
					day += 31;
					break;
				case 4:
				case 6:
				case 9:
				case 11:
					day += 30;
					break;
				case 2:
					day += 28;
					day += leap;
					break;
				default:
					break;
				}
				if( days < day)
				{
					ltime->wMonth = month;
					if( month == 1)
					{
						ltime->wDay = days;
						break;
					}
					else
					{
						ltime->wDay = days - tmpday+1;
						break;
					}

				}
				else
				{
					tmpday = day;
				}
			}

	 }
	 else
	 {
		 ltime->wMonth = 1;
		 ltime->wDay = 1;
	 }
	
	  SetLocalTime(ltime);
      delete ltime;
	
 }