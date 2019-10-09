//////////////////////////////////////////////////////////////////////////
// Auth: wb
// Date: 2012/7/26
//////////////////////////////////////////////////////////////////////////
#pragma once

namespace ECore
{
	//////////////////////////////////////////////////////////////////////
	// ISpeedTestReporter

	class ISpeedTestReporter
	{
	public:
		virtual void Report(LPCSTR szReport) =0;
	};

	//////////////////////////////////////////////////////////////////////
	// SpeedTester
	
	class ECORE_API SpeedTester
	{
		struct tagItem
		{
			CHAR			title[SHORT_STRING];
			UINT			times;
			DOUBLE			currTime;
			DOUBLE			totalTime;
			DOUBLE			avgTime;
			DOUBLE			maxTime;
			LARGE_INTEGER	startTime;
			LARGE_INTEGER	endTime;
		};

	public:
		SpeedTester();

		void Begin(DWORD dwCRC, LPCSTR szTitle);
		void End(DWORD dwCRC);

		void Clear();
		void Report(ISpeedTestReporter* pReporter);
		
		static SpeedTester* Inst() { static SpeedTester inst; return &inst; }

	private:
		LARGE_INTEGER			m_freq;
		std::map<DWORD,tagItem>	m_itemMap;
		DWORD					m_lastTime;	
	};

	//////////////////////////////////////////////////////////////////////
	// SpeedTestHelper

	class SpeedTestHelper
	{
		DWORD m_dwCRC;
	public:
		SpeedTestHelper(DWORD dwCRC, LPCSTR szTitle)
			: m_dwCRC(dwCRC)
		{
			SpeedTester::Inst()->Begin( m_dwCRC, szTitle ); 
		}

		~SpeedTestHelper()
		{
			SpeedTester::Inst()->End( m_dwCRC );
		}
	};

	//////////////////////////////////////////////////////////////////////
	// ºê¶¨Òå

#define SPEED_TEST_ON

#ifdef SPEED_TEST_ON
	extern ECORE_API BOOL g_bSpeedTest;
#	define SPEED_TEST_BEGIN(title) \
					{\
					static DWORD _crc = Crc32(title);\
					if( g_bSpeedTest ) SpeedTester::Inst()->Begin( _crc, title );
#	define SPEED_TEST_BEGIN_CRC(crc,title) \
					{\
					DWORD _crc = crc;\
					if( g_bSpeedTest ) SpeedTester::Inst()->Begin( _crc, title );
#	define SPEED_TEST_END() \
					if( g_bSpeedTest ) SpeedTester::Inst()->End( _crc );\
					}
#	define SPEED_TEST_REPORT(reporter) if( g_bSpeedTest ) SpeedTester::Inst()->Report(reporter);
#else
#	define SPEED_TEST_BEGIN(title) void;
#	define SPEED_TEST_BEGIN_CRC(crc,title) void;
#	define SPEED_TEST_END() void;
#	define SPEED_TEST_REPORT(reporter) void;
#endif

}//end namespace ECore

