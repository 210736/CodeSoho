#include "StdAfx.h"
#include "UDPSender.h"

CUDPSender::CUDPSender(void)
{
	ZeroMemory(&m_destSockAddr, sizeof(m_destSockAddr));
	m_dwDestIP = 0;
	m_nDestPort = 0;
}

CUDPSender::CUDPSender(DWORD destIP,int destPort)
{
	//加载套接字库

	WORD wVersionRequested;
	WSADATA wsadata;
	int err;

	wVersionRequested=MAKEWORD(2,2);
	err=WSAStartup(wVersionRequested,&wsadata);
	if (err!=0)
	{
		TRACE("套接字库加载失败！");
	}

	if (LOBYTE(wsadata.wVersion)!=2 || HIBYTE(wsadata.wVersion)!=2)
	{
		WSACleanup();
		TRACE("套接字库2.0版本加载失败！");
	}

	ZeroMemory(&m_destSockAddr, sizeof(m_destSockAddr));
	m_dwDestIP = destIP;
	m_nDestPort = destPort;

	m_destSockAddr.sin_family = AF_INET;
	m_destSockAddr.sin_addr.S_un.S_addr = htonl(m_dwDestIP);
	m_destSockAddr.sin_port = htons(m_nDestPort);
}


CUDPSender::~CUDPSender(void)
{
	if (m_socket!=INVALID_SOCKET)
	{
		closesocket(m_socket);
	}
	m_socket=NULL;
	//关闭套接字库
	WSACleanup();
}

BOOL CUDPSender::Init()
{
	m_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_socket == INVALID_SOCKET)
	{
		TRACE("创建发送套接字失败！");
		return FALSE;
	}

	//bool noLoop(FALSE);
	//join_multi_tag=setsockopt(m_socket,IPPROTO_IP,IP_MULTICAST_LOOP,(char*)&noLoop,sizeof(bool)); 

	//设置ttl
	int ttl=64;
	if (SOCKET_ERROR == setsockopt(m_socket, IPPROTO_IP, IP_TTL, (char*)&ttl, sizeof(ttl)))
	{
		TRACE("setsockopt之IP_TTL出错！");
		return FALSE;
	}

	if (SOCKET_ERROR == setsockopt(m_socket, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&ttl, sizeof(ttl)))
	{
		TRACE("setsockopt之IP_MULTICAST_TTL出错！");
		return FALSE;
	}
	
	return TRUE;
}

void CUDPSender::Close()
{
	int closetag=closesocket(m_socket);
	if (closetag==SOCKET_ERROR )
	{
		TRACE("关闭发送套接字出错!");
	}
	m_socket=NULL;
}

void CUDPSender::SetDestIP(DWORD destIP)
{
	m_dwDestIP = destIP;
	m_destSockAddr.sin_addr.S_un.S_addr = htonl(m_dwDestIP);
}

void CUDPSender::SetDestPort(int destPort)
{
	m_nDestPort = destPort;
	m_destSockAddr.sin_port = htons(m_nDestPort);
}

int CUDPSender::SendData(char* buf, int buflen)
{
	int sent_len=0;//实际发送了的字节数目
	int dest_len=sizeof(SOCKADDR);

	sent_len = sendto(m_socket,buf,buflen,0,(struct sockaddr*)&m_destSockAddr,dest_len);

	//发送正常时就返回实际发送了的数据字节数目
	if (sent_len>0)
	{
		return sent_len;
	}

	if (sent_len==SOCKET_ERROR)
	{
		return FALSE;
	}

	return sent_len;
}
