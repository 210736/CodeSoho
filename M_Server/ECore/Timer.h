#pragma once

namespace ECore
{
	/**	\class Timer
		\brief 时间管理工具
		\par
			<br>.游戏中所有用到时间的地方都需统一从此取得
			<br>.提供整体运行时间和两帧间隔时间
			<br>.内部保存原始数据(毫秒)和浮点数据,前者为了减少浮点误差,后者提供给游戏使用
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

			// 计算deltaTime
			double dDelta = ((double)m_liCurrentTime.QuadPart - (double)m_liLastTime.QuadPart) / (double)m_liFreq.QuadPart;
			m_fDelta = (float)dDelta;
			m_dwDelta = DWORD(dDelta * 1000.0);

			// 计算累计时间
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
		LARGE_INTEGER	m_liFirstTime;			// 初始化时得到的系统时间，秒
		LARGE_INTEGER	m_liLastTime;			// 上一次Update得到的系统时间，秒
		LARGE_INTEGER	m_liCurrentTime;		// 本次Update得到的系统时间，秒

		DWORD			m_dwElapse;				// 程序从启动到现在的总时间，毫秒
		DWORD			m_dwDelta;				// 两次Update之间的间隔时间，毫秒

		float			m_fElapse;				// 秒
		float			m_fDelta;				// 秒

	private:
		LARGE_INTEGER	m_liFreq;
	};
}
