//-----------------------------------------------------------------------------
// author Lyp
// date 2004-07-02
// last 2009-04-03
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "wan_define.h"

namespace ECore {
namespace Wan {



ECORE_API LPCTSTR g_szDisconnectReason[]=
{
	{_T("EDR_FIN")},
	{_T("EDR_Kick")},
	{_T("EDR_FailedAuth")},
	{_T("EDR_AuthTimeOut")},
	{_T("EDR_CPortError")},
	{_T("EDR_CPortBindError")},
	{_T("EDR_TooManyRecv")},
	{_T("EDR_TooManySend")},
	{_T("EDR_TooBigRecv")},
	{_T("EDR_TooBigSend")},
	{_T("EDR_WSASendError")},
	{_T("EDR_WSARecvError")},
};




}
}