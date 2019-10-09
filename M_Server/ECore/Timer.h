#pragma once

namespace ECore
{
	/**	\class Timer
		\brief ʱ�������
		\par
			<br>.��Ϸ�������õ�ʱ��ĵط�����ͳһ�Ӵ�ȡ��
			<br>.�ṩ��������ʱ�����֡���ʱ��
			<br>.�ڲ�����ԭʼ����(����)�͸�������,ǰ��Ϊ�˼��ٸ������,�����ṩ����Ϸʹ��
	*/
	class ECORE_API Timer
	{
	public:
		Timer::Timer(void)
			: m_dwElapse(0)
			, m_dwDelta(0)
			, m_fElapse(0.0f)
			, m_fDelta(0.0f)
		{
			QueryPerformanceFrequency(&m_liFreq);
			QueryPerformanceCounter(&m_liFirstTime);
		}

		INLINE void Init()
		{
			QueryPerformanceCounter(&m_liCurrentTime);
			m_liLastTime = m_liCurrentTime;
		}
		
		INLINE void Update()
		{
			m_liLastTime = m_liCurrentTime;

			QueryPerformanceCounter(&m_liCurrentTime);

			// ����deltaTime
			double dDelta = ((double)m_liCurrentTime.QuadPart - (double)m_liLastTime.QuadPart) / (double)m_liFreq.QuadPart;
			m_fDelta = (float)dDelta;
			m_dwDelta = DWORD(dDelta * 1000.0);

			// �����ۼ�ʱ��
			double dElapse = ((double)m_liCurrentTime.QuadPart - (double)m_liFirstTime.QuadPart) / (double)m_liFreq.QuadPart;
			m_fElapse = (float)dElapse;
			m_dwElapse = DWORD(dElapse * 1000.0);
		}

		INLINE DWORD GetDeltaDW()	{ return m_dwDelta;}
		INLINE float GetDelta()		{ return m_fDelta; }

		INLINE DWORD GetElapseDW()	{ return m_dwElapse; }
		INLINE float GetElapse()	{ return m_fElapse; }

		INLINE DWORD GetDeltaMicroSec()	
		{ 
			double dDelta = ((double)m_liCurrentTime.QuadPart - (double)m_liLastTime.QuadPart) / (double)m_liFreq.QuadPart;
			return DWORD(dDelta * 1000000); 
		}

	protected:
		LARGE_INTEGER	m_liFirstTime;			// ��ʼ��ʱ�õ���ϵͳʱ�䣬��
		LARGE_INTEGER	m_liLastTime;			// ��һ��Update�õ���ϵͳʱ�䣬��
		LARGE_INTEGER	m_liCurrentTime;		// ����Update�õ���ϵͳʱ�䣬��

		DWORD			m_dwElapse;				// ��������������ڵ���ʱ�䣬����
		DWORD			m_dwDelta;				// ����Update֮��ļ��ʱ�䣬����

		float			m_fElapse;				// ��
		float			m_fDelta;				// ��

	private:
		LARGE_INTEGER	m_liFreq;
	};
}
