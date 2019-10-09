#include "stdafx.h"
#include "UDPReceiver.h"

CUDPReceiver::CUDPReceiver(void)
{
	m_dwLocalIP = 0;
	m_nLocalPort = 0;
	m_dwGroupIP = 0;
}

CUDPReceiver::CUDPReceiver(DWORD localIP,int localPort,DWORD groupIP)
{
	m_dwLocalIP = localIP;
	m_nLocalPort = localPort;
	m_dwGroupIP = groupIP;
	//
	//加载套接字库
	WORD wVersionRequested;
	WSADATA wsadata;
	int err;
	wVersionRequested=MAKEWORD(2,2);
	err=WSAStartup(wVersionRequested,&wsadata);
	if (err!=0)
	{
		//return FALSE;
		TRACE("套接字库加载失败！");
	}
	//
	if (LOBYTE(wsadata.wVersion)!=2||HIBYTE(wsadata.wVersion)!=2)
	{
		WSACleanup();
		//return FALSE;
		TRACE("套接字库2.0版本加载失败！");
	}
}

CUDPReceiver::~CUDPReceiver(void)
{
	if (m_socket!=INVALID_SOCKET)
	{
		closesocket(m_socket);
	}
	m_socket=NULL;
	//关闭套接字库
	WSACleanup();
}

BOOL CUDPReceiver::createsocket()
{
	m_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_socket == INVALID_SOCKET)
	{
		TRACE("创建接收套接字失败！");
		return FALSE;
	}
	int  iMode=1;
	ioctlsocket(m_socket,FIONBIO,(u_long FAR*)&iMode);
	return TRUE;
//非阻塞 modify zw 
}

BOOL CUDPReceiver::bindsocket()
{
	sockaddr_in localSockAddr;
	memset(&localSockAddr, 0, sizeof(localSockAddr));
	localSockAddr.sin_family = AF_INET;
	localSockAddr.sin_addr.S_un.S_addr = htonl(m_dwLocalIP);
	localSockAddr.sin_port = htons(m_nLocalPort);

	//设置套接字属性：允许端口复用
	const int reuse = 1;
	if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse)) == SOCKET_ERROR)
	{
		TRACE("setsockopt address reuse failed!");
		int errorcode=WSAGetLastError();
		CString err;
		err.Format(_T("错误代码%d"),errorcode);
		TRACE(err);
		return FALSE;
	}
	//绑定套接字
	int bindtag;
    bindtag=bind(m_socket,(SOCKADDR*)&localSockAddr,sizeof(SOCKADDR));
	if (SOCKET_ERROR==bindtag)
	{
		closesocket(m_socket);
		TRACE("bind receiver socket failed!");
		int errorcode=WSAGetLastError();
		CString err;
		err.Format(_T("错误代码%d"),errorcode);
		TRACE(err);
		return FALSE;	
	}
	//双网卡时，设置接收网卡的套接字属性设置如下：
	DWORD NetOrderIP = htonl(m_dwLocalIP);
	if (setsockopt(m_socket, IPPROTO_IP, IP_MULTICAST_IF, (char*)&NetOrderIP, sizeof(NetOrderIP)) == SOCKET_ERROR)
	{
		TRACE("组播接收设置本机接收IP地址失败！");
		int errorcode=WSAGetLastError();
		CString err;
		err.Format(_T("错误代码%d"),errorcode);
		TRACE(err);
		return FALSE;
	}

    return TRUE;
}

BOOL CUDPReceiver::init(int udptype)
{
	if (udptype==UNICAST)
	{
		//single receiver init: createsocket and bind socket
		return  (createsocket()&&bindsocket());
		//
	}
	if (udptype==MULTICAST)
	{
		//multi receiver init: createsocket, bindsocket and joinmulticastgroup
		return  (createsocket()&&bindsocket()&&JoinMultiCastGroup());
		//
	}
	return FALSE;
	//
}

BOOL CUDPReceiver::JoinMultiCastGroup()
{
	ip_mreq ipmreq;
	//组播IP
	ipmreq.imr_multiaddr.S_un.S_addr = htonl(m_dwGroupIP);
	//本地接收IP
	ipmreq.imr_interface.S_un.S_addr = htonl(m_dwLocalIP);
	//设置套接字属性
	int join_multi_tag;
	join_multi_tag=setsockopt(m_socket,IPPROTO_IP,IP_ADD_MEMBERSHIP,(char FAR*)&ipmreq,sizeof(ipmreq));
	//
	if (join_multi_tag==SOCKET_ERROR)
	{
		TRACE("加入组播组失败!");
		int errorcode=WSAGetLastError();
		CString err;
		err.Format(_T("错误代码%d"),errorcode);
		TRACE(err);
		return FALSE;
	}

	return TRUE;
}

void CUDPReceiver::SetReadEvent(HANDLE& pHandle)
{
	//为套接字设置一个读取事件，一旦数据到达套接字，读事件就被激活，用于通知
	//应用程序开始接收数据
	WSAEventSelect(m_socket,pHandle,FD_READ);
}

//需要前面加上内联吗？？什么是内联
DWORD CUDPReceiver::GetFromIP()
{
	DWORD fromip;
	fromip=ntohl(m_fromSockAddr.sin_addr.S_un.S_addr);
	return fromip;
}

int CUDPReceiver::GetFromPort()
{
	int nFromPort = ntohs(m_fromSockAddr.sin_port);
	return nFromPort;
}

int CUDPReceiver:: ReadData(char* buf,int buflen)
{
	//先把m_fromSockAddr清空
	memset(&m_fromSockAddr,0,sizeof(sockaddr_in));
	//清空接收缓存区
	memset(buf,0,buflen);
	//recv_len是实际接收到的字节数目
	int recv_len=0;
	int from_len=sizeof(SOCKADDR);
	recv_len=recvfrom(m_socket,buf,buflen,0,(struct sockaddr*)&m_fromSockAddr,&from_len);
	//
	//接收正常时就返回实际收到数据字节数目
	if (recv_len>0)
	{
		return recv_len;
	}
	//出错时就返回FALSE，调试时可看到错误代码
	int errorcode=WSAGetLastError();
	CString err;
	err.Format(_T("错误代码%d"),errorcode);
	return FALSE;
}

void CUDPReceiver::close()
{
	int closetag=closesocket(m_socket);
	if (closetag==SOCKET_ERROR )
	{
		TRACE("Error accurs when closing reveiver socket!");
		int errorcode=WSAGetLastError();
		CString err;
		err.Format(_T("错误代码%d"),errorcode);
		TRACE(err);
	}
	m_socket=NULL;
}

