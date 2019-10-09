#pragma once

class CUDPSender
{
public:
	CUDPSender(void);
	CUDPSender(DWORD destIP,int destPort);

public:
	~CUDPSender(void);
	BOOL Init();
	void Close();

	void SetDestIP(DWORD destIP);
	void SetDestPort(int destPort);

	int SendData(char* buf,int buflen);

private:
	SOCKET m_socket;
	sockaddr_in m_destSockAddr;
	DWORD m_dwDestIP;
	int m_nDestPort;
};
