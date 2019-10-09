
#ifndef _DUPAPI_H
#define _DUPAPI_H
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DUPAPI_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DUPAPI_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef DUPAPI_EXPORTS
#define DUPAPI_API __declspec(dllexport)
#else
#define DUPAPI_API __declspec(dllimport)
#endif


#define DUP_SUCCESS					0
#define ERROR_DUP_TIMEOUT			1
#define ERROR_DUP_INVALID_ADDRESS	2
#define ERROR_DUP_NOT_PRIMARY		3
#define ERROR_DUP_STATUS			4
#define ERROR_DUP_MODE				5
#define ERROR_DUP_LINE				6
#define ERROR_DUP_INVALID_PARAMETER 7

//主副机状态 
typedef struct _DUP_PS_STATUS{
	unsigned char StatusOfThis;
	unsigned char StatusOfOther;
}DUP_PS_STATUS, *PDUP_PS_STATUS;

//双工状态
typedef struct _DUP_STATUS{
	unsigned char StatusOfThis;
	unsigned char StatusOfOther;
	unsigned char Line1Status;
	unsigned char Line2Status;
	unsigned char SoftStatusOfThis;
	unsigned char NetStatusOfThis;
	unsigned char Mode;
	unsigned char SoftStatusOfOther;
	unsigned char NetStatusOfOther;
}DUP_STATUS, *PDUP_STATUS;

#ifdef __cplusplus
extern "C" {
#endif

//请求切换
int DupSwitch(void);

//获取主/副机状态
int DupGetPSStatus(DUP_PS_STATUS *pPSStatus);

//等待主/副机状态改变
int DupWaitPSStatusChg(unsigned int *pStatusChanged, DUP_PS_STATUS *pPSStatus, DWORD msTimeOut);

//获取双工状态
int DupGetStatus(DUP_STATUS *pDupStatus);

//等待双工状态改变
int DupWaitStatusChg(unsigned int *pStatusChanged, DUP_STATUS *pDupStatus, DWORD msTimeOut);

//强制设置主/副机
int DupSetPSStatus(unsigned int iStatus);

//恢复自动方式
int DupAutoMode(void);

#ifdef __cplusplus
}
#endif


#endif //_DUPAPI_H