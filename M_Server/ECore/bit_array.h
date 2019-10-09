#pragma once

namespace ECore
{
	/** \class BitArray
		\brief λ����
	*/
	class BitArray
	{
		enum
		{
			US=32
		};
	public:
		BitArray(void)
			: m_pData(0)
			, m_size(0)
			, m_numBytes(0)
		{}

		~BitArray(void)
		{
			Destroy();
		}

		void Create(int size)
		{
			ASSERT(m_pData==NULL);
			ASSERT(size>0);

			m_size=size;

			int numDWords=(size/US)+1;
			m_numBytes=numDWords*US/8;
			m_pData=(DWORD*)new BYTE[m_numBytes];
		}		
		
		void Destroy()
		{
			SAFE_DEL_ARRAY(m_pData);
			m_size=0;
			m_numBytes=0;
		}

		int Size() const {	return m_size;	}

		bool GetValue(int bit) const
		{
			const DWORD HEX1=((unsigned) 0x00000001);

			ASSERT(bit>=0 && bit<m_size);
			return (*(m_pData+bit/US) & (HEX1 << (US-1-(bit % US))))!=0;
		}

		void SetValue(int bit,bool val)
		{
			const DWORD HEX1=((unsigned) 0x00000001);

			ASSERT(bit>=0 && bit<m_size);

			if(val)
				*(m_pData+bit/US) |= (HEX1 << (US-1-(bit % US)));//set bit to 1
			else
				*(m_pData+bit/US) &= (~(HEX1 << (US-1-(bit % US))));//set bit to 0
		}
		
		DWORD GetRawSize() const { return m_numBytes; }

		VOID GetRawData(BYTE *pData, DWORD dwSize) const
		{
			memcpy(pData, m_pData, dwSize);
		}

		void Zero()
		{
			ZeroMemory(m_pData,m_numBytes);
		}

	private:
		DWORD	*m_pData;//ʹ��4�ֽ�ָ��,�������Ѱַ�ٶ�
		int		m_size;//number of elements
		int		m_numBytes;//buffer size
	};
}//namespace ECore