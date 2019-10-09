//-----------------------------------------------------------------------------
// Auth: Sxg
// Date: 2010-01-04
//-----------------------------------------------------------------------------
#pragma once

namespace ECore
{

//-----------------------------------------------------------------------------
// 时间周期
//-----------------------------------------------------------------------------
enum ETimePeriod
{
	ETP_Null		= -1,
	ETP_Minute		= 0,
	ETP_Hour		= 1,
	ETP_Day			= 2,
	ETP_Week		= 3,
	ETP_Month		= 4,
	ETP_Year		= 5,

	ETP_End
};

//------------------------------------------------------------------------------------
// 常量定义
//------------------------------------------------------------------------------------
const INT X_DATETIME_LEN			= 19;				// datetime形式 -- 2010-01-04 16:01:55
const INT X_SECONDTIME_LEN			= 10;				// 时:分:秒形式 -- 999:01:55

const INT X_SECONDS_PER_MINUTE		= 60;							// 一分钟有多少秒
const INT X_SECONDS_PER_HOUR		= 60 * X_SECONDS_PER_MINUTE;	// 一小时有多少秒
const INT X_SECONDS_PER_DAY			= 24 * X_SECONDS_PER_HOUR;		// 一天有多少秒
const INT X_SECONDS_PER_WEEK		= 7 * X_SECONDS_PER_DAY;		// 一周有多少秒

// 月份									0	1	2	3	4	5	6	7	8	9	10	11	12
const INT X_DAYS_PER_MONTH[]		= {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; // 非闰年


//------------------------------------------------------------------------------------
// DWORD表示的时间 -- 转换为DWORD比较，值大者时间靠后
//------------------------------------------------------------------------------------
struct ECORE_API tagDateTime
{
	DWORD	sec		: 6,	// 0 - 59
			min		: 6,	// 0 - 59
			hour    : 5,	// 0 - 23
			day		: 5,	// 1 - 31
			month	: 4,	// 1 - 12
			year	: 6;	// 两位数，比如:10, 表示2010.最大表示到2063年

	//--------------------------------------------------------------------------------
	// 构造
	//--------------------------------------------------------------------------------
	tagDateTime()
	{
		// 2000-01-01 00:00:00
		sec		= 0;
		min		= 0;
		hour	= 0;
		day		= 1;
		month	= 1;
		year	= 0;
	}

	tagDateTime(BYTE bySec, BYTE byMin, BYTE byHour, BYTE byDay, BYTE byMonth, BYTE byYear)
	{
		sec		= bySec;
		min		= byMin;
		hour	= byHour;
		day		= byDay;
		month	= byMonth;
		year	= byYear;
	}

	tagDateTime(const DWORD& dwTime)
	{
		*this = dwTime;
	}

	BOOL IsValid()
	{
		return (0 != day && 0 != month);
	}

	//--------------------------------------------------------------------------------
	// 拷贝构造
	//--------------------------------------------------------------------------------
	tagDateTime(const tagDateTime& dwTime)
	{
		*this = dwTime;
	}

	//--------------------------------------------------------------------------------
	// clear
	//--------------------------------------------------------------------------------
	VOID Clear()
	{
		// 2000-01-01 00:00:00
		sec		= 0;
		min		= 0;
		hour	= 0;
		day		= 1;
		month	= 1;
		year	= 0;
	}

	//--------------------------------------------------------------------------------
	// operator =
	//--------------------------------------------------------------------------------
	tagDateTime& operator = (const tagDateTime& dwTime)
	{ 
		sec		= dwTime.sec;
		min		= dwTime.min; 
		hour	= dwTime.hour;
		day		= dwTime.day;
		month	= dwTime.month;
		year	= dwTime.year;

		return *this;
	}

	//--------------------------------------------------------------------------------
	// operator =
	//--------------------------------------------------------------------------------
	tagDateTime& operator = (DWORD dwTime)
	{
		return (*this = *(tagDateTime*)&dwTime);
	}

	//--------------------------------------------------------------------------------
	// 转换为DWORD
	//--------------------------------------------------------------------------------
	operator DWORD()
	{
		return *(DWORD*)this;
	}

	operator const DWORD() const
	{
		return *(DWORD*)this;
	}
};

//-------------------------------------------------------------------------------------
// 是否为闰年
//-------------------------------------------------------------------------------------
INLINE BOOL ECORE_API IsLeapYear(INT nYear)
{
	return !(nYear % 4) && (nYear % 100) || !(nYear % 400);
}

//-------------------------------------------------------------------------------------
// 计算指定年份有几天
//-------------------------------------------------------------------------------------
INLINE INT CalDaysInYear(INT nYear)
{
	ASSERT(nYear > 0);

	if( IsLeapYear(nYear) )
	{
		return 366;
	}
	
	return 365;
}

//-------------------------------------------------------------------------------------
// 计算指定月份有几天
//-------------------------------------------------------------------------------------
INLINE INT CalDaysInMonth(INT nYear, INT nMonth)
{
	ASSERT(nYear > 0);
	ASSERT(nMonth > 0 && nMonth <= 12);

	if( 2 == nMonth && IsLeapYear(nYear) )
	{
		return 29;
	}
	else
	{
		return X_DAYS_PER_MONTH[nMonth];
	}
}

//-------------------------------------------------------------------------------------
// 根据年份及该年所流逝的时间(单位: 秒)，计算具体年月日 小时分秒
//-------------------------------------------------------------------------------------
tagDateTime CalTimeByYearAndRunSecond(INT nYear, DWORD dwRunSecond);

//-------------------------------------------------------------------------------------
// 得到当前时间
//-------------------------------------------------------------------------------------
tagDateTime ECORE_API GetCurDateTime();

//-------------------------------------------------------------------------------------
// 判断某个时间是一年中的第几天
//-------------------------------------------------------------------------------------
INT ECORE_API WhichDayInYear(const tagDateTime& dwTime);

//-------------------------------------------------------------------------------------
// 判断某个时间是周几
//-------------------------------------------------------------------------------------
DWORD ECORE_API WhichDayInWeek(const tagDateTime& dwTime);

//-------------------------------------------------------------------------------------
// 判断某个时间一年中的第几周
//-------------------------------------------------------------------------------------
DWORD ECORE_API WhichWeekInYear(const tagDateTime& dwTime);

//-------------------------------------------------------------------------------------
// 计算两个时间差 -- 要求：dwDstTime >= dwSrcTime
//-------------------------------------------------------------------------------------
DWORD ECORE_API CalcTimeDiff(tagDateTime dwDstTime, tagDateTime dwSrcTime);

//-------------------------------------------------------------------------------------
// 计算两个时间的日期差
//-------------------------------------------------------------------------------------
INT ECORE_API CalDateDiff(tagDateTime stDate1, tagDateTime stDate2, BYTE byHour = 0, BYTE byMinute = 0, BYTE bySecond = 0);

//-------------------------------------------------------------------------------------
// 某个时间加上某个值
//-------------------------------------------------------------------------------------
tagDateTime ECORE_API IncTime(tagDateTime dwSrcTime, INT nIncSecond);

//-----------------------------------------------------------------------------
// 某个时间减去某个值
//-----------------------------------------------------------------------------
tagDateTime ECORE_API DecTime(tagDateTime dwSrcTime, INT nDecSecond);

//-------------------------------------------------------------------------------------
// 转化成: 年-月-日 时:分:秒 如：2010-01-04 16:01:55
//-------------------------------------------------------------------------------------
BOOL ECORE_API Date2String(OUT CHAR szTime[], INT nSzArray, const tagDateTime &dwTime);
BOOL ECORE_API DateTime2String(OUT CHAR szTime[], INT nSzArray, const tagDateTime &dwTime);
BOOL ECORE_API DateTime2StringNoSecond(OUT CHAR szTime[], INT nSzArray, const tagDateTime &dwTime);


//-------------------------------------------------------------------------------------
// 将 年-月-日 时:分:秒 格式的数据转换为tagDateTime
//-------------------------------------------------------------------------------------
BOOL ECORE_API String2DateTime(OUT tagDateTime &dwTime, const CHAR szTime[], INT nSzArray);

INLINE tagDateTime ECORE_API String2DateTime(const CHAR szTime[], INT nSzArray)
{
	tagDateTime stTime;

	BOOL bRes = String2DateTime(stTime, szTime, nSzArray);
	ASSERT(bRes);

	return stTime;
}

//-------------------------------------------------------------------------------------
// 将秒数据转换为时:分:秒格式
//-------------------------------------------------------------------------------------
BOOL ECORE_API Second2String(OUT CHAR szTime[], INT nSzArray, INT nSecond, BOOL bShort=FALSE);
BOOL ECORE_API Second2StringW(OUT wchar_t szTime[], INT nSzArray, INT nSecond);

#ifdef _UNICODE
    #define tSecond2String  ECore::Second2StringW
#else
    #define tSecond2String  ECore::Second2String
#endif

}//namespace ECore