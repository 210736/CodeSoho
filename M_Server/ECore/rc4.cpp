/*
RC4加密算法
RC4加密算法是大名鼎鼎的RSA三人组中的头号人物Ron Rivest在1987年设计的密钥长度可变的流加密算法簇。
之所以称其为簇，是由于其核心部分的S-box长度可为任意，但一般为256字节。该算法的速度可以达到DES加密的10倍左右，
且具有很高级别的非线性。RC4起初是用于保护商业机密的。但是在1994年9月，它的算法被发布在互联网上，
也就不再有什么商业机密了。RC4也被叫做ARC4（Alleged RC4——所谓的RC4），因为RSA从来就没有正式发布过这个算法。
根据目前的分析结果，没有任何的分析对于密钥长度达到128位的RC4有效，所以，RC4是目前最安全的加密算法之一，大家可以放心使用！
*/
//-----------------------------------------------------------------------------
// File: rc4.h
// Desc: rc4算法
// Auth: 王炜
// Date: 2012/10/13
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "rc4.h"

namespace ECore{
//-----------------------------------------------------------------------------
//	参数1是一个256长度的char型数组，定义为: unsigned char sBox[256];
//	参数2是密钥，其内容可以随便定义：char key[256];
//	参数3是密钥的长度，Len = strlen(key);
//-----------------------------------------------------------------------------
void RC4::init(unsigned char *s, unsigned char *key, unsigned long Len)
{
	int i =0, j = 0, k[256] = {0};
	unsigned char tmp = 0;
	for(i=0;i<256;i++)
	{
		s[i]=i;
		k[i]=key[i%Len];
	}
	for (i=0; i<256; i++)
	{
		j=(j+s[i]+k[i])%256;
		tmp = s[i];
		s[i] = s[j]; //交换s[i]和s[j]
		s[j] = tmp;
	}
}
//-----------------------------------------------------------------------------
//	参数1是上边rc4_init函数中，被搅乱的S-box;
//	参数2是需要加密的数据data;
//	参数3是data的长度;
//-----------------------------------------------------------------------------
void RC4::crypt(unsigned char *s, unsigned char *Data, unsigned long Len)
{
	int x = 0, y = 0, t = 0, i = 0;
	unsigned char tmp;
	for(i=0;i<Len;i++)
	{
		x=(x+1)%256;
		y=(y+s[x])%256;
		tmp = s[x];
		s[x] = s[y]; //交换s[x]和s[y]
		s[y] = tmp;
		t=(s[x]+s[y])%256;
		Data[i] ^= s[t];
	}
}
//-----------------------------------------------------------------------------
bool RC4::encrypt(const char* pKey, const char* pData, char* pRes, int nResLen)
{
	unsigned char Buf[256]={0};
	int Len = strlen(pData);
	if (Len * 2 > nResLen)
		return false;

	PBYTE Data = (PBYTE)malloc(Len + 1);
	memcpy(Data, pData, Len + 1);

	init(Buf, (PBYTE)pKey, strlen(pKey));
	crypt(Buf, (PBYTE)Data, Len);

	for (int i=0; Data[i]; i++)
	{
		sprintf(pRes + i*2, "%02x", Data[i]);
	}

	free(Data);
	return true;
}
//-----------------------------------------------------------------------------
bool RC4::decrypt(const char* pKey, const char* pData, char* pRes, int nResLen)
{
	unsigned char Buf[256]={0};
	if (strlen(pData) > nResLen)
		return false;
	
	memset(pRes, 0, nResLen);
	char* pTmp = pRes;
	while (pData[0] && pData[1])
	{
		sscanf(pData, "%02x", pTmp);
		pData += 2;
		pTmp += 1;
	}

	init(Buf, (PBYTE)pKey, strlen(pKey));
	crypt(Buf, (PBYTE)pRes, strlen(pRes));

	return true;
}
//-----------------------------------------------------------------------------
}