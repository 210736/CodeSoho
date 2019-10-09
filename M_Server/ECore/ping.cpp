//-----------------------------------------------------------------------------
// author Aslan
// date 2003-03-7
// last 2008-04-7
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "ping.h"

#include "callback.h"
#include "executor_fun.h"
#include "thread_mgr.h"

namespace ECore {

// ICMP packet types
CONST INT ICMP_ECHO_REPLY		= 0;
CONST INT ICMP_DEST_UNREACH		= 3;
CONST INT ICMP_TTL_EXPIRE		= 11;
CONST INT ICMP_ECHO_REQUEST		= 8;
CONST INT ICMP_MIN				= 8;	// min ICMP packet size
CONST INT DEFAULT_PACKET_SIZE	= 32;
CONST INT DEFAULT_TTL			= 128;


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOL Ping::Init(DWORD dwTimeOut)
{
	m_dwTimeOut	= dwTimeOut;
	m_nSerial =	0;
	m_bClose =	0;

	m_mapResult.Clear();
	m_mapThreadID.Clear();
    return TRUE;
}

//----------------------------------------------------------------------------- 
//
//-----------------------------------------------------------------------------
VOID Ping::Destroy()
{
	// 停止所有线程
	m_pThreadMgr->ShutDown();
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
INT Ping::Start(LPCSTR szIP)
{
	// 得到一个新的序列号
	INT nSeqNum = InterlockedIncrementEx((LPLONG)&m_nSerial);

	// 看看是否可以启动ping
	SOCKET sock;
	sockaddr_in dest;
	ZeroMemory(&dest, sizeof(dest));

	if( Setup(szIP, sock, dest) < 0 )
		return INVALID;

	// 生成线程
	CallbackBase* pExec	= new CallbackP3<Ping, INT, SOCKET, sockaddr_in>(this, &Ping::TDPing, nSeqNum, sock, dest);
	CallbackBase* pEnd	= new CallbackP0<Ping>(this, &Ping::CloseTDPing);

	DWORD dwThreadID = m_pThreadMgr->CreateThread(new ExecutorFun(pExec, pEnd), TRUE);

	if( VALID(dwThreadID) )
	{
		m_mapThreadID.Add(nSeqNum, dwThreadID);
		return nSeqNum;
	}

	return INVALID;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
VOID Ping::CloseAll()
{
	// 关闭线程管理器
	m_pThreadMgr->ShutDown();

	// 重置
	InterlockedExchangeEx((LPLONG)&m_nSerial, 0);
	InterlockedExchangeEx((LPLONG)&m_bClose, FALSE);

	m_mapResult.Clear();
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOL Ping::IsFinished(INT nHandle)
{
	if( !m_mapResult.IsExist(nHandle) || !m_mapThreadID.IsExist(nHandle) )
		return FALSE;

	return TRUE;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
INT Ping::GetResult(INT nHandle)
{
	if( !m_mapResult.IsExist(nHandle) || !m_mapThreadID.IsExist(nHandle) )
		return INVALID;

	DWORD dwThreadID = m_mapThreadID.Peek(nHandle);
	m_pThreadMgr->StopThread(dwThreadID);


	// 注意，下一句也可能返回GT_INVALID，因为可能远程主机无法ping通
	return m_mapResult.Peek(nHandle);

}


//-----------------------------------------------------------------------------
// ping线程
//-----------------------------------------------------------------------------
DWORD Ping::TDPing(INT nSeqNum, SOCKET sock, sockaddr_in dest)
{
	// 获得IP
	DWORD dwAddr = dest.sin_addr.S_un.S_addr;

	// 计算重试次数, 其中GT_NET_BLOCK_TIME是以微秒为单位
	INT nRetryTimes = m_dwTimeOut / (NET_BLOCK_TIME/1000);

	// 首先初始化接收缓冲
	sockaddr_in source;
	ZeroMemory(&source, sizeof(source));
	int len = sizeof(source);
	INT packet_size = EMax(sizeof(ICMPHeader), DEFAULT_PACKET_SIZE);

	CONST INT MAX_PING_DATA_SIZE	= 1024;
	CONST INT MAX_PING_PACKET_SIZE	= (MAX_PING_DATA_SIZE + sizeof(IPHeader));

	TCHAR szRcvTemp[MAX_PING_PACKET_SIZE];
	IPHeader* recv_buf = (IPHeader*)szRcvTemp;

	// 再初始化发送缓冲
	TCHAR szSendTemp[DEFAULT_PACKET_SIZE];
	ICMPHeader* send_buf = (ICMPHeader*)szSendTemp;
	InitPacket(send_buf, DEFAULT_PACKET_SIZE, nSeqNum);

	// 发送Ping
	if( Send(sock, dest, send_buf, DEFAULT_PACKET_SIZE ) < 0 )
		return INVALID;

	// 接收返回
	while( !m_bClose && nRetryTimes-- >= 0 )
	{
		// 设置阻塞时间
		fd_set fdread;

		timeval block_time;
		block_time.tv_sec = 0;
		block_time.tv_usec = NET_BLOCK_TIME;

		// "select" will change fdread, so this must put in the loop
		// Wait for the ping reply
		FD_ZERO(&fdread);
		FD_SET(sock, &fdread);
		INT nResult = select(0, &fdread, NULL, NULL, &block_time);
		if( nResult == 1 )
		{
			nResult = recvfrom(sock, (char*)recv_buf, 
				MAX_PING_PACKET_SIZE + sizeof(IPHeader), 0, (sockaddr*)&source, &len);

			unsigned short header_len = recv_buf->h_len * 4;
			ICMPHeader* icmphdr = (ICMPHeader*)((char*)recv_buf + header_len);

			INT nPackageSeq = icmphdr->seq;
			if( nPackageSeq != nSeqNum )
				continue;

			nResult = DecodeReply(recv_buf, packet_size, &source);

			// 存放结果后退出
			m_mapResult.Add(nPackageSeq, nResult);
			closesocket(sock);
			return 0;
		}

		if( SOCKET_ERROR == nResult )
		{
			ERR(_T("recvfrom failed in ping thread"));
			break;
		}
	}

	m_mapResult.Add(nSeqNum, INVALID);
	closesocket(sock);
	return INVALID;
	
}


//-----------------------------------------------------------------------------
// Creates ping packets
//-----------------------------------------------------------------------------
INT Ping::Setup(LPCSTR host, SOCKET& sd, sockaddr_in& dest)
{
    // Create the socket
    sd = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, 0, 0, 0);
    if( sd == INVALID_SOCKET )
	{
        ERR(_T("Failed to create raw socket"));
        return INVALID;
    }

	INT ttl = DEFAULT_TTL;
    if( SOCKET_ERROR == setsockopt(sd, IPPROTO_IP, IP_TTL, (const char*)&ttl, sizeof(ttl)) ) 
	{
        ERR(_T("TTL setsockopt failed"));
        return INVALID;
    }

    // Initialize the destination host info block
    memset(&dest, 0, sizeof(dest));

    // Turn first passed parameter into an IP address to ping
    unsigned int addr = inet_addr(host);
    if (addr != INADDR_NONE) 
	{
        // It was a dotted quad number, so save result
        dest.sin_addr.s_addr = addr;
        dest.sin_family = AF_INET;
    }
    else 
	{
        // Not in dotted quad form, so try and look it up
        hostent* hp = gethostbyname(host);
        if (hp != 0) 
		{
            // Found an address for that host, so save it
            memcpy(&(dest.sin_addr), hp->h_addr, hp->h_length);
            dest.sin_family = hp->h_addrtype;
        }
        else 
		{
            // Not a recognized hostname either!
            ERR(_T("Failed to resolve"));
            return INVALID;
        }
    }

    return 0;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
VOID Ping::InitPacket(ICMPHeader* icmp_hdr, INT packet_size, INT seq_no)
{
    // Set up the packet's fields
    icmp_hdr->type = ICMP_ECHO_REQUEST;
    icmp_hdr->code = 0;
    icmp_hdr->checksum = 0;
    icmp_hdr->id = (USHORT)GetCurrentProcessId();
    icmp_hdr->seq = seq_no;
    icmp_hdr->timestamp = timeGetTime();

    // "You're dead meat now, packet!"
    const unsigned long int deadmeat = 0xDEADBEEF;
    char* datapart = (char*)icmp_hdr + sizeof(ICMPHeader);
    int bytes_left = packet_size - sizeof(ICMPHeader);
    while( bytes_left > 0 )
	{
        memcpy(datapart, &deadmeat, EMin(int(sizeof(deadmeat)), bytes_left));
        bytes_left -= sizeof(deadmeat);
        datapart += sizeof(deadmeat);
    }

    // Calculate a checksum on the result
    icmp_hdr->checksum = Checksum((USHORT*)icmp_hdr, packet_size);
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
INT Ping::Send(SOCKET sd, const sockaddr_in& dest, ICMPHeader* send_buf, 
					INT packet_size)
{
    // Send the ping packet in send_buf as-is
    int bwrote = sendto(sd, (char*)send_buf, packet_size, 0, 
            (sockaddr*)&dest, sizeof(dest));
    
	if( bwrote == SOCKET_ERROR ) 
        return INVALID;

    return 0;
}


//-----------------------------------------------------------------------------
// -1:failure, -2:"try again" and 0 on success.
//-----------------------------------------------------------------------------
INT Ping::DecodeReply(IPHeader* reply, INT bytes, sockaddr_in* from) 
{
    // Skip ahead to the ICMP header within the IP packet
    unsigned short header_len = reply->h_len * 4;
    ICMPHeader* icmphdr = (ICMPHeader*)((char*)reply + header_len);

    // Make sure the reply is sane
    if (bytes < header_len + ICMP_MIN) 
	{
        ERR(_T("too few bytes from %s"), inet_ntoa(from->sin_addr));
        return INVALID;
    }
    else if( icmphdr->type != ICMP_ECHO_REPLY )
	{
        if( icmphdr->type != ICMP_TTL_EXPIRE ) 
		{
            if( icmphdr->type != ICMP_DEST_UNREACH )
                ERR(_T("Unknown ICMP packet type %d received"), int(icmphdr->type));

            return INVALID;
        }
    }
    else if( icmphdr->id != (USHORT)GetCurrentProcessId() ) 
	{
        // Must be a reply for another pinger running locally
        return -2;
    }

	if (icmphdr->type == ICMP_TTL_EXPIRE) 
		return INVALID;

	return timeGetTime() - icmphdr->timestamp;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
USHORT Ping::Checksum(USHORT* buffer, INT size) 
{
    unsigned long cksum = 0;
    while (size > 1)
	{
        cksum += *buffer++;
        size -= sizeof(USHORT);
    }

    if (size)
        cksum += *(UCHAR*)buffer;

    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >> 16);
    
    return (USHORT)(~cksum);
}


} // namespace ECore {
