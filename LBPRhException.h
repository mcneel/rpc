
#pragma once
#ifndef LBPRHLIB
#error RHLIB header included in non-Rhino compile
#endif

#include "LBPString.h"

class CLBPRhExceptionMessage
{
public:
	CLBPRhExceptionMessage(const CLBPString& sMess, const TCHAR * tszFile, int iLine)
	{
		m_sMess.Format("%s\n\nFile: %s\n\nLine: %u", sMess.Mbcs(), tszFile, iLine);
	}

public:
	CLBPString m_sMess;
};

class CLBPRhRunTimeException : public CLBPException
{
public:
	CLBPRhRunTimeException(const CLBPString& sMess) : CLBPException(sMess) { }

	virtual void Report(void) const;
};
