//-----------------------------------------------------------------------------
// Auth: Lyp
// Date: 2007/11/14
//-----------------------------------------------------------------------------
#pragma once

namespace ECore{
//-----------------------------------------------------------------------------
//!\brief stdio �ض���
//!
//!
//-----------------------------------------------------------------------------
class ECORE_API IORedir
{
public:
	BOOL Init();
	VOID Destroy();

	LPBYTE	Recv(DWORD& dwSize);
	BOOL	Send(LPVOID pMsg, DWORD dwMsgSize);

	//! ��Ӧ��Recv(),��Ϊ�ڴ����ڲ�����ģ�����Ӧ�ý����ڲ����ͷ�
	VOID FreeRecved(LPBYTE pMsg);

private:

	HANDLE		m_hProcess;
	HANDLE		m_hStdoutRdDup;
	HANDLE		m_hStdinWrDup;
};

} // namespace ECore {