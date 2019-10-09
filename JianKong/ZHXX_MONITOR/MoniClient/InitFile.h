#pragma once
#include <string>
class CInitFile
{
public:
	CInitFile(void);
	~CInitFile(void);

	void GetGroupCastIP(TCHAR* ip);
	int GetGroupCastPort();
	int GetTimeInterval();

};

