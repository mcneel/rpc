
#include "stdafx.h"

#include "LBPUnicodeTextFile.h"
#include "LBPString.h"
#include "LBP_UTF.h"

CLBPUnicodeTextFile::CLBPUnicodeTextFile(bool bIsUTF8) : CFile()
{
	m_bIsUTF8 = bIsUTF8;
}

CLBPUnicodeTextFile::CLBPUnicodeTextFile(LPCTSTR lpszFileName, UINT nOpenFlags, bool bIsUTF8) : CFile(lpszFileName, nOpenFlags)
{
	m_bIsUTF8 = bIsUTF8;

	WriteOrReadHeader(nOpenFlags);
}

BOOL CLBPUnicodeTextFile::Open(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError)
{
	BOOL bRet = FALSE;
	int iAttemptsCounter = 0;
	srand((unsigned)time(NULL));

	while (!bRet && (iAttemptsCounter < 100))
	{
//#if _MFC_VER >= 0x0800
//		bRet = Open2(lpszFileName, nOpenFlags, pError);
//#else
		bRet = CFile::Open(lpszFileName, nOpenFlags, pError);
//#endif
		if (!bRet)
		{
			iAttemptsCounter++;

			// Calculate random sleep time (approx 100ms).
			double dRandom = rand();
			dRandom /= RAND_MAX;
			dRandom -= 0.5;
			dRandom *= 10;
			Sleep((ULONG)(100+dRandom));
		}
	}

	if (bRet)
	{
		bRet = WriteOrReadHeader(nOpenFlags);
	}

	return bRet;
}

bool CLBPUnicodeTextFile::WriteOrReadHeader(UINT iFlags)
{
	// Skip or write the unicode flags
	if (iFlags & modeCreate)
	{
		UCHAR pBuf[3];
		int iBOMSize = 2;

		if (m_bIsUTF8)
		{
			pBuf[0] = (UCHAR)0xEF;
			pBuf[1] = (UCHAR)0xBB;
			pBuf[2] = (UCHAR)0xBF;

			iBOMSize = 3;
		}
		else
		{
			pBuf[0] = (UCHAR)0xFF;
			pBuf[1] = (UCHAR)0xFE;
		}

		Write(pBuf, iBOMSize);

		return true;
	} 

	return SkipOverBOM(m_bIsUTF8);
}

bool CLBPUnicodeTextFile::SkipOverBOM(bool& bIsUTF8)
{
	UCHAR pBuf[3];

	if (2 != Read(pBuf, 2))
		return false;

	if (pBuf[0] == (UCHAR)0xFF)
	{
		if (pBuf[1] == (UCHAR)0xFE)
		{
			bIsUTF8 = false;
			return true;
		}
	}

	// This might be UTF8
	if (pBuf[0] == (UCHAR)0xEF)
	{
		if (pBuf[1] == (UCHAR)0xBB)
		{
			if (1 == Read(pBuf + 2, 1))
			{
				if (pBuf[2] == (UCHAR)0xBF)
				{
					bIsUTF8 = true;
					return true;
				}
			}
		}
	}
	
	return false;
}

bool CLBPUnicodeTextFile::ReadEverything(CLBPString& sEverything)
{
	const DWORD dwSize = (DWORD)(GetLength() - GetPosition());

	const DWORD dwSizeIncTerm = dwSize + 2; // Terminator for UTF8 or UTF16.

	BYTE* pBuffer = new BYTE[dwSizeIncTerm];
	::ZeroMemory(pBuffer, dwSizeIncTerm);

	Read(pBuffer, dwSize);

	CLBP_UTF utf;
	if (m_bIsUTF8)
	{
		utf.SetUTF8((const LBP_UTF8_CHAR*)pBuffer);
	}
	else 
	{
		utf.SetUTF16((const LBP_UTF16_CHAR*)pBuffer);
	}

	sEverything = utf.String();
	delete [] pBuffer;

	return !sEverything.IsEmpty();
}


// 15th June 2018 John Croudy, https://mcneel.myjetbrains.com/youtrack/issue/RH-46786
// Length is in Chars. Multiply by size of the char for writing to the file.

bool CLBPUnicodeTextFile::WriteUTF8Line(const CLBPString& sLine)
{
	CLBPString sLine2 = sLine;
	sLine2 += L"\r\n";

	CLBP_UTF utf;
	utf.Set(sLine2);
	const auto lengthInChars = LBP_UTF_LengthInChars<LBP_UTF8_CHAR>(utf.UTF8());
	Write(utf.UTF8(), (UINT)(lengthInChars *  sizeof(LBP_UTF8_CHAR)));

	return true;
}

bool CLBPUnicodeTextFile::WriteUTF16Line(const CLBPString& sLine)
{
	CLBPString sLine2 = sLine;
	sLine2 += L"\r\n";

	CLBP_UTF utf;
	utf.Set(sLine2);
	const auto lengthInChars = LBP_UTF_LengthInChars<LBP_UTF16_CHAR>(utf.UTF16());
	Write(utf.UTF16(), (UINT)(lengthInChars * sizeof(LBP_UTF16_CHAR)));

	return true;
}

bool CLBPUnicodeTextFile::WriteLine(const CLBPString& sLine)
{
	if (m_bIsUTF8)
	{
		return WriteUTF8Line(sLine);
	}
	else
	{
		return WriteUTF16Line(sLine);
	}
}

#ifdef _MFC42
void CLBPUnicodeTextFile::SetLength(DWORD dwNewLen)
#else
void CLBPUnicodeTextFile::SetLength(ULONGLONG dwNewLen)
#endif
{
	const USHORT uBOMLength = m_bIsUTF8 ? 3 : 2;
	if (dwNewLen < uBOMLength)
	{
		// This will destroy the BOM.  Clear the file and re-write the BOM.
		CFile::SetLength(0);
		WriteOrReadHeader(CFile::modeCreate);
	}
	else
	{
		CFile::SetLength(dwNewLen);
	}
}

CLBPFile::CLBPFile() : CFile()
{
}

CLBPFile::CLBPFile(LPCTSTR lpszFileName, UINT nOpenFlags) : CFile(lpszFileName, nOpenFlags)
{
}

CLBPFile::~CLBPFile()
{
}

char CLBPFile::putc(char c)
{
	Write(&c, sizeof(char));
	return c;
}

short CLBPFile::putshort(short s)
{
	Write(&s, sizeof(short));
	return s;
}

char CLBPFile::getc()
{
	char c;
	Read(&c, sizeof(char));
	return c;
}

short CLBPFile::getshort()
{
	short s;
	Read(&s, sizeof(short));
	return s;
}
