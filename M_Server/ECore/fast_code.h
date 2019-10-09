#pragma once

namespace ECore
{
#define FP_BITS(fp) (* (DWORD *) &(fp))

INLINE FLOAT FastSqrt(FLOAT f)
{
	if (FP_BITS(f) == 0) // Check for square root of 0
		return 0.0f; 

	FP_BITS(f) = g_SqrtTable[(FP_BITS(f) >> 8) & 0xFFFF] | 
		((((FP_BITS(f) - 0x3F800000) >> 1) + 0x3F800000) & 0x7F800000);
	return f;
}

#undef FP_BITS

	
}//namespace ECore