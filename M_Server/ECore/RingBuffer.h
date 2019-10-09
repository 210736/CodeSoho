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
	 * 环形队列
	 */
	class RingBuffer
	{
	public:
		
		/**
		 * 构造
		 */
		RingBuffer()
			: m_pBuffer(NULL)
			, m_pBufferEnd(NULL)
			, m_pReadPointer(NULL)
			, m_pWritePointer(NULL)
			, m_dwDataSize(0)
		{}

		/**
		 * 析构
		 */
		~RingBuffer()
		{
			Destroy();
		}

		/**
		 * 初始化
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
		 * 销毁
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
		 * 读取
		 * @return 返回数据指针，队列为空返回NULL
		 */
		LPVOID Pop(OUT DWORD& dwSize)
		{
			ASSERT(m_pBuffer);

			// 读写指针重合表示队列为空
			if(m_pReadPointer == m_pWritePointer)
			{
				dwSize = 0;
				return NULL;
			}

			// 尾部不够4字节或size为0，跳转到首部
			if(DWORD(m_pBufferEnd - m_pReadPointer) < sizeof(DWORD)
				|| GET_DWORD(m_pReadPointer) == 0)
			{
				m_pReadPointer = m_pBuffer;
			}

			// 获取size
			dwSize = GET_DWORD(m_pReadPointer);

			// 后移读指针，跳过size
			m_pReadPointer += sizeof(DWORD);

			// 保存数据指针
			LPVOID pData = m_pReadPointer;

			// 后移读指针，跳过数据
			m_pReadPointer += dwSize;

			// 减少数据计数
			m_dwDataSize -= dwSize;

			// 返回数据指针
			return pData;
		}

		/**
		 * 写入
		 * @return 返回数据指针，队列已满返回NULL
		 */
		LPVOID Push(DWORD dwSize)
		{	
			ASSERT(m_pBuffer);

			if(dwSize == 0)
				return NULL;

			DWORD dwRealSize = dwSize + sizeof(DWORD);

			// 写指针在后面？
			if(m_pWritePointer >= m_pReadPointer) 
			{
				// 尾部空间不够？
				if(DWORD(m_pBufferEnd - m_pWritePointer) <= dwRealSize) 
				{
					// 首部空间足够？
					if(DWORD(m_pReadPointer - m_pBuffer) > dwRealSize) 
					{
						// 当前位置写入0，表示要跳转到首部
						if(DWORD(m_pBufferEnd - m_pWritePointer) >= sizeof(DWORD))
							GET_DWORD(m_pWritePointer) = 0;

						// 写指针移到首部
						m_pWritePointer = m_pBuffer;
					}
					// 首部空间不够
					else 
						return NULL;
				}
			}
			// 写指针在前面
			else 
			{
				// 空间不够？
				if(DWORD(m_pReadPointer - m_pWritePointer) <= dwRealSize) 
					return NULL;
			}

			// 写入size
			GET_DWORD(m_pWritePointer) = dwSize;

			// 后移写指针，跳过size
			m_pWritePointer += sizeof(DWORD);

			// 保存数据指针
			LPVOID pData = m_pWritePointer;

			// 后移写指针，跳过数据
			m_pWritePointer += dwSize;

			// 增加数据计数
			m_dwDataSize += dwSize;

			// 返回数据指针
			return pData;
		}

		/**
		 * 返回队列大小
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