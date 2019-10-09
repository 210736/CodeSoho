//-----------------------------------------------------------------------------
// File: bit_util.h
// Desc: 位操作工具函数
// Auth: Aslan
// Date: 2011-04-29
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {

//-----------------------------------------------------------------------------
// 统计一个DWORD以MSB的顺序第一个为1的位前面有多少个0
//-----------------------------------------------------------------------------
INLINE INT CountLeadingZeroBit(UINT32 dwValue)
{
	DWORD dwResult = 0;

	_BitScanReverse(&dwResult, dwValue);

	dwResult ^= 31;

	return dwResult;
}

//-----------------------------------------------------------------------------
// 是否是2的整数次幂
//-----------------------------------------------------------------------------
template <typename T>
INLINE BOOL IsPowerOfTwo(T x)
{
	return (x & (x-1)) == 0;
}

//------------------------------------------------------------------------------
// 计算一个UINT8里面有多少个1
//------------------------------------------------------------------------------
INLINE UINT8 CountBits(UINT8 v)
{
	UINT8 c = v;

	c = ((c>>1) & 0x55) + (c & 0x55);
	c = ((c>>2) & 0x33) + (c & 0x33);
	c = ((c>>4) & 0x0f) + (c & 0x0f);

	return c;
}

//------------------------------------------------------------------------------
// 计算一个UINT16里面有多少个1
//------------------------------------------------------------------------------
INLINE UINT8 CountBits(UINT16 v)
{
	return CountBits((UINT8)(v & 0xff)) + CountBits((UINT8)((v >> 8) & 0xff));
}

//-------------------------------------------------------------------------------
// 计算一个UINT32里面有多少个1
//-------------------------------------------------------------------------------
INLINE UINT8 CountBits(UINT32 v)
{
	return CountBits((UINT8)(v & 0xff)) + CountBits((UINT8)((v >> 8) & 0xff)) + 
		   CountBits((UINT8)((v >> 16) & 0xff)) + CountBits((UINT8)((v >> 24) & 0xff));
}

INLINE UINT8 CountBits(UINT64 v)
{
	return CountBits((UINT8)(v & 0xff)) + CountBits((UINT8)((v >> 8) & 0xff)) + 
		CountBits((UINT8)((v >> 16) & 0xff)) + CountBits((UINT8)((v >> 24) & 0xff)) +
		CountBits((UINT8)((v >> 32) & 0xff)) + CountBits((UINT8)((v >> 40) & 0xff)) +
		CountBits((UINT8)((v >> 48) & 0xff)) + CountBits((UINT8)((v >> 56) & 0xff));
}

}