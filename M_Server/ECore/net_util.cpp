//-----------------------------------------------------------------------------
//!\file net_util.cpp
//!\desc 封装某些常用网络应用
//!\author Aslan
//! 
//!\date 2009-12-30
//! last 2009-12-30
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "net_util.h"

#include <nb30.h>
#pragma comment( lib, "netapi32.lib" )

namespace ECore{

//-----------------------------------------------------------------------------
// 初始化网络
//-----------------------------------------------------------------------------
BOOL NetUtil::InitNetwork()
{
	WORD wVersionRequested;
	WSADATA wsaData;

	// 先尝试winsocket2.2
	wVersionRequested = MAKEWORD( 2, 2 );
	INT nErr = WSAStartup( wVersionRequested, &wsaData );
	if( 0 != nErr ) 
	{
		// 尝试winsocket2.1
		wVersionRequested = MAKEWORD( 2, 1 );
		nErr = WSAStartup( wVersionRequested, &wsaData );
		if( 0 != nErr ) 
		{
			// 尝试winsocket1.1
			wVersionRequested = MAKEWORD( 1, 1 );
			nErr = WSAStartup( wVersionRequested, &wsaData );
			if( 0 != nErr ) 
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 销毁网络层
//-----------------------------------------------------------------------------
VOID NetUtil::DestroyNetwork()
{
	::WSACleanup();
}

//-----------------------------------------------------------------------------
// 得到本机名
//-----------------------------------------------------------------------------
std::string NetUtil::GetHostName()
{
	CHAR szHostName[LONG_STRING*2] = {0};
	gethostname(szHostName, sizeof(szHostName)-1);
	szHostName[sizeof(szHostName) - 1] = '\0';

	return std::string(szHostName);
}

//-----------------------------------------------------------------------------
// 通过机器名得到IP地址
//-----------------------------------------------------------------------------
BOOL NetUtil::GetIPByHostName(DWORD& dwIP, LPCSTR szHostName)
{
	ASSERT(szHostName);
	hostent* pHost = gethostbyname(szHostName);

	if( pHost )
	{
		INT n = 0;
		while( pHost->h_addr_list[n] )
		{
			dwIP = *((DWORD*)pHost->h_addr_list[n]);
			++n;
		}
		return TRUE;
	}
	else
		return FALSE;
}

//-----------------------------------------------------------------------------
// 得到本机的广域网IP
//-----------------------------------------------------------------------------
BOOL NetUtil::GetWANIP(DWORD& dwIP)
{
	SOCKET sock = WSASocket(AF_INET, SOCK_DGRAM, 0, 0, 0, 0);
	if( SOCKET_ERROR == sock )
	{
		return FALSE;
	}

	INTERFACE_INFO InterfaceInfo[128];
	DWORD dwBytes = 0;
	if( SOCKET_ERROR == WSAIoctl(sock, SIO_GET_INTERFACE_LIST, 0, 0, 
		&InterfaceInfo,	sizeof(InterfaceInfo), &dwBytes, 0, 0) ) 
	{
		closesocket(sock);
		return FALSE;
	}

	closesocket(sock);

	INT nIPNum = dwBytes / sizeof(INTERFACE_INFO);
	for(INT n = 0; n < nIPNum; ++n) 
	{
		u_long nFlags = InterfaceInfo[n].iiFlags;

		// 不可用
		if( !(nFlags & IFF_UP) )
			continue;

		// 回环接口
		if( nFlags & IFF_LOOPBACK )
			continue;

		// 得到IP地址
		sockaddr_in* pAddress;
		pAddress = (sockaddr_in *)&(InterfaceInfo[n].iiAddress);
		dwIP = pAddress->sin_addr.S_un.S_addr;

		// 检测是外网还是内网IP
		if( !( pAddress->sin_addr.S_un.S_un_b.s_b1 == 10 
			|| (pAddress->sin_addr.S_un.S_un_b.s_b1 == 172 && pAddress->sin_addr.S_un.S_un_b.s_b2 >= 16 && pAddress->sin_addr.S_un.S_un_b.s_b2 <= 31)
			|| (pAddress->sin_addr.S_un.S_un_b.s_b1 == 192 && pAddress->sin_addr.S_un.S_un_b.s_b2 == 168) ))
		{
			return TRUE;
		}
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 得到本机的局域网地址
//-----------------------------------------------------------------------------
BOOL NetUtil::GetLANIP(DWORD& dwIP)
{
	SOCKET sock = WSASocket(AF_INET, SOCK_DGRAM, 0, 0, 0, 0);
	if( SOCKET_ERROR == sock )
	{
		return FALSE;
	}

	INTERFACE_INFO InterfaceInfo[128];
	DWORD dwBytes = 0;
	if( SOCKET_ERROR == WSAIoctl(sock, SIO_GET_INTERFACE_LIST, 0, 0, 
		&InterfaceInfo,	sizeof(InterfaceInfo), &dwBytes, 0, 0) ) 
	{
		closesocket(sock);
		return FALSE;
	}

	closesocket(sock);

	INT nIPNum = dwBytes / sizeof(INTERFACE_INFO);
	for(INT n = 0; n < nIPNum; ++n) 
	{
		u_long nFlags = InterfaceInfo[n].iiFlags;

		// 不可用
		if( !(nFlags & IFF_UP) )
			continue;

		// 回环接口
		if( nFlags & IFF_LOOPBACK )
			continue;

		// 得到IP地址
		sockaddr_in* pAddress;
		pAddress = (sockaddr_in*)&(InterfaceInfo[n].iiAddress);
		dwIP = pAddress->sin_addr.S_un.S_addr;

		// 检测是外网还是内网IP
		if( pAddress->sin_addr.S_un.S_un_b.s_b1 == 10 
			|| (pAddress->sin_addr.S_un.S_un_b.s_b1 == 172 && pAddress->sin_addr.S_un.S_un_b.s_b2 >= 16 && pAddress->sin_addr.S_un.S_un_b.s_b2 <= 31)
			|| (pAddress->sin_addr.S_un.S_un_b.s_b1 == 192 && pAddress->sin_addr.S_un.S_un_b.s_b2 == 168) )
		{
			return TRUE;
		}
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 整型IP转化成字符串形式IP
//-----------------------------------------------------------------------------
std::string NetUtil::GetStringIP(DWORD dwIP)
{
	in_addr address;
	address.S_un.S_addr = dwIP;
	return std::string(inet_ntoa(address));
}

//-----------------------------------------------------------------------------
// 字符串形式IP转化成整型IP
//-----------------------------------------------------------------------------
DWORD NetUtil::GetIntegerIP(LPCSTR szIP)
{
	return inet_addr(szIP);
}

#include"iphlpapi.h"
#pragma comment(lib,"Iphlpapi.lib")
//-------------------------------------------------------------------------
// 得到本机的MAC地址，可能会有多个
//-------------------------------------------------------------------------
BOOL NetUtil::GetMAC(list<UINT64>& listMAC)
{
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter=NULL;
	DWORD dwRetVal=0;
	pAdapterInfo = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	if(GetAdaptersInfo(pAdapterInfo,&ulOutBufLen)!=ERROR_SUCCESS)
	{
		free(pAdapterInfo);
		pAdapterInfo=(IP_ADAPTER_INFO*)malloc(ulOutBufLen);
	}
	
	if((dwRetVal=GetAdaptersInfo(pAdapterInfo,&ulOutBufLen))==NO_ERROR)
	{
		pAdapter=pAdapterInfo;
		while(pAdapter)
		{
			if(	pAdapter->Type != MIB_IF_TYPE_LOOPBACK )
			{
				UINT64 u64Mac;
				u64Mac = *(UINT64*)pAdapter->Address;
				
				if( u64Mac != 0 )
					listMAC.push_back(u64Mac);
			}

			pAdapter=pAdapter->Next;
		}
		free(pAdapterInfo);
		return TRUE;
	}
	else
	{
		free(pAdapterInfo);
		return FALSE;
	}
}

//-----------------------------------------------------------------------------
// 取得上网卡的IP地址，使用成吉思汗的方法
//-----------------------------------------------------------------------------
BOOL NetUtil::GetMacAddress(UINT64 &u64Mac)
{
	try
	{
		typedef struct _ASTAT_     
		{
			ADAPTER_STATUS	adapt;
			NAME_BUFFER		NameBuff[ 32 ];
		}ASTAT, *PASTAT;

		typedef struct _LANA_ENUM
		{ // le
			UCHAR	length;
			UCHAR	lana[MAX_LANA];
		}LANA_ENUM;

		NCB ncb;
		memset( &ncb, 0, sizeof( ncb ) );

		ASTAT Adapter;
		LANA_ENUM lana_enum;
		memset( &lana_enum, 0, sizeof( lana_enum ) );

		ncb.ncb_command = NCBENUM;
		ncb.ncb_buffer = (unsigned char*)&lana_enum;
		ncb.ncb_length = sizeof( LANA_ENUM );

		UCHAR uRetCode = Netbios( &ncb );
		if( NRC_GOODRET != uRetCode )
			return FALSE;

		for( int lana=0; lana < lana_enum.length; ++lana )
		{
			ncb.ncb_command = NCBRESET;
			ncb.ncb_lana_num = lana_enum.lana[ lana ];
			uRetCode = Netbios( &ncb );
			if( uRetCode == NRC_GOODRET )
				break;
		};

		if( NRC_GOODRET != uRetCode )
			return FALSE;

		memset( &ncb, 0, sizeof(ncb) );

		ncb.ncb_command	 = NCBASTAT;
		ncb.ncb_lana_num = lana_enum.lana[0];
		strcpy( (char*)ncb.ncb_callname, "*" );
		ncb.ncb_buffer	 = (unsigned char*)&Adapter;
		ncb.ncb_length	 = sizeof( Adapter );

		uRetCode = Netbios( &ncb );
		if( NRC_GOODRET != uRetCode )
			return FALSE;

		u64Mac = *(UINT64*)Adapter.adapt.adapter_address;
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}

}