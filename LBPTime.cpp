
#include "stdafx.h"
#include "LBPTime.h"
#include "LBPLibUtilities.h"


CLBPTime::CLBPTime(const CLBPString& s)
{
	SetAsString(s);
}

CLBPTime& CLBPTime::operator=(const CLBPString& s)
{
	if(IsValidTime(s))
	{
		SetAsString(s);
	}
	else
	{
		*this = CLBPTime(0);
	}
	return *this;
}


CLBPString CLBPTime::AsString(void) const
{
	CLBPString s;
	s.Format(L"%04u.%02u.%02u_%02u:%02u:%02u", GetYear(), GetMonth(),  GetDay(),
						   GetHour(), GetMinute(), GetSecond());
	return s;
}

bool CLBPTime::SetAsString(const CLBPString& s)
{
	if(!IsValidTime(s))
		return false;

	const WCHAR* const wsz = s;
	*this = CLBPTime(LBP_wtoi(wsz), LBP_wtoi(wsz+5), LBP_wtoi(wsz+8), LBP_wtoi(wsz+11), LBP_wtoi(wsz+14), LBP_wtoi(wsz+17));
	return true;
}

bool CLBPTime::IsValidTime(const CLBPString& sTime)
{
	if (19 != sTime.GetLength())
		return false;

	const wchar_t* sz = sTime.Wide();

	if ((sz[4] != L'.') || (sz[7] != L'.') || (sz[10] != L'_') || (sz[13] != L':') || (sz[16] != L':'))
		return false;

	const int y = LBP_wtoi(sz);
	const int m = LBP_wtoi(sz+5);
	const int d = LBP_wtoi(sz+8);
	const int h = LBP_wtoi(sz+11);
	const int n = LBP_wtoi(sz+14);
	const int s = LBP_wtoi(sz+17);

	if ((y < 1900) || (y > 2500))
		return false;

	if ((m < 1) || (m > 12))
		return false;

	if ((d < 1) || (d > 31))
		return false;

	if ((h < 0) || (h > 23))
		return false;

	if ((n < 0) || (n > 59))
		return false;

	if ((s < 0) || (s > 59))
		return false;

	return true;
}
