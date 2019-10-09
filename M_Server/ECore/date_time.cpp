//-----------------------------------------------------------------------------
// Auth: Sxg
// Date: 2010-01-04
//-----------------------------------------------------------------------------
#define _CRT_SECURE_NO_DEPRECATE

#include "stdafx.h"
#include "date_time.h"

namespace ECore
{

//-------------------------------------------------------------------------------------
// 得到当前时间
//-------------------------------------------------------------------------------------
tagDateTime ECORE_API GetCurDateTime()
{
	SYSTEMTIME CurTime;
	GetLocalTime(&CurTime);

	tagDateTime dwTime;
	dwTime.sec		= CurTime.wSecond;
	dwTime.min		= CurTime.wMinute;
	dwTime.hour		= CurTime.wHour;
	dwTime.day		= CurTime.wDay;
	dwTime.month	= CurTime.wMonth;
	dwTime.year		= (CurTime.wYear) % 100;

	return dwTime;
}

//-------------------------------------------------------------------------------------
// 判断某个时间是一年中的第几天
//-------------------------------------------------------------------------------------
INT ECORE_API WhichDayInYear(const tagDateTime& dwTime)
{
	if( dwTime.month > 12 )
	{
		ASSERT(dwTime.month <= 12);
		return 0;
	}

	INT nRetDay = 0;
	for( INT i = 1; i < (INT)dwTime.month; ++i )
	{
		nRetDay += CalDaysInMonth((INT)dwTime.year + 2000, i);
	}

	nRetDay += dwTime.day;

	return nRetDay;
}

//---------------------------------------------------------------------------
// 判断某个时间是周几(0-6,星期日-星期六)
//---------------------------------------------------------------------------
DWORD ECORE_API WhichDayInWeek(const tagDateTime& dwTime)
{
	// 年份
	INT nYear = (INT)dwTime.year + 2000;

	// 一年中的第几天
	INT nDay = WhichDayInYear(dwTime);

	// 泰勒公式：x-1+[(x-1)/4]-[(x-1)/100]+[(x-1)/400]+c
	return ( (nYear-1) + (nYear-1)/4 - (nYear-1)/100 + (nYear-1)/400 + nDay ) % 7;
}

//-------------------------------------------------------------------------------------
// 判断某个时间一年中的第几周
//-------------------------------------------------------------------------------------
DWORD ECORE_API WhichWeekInYear( const tagDateTime& dwTime )
{
	// 元旦是周几
	INT nFirstDay = WhichDayInWeek(tagDateTime(0, 0, 0, 1, 1, dwTime.year));

	// 计算指定时间是一年中的第几天
	INT nWhichDay = WhichDayInYear(dwTime);

	// 计算当元旦为周一时指定时间为第几周
	INT nWeek = nWhichDay / 7 + ((nWhichDay % 7 != 0) ? 1 : 0);

	// 计算其他情况
	if (nFirstDay != 1)
	{
		nFirstDay = (nFirstDay == 0) ? 7 : nFirstDay;

		INT nModDay = nWhichDay % 7;
		nModDay = (nModDay == 0) ? 7 : nModDay;

		if (nModDay >= 9-nFirstDay && nModDay <= 7)
			nWeek += 1;
	}

	return nWeek;
}

//-------------------------------------------------------------------------------------
// 计算两个日期的时间差，返回秒
//-------------------------------------------------------------------------------------
DWORD ECORE_API CalcTimeDiff(tagDateTime dwDstTime, tagDateTime dwSrcTime)
{
	//ASSERT((DWORD)dwDstTime >= (DWORD)dwSrcTime);

	// 是否为同一时间点
	if( *(DWORD*)(&dwDstTime) == *(DWORD*)(&dwSrcTime) )
	{
		return 0;
	}

	// 相对于该年份1月1日0时0分0秒所流逝时间
	DWORD dwSrcRun = (WhichDayInYear(dwSrcTime) - 1) * X_SECONDS_PER_DAY 
						+ dwSrcTime.hour * X_SECONDS_PER_HOUR 
						+ dwSrcTime.min * 60 
						+ dwSrcTime.sec;


	// 相对于该年份1月1日0时0分0秒所流逝时间
	DWORD dwDestRun = (WhichDayInYear(dwDstTime) - 1) * X_SECONDS_PER_DAY 
						+ dwDstTime.hour * X_SECONDS_PER_HOUR 
						+ dwDstTime.min * 60 
						+ dwDstTime.sec;

	// 计算年
	for( INT i = dwSrcTime.year; i < (INT)dwDstTime.year; ++i )
	{
		// 流逝的整年数
		dwDestRun += CalDaysInYear(i + 2000) * X_SECONDS_PER_DAY;
	}

	// ASSERT( dwDestRun >= dwSrcRun );
	if( dwDestRun < dwSrcRun )
	{
		// ASSERT(0);	
		return 0;
	}

	return (dwDestRun - dwSrcRun);
}

//-------------------------------------------------------------------------------------
// 计算两个日期的日期差，可以返回负值
//-------------------------------------------------------------------------------------
INT ECORE_API CalDateDiff(tagDateTime stDate1, tagDateTime stDate2, BYTE byHour /*= 0*/, BYTE byMinute /*= 0*/, BYTE bySecond /*= 0*/)
{
	ASSERT(stDate1.IsValid() && stDate2.IsValid());

	BOOL bNeg = FALSE;

	if( (DWORD)stDate1 < (DWORD)stDate2 )
	{
		tagDateTime temp = stDate1;
		stDate1 = stDate2;
		stDate2 = temp;

		bNeg = TRUE;
	}

	// 到这里，stDate1一定大于等于stDate2，则计算相差多少天
	INT nSrc = WhichDayInYear(stDate2);

	INT nDest = WhichDayInYear(stDate1);

	// 计算年
	for(INT n = stDate2.year; n < (INT)stDate1.year; ++n)
	{
		nDest += CalDaysInYear(n+2000);
	}

	ASSERT(nDest > 0 && nSrc > 0 && nDest >= nSrc);

	INT nDayDiff = nDest - nSrc;
	
	// 时间点修正
	tagDateTime stTime1, stTime2, stCheckPoint;
	stTime1.hour = stDate1.hour;	stTime1.min = stDate1.min;	stTime1.sec = stDate1.sec;
	stTime2.hour = stDate2.hour;	stTime2.min = stDate2.min;	stTime2.sec = stDate2.sec;
	stCheckPoint.hour = byHour;	stCheckPoint.min = byMinute;	stCheckPoint.sec = bySecond;
	if (stTime2 < stCheckPoint && stTime1 >= stCheckPoint)
	{
		++nDayDiff;
	}
	else if (stTime2 >= stCheckPoint && stTime1 < stCheckPoint)
	{
		--nDayDiff;
	}

	return (bNeg ? -nDayDiff : nDayDiff);
	
}

//-------------------------------------------------------------------------------------
// 根据年份及该年所流逝的时间(单位: 秒)，计算具体年月日 小时分秒
//-------------------------------------------------------------------------------------
tagDateTime CalTimeByYearAndRunSecond(INT nYear, DWORD dwRunSecond)
{
	ASSERT(nYear >= 0 && nYear < 64);

	// 初始化结果时间
	tagDateTime dwRetTime;

	// 计算
	INT nDays = dwRunSecond / X_SECONDS_PER_DAY;			// 整天数
	INT nSecs  = dwRunSecond % X_SECONDS_PER_DAY;		// 零碎剩余的妙数

	// 计算年份
	dwRetTime.year = nYear;
	while( nDays >= 365 )
	{
		INT nDaysPerYear = CalDaysInYear((INT)dwRetTime.year + 2000);

		if( nDays < nDaysPerYear )
		{
			// 不足1年
			break;
		}

		++dwRetTime.year;
		nDays -= nDaysPerYear;
	}

	// 计算月
	dwRetTime.month = 1;
	dwRetTime.day = 1;
	while( nDays >= 28 )
	{
		INT nDaysPerMonth = CalDaysInMonth((INT)dwRetTime.year + 2000, dwRetTime.month);

		if( nDays < nDaysPerMonth )
		{
			// 不足一个月
			break;
		}

		++dwRetTime.month;
		nDays -= nDaysPerMonth;
	}

	// 计算日
	dwRetTime.day += nDays;

	// 计算小时分秒
	dwRetTime.hour = nSecs / X_SECONDS_PER_HOUR;
	dwRetTime.min = (nSecs / 60) % 60;
	dwRetTime.sec = nSecs % 60;

	return dwRetTime;
}

//-----------------------------------------------------------------------------
// 某个时间加上某个值
//-----------------------------------------------------------------------------
tagDateTime ECORE_API IncTime(tagDateTime dwSrcTime, INT nIncSecond)
{
	ASSERT(nIncSecond >= 0);
	
	// 相对于该年份1月1日0时0分0秒所流逝时间
	DWORD dwSrcRun = (WhichDayInYear(dwSrcTime) - 1) * X_SECONDS_PER_DAY 
						+ dwSrcTime.hour * X_SECONDS_PER_HOUR 
						+ dwSrcTime.min * 60 
						+ dwSrcTime.sec;


	// 加上递增的时间
	DWORD dwRunTime = dwSrcRun + nIncSecond;

	return CalTimeByYearAndRunSecond(dwSrcTime.year, dwRunTime);
}

//-----------------------------------------------------------------------------
// 某个时间减去某个值
//-----------------------------------------------------------------------------
tagDateTime ECORE_API DecTime(tagDateTime dwSrcTime, INT nDecSecond)
{
	// 相对于该年份1月1日0时0分0秒所流逝时间
	DWORD dwSrcRun = (WhichDayInYear(dwSrcTime) - 1) * X_SECONDS_PER_DAY 
						+ dwSrcTime.hour * X_SECONDS_PER_HOUR 
						+ dwSrcTime.min * 60 
						+ dwSrcTime.sec;

	INT nDaysPerYear = CalDaysInYear((INT)dwSrcTime.year + 2000);
	DWORD dwLeftSecond = (nDaysPerYear * X_SECONDS_PER_DAY - dwSrcRun) + nDecSecond;

	// 初始化结果时间
	tagDateTime dwRetTime;

	// 先计算年份
	dwRetTime.year = dwSrcTime.year;
	while( dwLeftSecond >= 365 * X_SECONDS_PER_DAY )
	{
		DWORD dwSecsPerYear = CalDaysInYear((INT)dwRetTime.year + 2000) * X_SECONDS_PER_DAY;
		if( dwLeftSecond < dwSecsPerYear )
		{
			// 不足1年
			break;
		}

		--dwRetTime.year;
		dwLeftSecond -= dwSecsPerYear;
	}

	return CalTimeByYearAndRunSecond((INT)dwRetTime.year, CalDaysInYear((INT)dwRetTime.year + 2000) * X_SECONDS_PER_DAY - dwLeftSecond);
}

//-------------------------------------------------------------------------------------
// 转化成: 年-月-日 如：2010-01-04 16:01:55
//-------------------------------------------------------------------------------------
BOOL ECORE_API DateTime2String(OUT CHAR szTime[], INT nSzArray, const tagDateTime &dwTime)
{
	if( nSzArray <= X_DATETIME_LEN )
	{
		return FALSE;
	}

	sprintf(szTime, "20%02d-%02d-%02d %02d:%02d:%02d", 
		dwTime.year, dwTime.month, dwTime.day, dwTime.hour, dwTime.min, dwTime.sec);

	return TRUE;
}


//-------------------------------------------------------------------------------------
// 转化成: 年-月-日 如：2010-01-04 16:01, 不带秒
//-------------------------------------------------------------------------------------
BOOL ECORE_API DateTime2StringNoSecond(OUT CHAR szTime[], INT nSzArray, const tagDateTime &dwTime)
{
	if( nSzArray <= X_DATETIME_LEN )
	{
		return FALSE;
	}

	sprintf(szTime, "20%02d-%02d-%02d %02d:%02d", 
		dwTime.year, dwTime.month, dwTime.day, dwTime.hour, dwTime.min);

	return TRUE;
}



//-------------------------------------------------------------------------------------
// 转化成: 年-月-日 如：2010-01-04
//-------------------------------------------------------------------------------------
BOOL ECORE_API Date2String(OUT CHAR szTime[], INT nSzArray, const tagDateTime &dwTime)
{
	if( nSzArray <= X_DATETIME_LEN )
	{
		return FALSE;
	}

	sprintf(szTime, "20%02d-%02d-%02d", 
		dwTime.year, dwTime.month, dwTime.day);

	return TRUE;
}

//-------------------------------------------------------------------------------------
// 将 年-月-日 格式的数据转换为tagDateTime
//-------------------------------------------------------------------------------------
BOOL ECORE_API String2DateTime(OUT tagDateTime &dwTime, const CHAR szTime[], INT nSzArray)
{
	dwTime.Clear();

	if( nSzArray != X_DATETIME_LEN )
	{
		return FALSE;
	}

	/************************************
	*  2010-01-04 16:01:55
	*  0123456789ABCDEFGHI19
	*************************************/

	CHAR p[X_DATETIME_LEN + 1];
	memcpy(p, szTime, X_DATETIME_LEN);
	p[4] = p[7] = p[10] = p[13] = p[16] = p[19] = '\0';

	dwTime.year		= atoi(p + 2);
	dwTime.month	= atoi(p + 5);
	dwTime.day		= atoi(p + 8);
	dwTime.hour		= atoi(p + 11);
	dwTime.min		= atoi(p + 14);
	dwTime.sec		= atoi(p + 17);

	return TRUE;
}

//-------------------------------------------------------------------------------------
// 将秒数据转换为时:分:秒格式
//-------------------------------------------------------------------------------------
BOOL ECORE_API Second2String( OUT CHAR szTime[], INT nSzArray, INT nSecond, BOOL bShort )
{
	if (nSzArray < X_SECONDTIME_LEN || nSecond < 0)
	{
		return FALSE;
	}

	INT nHour	= nSecond / X_SECONDS_PER_HOUR;
	nSecond %= X_SECONDS_PER_HOUR;
	INT nMinute	= nSecond / X_SECONDS_PER_MINUTE;
	nSecond %= X_SECONDS_PER_MINUTE;

	if( bShort )
		sprintf(szTime, "%02d:%02d:%02d", nHour, nMinute, nSecond);
	else
		sprintf(szTime, "%03d:%02d:%02d", nHour, nMinute, nSecond);

	return TRUE;
}

BOOL ECORE_API Second2StringW(OUT wchar_t szTime[], INT nSzArray, INT nSecond)
{
    if (nSzArray < X_SECONDTIME_LEN || nSecond < 0)
    {
        return FALSE;
    }

    INT nHour	= nSecond / X_SECONDS_PER_HOUR;
    nSecond %= X_SECONDS_PER_HOUR;
    INT nMinute	= nSecond / X_SECONDS_PER_MINUTE;
    nSecond %= X_SECONDS_PER_MINUTE;

    swprintf(szTime, L"%03d:%02d:%02d", 
        nHour, nMinute, nSecond);

    return TRUE;
}
}//namespace ECore