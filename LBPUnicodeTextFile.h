
#pragma once

#ifndef LBPEXPORT
#define LBPEXPORT
#endif

#include "LBPString.h"

class LBPEXPORT CLBPFile : public CFile
{
public:
	CLBPFile();
	CLBPFile(LPCTSTR lpszFileName, UINT nOpenFlags);

	virtual ~CLBPFile();

public:

	char putc(char c);
	char getc();

	short putshort(short s);
	short getshort();



};

class LBPEXPORT CLBPUnicodeTextFile : public CFile  
{
public:
	CLBPUnicodeTextFile(bool bIsUTF8 = false);
	CLBPUnicodeTextFile(LPCTSTR lpszFileName, UINT nOpenFlags, bool bIsUTF8 = false);

	bool WriteLine(const CLBPString& sLine);
	bool ReadEverything(CLBPString& sEverything);

public:
#ifdef _MFC42
	virtual void SetLength(DWORD dwNewLen);
#else
	virtual void SetLength(ULONGLONG dwNewLen);
#endif
	virtual BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError = NULL);

protected:
	bool WriteOrReadHeader(UINT iFlags);
	bool WriteUTF8Line(const CLBPString& sLine);
	bool WriteUTF16Line(const CLBPString& sLine);
	bool SkipOverBOM(bool& bIsUTF8);

protected:
	bool m_bIsUTF8;
};
