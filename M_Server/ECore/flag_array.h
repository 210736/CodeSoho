//-------------------------------------------------------------------------------
// filename: flag_array.h
// author: aslan
// actor:
// data: 2010-03-01
// last:
// brief: �޷�������λ��ʾ����
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

	// �����ж�, ���ص�һ������������������
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
	DWORD					m_dwFlags[nBlockNum];		// �޷������ͱ�־λ����
};

//---------------------------------------------------------------------------------
// ����
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
// ����
//---------------------------------------------------------------------------------
template<INT nSize>
VOID FlagArray<nSize>::Export( DWORD* pFlags ) const
{
	if (!VALID(pFlags))
		return;

	memcpy(pFlags, m_dwFlags, sizeof(DWORD)*nBlockNum);
}

//---------------------------------------------------------------------------------
// ���ñ�־λ
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
// ���ö��
//---------------------------------------------------------------------------------
template<INT nSize>
VOID FlagArray<nSize>::SetMany( INT nArgNum, ... )
{
	// ��������
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
// ȡ����־λ
//---------------------------------------------------------------------------------
template<INT nSize>
INLINE VOID FlagArray<nSize>::Unset(INT nIndex)
{
	if( nIndex < 0 || nIndex >= nSize ) return;

	m_dwFlags[nIndex>>5] &= ~(1 << (nIndex & 0x1F));
}

//---------------------------------------------------------------------------------
// ĳ����־λ�Ƿ��Ѿ�����
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
			// ��Ҫ�ҵ���һ��������Ҫ�������
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
			// ��Ҫ�ҵ���һ��������Ҫ�������
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
// �Ƿ��н���
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
// ���ұ�־λ��Ӧ�Ŀ�����
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
// �õ����õ�λ����
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
// �õ�����һ��FlagArray�ཻ������õ�λ�ĸ���
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
// �����ض���־��FlagArray��ֻ��ʼ��һ��
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
