#pragma once

#ifndef LBPEXPORT
#define LBPEXPORT
#endif

#include "LBPString.h"

class LBPEXPORT CLBPTime : public CTime
{
public:
	CLBPTime() { }

	CLBPTime(time_t time) : CTime(time) { }

	CLBPTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec, int nDST = -1)
	 : CTime(nYear, nMonth, nDay, nHour, nMin, nSec, nDST) { }

#if defined LBPLIB_WINDOWS_SPECIFIC
	CLBPTime(WORD wDosDate, WORD wDosTime, int nDST = -1)
	 : CTime(wDosDate, wDosTime, nDST) { }

	CLBPTime(const SYSTEMTIME& st, int nDST = -1)
	 : CTime(st, nDST) { }

	CLBPTime(const FILETIME& ft, int nDST = -1)
	 : CTime(ft, nDST) { }
#endif

	CLBPTime(const CLBPString& s);
	CLBPTime& operator=(const CLBPString& s);

	CLBPString AsString(void) const;
	bool SetAsString(const CLBPString& s);

	static bool IsValidTime(const CLBPString& s);
};
