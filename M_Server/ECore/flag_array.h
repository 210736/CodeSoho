//-------------------------------------------------------------------------------
// filename: flag_array.h
// author: aslan
// actor:
// data: 2010-03-01
// last:
// brief: 无符号整型位标示数组
//-------------------------------------------------------------------------------
#pragma once

template<INT nSize>
class FlagArray
{
public:
	FlagArray() { Clear(); }

	VOID					Import(const DWORD* pFlags);
	VOID					Import(const DWORD* pFlags, INT nSize);
	VOID					Export(DWORD* pFlags) const;

	VOID					Swap(INT nIndexA, INT nIndexB);
	VOID					Clear() { ZeroMemory(m_dwFlags, sizeof(m_dwFlags)); }
	VOID					Set(INT nIndex);
	VOID					SetMany(INT nArgNum, ...);
	VOID					Unset(INT nIndex);
	FlagArray<nSize>&		operator|(INT nIndex);
	FlagArray<nSize>&		operator|=(const FlagArray<nSize>& flags);
	FlagArray<nSize>&		operator&=(const FlagArray<nSize>& flags);
	FlagArray<nSize>&		operator^=(const FlagArray<nSize>& flags);
	BOOL					operator==(const FlagArray<nSize>& flags) const;
	BOOL					operator!=(const FlagArray<nSize>& flags) const;
	DWORD					operator[](INT nBlockIndex)  const;	

	BOOL					IsSet(INT nIndex) const;

	// 掩码判断, 返回第一个不符合条件的索引
	BOOL					IsSetAll(const FlagArray<nSize>& flags, OUT INT *pFailedIndex = NULL) const;
	BOOL					IsSetAny(const FlagArray<nSize>& flags, OUT INT *pFailedIndex = NULL) const;
    BOOL					IsNotSetAll(const FlagArray<nSize>& flags) const;
    BOOL					IsNotSetAny(const FlagArray<nSize>& flags) const;
	BOOL					IsIntersectant(const FlagArray<nSize>& flags) const;

	static INT				FitBlock(INT nIndex);
	DWORD					ExportSect(INT nIndex) const;
	const std::string		ToString(CHAR chSeparator = ' ') const;
	const wstring			ToString(WCHAR chSeparator = L' ') const;

	INT						Count() const;
	INT						GetIntersectCount(const FlagArray<nSize>& flags) const;

	static const INT		nBlockNum = (nSize>>5) + ((nSize & 0x1F) ? 1: 0);

private:
	DWORD					m_dwFlags[nBlockNum];		// 无符号整型标志位数组
};

//---------------------------------------------------------------------------------
// 导入
//---------------------------------------------------------------------------------
template<INT nSize>
INLINE VOID FlagArray<nSize>::Import(const DWORD *pFlags)
{
	if (VALID(pFlags))
	{
		memcpy(m_dwFlags, pFlags, sizeof(m_dwFlags));
	}
	else
	{
		ZeroMemory(m_dwFlags, sizeof(m_dwFlags));
	}
}

template<INT nSize>
INLINE VOID FlagArray<nSize>::Import(const DWORD *pFlags, INT nSize)
{
	if (VALID(pFlags))
	{
		memcpy(m_dwFlags, pFlags, EMin(sizeof(m_dwFlags),nSize));
	}
	else
	{
		ZeroMemory(m_dwFlags, sizeof(m_dwFlags));
	}
}
//---------------------------------------------------------------------------------
// 导出
//---------------------------------------------------------------------------------
template<INT nSize>
VOID FlagArray<nSize>::Export( DWORD* pFlags ) const
{
	if (!VALID(pFlags))
		return;

	memcpy(pFlags, m_dwFlags, sizeof(DWORD)*nBlockNum);
}

//---------------------------------------------------------------------------------
// 设置标志位
//---------------------------------------------------------------------------------
template<INT nSize>
INLINE VOID FlagArray<nSize>::Set(INT nIndex)
{
	if( nIndex < 0 || nIndex >= nSize ) return;

	m_dwFlags[nIndex>>5] |= (1 << (nIndex & 0x1F));
}

template<INT nSize>
FlagArray<nSize>& FlagArray<nSize>::operator|( INT nIndex )
{
	Set(nIndex);
	return *this;
}

template<INT nSize>
DWORD FlagArray<nSize>::operator[](INT nBlockIndex)		const
{
	ASSERT( nBlockIndex >= 0 || nBlockIndex < nBlockNum );

	return m_dwFlags[nBlockIndex];
}

template<INT nSize>
FlagArray<nSize>& FlagArray<nSize>::operator|=(const FlagArray<nSize>& flags)
{
	for (INT n=0; n<nBlockNum; n++)
	{
		m_dwFlags[n] |= flags.m_dwFlags[n];
	}

	return *this;
}

template<INT nSize>
FlagArray<nSize>& FlagArray<nSize>::operator&=(const FlagArray<nSize>& flags)
{
	for (INT n=0; n<nBlockNum; n++)
	{
		m_dwFlags[n] &= flags.m_dwFlags[n];
	}

	return *this;
}

template<INT nSize>
FlagArray<nSize>& FlagArray<nSize>::operator^=(const FlagArray<nSize>& flags)
{
	for (INT n=0; n<nBlockNum; n++)
	{
		m_dwFlags[n] ^= flags.m_dwFlags[n];
	}

	return *this;
}

template<INT nSize>
BOOL FlagArray<nSize>::operator==(const FlagArray<nSize>& flags) const
{
	if( this == & flags )
		return TRUE;

	if( nSize<32 )
		return m_dwFlags[0] == flags.m_dwFlags[0];

	return 0 == memcmp(m_dwFlags, flags.m_dwFlags, sizeof(m_dwFlags));
}

template<INT nSize>
BOOL FlagArray<nSize>::operator!=(const FlagArray<nSize>& flags) const
{
	return !operator==(flags);
}

//---------------------------------------------------------------------------------
// 设置多个
//---------------------------------------------------------------------------------
template<INT nSize>
VOID FlagArray<nSize>::SetMany( INT nArgNum, ... )
{
	// 解析参数
	va_list al;
	va_start(al, nArgNum);
	for(INT i=0; i<nArgNum; ++i)
	{
		INT nIndex = va_arg(al, INT);

		Set(nIndex);
	}

	va_end(al);
}

//---------------------------------------------------------------------------------
// 取消标志位
//---------------------------------------------------------------------------------
template<INT nSize>
INLINE VOID FlagArray<nSize>::Unset(INT nIndex)
{
	if( nIndex < 0 || nIndex >= nSize ) return;

	m_dwFlags[nIndex>>5] &= ~(1 << (nIndex & 0x1F));
}

//---------------------------------------------------------------------------------
// 某个标志位是否已经设置
//---------------------------------------------------------------------------------
template<INT nSize>
INLINE BOOL FlagArray<nSize>::IsSet(INT nIndex) const
{
	if( nIndex < 0 || nIndex >= nSize ) return FALSE;

	return !!(m_dwFlags[nIndex>>5] & (1 << (nIndex & 0x1F)));
}

template<INT nSize>
BOOL FlagArray<nSize>::IsSetAny( const FlagArray<nSize>& flags, OUT INT *pFailedIndex ) const
{
	for (INT n=0; n<nBlockNum; n++)
	{
		if (m_dwFlags[n] & flags.m_dwFlags[n])
		{
			// 需要找到第一个不符合要求的索引
			if(VALID(pFailedIndex))
			{
				for (INT i=0; i<32; ++i)
				{
					if ((m_dwFlags[n] & (1 << i)) && (flags.m_dwFlags[n] & (1 << i)))
					{
						*pFailedIndex = (n << 5) + i;
						break;
					}
				}
			}

			return TRUE;
		}
	}

	return FALSE;
}

template<INT nSize>
BOOL FlagArray<nSize>::IsSetAll( const FlagArray<nSize>& flags, OUT INT *pFailedIndex ) const
{
	for (INT n=0; n<nBlockNum; n++)
	{
		if ((m_dwFlags[n] & flags.m_dwFlags[n]) != m_dwFlags[n])
		{
			// 需要找到第一个不符合要求的索引
			if(VALID(pFailedIndex))
			{
				for (INT i=0; i<32; ++i)
				{
					if ((m_dwFlags[n] & (1 << i)) && !(flags.m_dwFlags[n] & (1 << i)))
					{
						*pFailedIndex = (n << 5) + i;
						break;
					}
				}
			}

			return FALSE;
		}
	}

	return TRUE;
}

template<INT nSize>
BOOL FlagArray<nSize>::IsNotSetAll( const FlagArray<nSize>& flags ) const
{
    for (INT n=0; n<nBlockNum; n++)
    {
        if (m_dwFlags[n] & flags.m_dwFlags[n])
            return FALSE;
    }

    return TRUE;
}

template<INT nSize>
BOOL FlagArray<nSize>::IsNotSetAny( const FlagArray<nSize>& flags ) const
{
    for (INT n=0; n<nBlockNum; n++)
    {
        if ((m_dwFlags[n] & flags.m_dwFlags[n]) != flags.m_dwFlags[n])
            return TRUE;
    }

    return FALSE;
}

//---------------------------------------------------------------------------------
// 是否有交集
//---------------------------------------------------------------------------------
template<INT nSize>
BOOL FlagArray<nSize>::IsIntersectant( const FlagArray<nSize>& flags ) const
{
	for (INT n=0; n<nBlockNum; n++)
	{
		if (m_dwFlags[n] & flags.m_dwFlags[n])
			return TRUE;
	}

	return FALSE;
}

//---------------------------------------------------------------------------------
// 查找标志位对应的块索引
//---------------------------------------------------------------------------------
template<INT nSize>
INT FlagArray<nSize>::FitBlock( INT nIndex )
{
	INT nBlockIndex = nIndex >> 5;
	return nBlockIndex < nBlockNum ? nBlockIndex : INVALID;
}

template<INT nSize>
DWORD FlagArray<nSize>::ExportSect(INT nIndex) const
{
	ASSERT( nIndex >= 0 && nIndex < nSize );
	return m_dwFlags[nIndex>>5];
}

template<INT nSize>
VOID FlagArray<nSize>::Swap(INT nIndexA, INT nIndexB)
{
	BOOL bA = IsSet(nIndexA);
	BOOL bB = IsSet(nIndexB);
	if (bA != bB)
	{
		Set(bA ? nIndexB : nIndexA);
		Unset(bB ? nIndexA : nIndexB);
	}
}

template<INT nSize>
const std::string FlagArray<nSize>::ToString(CHAR chSeparator /*= ' '*/) const
{
	stringstream ssPool;
	for (INT n=0; n<nSize; n++)
	{
		if (IsSet(n))
			ssPool << n << chSeparator;
	}

	return ssPool.str();
}

template<INT nSize>
const wstring FlagArray<nSize>::ToString( WCHAR chSeparator /*= _T(' ')*/ ) const
{
	wstringstream ssPool;
	for (INT n=0; n<nSize; n++)
	{
		if (IsSet(n))
			ssPool << n << chSeparator;
	}

	return ssPool.str();
}

//----------------------------------------------------------------------------------
// 得到设置的位个数
//----------------------------------------------------------------------------------
template<INT nSize>
INLINE INT FlagArray<nSize>::Count() const
{
	INT nCount = 0;
	for(INT n = 0; n < nBlockNum; ++n)
		nCount += CountBits((UINT32)m_dwFlags[n]);

	return nCount;
}

//---------------------------------------------------------------------------------
// 得到和另一个FlagArray相交后的设置的位的个数
//---------------------------------------------------------------------------------
template<INT nSize>
INLINE INT FlagArray<nSize>::GetIntersectCount(const FlagArray<nSize>& flags) const
{
	INT nCount = 0;

	for(INT n=0; n < nBlockNum; ++n)
	{
		UINT32 dwFlag = m_dwFlags[n] & flags.m_dwFlags[n];
		
		if( !dwFlag ) continue;

		nCount += CountBits(dwFlag);
	}

	return nCount;
}

//---------------------------------------------------------------------------------
// 创建特定标志的FlagArray，只初始化一次
//---------------------------------------------------------------------------------
#define	MCreateLocalFlagArray(SIZE, NAME, ARGNUM, ...)							\
	__declspec(thread) static bool bInited##NAME = false;						\
	__declspec(thread) static DWORD dwBuffer##NAME[FlagArray<SIZE>::nBlockNum];	\
	FlagArray<SIZE> &NAME = *(FlagArray<SIZE> *)(dwBuffer##NAME) ; 				\
	if (!bInited##NAME)															\
	{																			\
		bInited##NAME = true;													\
		NAME.Clear();															\
		NAME.SetMany(ARGNUM, __VA_ARGS__);										\
	}
