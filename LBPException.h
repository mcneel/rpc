
#pragma once

#ifndef LBPEXPORT
#define LBPEXPORT
#endif

#include "LBPString.h"

class LBPEXPORT CLBPException
{
public:
	CLBPException(const wchar_t* sMess);
	virtual ~CLBPException() { }

	virtual void Report(void) const = 0;

protected:
	CLBPString m_sMessage;
};

class LBPEXPORT CLBPRunTimeException : public CLBPException
{
public:
	CLBPRunTimeException(const wchar_t* sMess);

	virtual void Report(void) const;
};

class LBPEXPORT CLBPProgramException : public CLBPException
{
public:
	CLBPProgramException(const wchar_t* sMess);
};
