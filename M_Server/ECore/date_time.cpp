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
// �õ���ǰʱ��
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
// �ж�ĳ��ʱ����һ���еĵڼ���
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
// �ж�ĳ��ʱ�����ܼ�(0-6,������-������)
//---------------------------------------------------------------------------
DWORD ECORE_API WhichDayInWeek(const tagDateTime& dwTime)
{
	// ���
	INT nYear = (INT)dwTime.year + 2000;

	// һ���еĵڼ���
	INT nDay = WhichDayInYear(dwTime);

	// ̩�չ�ʽ��x-1+[(x-1)/4]-[(x-1)/100]+[(x-1)/400]+c
	return ( (nYear-1) + (nYear-1)/4 - (nYear-1)/100 + (nYear-1)/400 + nDay ) % 7;
}

//-------------------------------------------------------------------------------------
// �ж�ĳ��ʱ��һ���еĵڼ���
//-------------------------------------------------------------------------------------
DWORD ECORE_API WhichWeekInYear( const tagDateTime& dwTime )
{
	// Ԫ�����ܼ�
	INT nFirstDay = WhichDayInWeek(tagDateTime(0, 0, 0, 1, 1, dwTime.year));

	// ����ָ��ʱ����һ���еĵڼ���
	INT nWhichDay = WhichDayInYear(dwTime);

	// ���㵱Ԫ��Ϊ��һʱָ��ʱ��Ϊ�ڼ���
	INT nWeek = nWhichDay / 7 + ((nWhichDay % 7 != 0) ? 1 : 0);

	// �����������
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
// �����������ڵ�ʱ��������
//-------------------------------------------------------------------------------------
DWORD ECORE_API CalcTimeDiff(tagDateTime dwDstTime, tagDateTime dwSrcTime)
{
	//ASSERT((DWORD)dwDstTime >= (DWORD)dwSrcTime);

	// �Ƿ�Ϊͬһʱ���
	if( *(DWORD*)(&dwDstTime) == *(DWORD*)(&dwSrcTime) )
	{
		return 0;
	}

	// ����ڸ����1��1��0ʱ0��0��������ʱ��
	DWORD dwSrcRun = (WhichDayInYear(dwSrcTime) - 1) * X_SECONDS_PER_DAY 
						+ dwSrcTime.hour * X_SECONDS_PER_HOUR 
						+ dwSrcTime.min * 60 
						+ dwSrcTime.sec;


	// ����ڸ����1��1��0ʱ0��0��������ʱ��
	DWORD dwDestRun = (WhichDayInYear(dwDstTime) - 1) * X_SECONDS_PER_DAY 
						+ dwDstTime.hour * X_SECONDS_PER_HOUR 
						+ dwDstTime.min * 60 
						+ dwDstTime.sec;

	// ������
	for( INT i = dwSrcTime.year; i < (INT)dwDstTime.year; ++i )
	{
		// ���ŵ�������
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
// �����������ڵ����ڲ���Է��ظ�ֵ
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

	// �����stDate1һ�����ڵ���stDate2���������������
	INT nSrc = WhichDayInYear(stDate2);

	INT nDest = WhichDayInYear(stDate1);

	// ������
	for(INT n = stDate2.year; n < (INT)stDate1.year; ++n)
	{
		nDest += CalDaysInYear(n+2000);
	}

	ASSERT(nDest > 0 && nSrc > 0 && nDest >= nSrc);

	INT nDayDiff = nDest - nSrc;
	
	// ʱ�������
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
// ������ݼ����������ŵ�ʱ��(��λ: ��)��������������� Сʱ����
//-------------------------------------------------------------------------------------
tagDateTime CalTimeByYearAndRunSecond(INT nYear, DWORD dwRunSecond)
{
	ASSERT(nYear >= 0 && nYear < 64);

	// ��ʼ�����ʱ��
	tagDateTime dwRetTime;

	// ����
	INT nDays = dwRunSecond / X_SECONDS_PER_DAY;			// ������
	INT nSecs  = dwRunSecond % X_SECONDS_PER_DAY;		// ����ʣ�������

	// �������
	dwRetTime.year = nYear;
	while( nDays >= 365 )
	{
		INT nDaysPerYear = CalDaysInYear((INT)dwRetTime.year + 2000);

		if( nDays < nDaysPerYear )
		{
			// ����1��
			break;
		}

		++dwRetTime.year;
		nDays -= nDaysPerYear;
	}

	// ������
	dwRetTime.month = 1;
	dwRetTime.day = 1;
	while( nDays >= 28 )
	{
		INT nDaysPerMonth = CalDaysInMonth((INT)dwRetTime.year + 2000, dwRetTime.month);

		if( nDays < nDaysPerMonth )
		{
			// ����һ����
			break;
		}

		++dwRetTime.month;
		nDays -= nDaysPerMonth;
	}

	// ������
	dwRetTime.day += nDays;

	// ����Сʱ����
	dwRetTime.hour = nSecs / X_SECONDS_PER_HOUR;
	dwRetTime.min = (nSecs / 60) % 60;
	dwRetTime.sec = nSecs % 60;

	return dwRetTime;
}

//-----------------------------------------------------------------------------
// ĳ��ʱ�����ĳ��ֵ
//-----------------------------------------------------------------------------
tagDateTime ECORE_API IncTime(tagDateTime dwSrcTime, INT nIncSecond)
{
	ASSERT(nIncSecond >= 0);
	
	// ����ڸ����1��1��0ʱ0��0��������ʱ��
	DWORD dwSrcRun = (WhichDayInYear(dwSrcTime) - 1) * X_SECONDS_PER_DAY 
						+ dwSrcTime.hour * X_SECONDS_PER_HOUR 
						+ dwSrcTime.min * 60 
						+ dwSrcTime.sec;


	// ���ϵ�����ʱ��
	DWORD dwRunTime = dwSrcRun + nIncSecond;

	return CalTimeByYearAndRunSecond(dwSrcTime.year, dwRunTime);
}

//-----------------------------------------------------------------------------
// ĳ��ʱ���ȥĳ��ֵ
//-----------------------------------------------------------------------------
tagDateTime ECORE_API DecTime(tagDateTime dwSrcTime, INT nDecSecond)
{
	// ����ڸ����1��1��0ʱ0��0��������ʱ��
	DWORD dwSrcRun = (WhichDayInYear(dwSrcTime) - 1) * X_SECONDS_PER_DAY 
						+ dwSrcTime.hour * X_SECONDS_PER_HOUR 
						+ dwSrcTime.min * 60 
						+ dwSrcTime.sec;

	INT nDaysPerYear = CalDaysInYear((INT)dwSrcTime.year + 2000);
	DWORD dwLeftSecond = (nDaysPerYear * X_SECONDS_PER_DAY - dwSrcRun) + nDecSecond;

	// ��ʼ�����ʱ��
	tagDateTime dwRetTime;

	// �ȼ������
	dwRetTime.year = dwSrcTime.year;
	while( dwLeftSecond >= 365 * X_SECONDS_PER_DAY )
	{
		DWORD dwSecsPerYear = CalDaysInYear((INT)dwRetTime.year + 2000) * X_SECONDS_PER_DAY;
		if( dwLeftSecond < dwSecsPerYear )
		{
			// ����1��
			break;
		}

		--dwRetTime.year;
		dwLeftSecond -= dwSecsPerYear;
	}

	return CalTimeByYearAndRunSecond((INT)dwRetTime.year, CalDaysInYear((INT)dwRetTime.year + 2000) * X_SECONDS_PER_DAY - dwLeftSecond);
}

//-------------------------------------------------------------------------------------
// ת����: ��-��-�� �磺2010-01-04 16:01:55
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
// ת����: ��-��-�� �磺2010-01-04 16:01, ������
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
// ת����: ��-��-�� �磺2010-01-04
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
// �� ��-��-�� ��ʽ������ת��ΪtagDateTime
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
// ��������ת��Ϊʱ:��:���ʽ
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