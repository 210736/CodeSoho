//-----------------------------------------------------------------------------
//!\author Aslan
//!\date 2003-03-7
//! last 2008-04-7
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class ECORE_API Ping
{
public:
	//-------------------------------------------------------------------------
	// dwTimeOut:ping 超时时间, 单位毫秒
	//-------------------------------------------------------------------------
	BOOL Init(DWORD dwTimeOut);
	VOID Destroy();

	//-------------------------------------------------------------------------
	// 开始ping操作,失败返回GT_INVALID
	//-------------------------------------------------------------------------
	INT	 Start(LPCSTR szIP);

	//-------------------------------------------------------------------------
	// 关闭所有ping的线程,会删除已经ping得的结果
	//-------------------------------------------------------------------------
	VOID CloseAll();

	//-------------------------------------------------------------------------
	// 检查ping是否结束
	//-------------------------------------------------------------------------
	BOOL IsFinished(INT nHandle);

	//-------------------------------------------------------------------------
	// 成功返回ping时间,ping尚未结束或者失败返回GT_INVALID
	//-------------------------------------------------------------------------
	INT	 GetResult(INT nHandle);

	


private:

#pragma pack(push, 1)

	//-------------------------------------------------------------------------
	// IP包头
	//-------------------------------------------------------------------------
	struct IPHeader
	{
		BYTE	h_len:4;		// Length of the header in dwords
		BYTE	version:4;		// Version of IP, 4 bits
		BYTE	tos;			// Type of service
		USHORT	total_len;		// Length of the packet in dwords
		USHORT	ident;			// unique identifier
		USHORT	flags;			// Flags
		BYTE	ttl;			// Time to live
		BYTE	proto;			// Protocol number (TCP, UDP etc)
		USHORT	checksum;		// IP checksum
		ULONG	source_ip;
		ULONG	dest_ip;
	};

	//---------------------------------------------------------------------------
	// ICMP包头
	//---------------------------------------------------------------------------
	struct ICMPHeader
	{
		BYTE	type;			// ICMP packet type
		BYTE	code;			// Type sub code
		USHORT	checksum;
		USHORT	id;
		USHORT	seq;
		ULONG	timestamp;		// not part of ICMP, but we need it
	};

	//---------------------------------------------------------------------------
	// IP选项
	//---------------------------------------------------------------------------
	typedef struct
	{
		unsigned char Ttl;					// Time To Live
		unsigned char Tos;					// Type Of Service
		unsigned char Flags;				// IP header flags
		unsigned char OptionsSize;			// Size in bytes of options data
		unsigned char *OptionsData;			// Pointer to options data
	} IP_OPTION_INFORMATION, * PIP_OPTION_INFORMATION;

	//---------------------------------------------------------------------------
	// PING返回
	//---------------------------------------------------------------------------
	typedef struct
	{
		DWORD			Address;			// Replying address
		unsigned long	Status;				// Reply status
		unsigned long	RoundTripTime;		// RTT in milliseconds
		unsigned short	DataSize;			// Echo data size
		unsigned short	Reserved;			// Reserved for system use
		void			*Data;				// Pointer to the echo data
		IP_OPTION_INFORMATION Options;		// Reply options
	} IP_ECHO_REPLY, * PIP_ECHO_REPLY;

#pragma pack(pop)


	TObj<ThreadMgr>	m_pThreadMgr;
	DWORD VOLATILE 	m_dwTimeOut;
	BOOL VOLATILE 	m_bClose;
	INT VOLATILE 	m_nSerial;

	MapTS<INT, INT>		m_mapResult;
	MapTS<INT, DWORD>	m_mapThreadID;


	//----------------------------------------------------------------------------
	// ping相关的工作函数
	//----------------------------------------------------------------------------
	INT		Setup(LPCSTR host, SOCKET& sd, sockaddr_in& dest);
	INT		Send(SOCKET sd, const sockaddr_in& dest, ICMPHeader* send_buf, INT packet_size);
	INT		DecodeReply(IPHeader* reply, INT bytes, sockaddr_in* from);
	VOID	InitPacket(ICMPHeader* icmp_hdr, INT packet_size, INT seq_no);
	USHORT	Checksum(USHORT* buffer, INT size);

	DWORD	TDPing(INT nSeqNum, SOCKET sock, sockaddr_in dest);
	DWORD	CloseTDPing() { InterlockedExchangeEx((LPLONG)&m_bClose, TRUE); return 0;}
};


} // namespace ECore {




