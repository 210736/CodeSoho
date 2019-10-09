//////////////////////////////////////////////////////////////////////////
// Auth: wb
// Date: 2012/7/26
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "speed_tester.h"

namespace ECore
{

	//////////////////////////////////////////////////////////////////////
#ifdef SPEED_TEST_ON
	BOOL g_bSpeedTest = FALSE;
#endif

	//////////////////////////////////////////////////////////////////////
	SpeedTester::SpeedTester()
	{
		QueryPerformanceFrequency(&m_freq);
		m_lastTime = timeGetTime();
	}

	//////////////////////////////////////////////////////////////////////
	void SpeedTester::Begin( DWORD dwCRC, LPCSTR szTitle )
	{
		std::map<DWORD,tagItem>::iterator iter = m_itemMap.find(dwCRC);
		if( iter != m_itemMap.end() )
		{
			QueryPerformanceCounter( &iter->second.startTime );
		}
		else
		{
			tagItem newItem;
			ZeroMemory( &newItem, sizeof(tagItem) );
			strncpy_s( newItem.title, szTitle, sizeof(newItem.title) - 1 );

			QueryPerformanceCounter( &newItem.startTime );

			m_itemMap.insert( make_pair( dwCRC, newItem ) );
		}
	}

	//////////////////////////////////////////////////////////////////////
	void SpeedTester::End( DWORD dwCRC )
	{
		std::map<DWORD,tagItem>::iterator iter = m_itemMap.find(dwCRC);
		if( iter != m_itemMap.end() )
		{
			tagItem& item = iter->second;

			QueryPerformanceCounter(&item.endTime);

			// 计算Begin和End之间的时间（毫秒）
			item.currTime = ((double)item.endTime.QuadPart - (double)item.startTime.QuadPart) / (double)m_freq.QuadPart;
			item.currTime *= 1000.0f;

			// 累计总运行时间
			item.totalTime += item.currTime;

			// 累计次数
			++item.times;

			// 更新最大时间
			if( item.currTime > item.maxTime )
				item.maxTime = item.currTime;
		}
	}

	//////////////////////////////////////////////////////////////////////
	void SpeedTester::Clear()
	{
		m_itemMap.clear();
	}

	//////////////////////////////////////////////////////////////////////
	void SpeedTester::Report( ISpeedTestReporter* pReporter )
	{
		// 定时进行一些处理
		if( timeGetTime() - m_lastTime > 5000 )
		{
			m_lastTime = timeGetTime();

			for( std::map<DWORD,tagItem>::iterator iter = m_itemMap.begin();
				 iter != m_itemMap.end(); ++iter )
			{
				tagItem& item = iter->second;

				// 计算平均时间
				if( item.times > 0 )
					item.avgTime = item.totalTime / DOUBLE(item.times);
				else
					item.avgTime = 0;

				// 清空总时间和次数
				item.totalTime = 0;
				item.times = 0;

				// 清空最大时间
				item.maxTime = 0;
			}
		}

		// 输出报表
		{
			stringstream ss;
			CHAR szItem[MID_STRING];

			for( std::map<DWORD,tagItem>::iterator iter = m_itemMap.begin();
				iter != m_itemMap.end(); ++iter )
			{
				tagItem& item = iter->second;

				if( item.avgTime > 1
					|| item.maxTime > 1 )
				{
					_snprintf_s( szItem, sizeof(szItem), sizeof(szItem) - 1, "%s: %.2f/%.2f\n", item.title, item.avgTime, item.maxTime ); 
					ss << szItem;
				}
			}

			pReporter->Report( ss.str().c_str() );
		}
	}
}//end namespace ECore
