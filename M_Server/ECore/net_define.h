//-----------------------------------------------------------------------------
// author Lyp
// last 2004-07-4
//-----------------------------------------------------------------------------
#pragma once
namespace ECore {



//-----------------------------------------------------------------------------
// ����
//-----------------------------------------------------------------------------
CONST INT NET_BLOCK_TIME = 1000*200;	// 200ms
CONST DWORD SLEEP_TIME = 500;


//-----------------------------------------------------------------------------
// ������Ϣ�����ṹ
//-----------------------------------------------------------------------------
struct tagNetCmd
{
	DWORD	dwID;
	DWORD	dwSize;
};

//-----------------------------------------------------------------------------
// ����������Ϣ�Ķ���
//-----------------------------------------------------------------------------
template<INT> class GetNetMsgName
{
public:
	operator LPCSTR() { return 0; }
};


#ifndef MSG_BEGIN
#define MSG_BEGIN(n)	template<> struct GetNetMsgName<__COUNTER__>{operator LPCSTR(){return #n;}}; \
						struct tag##n:public tagNetCmd{\
							INLINE static DWORD GetCrc() \
							{ \
								static CONST DWORD crc=Crc32(#n); \
								return crc; \
							} \
							tag##n(){ZeroMemory(this,sizeof(*this));dwID=GetCrc();dwSize=sizeof(*this);\
						}

#define MSG_END			}; 
#endif

} // namespace ECore {
