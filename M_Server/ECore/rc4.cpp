/*
RC4�����㷨
RC4�����㷨�Ǵ���������RSA�������е�ͷ������Ron Rivest��1987����Ƶ���Կ���ȿɱ���������㷨�ء�
֮���Գ���Ϊ�أ�����������Ĳ��ֵ�S-box���ȿ�Ϊ���⣬��һ��Ϊ256�ֽڡ����㷨���ٶȿ��ԴﵽDES���ܵ�10�����ң�
�Ҿ��кܸ߼���ķ����ԡ�RC4��������ڱ�����ҵ���ܵġ�������1994��9�£������㷨�������ڻ������ϣ�
Ҳ�Ͳ�����ʲô��ҵ�����ˡ�RC4Ҳ������ARC4��Alleged RC4������ν��RC4������ΪRSA������û����ʽ����������㷨��
����Ŀǰ�ķ��������û���κεķ���������Կ���ȴﵽ128λ��RC4��Ч�����ԣ�RC4��Ŀǰ�ȫ�ļ����㷨֮һ����ҿ��Է���ʹ�ã�
*/
//-----------------------------------------------------------------------------
// File: rc4.h
// Desc: rc4�㷨
// Auth: ���
// Date: 2012/10/13
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "rc4.h"

namespace ECore{
//-----------------------------------------------------------------------------
//	����1��һ��256���ȵ�char�����飬����Ϊ: unsigned char sBox[256];
//	����2����Կ�������ݿ�����㶨�壺char key[256];
//	����3����Կ�ĳ��ȣ�Len = strlen(key);
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
		s[i] = s[j]; //����s[i]��s[j]
		s[j] = tmp;
	}
}
//-----------------------------------------------------------------------------
//	����1���ϱ�rc4_init�����У������ҵ�S-box;
//	����2����Ҫ���ܵ�����data;
//	����3��data�ĳ���;
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
		s[x] = s[y]; //����s[x]��s[y]
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