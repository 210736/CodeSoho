//-----------------------------------------------------------------------------
// Auth: Sxg
// Date: 2010-01-04
//-----------------------------------------------------------------------------
#pragma once

namespace ECore
{

//-----------------------------------------------------------------------------
// ʱ������
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
// ��������
//------------------------------------------------------------------------------------
const INT X_DATETIME_LEN			= 19;				// datetime��ʽ -- 2010-01-04 16:01:55
const INT X_SECONDTIME_LEN			= 10;				// ʱ:��:����ʽ -- 999:01:55

const INT X_SECONDS_PER_MINUTE		= 60;							// һ�����ж�����
const INT X_SECONDS_PER_HOUR		= 60 * X_SECONDS_PER_MINUTE;	// һСʱ�ж�����
const INT X_SECONDS_PER_DAY			= 24 * X_SECONDS_PER_HOUR;		// һ���ж�����
const INT X_SECONDS_PER_WEEK		= 7 * X_SECONDS_PER_DAY;		// һ���ж�����

// �·�									0	1	2	3	4	5	6	7	8	9	10	11	12
const INT X_DAYS_PER_MONTH[]		= {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; // ������


//------------------------------------------------------------------------------------
// DWORD��ʾ��ʱ�� -- ת��ΪDWORD�Ƚϣ�ֵ����ʱ�俿��
//------------------------------------------------------------------------------------
struct ECORE_API tagDateTime
{
	DWORD	sec		: 6,	// 0 - 59
			min		: 6,	// 0 - 59
			hour    : 5,	// 0 - 23
			day		: 5,	// 1 - 31
			month	: 4,	// 1 - 12
			year	: 6;	// ��λ��������:10, ��ʾ2010.����ʾ��2063��

	//--------------------------------------------------------------------------------
	// ����
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
	// ��������
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
	// ת��ΪDWORD
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
// �Ƿ�Ϊ����
//-------------------------------------------------------------------------------------
INLINE BOOL ECORE_API IsLeapYear(INT nYear)
{
	return !(nYear % 4) && (nYear % 100) || !(nYear % 400);
}

//-------------------------------------------------------------------------------------
// ����ָ������м���
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
// ����ָ���·��м���
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
// ������ݼ����������ŵ�ʱ��(��λ: ��)��������������� Сʱ����
//-------------------------------------------------------------------------------------
tagDateTime CalTimeByYearAndRunSecond(INT nYear, DWORD dwRunSecond);

//-------------------------------------------------------------------------------------
// �õ���ǰʱ��
//-------------------------------------------------------------------------------------
tagDateTime ECORE_API GetCurDateTime();

//-------------------------------------------------------------------------------------
// �ж�ĳ��ʱ����һ���еĵڼ���
//-------------------------------------------------------------------------------------
INT ECORE_API WhichDayInYear(const tagDateTime& dwTime);

//-------------------------------------------------------------------------------------
// �ж�ĳ��ʱ�����ܼ�
//-------------------------------------------------------------------------------------
DWORD ECORE_API WhichDayInWeek(const tagDateTime& dwTime);

//-------------------------------------------------------------------------------------
// �ж�ĳ��ʱ��һ���еĵڼ���
//-------------------------------------------------------------------------------------
DWORD ECORE_API WhichWeekInYear(const tagDateTime& dwTime);

//-------------------------------------------------------------------------------------
// ��������ʱ��� -- Ҫ��dwDstTime >= dwSrcTime
//-------------------------------------------------------------------------------------
DWORD ECORE_API CalcTimeDiff(tagDateTime dwDstTime, tagDateTime dwSrcTime);

//-------------------------------------------------------------------------------------
// ��������ʱ������ڲ�
//-------------------------------------------------------------------------------------
INT ECORE_API CalDateDiff(tagDateTime stDate1, tagDateTime stDate2, BYTE byHour = 0, BYTE byMinute = 0, BYTE bySecond = 0);

//-------------------------------------------------------------------------------------
// ĳ��ʱ�����ĳ��ֵ
//-------------------------------------------------------------------------------------
tagDateTime ECORE_API IncTime(tagDateTime dwSrcTime, INT nIncSecond);

//-----------------------------------------------------------------------------
// ĳ��ʱ���ȥĳ��ֵ
//-----------------------------------------------------------------------------
tagDateTime ECORE_API DecTime(tagDateTime dwSrcTime, INT nDecSecond);

//-------------------------------------------------------------------------------------
// ת����: ��-��-�� ʱ:��:�� �磺2010-01-04 16:01:55
//-------------------------------------------------------------------------------------
BOOL ECORE_API Date2String(OUT CHAR szTime[], INT nSzArray, const tagDateTime &dwTime);
BOOL ECORE_API DateTime2String(OUT CHAR szTime[], INT nSzArray, const tagDateTime &dwTime);
BOOL ECORE_API DateTime2StringNoSecond(OUT CHAR szTime[], INT nSzArray, const tagDateTime &dwTime);


//-------------------------------------------------------------------------------------
// �� ��-��-�� ʱ:��:�� ��ʽ������ת��ΪtagDateTime
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
// ��������ת��Ϊʱ:��:���ʽ
//-------------------------------------------------------------------------------------
BOOL ECORE_API Second2String(OUT CHAR szTime[], INT nSzArray, INT nSecond, BOOL bShort=FALSE);
BOOL ECORE_API Second2StringW(OUT wchar_t szTime[], INT nSzArray, INT nSecond);

#ifdef _UNICODE
    #define tSecond2String  ECore::Second2StringW
#else
    #define tSecond2String  ECore::Second2String
#endif

}//namespace ECore