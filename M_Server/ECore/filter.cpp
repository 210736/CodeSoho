//-----------------------------------------------------------------------------
// Auth: Lyp
// Time: 2003-1-8
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "filter.h"

#include "epk_define.h"
#include "lzo\lzo_asm.h"
#include "lzo\lzoconf.h"
#include "lzo\lzo1x.h"
#include "zlib\zlib.h"
#include "lzma\LzmaEnc.h"
#include "lzma\LzmaDec.h"

namespace ECore{


static void *SzAlloc(void *p, size_t size) { p = p; return g_pMemCache->Alloc(size); }
static void SzFree(void *p, void *address) { p = p; g_pMemCache->Free(address); }
ISzAlloc g_Alloc = { SzAlloc, SzFree };

struct CLzmaEnc;
CLzmaEnc* f_pLzmaEnc;
BYTE f_byLZMAPROP[256];
//-----------------------------------------------------------------------------
// construction / destruction
//-----------------------------------------------------------------------------
Filter::Filter()
{
	Init(EF_Lzo1X);	// minilzo需要初始化
}


//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL Filter::Init(EFilter eFilter)
{
	lzo_init();

	CLzmaEncProps props;

	ZeroMemory(f_byLZMAPROP, sizeof(f_byLZMAPROP));
	ZeroMemory(&props, sizeof(props));

	LzmaEncProps_Init(&props);
	f_pLzmaEnc = (CLzmaEnc *)LzmaEnc_Create(&g_Alloc);
	SRes res = LzmaEnc_SetProps(f_pLzmaEnc, &props);

	size_t outPropsSize = 5;
	LzmaEnc_WriteProperties(f_pLzmaEnc, f_byLZMAPROP, &outPropsSize);

	return TRUE;
}


//-----------------------------------------------------------------------------
// 检查各种过滤器算法需要的额外运算空间
//-----------------------------------------------------------------------------
DWORD Filter::GetExtSize(DWORD dwSize, EFilter eFilter)
{
	return (dwSize/64) + 16 + 3 + 4096;
}

#pragma warning(disable:4996)
//-----------------------------------------------------------------------------
// 过滤
//-----------------------------------------------------------------------------
VOID Filter::Filt(tagFiltParam *pParam)
{
	switch(pParam->eFilter)
	{
	case EF_Changeless:
		memcpy(pParam->pMemOut, pParam->pMemIn, pParam->stInData);
		pParam->stOutData = pParam->stInData;
		break;

	case EF_Lzo1X:
		if( pParam->bCompress )	// compress
		{
			// 必须用局部变量，因为外部可能多线程调用此函数
			long __LZO_MMODEL LzoWorkMem[((LZO1X_1_MEM_COMPRESS)+(sizeof(long)-1))/sizeof(long)];

			lzo1x_1_compress((lzo_byte*)pParam->pMemIn, (lzo_uint)pParam->stInData,
				(lzo_byte*)pParam->pMemOut, (lzo_uint*)&(pParam->stOutData), LzoWorkMem);
/*
			if( pParam->stOutData > pParam->stInData)
			{
				memcpy(pParam->pMemOut, pParam->pMemIn, pParam->stInData);
				pParam->eFilter = EF_Changeless;
				pParam->stOutData = pParam->stInData;
			}
			*/
		}
		else	// decompress
		{
#ifndef X64
			// 使用汇编优化
			lzo1x_decompress_asm_fast((lzo_byte*)pParam->pMemIn, (lzo_uint)pParam->stInData,
				(lzo_byte*)pParam->pMemOut, (lzo_uint*)&(pParam->stOutData), NULL);
#else
			lzo1x_decompress((lzo_byte*)pParam->pMemIn, pParam->stInData,
				(lzo_byte*)pParam->pMemOut, (lzo_uint*)&(pParam->stOutData), NULL);
#endif
		}
		break;

	case EF_Lzo1X999:
		if( pParam->bCompress )	// compress
		{
			// 必须用局部变量，因为外部可能多线程调用此函数
			long __LZO_MMODEL LzoWorkMem999[((LZO1X_999_MEM_COMPRESS)+(sizeof(long)-1))/sizeof(long)];

			lzo1x_999_compress((lzo_byte*)pParam->pMemIn, pParam->stInData,
				(lzo_byte*)pParam->pMemOut, (lzo_uint*)&(pParam->stOutData), LzoWorkMem999);

			lzo1x_optimize((lzo_byte*)pParam->pMemOut, pParam->stOutData,
				(lzo_byte*)pParam->pMemIn, (lzo_uint*)&(pParam->stInData), LzoWorkMem999);


			if( pParam->stOutData >= pParam->stInData)
			{
				memcpy(pParam->pMemOut, pParam->pMemIn, pParam->stInData);
				pParam->eFilter = EF_Changeless;
				pParam->stOutData = pParam->stInData;
			}
		}
		else	// decompress
		{
#ifndef X64
			// 使用汇编优化
			lzo1x_decompress_asm_fast((lzo_byte*)pParam->pMemIn, (lzo_uint)pParam->stInData,
				(lzo_byte*)pParam->pMemOut, (lzo_uint*)&(pParam->stOutData), NULL);
#else
			lzo1x_decompress((lzo_byte*)pParam->pMemIn, pParam->stInData,
				(lzo_byte*)pParam->pMemOut, (lzo_uint*)&(pParam->stOutData), NULL);
#endif
		}
		break;

	case EF_LZMA:
		if( pParam->bCompress )
		{
			pParam->stOutData = pParam->dwOutBuffer;

			m_Lock.Lock();
			SRes res = LzmaEnc_MemEncode(f_pLzmaEnc, (BYTE*)pParam->pMemOut, (SizeT*)&(pParam->stOutData), 
				(BYTE*)pParam->pMemIn, pParam->stInData, 0, 0, &g_Alloc, &g_Alloc);
			m_Lock.Unlock();

			if( pParam->stOutData > pParam->stInData)
			{
				memcpy(pParam->pMemOut, pParam->pMemIn, pParam->stInData);
				pParam->eFilter = EF_Changeless;
				pParam->stOutData = pParam->stInData;
			}
		}
		else
		{
			pParam->stOutData = pParam->dwOutBuffer;

			m_Lock.Lock();
			ELzmaStatus status;
			SRes res = LzmaDecode((BYTE*)pParam->pMemOut, (SizeT*)&(pParam->stOutData), 
				(BYTE*)pParam->pMemIn, (SizeT*)&(pParam->stInData), f_byLZMAPROP, LZMA_PROPS_SIZE, LZMA_FINISH_ANY, &status, &g_Alloc);
			m_Lock.Unlock();
		}
		break;

	default:
		ASSERT(0);
		break;
	}

}

} // namespace ECore{
