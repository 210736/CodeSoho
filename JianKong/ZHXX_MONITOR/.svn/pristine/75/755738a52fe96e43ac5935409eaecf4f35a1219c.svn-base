#pragma once
//UDP的通信类型：单播，组播，广播
enum UDPType{
	UNICAST=1,
	MULTICAST,
	BROADCAST
};

class CUDPReceiver
{
public:
	CUDPReceiver(DWORD localIP,int localPort,DWORD groupIP);
	CUDPReceiver(void);
public:
	virtual ~CUDPReceiver(void);
	
	BOOL init(int udptype); //单播和组播接收是不一样的初始化  
	//负责创建，绑定，或许还有加入多播组等所有读数据前的初始化操作
	void close();  //套接字库的关闭，套接字的关闭
	void SetReadEvent(HANDLE& pHandle);  //为套接字设置读取事件
	int ReadData(char* buf,int buflen);
	DWORD GetFromIP();
	int GetFromPort();

private:
	BOOL createsocket();  //套接字库的打开，套接字的创建
	BOOL bindsocket();  //将套接字与本机IP和端口绑定
	BOOL JoinMultiCastGroup();//加入多播组

private:
	int        m_nLocalPort;
	DWORD m_dwLocalIP;
	DWORD m_dwGroupIP;
	SOCKET m_socket;
	sockaddr_in m_fromSockAddr;//发送方的地址
};
