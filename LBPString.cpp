
#include "stdafx.h"
#include "LBPString.h"
#include "LBPLibUtilities.h"
#include "LBP_CRC32.h"

//#define LBP_STRING_STATISTICS

#ifdef LBP_STRING_STATISTICS
static unsigned int g_iCount = 0;
static size_t g_TotalAllocated = 0;
#define INCREMENT_COUNT g_iCount++;
#else
#define INCREMENT_COUNT
#endif

wchar_t* CLBPString::m_wszEmpty = L"";

CLBPString::CLBPString()
	:
	m_wsz(m_wszEmpty),
	m_dwLengthCache(0)
{
	INCREMENT_COUNT
}

CLBPString::~CLBPString()
{
#ifdef LBP_STRING_STATISTICS
	g_iCount--;
	g_TotalAllocated -= m_dwSizeAllocation;
#endif

#ifdef _DEBUG
	//Deliberately demolish the contents of the string to make sure
	//out of scope errors are easy to see

	if(!IsEmpty())
	{
		const size_t dwWideSize = wcslen(m_wsz);
		for(size_t i=0;i<dwWideSize;i++)
		{
			m_wsz[i] = L'#';
		}
		m_wsz[dwWideSize] = 0;
	}

	if(m_sz)
	{
		const size_t dwMBCSSize = strlen(m_sz);
		FillMemory(m_sz, dwMBCSSize, '#');
	}
#endif

	if (m_wszEmpty != m_wsz && NULL != m_wsz)
	{
		LBPFREE(m_wsz);
	}
	if (m_sz)
	{
		LBPFREE(m_sz);
	}
}

CLBPString::CLBPString(const CLBPString& stringSrc)
{
	INCREMENT_COUNT
	*this = stringSrc;
}

CLBPString::CLBPString(const char* sz)
{
	INCREMENT_COUNT
	*this = sz;
}

CLBPString::CLBPString(const wchar_t* wsz)
{
	INCREMENT_COUNT
	*this = wsz;
}

CLBPString::CLBPString(wchar_t wch)
{
	INCREMENT_COUNT
	*this = wch;
}

#ifdef _NATIVE_WCHAR_T_DEFINED
CLBPString::CLBPString(const USHORT* wsz)
{
	INCREMENT_COUNT
	*this = wsz;
}

CLBPString::CLBPString(USHORT wch)
{
	INCREMENT_COUNT
	*this = wch;
}

CLBPString::CLBPString(const USHORT* wsz, DWORD dwCharacters)
{
	INCREMENT_COUNT

	Allocate(dwCharacters);

	wcsncpy(m_wsz, (const wchar_t*)wsz, dwCharacters);
	m_wsz[dwCharacters] = 0;

	m_dwLengthCache = dwCharacters;
}
#endif

CLBPString::CLBPString(char ch)
{
	INCREMENT_COUNT
	*this = ch;
}

CLBPString::CLBPString(const wchar_t* wsz, DWORD dwCharacters)
{
	INCREMENT_COUNT

	Allocate(dwCharacters);

	wcsncpy(m_wsz, wsz, dwCharacters);
	m_wsz[dwCharacters] = 0;

	m_dwLengthCache = dwCharacters; // John
}

void LBP_SetStringTestOptimizationsOn(void)
{
	//g_bOptimizeStringGetLength = true;
}

DWORD CLBPString::CRC() const
{
	if(IsEmpty())
		return 0x1234;

	return CLBP_CRC32::Calculate(m_wsz, GetLength() * sizeof(wchar_t));
}

char CLBPString::GetAtMultibyte(int nIndex) const
{
	return ConvertWideCharToMBCSChar(m_wsz[nIndex]);
}

void CLBPString::SetAt(int nIndex, char ch)
{
	SetAt(nIndex, ConvertMBCSCharToWideChar(ch));
}

const char* CLBPString::AsMBCSString() const
{
	SyncMBCS();
	return m_sz;
}

const char* CLBPString::Mbcs() const
{
	SyncMBCS();
	return m_sz;
}

wchar_t* CLBPString::AsNonConstWideString() const
{
	return const_cast<wchar_t*>(AsWideString());
}

char* CLBPString::AsNonConstMBCSString() const
{
	return const_cast<char*>(AsMBCSString());
}

#ifdef _UNICODE
wchar_t* CLBPString::AsNonConstTString() const
{
	return AsNonConstWideString();
}

const wchar_t* CLBPString::AsTString() const
{
	return AsWideString();
}

const wchar_t* CLBPString::T() const
{
	return AsWideString();
}

#else
char* CLBPString::AsNonConstTString() const
{
	return AsNonConstMBCSString();
}

const char* CLBPString::AsTString() const
{
	return AsMBCSString();
}

const char* CLBPString::T() const
{
	return AsMBCSString();
}
#endif


bool CLBPString::operator==(const wchar_t* wsz) const
{
	return 0==CompareNoCase(wsz);
}

bool CLBPString::operator==(const char* sz) const
{
	return 0==CompareNoCase(sz);
}

bool CLBPString::operator!=(const wchar_t* wsz) const
{
	return !(*this==wsz);
}

bool CLBPString::operator!=(const char* sz) const
{
	return !(*this==sz);
}

bool CLBPString::operator<(const CLBPString& s2) const
{
  return (CompareNoCase(s2.Wide()) < 0) ? true : false;
}

bool CLBPString::operator>(const CLBPString& s2) const
{
  return (CompareNoCase(s2.Wide()) > 0) ? true : false;
}

bool CLBPString::operator<=(const CLBPString& s2) const
{
  return (CompareNoCase(s2.Wide()) <= 0) ? true : false;
}

bool CLBPString::operator>=(const CLBPString& s2) const
{
  return (CompareNoCase(s2.Wide()) >= 0) ? true : false;
}




bool AFXAPI operator == (const wchar_t * wsz, const CLBPString& s)
{
	return 0 == s.CompareNoCase(wsz);
}
bool AFXAPI operator != (const wchar_t * wsz, const CLBPString& s)
{
	return 0 != s.CompareNoCase(wsz);
}

bool AFXAPI operator == (const char * wsz, const CLBPString& s)
{
	return 0 == s.CompareNoCase(wsz);
}
bool AFXAPI operator != (const char * wsz, const CLBPString& s)
{
	return 0 != s.CompareNoCase(wsz);
}


// overloaded assignment

/*const CLBPString& CLBPString::operator=(const CLBPString& stringSrc) const
{
	m_wsz = stringSrc.m_wsz;
	m_dwLengthCache = stringSrc.m_dwLengthCache;
	m_sz = stringSrc.m_sz;
	m_dwSizeAllocation = 0;
}*/

const CLBPString& CLBPString::operator=(const CLBPString& stringSrc)
{
	//This is an optimization - we can't put the m_wszEmpty check in the header
	//because it won't link, but I don't want to un-inline the IsEmpty call.  This is the function
	//that most needs the optimization - see also operator=(const wchar_t*)
	if(stringSrc.m_wszEmpty == stringSrc.m_wsz || stringSrc.IsEmpty())
	{
		Empty();
		return *this;
	}

	//Optimization for return by value
	if (stringSrc.m_bAllowAutoReleaseOwnership)
	{
		const int iLength = stringSrc.GetLength();
		wchar_t* p = const_cast<CLBPString&>(stringSrc).ReleaseOwnership();
		TakeOwnershipOfBuffer(p, iLength);
		//TakeOwnershipOfBuffer(p);
		m_dwLengthCache = iLength;
	}
	else
	{
		const DWORD dwLen = stringSrc.GetLength()+1;

		Allocate(dwLen);
		memmove(m_wsz, stringSrc.m_wsz, dwLen*sizeof(wchar_t));

		m_dwLengthCache = dwLen-1;
	}

	return *this;
}


const CLBPString& CLBPString::operator=(wchar_t ch)
{
	Allocate(1);

	m_wsz[0] = ch;
	m_wsz[1] = 0;

	m_dwLengthCache = 1;

	return *this;

}

const CLBPString& CLBPString::operator=(char ch)
{
	return *this = ConvertMBCSCharToWideChar(ch);
}

const CLBPString& CLBPString::operator=(const char* lpsz)
{
	if(!lpsz)
	{
		Empty();
		return *this;
	}

	size_t dwChars = strlen(lpsz);
	Allocate(dwChars);

#ifdef LBPLIB_WINDOWS_SPECIFIC
	MultiByteToWideChar(CP_ACP, 0, lpsz, -1, m_wsz, (int)GetAllocation());
#else
	mbstowcs(m_wsz, lpsz, dwChars+1);
#endif

	m_dwLengthCache = (int)dwChars;

	return *this;
}

const CLBPString& CLBPString::operator=(const wchar_t* lpsz)
{
	if(m_wszEmpty == lpsz || NULL == lpsz || lpsz[0] == 0)
	{
		Empty();
		return *this;
	}

	if(m_wsz && wcscmp(m_wsz, lpsz) == 0)
	{
		return *this;
	}

	const DWORD dwLen = (DWORD)wcslen(lpsz)+1;

//#ifdef _DEBUG
//	if(dwLen > 15)
//	{
//		Sleep(0);	//Good place for a breakpoint.
//	}
//#endif

	Allocate(dwLen);
	//wcscpy(m_wsz, lpsz);
	//memmove should be a little faster - doesn't have to check for the terminator
	memmove(m_wsz, lpsz, dwLen*sizeof(wchar_t));

	m_dwLengthCache = dwLen-1;

	return *this;
}

const CLBPString& CLBPString::Set(const wchar_t* lpsz, DWORD dwCharacters)
{
	Allocate(dwCharacters);
	memmove(m_wsz, lpsz, dwCharacters * sizeof(wchar_t));
	m_wsz[dwCharacters] = 0;
	m_dwLengthCache = dwCharacters;

	return *this;
}

const CLBPString& CLBPString::Set(const char* lpsz, DWORD dwCharacters)
{
	CLBPString sCopy = lpsz;

	return *this = Set(sCopy.Wide(), dwCharacters);
}

const CLBPString& CLBPString::Set(double d)
{
    const int iBufferSize = 50;
    GetWideBuffer(iBufferSize);
    
#ifdef LBPRHLIB
    ON_wString::FormatIntoBuffer(m_wsz, iBufferSize, L"%.15g", d);
#else
	_swprintf_l(m_wsz, iBufferSize, L"%.15g", LBP_Locale(), d);
#endif
    ReleaseWideBuffer();
    return *this;
}

const CLBPString& CLBPString::Set(float f)
{
    const int iBufferSize = 50;
    GetWideBuffer(iBufferSize);
    
#ifdef LBPRHLIB
    ON_wString::FormatIntoBuffer(m_wsz, iBufferSize, L"%.4f", f);
#else
	_swprintf_l(m_wsz, iBufferSize, L"%.4f", LBP_Locale(), f);
#endif
    
    ReleaseWideBuffer();
    return *this;
}

const CLBPString& CLBPString::Set(int i)
{
	const int iBufferSize = 20;
	GetWideBuffer(iBufferSize);
#ifdef LBPRHLIB
    ON_wString::FormatIntoBuffer(m_wsz, iBufferSize, L"%d", i);
#else
	_swprintf_l(m_wsz, iBufferSize, L"%d", LBP_Locale(), i);
#endif
	ReleaseWideBuffer();

	return *this;
}

// string concatenation

// concatenate from another CLBPString
const CLBPString& CLBPString::operator+=(const CLBPString& string)
{
	//return *this += string.AsWideString();
	if (string.IsEmpty())
		return *this;

	return Append(string.Wide(), string.GetLength());
}

// concatenate a single character
const CLBPString& CLBPString::operator+=(char ch)
{
	return *this += ConvertMBCSCharToWideChar(ch);
}

const CLBPString& CLBPString::operator+=(wchar_t ch)
{
	const DWORD dwRequired = GetLength() + 1;

	Allocate(dwRequired);

	m_wsz[dwRequired-1] = ch;
	m_wsz[dwRequired] = 0;

	m_dwLengthCache = dwRequired;

	return *this;
}

const CLBPString& CLBPString::operator+=(const char* lpsz)
{
	return *this += CLBPString(lpsz);
}

const CLBPString& CLBPString::operator+=(const wchar_t* lpsz)
{
	if(!lpsz) return *this;
	if(0 == *lpsz) return *this;

	const int iLengthIncoming = (int)wcslen(lpsz);

	return Append(lpsz, iLengthIncoming);

}
  
const CLBPString& CLBPString::Append(const wchar_t* lpsz, int iCount)
{
	if(!lpsz) return *this;
	if(0 == *lpsz) return *this;
	if (0==iCount)
		return *this;

	const int iLength = GetLength();

	ASSERT(*(m_wsz+iLength) == 0);

	CLBPString* pS = NULL;

	const wchar_t* wszToUse = lpsz;

	// Always do this check; we MUST copy the string if it's a subset of the current string.
	if ((lpsz >= m_wsz) && (lpsz < m_wsz+iLength+1))
	{
		// lpsz is a subset of the current string - this means we have to copy it, unfortunately
		pS = new CLBPString(lpsz);
		wszToUse = pS->Wide();
		#pragma message ("CLBPString: Subset optimisation")
	}

	const int iLengthIncoming = iCount;
	const DWORD dwRequired = (DWORD)(iLength + iLengthIncoming + 1);

	Allocate(dwRequired);

	memmove(m_wsz+iLength, wszToUse, iLengthIncoming*sizeof(wchar_t));
	*(m_wsz+iLength+iLengthIncoming) = 0;

	m_dwLengthCache = dwRequired - 1;

	delete pS;

	return *this;
}


//These functions rely on automatic conversion - it's critical they don't use the BV form
#define LBPSTRING_IMPL_OPERATORADD CLBPStringBV s(string);s+=ch;return s
#define LBPSTRING_IMPL_OPERATORADD_PREFIX CLBPStringBV s(ch);s+=string;return s

CLBPStringBV AFXAPI operator+(const CLBPString& string1, const CLBPString& string2)
{
	CLBPStringBV s(string1);
	s+=string2;

	return s;
}

CLBPStringBV AFXAPI operator+(const CLBPString& string, char ch) {LBPSTRING_IMPL_OPERATORADD;}
CLBPStringBV AFXAPI operator+(const CLBPString& string, wchar_t ch) {LBPSTRING_IMPL_OPERATORADD;}
CLBPStringBV AFXAPI operator+(const CLBPString& string, const char* ch) {LBPSTRING_IMPL_OPERATORADD;}
CLBPStringBV AFXAPI operator+(const CLBPString& string, const wchar_t* ch) {LBPSTRING_IMPL_OPERATORADD;}

CLBPStringBV AFXAPI operator+(char ch, const CLBPString& string) {LBPSTRING_IMPL_OPERATORADD_PREFIX;}
CLBPStringBV AFXAPI operator+(wchar_t ch, const CLBPString& string) {LBPSTRING_IMPL_OPERATORADD_PREFIX;}
CLBPStringBV AFXAPI operator+(const char* ch, const CLBPString& string) {LBPSTRING_IMPL_OPERATORADD_PREFIX;}
CLBPStringBV AFXAPI operator+(const wchar_t* ch, const CLBPString& string) {LBPSTRING_IMPL_OPERATORADD_PREFIX;}

#ifdef _NATIVE_WCHAR_T_DEFINED
CLBPStringBV AFXAPI operator+(const CLBPString& string, const USHORT* lpsz)		{ return operator+(string, (const wchar_t*)lpsz); }
CLBPStringBV AFXAPI operator+(const USHORT* lpsz, const CLBPString& string)		{ return operator+((const wchar_t*)lpsz, string); }
CLBPStringBV AFXAPI operator+(USHORT ch, const CLBPString& string)				{ return operator+((wchar_t)ch, string); }
CLBPStringBV AFXAPI operator+(const CLBPString& string, USHORT ch)				{ return operator+(string, (wchar_t)ch); }
#endif
// string comparison
int CLBPString::Compare(const wchar_t* wsz) const
{
	if ((NULL == wsz) && IsEmpty())
		return 0;

	return wcscmp(m_wsz, (NULL == wsz) ? L"" : wsz);
}

int CLBPString::CompareNoCase(const wchar_t* wsz) const
{
	if ((NULL == wsz) && IsEmpty())
		return 0;

	return _wcsicmp(m_wsz, (NULL == wsz) ? L"" : wsz);
}

int CLBPString::Compare(const char* wsz) const
{
	return Compare(CLBPString(wsz).AsWideString());
}

int CLBPString::CompareNoCase(const char* wsz) const
{
	return CompareNoCase(CLBPString(wsz).AsWideString());
}

// NLS aware comparison, case sensitive
int CLBPString::Collate(const wchar_t* wsz) const
{
	if ((NULL == wsz) && IsEmpty())
		return 0;

#ifdef LBPLIB_APPLE_SPECIFIC
	return wcscoll(m_wsz, (NULL == wsz) ? L"" : wsz);
#else
	return _wcsicoll(m_wsz, (NULL == wsz) ? L"" : wsz);
#endif
}

int CLBPString::CollateNoCase(const wchar_t* wsz) const
{
	if ((NULL == wsz) && IsEmpty())
		return 0;

	return wcscoll(m_wsz, (NULL == wsz) ? L"" : wsz);
}

int CLBPString::Collate(const char* wsz) const
{
	return Collate(CLBPString(wsz).AsWideString());
}

int CLBPString::CollateNoCase(const char* wsz) const
{
	return CollateNoCase(CLBPString(wsz).AsWideString());
}

// simple sub-string extraction
CLBPStringBV CLBPString::Mid(int nFirst, int nCount) const
{
	if (nFirst < 0) nFirst = 0;
	if (nCount < 0) nCount = 0;

	const int iLength = GetLength();

	if (nFirst + nCount > iLength)		nCount = iLength - nFirst;
	if (nFirst > iLength)				nCount = 0;

	// optimize case of returning entire string
	if (nFirst == 0 && nCount == iLength)
		return *this;

	//Cut the string so that the copy will only take the correct bit
	wchar_t w = m_wsz[nFirst+nCount];
	m_wsz[nFirst+nCount] = 0;

	const CLBPString s(m_wsz+nFirst, nCount);	//Copy the string from the start of the block
	s.SetAllowAutoReleaseOwnership();

	//Put the character back
	m_wsz[nFirst+nCount] = w;

	return s;
}

const wchar_t* CLBPString::Mid(int nFirst) const
{
	if (nFirst < 0)		nFirst = 0;

	const wchar_t* p = m_wsz;

	const int iLength = GetLength();

	if (nFirst <= iLength)
		p = m_wsz + nFirst;

#ifdef _DEBUG
	#pragma message("CLBPString::Mid(int) - regression test included.\n")
	ASSERT(Mid(nFirst, GetLength()-nFirst).Compare(p)==0);
#endif

	return p;
}

CLBPStringBV CLBPString::Left(int nCount) const
{
	return Mid(0, nCount);
}

void CLBPString::Truncate(int nCount)
{
	if (nCount < 0) nCount = 0;

#ifdef _DEBUG
	m_bAllowAutoReleaseOwnership = false;
	CLBPString sCopy(*this);
#endif
	if (0==nCount)
	{
		Empty();
	}
	else
	{
		const int iLength = GetLength();
		if (nCount < iLength)
		{
			m_wsz[nCount] = 0;
			m_dwLengthCache = nCount;
		}
	}

#ifdef _DEBUG
	#pragma message("CLBPString::Truncate - regression test included.\n")
	ASSERT(Compare(sCopy.Left(nCount).Wide())==0);
#endif
	
	return;
}

void CLBPString::TruncateMid(int nPos)
{
	if (nPos < 0) nPos = 0;

#ifdef _DEBUG
	#pragma message("CLBPString::TruncateMid - regression test included.\n")
	CLBPString s = Mid(nPos);
#endif

	const int iLength = GetLength();

	if (nPos <= iLength)
	{
		memmove(m_wsz, m_wsz+nPos, (iLength-nPos+1)*sizeof(wchar_t));

		m_dwLengthCache = iLength-nPos;
	}

	ASSERT(s.Compare(m_wsz)==0);
}

const wchar_t* CLBPString::Right(int nCount) const 
{
	if (nCount < 0) nCount = 0;

	const wchar_t* p = m_wsz;

	const int iLength = GetLength();

	if (nCount < iLength)
		p = m_wsz + (iLength-nCount);

#ifdef _DEBUG
	#pragma message("CLBPString::Right - regression test included.\n")
	ASSERT(CLBPString(Mid(GetLength()-nCount)).Compare(p)==0);
#endif

	return p;
}



//  characters from beginning that are also in passed string
CLBPStringBV CLBPString::SpanIncluding(const char* lpszCharSet) const
{
	return SpanIncluding(CLBPString(lpszCharSet).AsWideString());
}

CLBPStringBV CLBPString::SpanIncluding(const wchar_t* lpszCharSet) const
{
	return Left((int)wcsspn(m_wsz, lpszCharSet));
}

CLBPStringBV CLBPString::SpanExcluding(const char* lpszCharSet) const
{
	return SpanExcluding(CLBPString(lpszCharSet).AsWideString());
}

CLBPStringBV CLBPString::SpanExcluding(const wchar_t* lpszCharSet) const
{
	return Left((int)wcscspn(m_wsz, lpszCharSet));
}

#ifdef _NATIVE_WCHAR_T_DEFINED
CLBPStringBV CLBPString::SpanIncluding(const USHORT* lpszCharSet) const { return SpanIncluding((const wchar_t*)lpszCharSet); }
CLBPStringBV CLBPString::SpanExcluding(const USHORT* lpszCharSet) const { return SpanExcluding((const wchar_t*)lpszCharSet); }
#endif

void CLBPString::MakeUpper()
{
	if(IsEmpty()) return;
	_wcsupr(m_wsz);
}

void CLBPString::MakeLower()
{
	if(IsEmpty()) return;
	_wcslwr(m_wsz);
}

void CLBPString::MakeReverse()
{
	if(IsEmpty()) return;
	_wcsrev(m_wsz);
}


void CLBPString::TrimRight()
{
	const int iLength = GetLength();

	for (int i=iLength-1; i>=0; i--)
	{
		if (iswspace(m_wsz[i]))
		{
			m_dwLengthCache--;
			m_wsz[i] = 0;
		}
		else
		{
			break;
		}
	}
}

void CLBPString::TrimLeft()
{
	wchar_t* lpsz = m_wsz;
	int iCut = 0;

	while (*lpsz != '\0')
	{
		if (!iswspace(*lpsz)) break;
		lpsz++;
		iCut++;
	}

	if (lpsz != m_wsz)
	{
		// fix up data and length
		int nDataLength = (int)(GetLength() - (lpsz - m_wsz));
		memmove(m_wsz, lpsz, (nDataLength+1)*sizeof(wchar_t));
	}

	m_dwLengthCache -= iCut;
}


// trimming anything (either side)

// remove continuous occurrences of chTarget starting from right
void CLBPString::TrimRight(char chTarget)
{
	TrimRight(ConvertMBCSCharToWideChar(chTarget));
}

void CLBPString::TrimRight(const char* lpszTargets)
{
	TrimRight(CLBPString(lpszTargets).AsWideString());
}

void CLBPString::TrimLeft(char chTarget)
{
	TrimLeft(ConvertMBCSCharToWideChar(chTarget));
}

void CLBPString::TrimLeft(const char* lpszTargets)
{
	TrimLeft(CLBPString(lpszTargets).AsWideString());
}

void CLBPString::TrimRight(wchar_t w)
{
	//TrimRight(CLBPString(w).AsWideString());

	const int iLength = GetLength();

	for (int i=iLength-1; i>=0; i--)
	{
		if (m_wsz[i] == w)
		{
			m_dwLengthCache--;
			m_wsz[i] = 0;
		}
		else
		{
			break;
		}
	}
}

static __forceinline bool is_char_in_list(wchar_t ch, const wchar_t* list, size_t count)
{
	for (size_t i=0;i<count;i++)
	{
		if (ch == list[i])
		{
			return true;
		}
	}
	return false;
}

void CLBPString::TrimRight(const wchar_t* lpszTargetList)
{
	//CLBPString tl(lpszTargetList);
	const size_t iTargetCount = wcslen(lpszTargetList);

	for (int i=GetLength()-1; i>=0; i--)
	{
		//if (tl.Find(m_wsz[i]) != -1)
		if (is_char_in_list(m_wsz[i], lpszTargetList, iTargetCount))
		{
			m_wsz[i] = 0;
			m_dwLengthCache--;
		}
		else
		{
			break;
		}
	}
}

void CLBPString::TrimLeft(wchar_t chTarget)
{
	//TrimLeft(CLBPString(chTarget).AsWideString());

	wchar_t* lpsz = m_wsz;
	int iCut = 0;

	while (*lpsz != '\0')
	{
		//if (!iswspace(*lpsz)) break;
		if (*lpsz != chTarget) break;
		lpsz++;
		iCut++;
	}

	if (lpsz != m_wsz)
	{
		// fix up data and length
		int nDataLength = (int)(GetLength() - (lpsz - m_wsz));
		memmove(m_wsz, lpsz, (nDataLength+1)*sizeof(wchar_t));
	}

	m_dwLengthCache -= iCut;
}

void CLBPString::TrimLeft(const wchar_t* lpszTargets)
{
	if (wcslen(lpszTargets) == 0) return;

	wchar_t* lpsz = m_wsz;
	int iCut = 0;

	while (*lpsz != '\0')
	{
		if (wcschr(lpszTargets, *lpsz) == NULL)
			break;
		lpsz++;
		iCut++;
	}

	if (lpsz != m_wsz)
	{
		// fix up data and length
		int nDataLength = (int)(GetLength() - (lpsz - m_wsz));
		memmove(m_wsz, lpsz, (nDataLength+1)*sizeof(wchar_t));
	}

	m_dwLengthCache-=iCut;
}

// advanced manipulation

int CLBPString::Replace(char chOld, char chNew)
{
	return Replace(ConvertMBCSCharToWideChar(chOld), ConvertMBCSCharToWideChar(chNew));
}

int CLBPString::Replace(wchar_t chOld, wchar_t chNew)
{
	int nCount = 0;

	const int iLength = GetLength();

	if (chOld != chNew)
	{
		for(int i=0;i<iLength;i++)
		{
			if(m_wsz[i] == chOld)
			{
				m_wsz[i] = chNew;
				nCount++;
			}
		}
	}

	//Just in case chNew was /0
	m_dwLengthCache = -1;
	return nCount;
}

int CLBPString::Replace(const char* lpszOld, const char* lpszNew)
{
	CLBPString sOld(lpszOld);
	CLBPString sNew(lpszNew);

	return Replace(sOld.AsWideString(), sNew.AsWideString());

}

int CLBPString::Replace(const wchar_t* lpszOld, const wchar_t* lpszNew)
{
	if(IsEmpty())
		return 0;

	const int iNewLength = (int)wcslen(lpszNew);
	const int iOldLength = (int)wcslen(lpszOld);

	ASSERT(iOldLength != 0);

	const bool bSingleCharOld = iOldLength == 1;

	int iPos = 0, i=0, iCount = 0;
	const int iGrow = (int)(iNewLength-iOldLength);
	CLBPString sSuffix;

	//Keep trying to find the string until there are no more left
	while((i=(bSingleCharOld ? Find(*lpszOld, iPos) : Find(lpszOld, iPos))) != -1)
	{
		//i is the current start of the string to be replaced

		iCount++;
		//In this case, no reallocation or moving has to be done
		if(iGrow==0)
		{
			memmove((void*)(m_wsz+i), (void*)lpszNew, iOldLength*sizeof(wchar_t));
		}
		else
		{
			//Only allocate more space if we need to (hopefully this will only happen once)
			const int iLength = GetLength();

			if(iGrow > 0)
			{
				Allocate(iLength+iGrow+1);
			}

			const int iCrop = i+iOldLength;
			sSuffix = Mid(iCrop);
			const int iSufLen = iLength - (iCrop);

			memmove(m_wsz+i, lpszNew, iNewLength*sizeof(wchar_t));
			memmove(m_wsz+i+iNewLength, sSuffix.Wide(), iSufLen*sizeof(wchar_t));

			m_wsz[i+iNewLength+iSufLen]=0;

			m_dwLengthCache+=iGrow;
		}
		iPos =i+iNewLength;
	}

	return iCount;
}

int CLBPString::Remove(char chRemove)
{
	return Remove(ConvertMBCSCharToWideChar(chRemove));

}

/*int CLBPString::Remove(wchar_t chRemove)
{
	return Replace(chRemove, L'');
}*/

// Remove all occurrences of character 'chRemove'
int CLBPString::Remove(wchar_t chRemove)
{
	const int nLength = GetLength();
	if (0 == nLength) return 0;

	wchar_t* pszBuffer = m_wsz;

	wchar_t* pszSource = pszBuffer;
	wchar_t* pszDest = pszBuffer;
	wchar_t* pszEnd = pszBuffer+nLength;

	while( pszSource < pszEnd )
	{
		wchar_t* pszNewSource = pszSource+1;
		if( *pszSource != chRemove )
		{
			// Copy the source to the destination.  Remember to copy all bytes of an MBCS character
			const size_t NewSourceGap = 1;
			wchar_t* pszNewDest = pszDest + NewSourceGap;
			size_t i = 0;
			for (i = 0;  pszDest != pszNewDest && i < NewSourceGap; i++)
			{
				*pszDest = *pszSource;
				pszSource++;
				pszDest++;
			}
		}
		pszSource = pszNewSource;
	}
	*pszDest = 0;
	int nCount = int( pszSource-pszDest );

	m_dwLengthCache = -1;

	return nCount;
}



int CLBPString::Insert(int nIndex, char ch, int iCount)
{
	//return Insert(nIndex, CLBPString(ch).AsWideString());
	return Insert(nIndex, ConvertMBCSCharToWideChar(ch), iCount);
}

int CLBPString::Insert(int nIndex, wchar_t ch, int iCount)
{
	if(nIndex < 0) nIndex = 0;
	if(nIndex > GetLength()) nIndex = GetLength();

	const int iLength = GetLength();
	const int iNewLength = iLength + iCount;

	Allocate(iNewLength);

	memmove(m_wsz+iCount+nIndex, m_wsz+nIndex, (iLength-nIndex+1)*sizeof(wchar_t)); // 29.8.2011 John Croudy, subtract index from length.

	for (int i=0;i<iCount;i++)
	{
		m_wsz[i+nIndex]=ch;
	}

	m_dwLengthCache = iNewLength;

	return GetLength();
}

int CLBPString::Insert(int nIndex, const char* pstr)
{
	return Insert(nIndex, CLBPString(pstr).AsWideString());
}

int CLBPString::Insert(int nIndex, const wchar_t* pSource)
{
	if(nIndex < 0) nIndex = 0;
	if(nIndex > GetLength()) nIndex = GetLength();

	//const CLBPString sSuffix = Mid(nIndex);

	const int iLength = GetLength();
	const size_t iLengthSource = wcslen(pSource);
	const int iNewLength = (int)(iLengthSource+iLength);

	Allocate(iNewLength);

	memmove(m_wsz+iLengthSource+nIndex, m_wsz+nIndex, (iLength-nIndex+1)*sizeof(wchar_t)); // 29.8.2011 John Croudy, subtract index from length.
	memmove(m_wsz+nIndex, pSource, iLengthSource*sizeof(wchar_t));

	//m_wsz[nIndex] = 0;
	//wcscat(m_wsz, pSource);
	//wcscat(m_wsz, sSuffix);

	m_dwLengthCache = iNewLength;

	return GetLength();
}

int CLBPString::Delete(int nIndex, int nCount)
{
	CLBPString sSuffix = Mid(nIndex+nCount);
	m_wsz[nIndex] = 0;
	wcscat(m_wsz, sSuffix);

	m_dwLengthCache = -1;

	return GetLength();
}


	// searching
int CLBPString::FindOneOf(const char* lpszCharSet) const {	return FindOneOf(CLBPString(lpszCharSet).AsWideString());}
int CLBPString::ReverseFind(char ch) const				{	return ReverseFind(ConvertMBCSCharToWideChar(ch)); }

int CLBPString::ReverseFind(wchar_t ch) const
{
	const int iLength = GetLength();

	for(int i=iLength-1; i>=0;i--)
	{
		if(m_wsz[i]==ch)
		{
			return i;
		}
	}

	return -1;
}

int CLBPString::FindOneOf(const wchar_t* lpszCharSet) const
{
	int iSetSize = (int)wcslen(lpszCharSet);
	if(iSetSize == 0) return -1;

	const int iLength = GetLength();

	for(int i=0; i<iLength;i++)
	{
		for(int j=0; j<iSetSize;j++)
		{
			if(m_wsz[i] == lpszCharSet[j]) return i;
		}
	}

	return -1;
}

int CLBPString::Find(char ch) const						{	return Find(ConvertMBCSCharToWideChar(ch), 0); }
int CLBPString::Find(char ch, int nStart) const			{	return Find(ConvertMBCSCharToWideChar(ch), nStart); }

bool CLBPString::ContainsNoCase(const CLBPString& sSub) const
{
	CLBPString sSubUpper = sSub;
	sSubUpper.MakeUpper();

	CLBPString sThisUpper = *this;
	sThisUpper.MakeUpper();

	return sThisUpper.Contains(sSubUpper);
}

bool CLBPString::StartsWith(const CLBPString& sSub) const
{
	if(sSub.IsEmpty()) return false;
	if(sSub.GetLength() > GetLength()) return false;

	CLBPString sStart = Left(sSub.GetLength());

	return sStart.Compare(sSub.AsWideString())==0;
}

bool CLBPString::StartsWithNoCase(const CLBPString& sSub) const
{
	if(sSub.IsEmpty()) return false;
	if(sSub.GetLength() > GetLength()) return false;

	CLBPString sStart = Left(sSub.GetLength());

	return sStart.CompareNoCase(sSub.AsWideString())==0;
}

bool CLBPString::EndsWith(const CLBPString& sSub) const
{
	if(sSub.IsEmpty()) return false;
	if(sSub.GetLength() > GetLength()) return false;

	CLBPString sEnd = Right(sSub.GetLength());
	return sEnd.Compare(sSub.AsWideString()) == 0;
}

bool CLBPString::EndsWithNoCase(const CLBPString& sSub) const
{
	if(sSub.IsEmpty()) return false;
	if(sSub.GetLength() > GetLength()) return false;

	CLBPString sEnd = Right(sSub.GetLength());
	return sEnd.CompareNoCase(sSub.AsWideString()) == 0;
}


int CLBPString::Count(char ch) const {	return Count(ConvertMBCSCharToWideChar(ch));}

int CLBPString::Count(wchar_t ch) const
{
	int iCount = 0;
	{
		const wchar_t* p = m_wsz;
		while(*p != 0)
		{
			if (*p == ch)
				iCount++;
			p++;
		}
	}
	
#ifdef _DEBUG
	#pragma message("CLBPString::Count - regression test included.\n")
	{
		int iCount2 = 0;
		int iLength = GetLength();
		for(int i=0; i<iLength; i++)
		{
			if(m_wsz[i]==ch) iCount2++;
		}
		ASSERT(iCount2 == iCount);
	}
#endif

	return iCount;
}

// simple formatting

// formatting (using wsprintf style formatting)
void AFX_CDECL CLBPString::Format(const wchar_t* lpszFormat, ...)
{
	va_list argList;
	va_start(argList, lpszFormat);
	FormatV(lpszFormat, argList);
	va_end(argList);
}

#ifdef _NATIVE_WCHAR_T_DEFINED
void AFX_CDECL CLBPString::Format(const USHORT* lpszFormat, ...)
{
	va_list argList;
	va_start(argList, lpszFormat);
	FormatV(lpszFormat, argList);
	va_end(argList);
}
#endif

void AFX_CDECL CLBPString::Format(const char* lpszFormat, ...)
{
	va_list argList;
	va_start(argList, lpszFormat);
	FormatV(CLBPString(lpszFormat).AsWideString(), argList);
	va_end(argList);
}


#if !defined LBPLIB_DISABLE_OLE
BSTR CLBPString::AllocSysString() const
{
	BSTR bstr = ::SysAllocStringLen(m_wsz, GetLength());
	if (bstr == NULL) AfxThrowMemoryException();
	return bstr;
}

BSTR CLBPString::SetSysString(BSTR* pbstr) const
{
	if (!::SysReAllocStringLen(pbstr, m_wsz, GetLength()))
		AfxThrowMemoryException();

	return *pbstr;
}
#endif


// Access to string implementation buffer as "C" character array

// get pointer to modifiable buffer at least as long as nMinBufLength
char* CLBPString::GetMBCSBuffer(int nMinBufLength)
{
	if(m_sz) LBPFREE (m_sz);
	m_sz = (char*)LBPMALLOC(nMinBufLength*2 * sizeof(char));

	return m_sz;
}

wchar_t* CLBPString::GetWideBuffer(int nMinBufLength)
{
	if (m_wszEmpty == m_wsz)
	{
		ASSERT(0 == m_dwSizeAllocation);
		Allocate(nMinBufLength);
		m_wsz[0] = 0;
	}
	else
	{
		Allocate(nMinBufLength);
	}

	return m_wsz;
}


	// release buffer, setting length to nNewLength (or to first nul if -1)
void CLBPString::ReleaseWideBuffer(int nNewLength)
{
	m_dwLengthCache = -1;

	//This is a no op
}

void CLBPString::ReleaseMBCSBuffer(int nNewLength)
{
	//Get the MBCS buffer into the wide
	*this = CLBPString(m_sz);

//	m_dwLengthCache = -1; John - In operator =
}

char* CLBPString::GetMBCSBufferSetLength(int nNewLength)
{
	return GetMBCSBuffer(nNewLength);
}

#ifdef _UNICODE
wchar_t* CLBPString::GetTBuffer(int nMinBufLength)
{
	return GetWideBuffer(nMinBufLength);
}

wchar_t* CLBPString::GetTBufferSetLength(int nNewLength)
{
	return GetWideBufferSetLength(nNewLength);
}

void CLBPString::ReleaseTBuffer(int nNewLength)
{
	ReleaseWideBuffer(nNewLength);
}
#else
char* CLBPString::GetTBuffer(int nMinBufLength)
{
	return GetMBCSBuffer(nMinBufLength);
}

char* CLBPString::GetTBufferSetLength(int nNewLength)
{
	return GetMBCSBufferSetLength(nNewLength);
}

void CLBPString::ReleaseTBuffer(int nNewLength)
{
	ReleaseMBCSBuffer(nNewLength);
}
#endif



wchar_t* CLBPString::GetWideBufferSetLength(int nNewLength)
{
	AllocateExact(nNewLength);

	return m_wsz;
}

bool CLBPString::IsValidIntegerNumber() const
{
	if(IsEmpty())
		{ ASSERT(!OLD_IsValidIntegerNumber()); return false; }

	//CLBPString s(m_wsz);
	//s.TrimLeft();
	//s.TrimRight();

	const int iLength = GetLength();

	bool bAtStart = true;
	bool bAtEnd = false;

	for(int i=0; i<iLength;i++)
	{
		wchar_t w = GetAtWide(i);

		//Skip past whitespace at the start of the string
		if (iswspace(w))
		{
			if (bAtStart)
			{
				//Skip past white space at the beginning of a string
				continue;
			}
			else
			{
				//Otherwise, whitespace can only appear at the end of the string
				bAtEnd = true;
				continue;
			}
		}

		if(!iswdigit(w) && w != L'-')
		{
			ASSERT(!OLD_IsValidIntegerNumber());
			return false;
		}

		//Nothing can come after spaces at the end.
		if (bAtEnd)
			{ ASSERT(!OLD_IsValidIntegerNumber()); return false; }

		bAtStart = false;
	}

	ASSERT(OLD_IsValidIntegerNumber());

	return true;
}

bool CLBPString::IsValidRealNumber() const
{
	if(IsEmpty())
		{ ASSERT(!OLD_IsValidRealNumber()); return false; }

	//CLBPString s(*this);
	//s.TrimLeft();
	//s.TrimRight();

	int iPuncCount = 0;
	int iECount = 0;

	const int iLength = GetLength();

	bool bAtStart = true;
	bool bAtEnd = false;

	for(int i=0; i<iLength;i++)
	{
		wchar_t w = GetAtWide(i);

		//Skip past whitespace at the start of the string
		if (iswspace(w))
		{
			if (bAtStart)
			{
				//Skip past white space at the beginning of a string
				continue;
			}
			else
			{
				//Otherwise, whitespace can only appear at the end of the string
				bAtEnd = true;
				continue;
			}
		}

		if (w==L'.' || w==L',') iPuncCount++;
		else
		if (w==L'e' || w==L'E') iECount++;
		else
		if (!(iswdigit(w) || w==L'-' || w==L'+'))
		{
			ASSERT(!OLD_IsValidRealNumber());
			return false;
		}

		//Nothing can come after spaces at the end.
		if (bAtEnd)
			{ ASSERT(!OLD_IsValidRealNumber()); return false; }

		bAtStart = false;
	}

	if(iPuncCount > 1 || iECount > 1) return false;

	ASSERT(OLD_IsValidRealNumber());

	return true;
}

bool CLBPString::IsValid4dPoint() const
{
	return IsCommaDelimitedDoubleArray(4);
}

bool CLBPString::IsValid3dPoint() const
{
	return IsCommaDelimitedDoubleArray(3);
}

bool CLBPString::IsValid2dPoint() const
{
	return IsCommaDelimitedDoubleArray(2);
}

bool CLBPString::IsValidMatrix() const
{
	return IsCommaDelimitedDoubleArray(16);
}

bool CLBPString::IsCommaDelimitedDoubleArray(int iDoubles) const
{
	if(IsEmpty())
		{ ASSERT(!OLD_IsCommaDelimitedDoubleArray(iDoubles)); return false; }

	if (iDoubles < 1)
		{ ASSERT(!OLD_IsCommaDelimitedDoubleArray(iDoubles)); return false; }

	if (1 == iDoubles)
		return IsValidRealNumber();

	CLBPString s(*this);
	s+=L",";

	for (int i = 0; i < iDoubles; i++)
	{
		const int iPos = s.Find(L',');

		if(iPos < 0) return false;

		const CLBPString sNum = s.Left(iPos);
		if (!sNum.IsValidRealNumber())
			return false;

		s = s.Mid(iPos + 1);
	}

	ASSERT(OLD_IsCommaDelimitedDoubleArray(iDoubles));

	return true;
}

#ifdef _DEBUG

bool CLBPString::OLD_IsValidIntegerNumber() const
{
	if(IsEmpty()) return false;

	CLBPString s(m_wsz);
	s.TrimLeft();
	s.TrimRight();

	const int iLength = s.GetLength();

	for(int i=0; i<iLength;i++)
	{
		wchar_t w = s.GetAtWide(i);

		if(!iswdigit(w) && w != L'-')
		{
			return FALSE;
		}
	}

	return true;
}

bool CLBPString::OLD_IsValidRealNumber() const
{
	if(IsEmpty()) return false;

	CLBPString s(m_wsz);
	s.TrimLeft();
	s.TrimRight();

	int iPuncCount = 0;
	int iECount = 0;

	const int iLength = s.GetLength();

	for(int i=0; i<iLength;i++)
	{
		wchar_t w = s.GetAtWide(i);

		if(!(iswdigit(w) || w==L'.' || w==L',' || w==L'-' || w==L'e' || w==L'E' || w==L'+'))
		{
			return false;
		}

		if(w==L'.' || w==L',') iPuncCount++;
		if(w==L'e' || w==L'E') iECount++;
	}

	if(iPuncCount > 1 || iECount > 1) return false;

	return true;
}

bool CLBPString::OLD_IsCommaDelimitedDoubleArray(int iDoubles) const
{
	if(IsEmpty()) return false;

	if (iDoubles < 1)
		return FALSE;

	if (1 == iDoubles)
		return IsValidRealNumber();

	CLBPString s = m_wsz + CLBPString(",");

	for (int i = 0; i < iDoubles; i++)
	{
		const int iPos = s.Find(L',');

		if(iPos < 0) return FALSE;

		const CLBPString sNum = s.Left(iPos);
		if (!sNum.IsValidRealNumber())
			return FALSE;

		s = s.Mid(iPos + 1);
	}

	return TRUE;
}

#endif

/*bool CLBPString::IsCommaDelimitedDoubleArray(int iDoubles) const
{
	if(iDoubles < 1) return FALSE;
	if(iDoubles == 1) return IsValidRealNumber();

	CLBPString sValue(m_wsz);

	for(int i=0;i<iDoubles-1;i++)
	{
		int iPos = 0;
		int iNewPos = 0;
		if(-1 == (iNewPos=sValue.Find(L',', iPos))) return FALSE;
		iPos = iNewPos+1;

		CLBPString s = sValue.Left(iPos-1);
		sValue = sValue.Mid(iPos);

		if(!s.IsValidRealNumber())
		{
			return FALSE;
		}
	}

	//Check the last one
	if(!sValue.IsValidRealNumber()) return FALSE;

	return TRUE;
}*/





//******************************************
//	Utility functions
//******************************************

char CLBPString::ConvertWideCharToMBCSChar(wchar_t w)
{
	char c[2];

#ifdef LBPLIB_WINDOWS_SPECIFIC
	int iRet = WideCharToMultiByte(CP_ACP, 0, &w, 2, c, 2, NULL, NULL);
#else
	size_t iRet = wcstombs(c, &w, 2);
#endif

	VERIFY(iRet != 0);
	return c[0];
}

wchar_t CLBPString::ConvertMBCSCharToWideChar(char c)
{
	wchar_t w;

#ifdef LBPLIB_WINDOWS_SPECIFIC
	int iRet = MultiByteToWideChar(CP_ACP, 0, &c, 1, &w, 1);
#else
	size_t iRet = mbstowcs(&w, &c, 1);
#endif

	VERIFY(iRet != 0);
	return w;
}


void CLBPString::SyncMBCS() const
{
	DWORD dwSize = (GetLength()+1)*2;

	char* pNew = (char*)LBPMALLOC (dwSize * sizeof(char));

#ifdef LBPLIB_WINDOWS_SPECIFIC
	WideCharToMultiByte(CP_ACP, 0, m_wsz, -1, pNew, dwSize, NULL, NULL);
#else
	wcstombs(pNew, m_wsz, dwSize * sizeof(char));
#endif

	//This stuff ensures that the MBCS buffer is not continually
	//changed for const CLBPStrings
	if(m_sz)
	{
		if(0==strcmp(m_sz, pNew))
		{
			LBPFREE(pNew);
			return;
		}
		LBPFREE(m_sz);
	}

	m_sz = pNew;
}

size_t CLBPString::AllocateExact(size_t dwAlloc)
{
	//Add the terminator
	++dwAlloc;

	if (m_wszEmpty == m_wsz)
	{
		ASSERT(0 == m_dwSizeAllocation);
		m_wsz = NULL;
	}

	if (dwAlloc > m_dwSizeAllocation)
	{
#ifdef LBP_STRING_STATISTICS
		g_TotalAllocated -= m_dwSizeAllocation;
#endif
		m_dwSizeAllocation = dwAlloc;

#ifdef LBP_STRING_STATISTICS
		g_TotalAllocated += m_dwSizeAllocation;
#endif
		const size_t dwNumBytes = m_dwSizeAllocation * sizeof(wchar_t);
		m_wsz = (wchar_t*)LBPREALLOC(m_wsz, dwNumBytes);
	}

	return m_dwSizeAllocation;
}

size_t CLBPString::Allocate(size_t dwAlloc, bool bExact)
{
	if (dwAlloc+1 <= m_dwSizeAllocation)
		return m_dwSizeAllocation;

	size_t dwNewAllocation = dwAlloc; // In characters.

	if (!bExact)
	{
		if (dwAlloc < 32)
		{
			// Never allocate less than 32 characters.
			dwNewAllocation = 32;
		}
		else
		if (dwAlloc > 100000)
		{
			// If the allocation is really big, don't bloat it - just add a reasonable amount on the end.
			dwNewAllocation = dwAlloc + 10000;
		}
		else
		{
			dwNewAllocation = dwAlloc * 4;
		}
	}

	return AllocateExact(dwNewAllocation);
}

size_t CLBPString::GetAllocation() const
{
	return m_dwSizeAllocation;
}

//Never called
void	CLBPString::Destroy()
{
}



#define TCHAR_ARG   TCHAR
#define WCHAR_ARG   WCHAR
#define CHAR_ARG    char

#ifdef _X86_
	#define DOUBLE_ARG  _AFX_DOUBLE
#else
	#define DOUBLE_ARG  double
#endif

#define FORCE_ANSI      0x10000
#define FORCE_UNICODE   0x20000
#define FORCE_INT64     0x40000


void SwitchStringFormatMarkers(CLBPString& s)
{
	int iLwrStore[50];
	int iUprStore[50];

	iLwrStore[0] = -1;
	iUprStore[0] = -1;

	int nPos = 0, i=0, iCounter = 0;

	while((i=s.Find(L"%s", nPos)) != -1)
	{
		iLwrStore[iCounter++] = i+1;
		iLwrStore[iCounter] = -1;
		nPos = i+1;

	}

	iCounter = 0;nPos = 0;
	while((i=s.Find(L"%S", nPos)) != -1)
	{
		iUprStore[iCounter++] = i+1;
		iUprStore[iCounter] = -1;
		nPos = i+1;
	}

	iCounter = 0;
	while(iLwrStore[iCounter] != -1)
	{
		s.SetAt(iLwrStore[iCounter++], 'S');
	}

	iCounter = 0;
	while(iUprStore[iCounter] != -1)
	{
		s.SetAt(iUprStore[iCounter++], 's');
	}
}

void CLBPString::InternalFormatV(const wchar_t* lpszFormat, va_list argList)
{
#if LBPLIB_RHINOVER < 6  || !defined LBPRHLIB
	ASSERT(AfxIsValidString(lpszFormat));
	va_list argListSave = argList;
#endif
	
	CLBPString sFormat = lpszFormat;

#ifdef _UNICODE
	SwitchStringFormatMarkers(sFormat);
#endif

#if LBPLIB_RHINOVER >= 6 && defined LBPRHLIB
    const int nMaxLen = ON_wString::FormatVargsOutputCount(sFormat, argList);
#else
    //Use CString to figure out the length of the thing:
    //Ouch - but better than the potentially bug happy checking code from CString
    CString s;
	s.FormatV(sFormat.AsTString(), argList);
	const int nMaxLen = s.GetLength();
#endif

	//We have to use a string in case the buffer is already used in the arg list
	CLBPString sTemp;
	wchar_t* pBuffer = sTemp.GetWideBuffer(nMaxLen+1);

	CLBPString sFormatString(lpszFormat);
	SwitchStringFormatMarkers(sFormatString);

#if LBPLIB_RHINOVER >= 6 && defined LBPRHLIB

    ON_wString::FormatVargsIntoBuffer(pBuffer, nMaxLen+1, sFormatString.Wide(), argList);
	sTemp.ReleaseWideBuffer();

	#ifdef _DEBUG
	ON_wString s;
	s.FormatVargs(sFormatString, argList);
	VERIFY(sTemp == (const wchar_t*)s);
	#endif

#else

    // ###############################################################################################
    //
    // 12th August 2015 John Croudy. Non-V6 and Non-Rhino stuff still needs this code!
    //
    // This is a temp fix while Andy is on the road. Andy needs to look at this when he gets back to Finland.

    VERIFY(_vswprintf_l(pBuffer, sFormatString.AsWideString(), LBP_Locale(), argListSave) <= (int)sTemp.GetAllocation());
    sTemp.ReleaseWideBuffer();
    va_end(argListSave);

    // ###############################################################################################

#endif

	*this = sTemp;
}

//#define TEST_BAD_LOCALE

void CLBPString::FormatV(const wchar_t* lpszFormat, va_list argList)
{
#ifdef TEST_BAD_LOCALE
	_wsetlocale(LC_ALL, L"German_Germany.1258");
	InternalFormatV(lpszFormat, argList);
	const CLBPString save = *this;
	_wsetlocale(LC_ALL, L"C");
	InternalFormatV(lpszFormat, argList);
	ASSERT(save == Wide());
#else
	InternalFormatV(lpszFormat, argList);
#endif
}

void CLBPString::ConvertLineEndingsToLFs(void)
{
	// Convert any CR-LF pairs to lone LFs. Convert any lone CRs to LFs.
	int length = GetLength();
	for (int i = 0; i < length; i++)
	{
		const wchar_t c1 = GetAtWide(i);
		const wchar_t c2 = (i < (length-1)) ? GetAtWide(i+1) : 0;

		if (c1 == L'\r')
		{
			if (c2 == L'\n')
			{
				Delete(i);
				length--;
			}
			else
			{
				SetAt(i, L'\n');
			}
		}
	}
}

void CLBPString::ConvertLineEndingsToCRLFs(void)
{
	// Convert any CRs to CR-LFs. Convert any LFs to CR-LFs.
	int length = GetLength();
	for (int i = 0; i < length; i++)
	{
		const wchar_t c1 = GetAtWide(i);
		const wchar_t c2 = (i < (length-1)) ? GetAtWide(i+1) : 0;

		if ((c1 == L'\r') || (c1 == L'\n'))
		{
			if ((c1 == L'\r') && (c2 == L'\n'))
			{
				i++;
			}
			else
			{
				Insert(i, L'\r');
				SetAt(++i, L'\n');
				length++;
			}
		}
	}
}

void CLBPString::URLEncode(bool bForANSI)
{
	if (IsEmpty())
		return;

	const auto iLength = GetLength();

	size_t calculatedLength = 0;
	for (int i = 0; i < iLength; i++)
	{
		const auto& w = m_wsz[i];

		switch (w)
		{
			case L'&':
			case L'\n':
				calculatedLength += 5;
				break;
			case L'\"':
			case L'\'':
				calculatedLength += 6;
				break;
			case L'<':
			case L'>':
				calculatedLength += 4;
				break;
			case L'\r':
				break;
			default:
				calculatedLength++;
		}
	}

	const size_t allocation = (calculatedLength + 1) * sizeof(wchar_t);

	wchar_t* pBuffer = (wchar_t*)LBPMALLOC(allocation);
	auto* p = pBuffer;

	for (int i = 0; i < iLength; i++)
	{
		const auto& w = m_wsz[i];
		switch (w)
		{
		case L'&':
			*p++ = L'&';
			*p++ = L'a';
			*p++ = L'm';
			*p++ = L'p';
			*p++ = L';';
			break;
		case L'\"':
			*p++ = L'&';
			*p++ = L'q';
			*p++ = L'u';
			*p++ = L'o';
			*p++ = L't';
			*p++ = L';';
			break;
		case L'\'':
			*p++ = L'&';
			*p++ = L'a';
			*p++ = L'p';
			*p++ = L'o';
			*p++ = L's';
			*p++ = L';';
			break;
		case L'<':
			*p++ = L'&';
			*p++ = L'l';
			*p++ = L't';
			*p++ = L';';
			break;
		case L'>':
			*p++ = L'&';
			*p++ = L'g';
			*p++ = L't';
			*p++ = L';';
			break;
		case L'\r':
			break;
		case L'\n':
			*p++ = L'&';
			*p++ = L'#';
			*p++ = L'1';
			*p++ = L'0';
			*p++ = L';';
			break;
		default:
			*p++ = w;
		}
	}

	*p = 0;

	const size_t l = p - pBuffer;
	if (l != calculatedLength)
	{
		Sleep(0);
	}

	TakeOwnershipOfBuffer(pBuffer, (int)calculatedLength);

	if (bForANSI)
	{
		ConvertNonAsciiCharactersToEntities();
	}
}

bool CLBPString::NeedsURLEncode(bool bForANSI) const
{
	if (IsEmpty())
		return false;
	
	if (GetLength() > 6)
	{
		if (m_wsz[0] == L'b' &&
			m_wsz[1] == L'a' &&
			m_wsz[2] == L's' &&
			m_wsz[3] == L'e' &&
			m_wsz[4] == L'6' &&
			m_wsz[5] == L'4' &&
			m_wsz[6] == L':')
		{
			return false;
		}
	}

	/*	https://mcneel.myjetbrains.com/youtrack/issue/RH-31874 - this is slow, replace with FindOneOf
	if (-1 != Find(L'&' )) return true;
	if (-1 != Find(L'\"')) return true;
	if (-1 != Find(L'\'')) return true;
	if (-1 != Find(L'<' )) return true;
	if (-1 != Find(L'>' )) return true;
	if (-1 != Find(L'\n')) return true;
	if (-1 != Find(L'\r')) return true;
	*/

	if (-1 != FindOneOf(L"&\"\'<>\n\r"))
		return true;

	if (!bForANSI)
		return false;

	if (-1 != FindFirstNonAsciiCharacter())
		return true;

	return false;
}

void CLBPString::ConvertNonAsciiCharactersToEntities()
{
	int iPos = -1;
	while(-1 != (iPos = FindFirstNonAsciiCharacter()))
	{
		unsigned int iCode = GetAtWide(iPos);
		CLBPString sEntity;
		sEntity.Format(L"&#%d;", iCode);

		CLBPString s = GetAtWide(iPos);

		Replace(s.AsWideString(), sEntity);
	}
}

int CLBPString::FindFirstNonAsciiCharacter() const
{
	const int iLength = GetLength();

	for(int i=0;i<iLength;i++)
	{
		unsigned int iCode = GetAtWide(i);
		if(iCode > 127) return i;
	}

	return -1;
}

void CLBPString::URLUnencode()
{
	if(IsEmpty())
		return;

	if(-1 == Find(L'&'))
		return;

	const size_t length = GetLength();

	//No need to calculate a new length - the string is only going to get smaller, so just start with the
	//same size.
	wchar_t* pBuffer = (wchar_t*)LBPMALLOC((length + 1) * sizeof(wchar_t));

	wchar_t* pNew = pBuffer;
	const wchar_t* p = m_wsz;

	while (*p != 0)
	{
		const wchar_t p0 = *p;

		if (p0 == L'&')
		{
			const wchar_t p1 = *(p + 1);
			if (p1 == L'q')
			{
				if (L'u' == *(p + 2) && L'o' == *(p + 3) && L't' == *(p + 4) && L';' == *(p + 5))
				{
					p += 6;
					*pNew++ = L'\"';
					continue;
				}
			}
			else if (p1 == L'a')
			{
				if (L'p' == *(p + 2) && L'o' == *(p + 3) && L's' == *(p + 4) && L';' == *(p + 5))
				{
					p += 6;
					*pNew++ = L'\'';
					continue;
				}
				if (L'm' == *(p + 2) && L'p' == *(p + 3) && L';' == *(p + 4))
				{
					p += 5;
					*pNew++ = L'&';
					continue;
				}
			}
			else if (p1 == L'l')
			{
				if (L't' == *(p + 2) && L';' == *(p + 3))
				{
					p += 4;
					*pNew++ = L'<';
					continue;
				}
			}
			else if (p1 == L'g')
			{
				if (L't' == *(p + 2) && L';' == *(p + 3))
				{
					p += 4;
					*pNew++ = L'>';
					continue;
				}
			}
			else if (p1 == L'#')
			{
				if (L'1' == *(p + 2) && L'0' == *(p + 3) && L';' == *(p + 4))
				{
					p += 5;
					*pNew++ = L'\n';
					continue;
				}
			}
		}

		p++;
		*pNew++ = p0;
	}

	//Terminate
	*pNew = 0;

	TakeOwnershipOfBuffer(pBuffer, (int)(pNew - pBuffer));
}

bool CLBPString::IsURLEscapeSequence() const
{
	if(0==CompareNoCase(L"&quot;")) return true;
	if(0==CompareNoCase(L"&apos;")) return true;
	if(0==CompareNoCase(L"&lt;")) return true;
	if(0==CompareNoCase(L"&gt;")) return true;
	if(0==CompareNoCase(L"&amp;")) return true;
	if(0==CompareNoCase(L"&#10;")) return true;

	return false;
}

#if !defined LBPLIB_DISABLE_MFC_CONTROLS
CLBPStringBV CLBPString::WindowText(HWND hWnd, UINT uBufferLength)
{
	CLBPStringBV s;

	if (::IsWindow(hWnd))
	{
		::GetWindowText(hWnd, s.GetTBuffer(uBufferLength), uBufferLength-1);
		s.ReleaseTBuffer();
	}

	return s;
}

CLBPStringBV CLBPString::WindowText(CWnd* pWnd, UINT uBufferLength)
{
	return WindowText(pWnd->GetSafeHwnd(), uBufferLength);
}
#endif

CLBPStringBV CLBPString::HexString()
{
	CLBPStringBV sHex;

	const int iLength = GetLength();

	for(int i=0;i<iLength;i++)
	{
		CLBPString sThisHex;
		sThisHex.Format("%.4X", m_wsz[i]);
		sHex+=sThisHex;
	}
	return sHex;
}

CLBPStringBV CLBPString::HexStringMBCS()
{
	CLBPStringBV sHex;
	SyncMBCS();

	size_t iLength = strlen(m_sz);

	for(size_t i=0;i<iLength;i++)
	{
		CLBPString sThisHex;
		sThisHex.Format("%.2X", m_sz[i]);
		sHex+=sThisHex;
	}
	return sHex;
}


bool g_bNeedToSeed = true;

void CLBPString::RandomCharacters(int iLength)
{
	//Seed with the time
	if(g_bNeedToSeed)
	{
		srand((unsigned)time(NULL));
		//Gets over the seeming CRT bug where the first number isn't random
		rand();
		g_bNeedToSeed = false;
	}

	Empty();
	for(int i=0;i<iLength;i++)
	{
		float f = rand()/(float)RAND_MAX;

		wchar_t w = static_cast<wchar_t>((f * 90) + 32);

		*this+=w;
	}
}

CLBPString FormatString(const wchar_t* szFormatString, ...)
{
	va_list argList;
	va_start(argList, szFormatString);

	CLBPString s;
	s.FormatV(szFormatString, argList);
	va_end(argList);

	return s;
}

#if !defined LBPLIB_DISABLE_OLE
bool CLBPString::SetToClipboard(HWND hWnd) const
{
	if (!::OpenClipboard(hWnd))
		return false;

	::EmptyClipboard();

	const int iNumCharsIncTerm = GetLength() + 1;

	HGLOBAL h = ::GlobalAlloc(GMEM_DDESHARE, iNumCharsIncTerm * sizeof(wchar_t));
	if (NULL == h)
	{
		::CloseClipboard();
		return false;
	}

	wchar_t* wszBuffer = (wchar_t*)::GlobalLock(h);
	if (NULL == wszBuffer)
	{
		::CloseClipboard();
		return false;
	}

	wcsncpy(wszBuffer, m_wsz, iNumCharsIncTerm);

	::GlobalUnlock(h);

	::SetClipboardData(CF_UNICODETEXT, h);
	::CloseClipboard();

	return true;
}

bool CLBPString::GetFromClipboard(HWND hWnd)
{
	if (!::OpenClipboard(hWnd))
		return false;

	HGLOBAL h = ::GetClipboardData(CF_UNICODETEXT);
	if (NULL == h)
	{
		::CloseClipboard();
		return false;
	}

	wchar_t* wszBuffer = (wchar_t*)::GlobalLock(h);
	if (NULL == wszBuffer)
	{
		::CloseClipboard();
		return false;
	}

	const int iNumCharsIncTerm = (int)wcslen(wszBuffer) + 1;
	wchar_t* wsz = GetWideBuffer(iNumCharsIncTerm);
	wcsncpy(wsz, wszBuffer, iNumCharsIncTerm);
	ReleaseWideBuffer();

	::GlobalUnlock(h);

	::CloseClipboard();

	return true;
}
#endif

//See notes in the header about WIN64 optimizer.
#ifdef WIN64
int CLBPString::GetLengthImpl() const 
{ 
	m_dwLengthCache = ((int)wcslen(m_wsz));
	return (int)m_dwLengthCache;
}
#endif

//Sets the string to the actual buffer passed on - no copy is made.
void CLBPString::TakeOwnershipOfBuffer(wchar_t* wsz, int iLengthOfBuffer)
{
	if (m_wszEmpty != m_wsz && NULL != m_wsz)
	{
		LBPFREE(m_wsz);
	}

	if (m_sz) LBPFREE(m_sz);
	m_sz = NULL;

	int iLength;
	if (iLengthOfBuffer == -1)
	{
		iLength = (int)wcslen(wsz);
	}
	else
	{
		iLength=iLengthOfBuffer;
	}

	//const size_t iLength = (-1==iLengthOfBuffer) ? wcslen(wsz) : iLengthOfBuffer;

#ifdef LBP_STRING_STATISTICS
	g_TotalAllocated -= m_dwSizeAllocation;
#endif
	m_dwSizeAllocation = 1 + iLength;

#ifdef LBP_STRING_STATISTICS
	g_TotalAllocated += m_dwSizeAllocation;
#endif

	m_dwLengthCache = iLength;
	m_wsz = wsz;
}

//Releases the string buffer to be owned by the caller.  String is set to empty.
wchar_t* CLBPString::ReleaseOwnership(void)
{
	if (m_wszEmpty == m_wsz)
	{
		//We are pointing to the static empty buffer - we have to cook up a real buffer to make
		//this work
		ASSERT(0 == m_dwSizeAllocation);
		AllocateExact(0);
		m_wsz[0] = 0;
	}

#ifdef LBP_STRING_STATISTICS
	g_TotalAllocated -= m_dwSizeAllocation;
#endif

	wchar_t* pOut = m_wsz;
	m_wsz = NULL;

	if (m_sz) LBPFREE(m_sz);
	m_sz = NULL;

	m_dwSizeAllocation = 0;
	m_dwLengthCache = -1;

	Empty();

	return pOut;
}

void CLBPString::DumpStatistics(void) // Static.
{
	OutputDebugString(L"**** CLBPString: Statistics disabled; #define LBP_STRING_STATISTICS");
}

#ifdef USE_BYVALUE_OPTIMIZATION
CLBPStringBV::CLBPStringBV(const CLBPString& src)
: CLBPString(src)
{
	m_bAllowAutoReleaseOwnership = true;
}

CLBPStringBV::CLBPStringBV()
: CLBPString()
{
	m_bAllowAutoReleaseOwnership = true;
}
#endif

static LONG g_lStringCount = 0;

int LBP_GetStringCount() { return g_lStringCount; }

#ifdef USE_REFERENCE_COUNTED_STRINGS

CLBPString_RC::CLBPString_RC()
	:
	m_pData(NULL)
{
}

CLBPString_RC::CLBPString_RC(const CLBPString_RC& src)
{
	if (NULL != src.m_pData)
	{
		src.m_pData->AddRef();
	}

	m_pData = src.m_pData;
}

CLBPString_RC::~CLBPString_RC()
{
	if (NULL != m_pData)
	{
		m_pData->Release();
	}
}

CLBPString_RC& CLBPString_RC::operator = (const CLBPString_RC& src)
{
	if (NULL != src.m_pData)
	{
		src.m_pData->AddRef();
	}

	if (NULL != m_pData)
	{
		m_pData->Release();
	}

	m_pData = src.m_pData;

	return *this;
}

void CLBPString_RC::Empty(void)
{
	if (NULL != m_pData)
	{
		m_pData->Release();
		m_pData = NULL;
	}
}

CLBPString& CLBPString_RC::EmptyStringForWrite(void)
{
	//Cause the string to be independent
	SetString(L"");
	ASSERT(NULL != m_pData);
	ASSERT(m_pData->m_iRefCount == 1);
	return m_pData->m_string;
}

void CLBPString_RC::SetString(const CLBPString& s)
{
	if (NULL != m_pData)
	{
		m_pData->Release();
	}

	m_pData = new CLBPString_RC_Data(s);
}

//CLBPString CLBPString_RC::m_empty;
const CLBPString& CLBPString_RC::_empty(void)
{
	static const CLBPString _e;
	return _e;
}

#pragma message ("CLBPString_RC is reference-counted")

#else

#pragma message ("CLBPString_RC is a CLBPString")

#endif //USE_REFERENCE_COUNTED_STRINGS
