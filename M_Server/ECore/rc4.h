//-----------------------------------------------------------------------------
// File: rc4.h
// Desc: rc4Ëã·¨
// Auth: Íõì¿
// Date: 2012/10/13
//-----------------------------------------------------------------------------
#pragma once
//-----------------------------------------------------------------------------
namespace ECore {
//-----------------------------------------------------------------------------
class ECORE_API RC4
{
private:
	void init(unsigned char *s, unsigned char *key, unsigned long Len);
	void crypt(unsigned char *s, unsigned char *Data, unsigned long Len);
public:
	bool encrypt(const char* pKey, const char* pData, char* pRes, int nResLen);
	bool decrypt(const char* pKey, const char* pData, char* pRes, int nResLen);
};
//-----------------------------------------------------------------------------
}
