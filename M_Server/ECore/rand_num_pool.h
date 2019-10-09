//--------------------------------------------------------------------------------
//!\file rand_num_pool.h
//!\brief 随机数池，给定种子，生成随机数序列
//!
//!\date 2010-08-17
//! last 2010-08-17
//!\author xlguo
//--------------------------------------------------------------------------------
#pragma once

template<INT SIZE = 100>
class RandNumPool
{
public:
	RandNumPool(INT nSeed):m_nSeed(nSeed)		{ Prepare(SIZE); }

	// 最大为32767
	INT		operator[]( INT8 nIndex )			{ if (nIndex >= SIZE) return 0; else return m_vecRands[nIndex]; }
	INT		Get(INT8 nIndex, INT nMin, INT nMax){ return nMin + (*this)[nIndex] % (nMax-nMin+1); }
private:
	INT		GenRand()							{ return (((m_nSeed = m_nSeed * 214013L + 2531011L) >> 16) & 0x7fff ); }
	VOID	Prepare(INT nNum)	
	{ 
		for (INT i=0; i<nNum; ++i)
		{
			m_vecRands[i] = GenRand();
		}
	}

private:
	INT		m_nSeed;
	INT		m_vecRands[SIZE];
};
