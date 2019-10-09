/*
-----------------------------------------------------------------------------------
Auth: wb
Date: 2012/8/8
-----------------------------------------------------------------------------------
*/
#pragma once

namespace ECore 
{
	/**
	 * ���ζ���
	 */
	class RingBuffer
	{
	public:
		
		/**
		 * ����
		 */
		RingBuffer()
			: m_pBuffer(NULL)
			, m_pBufferEnd(NULL)
			, m_pReadPointer(NULL)
			, m_pWritePointer(NULL)
			, m_dwDataSize(0)
		{}

		/**
		 * ����
		 */
		~RingBuffer()
		{
			Destroy();
		}

		/**
		 * ��ʼ��
		 */
		void Init(DWORD dwBufferSize)
		{
			m_pBuffer = new BYTE[dwBufferSize];
			m_pBufferEnd = m_pBuffer + dwBufferSize;

			m_pReadPointer = m_pBuffer;
			m_pWritePointer = m_pBuffer;

			m_dwDataSize = 0;
		}

		/**
		 * ����
		 */
		void Destroy()
		{
			SAFE_DEL(m_pBuffer);

			m_pBufferEnd = NULL;
			m_pReadPointer = NULL;
			m_pWritePointer = NULL;

			m_dwDataSize = 0;
		}

		/**
		 * ��ȡ
		 * @return ��������ָ�룬����Ϊ�շ���NULL
		 */
		LPVOID Pop(OUT DWORD& dwSize)
		{
			ASSERT(m_pBuffer);

			// ��дָ���غϱ�ʾ����Ϊ��
			if(m_pReadPointer == m_pWritePointer)
			{
				dwSize = 0;
				return NULL;
			}

			// β������4�ֽڻ�sizeΪ0����ת���ײ�
			if(DWORD(m_pBufferEnd - m_pReadPointer) < sizeof(DWORD)
				|| GET_DWORD(m_pReadPointer) == 0)
			{
				m_pReadPointer = m_pBuffer;
			}

			// ��ȡsize
			dwSize = GET_DWORD(m_pReadPointer);

			// ���ƶ�ָ�룬����size
			m_pReadPointer += sizeof(DWORD);

			// ��������ָ��
			LPVOID pData = m_pReadPointer;

			// ���ƶ�ָ�룬��������
			m_pReadPointer += dwSize;

			// �������ݼ���
			m_dwDataSize -= dwSize;

			// ��������ָ��
			return pData;
		}

		/**
		 * д��
		 * @return ��������ָ�룬������������NULL
		 */
		LPVOID Push(DWORD dwSize)
		{	
			ASSERT(m_pBuffer);

			if(dwSize == 0)
				return NULL;

			DWORD dwRealSize = dwSize + sizeof(DWORD);

			// дָ���ں��棿
			if(m_pWritePointer >= m_pReadPointer) 
			{
				// β���ռ䲻����
				if(DWORD(m_pBufferEnd - m_pWritePointer) <= dwRealSize) 
				{
					// �ײ��ռ��㹻��
					if(DWORD(m_pReadPointer - m_pBuffer) > dwRealSize) 
					{
						// ��ǰλ��д��0����ʾҪ��ת���ײ�
						if(DWORD(m_pBufferEnd - m_pWritePointer) >= sizeof(DWORD))
							GET_DWORD(m_pWritePointer) = 0;

						// дָ���Ƶ��ײ�
						m_pWritePointer = m_pBuffer;
					}
					// �ײ��ռ䲻��
					else 
						return NULL;
				}
			}
			// дָ����ǰ��
			else 
			{
				// �ռ䲻����
				if(DWORD(m_pReadPointer - m_pWritePointer) <= dwRealSize) 
					return NULL;
			}

			// д��size
			GET_DWORD(m_pWritePointer) = dwSize;

			// ����дָ�룬����size
			m_pWritePointer += sizeof(DWORD);

			// ��������ָ��
			LPVOID pData = m_pWritePointer;

			// ����дָ�룬��������
			m_pWritePointer += dwSize;

			// �������ݼ���
			m_dwDataSize += dwSize;

			// ��������ָ��
			return pData;
		}

		/**
		 * ���ض��д�С
		 */
		DWORD GetBufferSize() { return DWORD( m_pBufferEnd - m_pBuffer ); }
		DWORD GetDataSize() { return m_dwDataSize; }

	private:

		BYTE*	m_pBuffer;
		BYTE*	m_pBufferEnd;
		BYTE*	m_pReadPointer;
		BYTE*	m_pWritePointer;

		DWORD	m_dwDataSize;
	};
} // namespace ECore 