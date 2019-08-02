
#pragma once

#ifdef _DEBUG
#define USE_REFERENCE_COUNTED_STRINGS
#endif

#ifndef LBPEXPORT
#define LBPEXPORT
#endif

#if defined (LBPLIB_APPLE_SPECIFIC)
#define REFCOUNT_INT int32_t
#define InterlockedIncrement OSAtomicIncrement32
#define InterlockedDecrement OSAtomicDecrement32
#endif

#if defined (LBPLIB_WINDOWS_SPECIFIC)
#define REFCOUNT_INT LONG
#endif

#ifdef USE_BYVALUE_OPTIMIZATION
#class CLBPStringBV
#else
#define CLBPStringBV CLBPString
#endif

class LBPEXPORT CLBPString
{
public:
	CLBPString();
	CLBPString(const CLBPString& stringSrc );
	CLBPString(const char* sz ); 
	CLBPString(const wchar_t* wsz );
	CLBPString(wchar_t wch);
	CLBPString(char ch);
	CLBPString(const wchar_t* wsz, DWORD dwCharacters);
	//CLBPString(const CString s);

#ifdef _NATIVE_WCHAR_T_DEFINED
	CLBPString(USHORT wch);
	CLBPString(const USHORT* wsz);
	CLBPString(const USHORT* wsz, DWORD dwCharacters);
#endif

	~CLBPString();

public:
	static void DumpStatistics(void);

	__forceinline int GetLength() const { return m_dwLengthCache != -1 ? (int)m_dwLengthCache : GetLengthImpl(); }
	__forceinline bool IsEmpty() const	{ return NULL == m_wsz || 0 == m_wsz[0]; }
	__forceinline void Empty()			//{ AllocateExact(0);m_wsz[0] = 0;m_dwLengthCache = 0; }
	{
		m_dwLengthCache = 0;

		if (m_wszEmpty == m_wsz)
		{
			return;
		} 
		else if (NULL == m_wsz)
		{
			m_wsz = m_wszEmpty; 
		}
		else
		{
			AllocateExact(0);m_wsz[0] = 0;
		}
	}

	DWORD CRC() const;

	char GetAtMultibyte(int nIndex) const;
	__forceinline wchar_t operator[](int nIndex) const	{ return GetAtWide(nIndex); }
	__forceinline wchar_t GetAtWide(int nIndex) const	{ return m_wsz[nIndex]; }
	__forceinline void SetAt(int nIndex, wchar_t ch)				{ m_wsz[nIndex] = ch; }
	void SetAt(int nIndex, char ch);
	
	__forceinline operator const char*() const	{ return AsMBCSString(); }
	__forceinline operator const wchar_t*() const { return AsWideString(); }
	operator CString() const { return T(); }

#ifdef _NATIVE_WCHAR_T_DEFINED
	operator const USHORT*() const			{ return (USHORT*)Wide(); }
	void SetAt(int nIndex, USHORT ch)		{ SetAt(nIndex, (WCHAR)ch); }
#endif

	USHORT GetAtUShort(int nIndex) const	{ return (USHORT)GetAtWide(nIndex);}
	

	//For extremely crappy non-const correct code (like Roy's).
	//operator char*() const;
	//operator WCHAR*() const;

	const char*		AsMBCSString() const;
	__forceinline const wchar_t*	AsWideString() const	{ return m_wsz; }

	const char*		Mbcs() const;
	__forceinline const wchar_t*	Wide() const			{ return m_wsz; }

	const USHORT*	UShort() const		   { return ((const USHORT*)Wide()); }
	USHORT*	AsNonConstUShortString() const { return ((USHORT*)Wide()); }

	char*	AsNonConstMBCSString() const;
	WCHAR*	AsNonConstWideString() const;

#ifdef _UNICODE
	wchar_t*			AsNonConstTString() const;
	const wchar_t*	AsTString() const;
	const wchar_t*	T() const;
#else
	char*			AsNonConstTString() const;
	const char*		AsTString() const;
	const char*		T() const;
#endif


	// overloaded assignment

	const CLBPString& operator=(const CLBPString& stringSrc);
	//const CLBPString& operator=(const CLBPString& stringSrc) const;
	const CLBPString& operator=(wchar_t ch);
	const CLBPString& operator=(char ch);
	const CLBPString& operator=(const char* lpsz);
	const CLBPString& operator=(const wchar_t* lpsz);
	//const CLBPString& operator=(const WCHAR* lpsz) const;
    
	//__forceinline const CLBPString& operator=(const CString s)	{ return *this = (const TCHAR*)s; }

	const CLBPString& Set(const wchar_t* lpsz, DWORD dwCharacters);
	const CLBPString& Set(const char* lpsz, DWORD dwCharacters);

	const CLBPString& Set(double d);
	const CLBPString& Set(float f);
	const CLBPString& Set(int i);

#ifdef _NATIVE_WCHAR_T_DEFINED
	const CLBPString& operator=(const USHORT* lpsz) { return operator=((const wchar_t*)lpsz); }
	const CLBPString& operator=(USHORT ch)			{ return operator=((wchar_t)ch); }
#endif

	

	// string concatenation

	// concatenate from another CLBPString
	const CLBPString& operator+=(const CLBPString& string);

	// concatenate a single character
	const CLBPString& operator+=(char ch);
	const CLBPString& operator+=(wchar_t ch);
	const CLBPString& operator+=(const char* lpsz);
	const CLBPString& operator+=(const wchar_t* lpsz);

	const CLBPString& Append(const wchar_t* lpsz, int iCount);

#ifdef _NATIVE_WCHAR_T_DEFINED
	const CLBPString& operator+=(const USHORT* lpsz) { return operator+=((const wchar_t*)lpsz); }
	const CLBPString& operator+=(USHORT ch)			 { return operator+=((wchar_t)ch); }
#endif

	friend CLBPStringBV AFXAPI operator+(const CLBPString& string1, const CLBPString& string2);
	friend CLBPStringBV AFXAPI operator+(const CLBPString& string, char ch);
	friend CLBPStringBV AFXAPI operator+(const CLBPString& string, WCHAR ch);
	friend CLBPStringBV AFXAPI operator+(char ch, const CLBPString& string);
	friend CLBPStringBV AFXAPI operator+(wchar_t ch, const CLBPString& string);
// the following are duplicates
//	friend CLBPStringBV AFXAPI operator+(const CLBPString& string, char ch);
//	friend CLBPStringBV AFXAPI operator+(char ch, const CLBPString& string);
//	friend CLBPStringBV AFXAPI operator+(WCHAR ch, const CLBPString& string);
	friend CLBPStringBV AFXAPI operator+(const CLBPString& string, const char* lpsz);
	friend CLBPStringBV AFXAPI operator+(const char* lpsz, const CLBPString& string);
	friend CLBPStringBV AFXAPI operator+(const CLBPString& string, const WCHAR* lpsz);
	friend CLBPStringBV AFXAPI operator+(const wchar_t* lpsz, const CLBPString& string);

#ifdef _NATIVE_WCHAR_T_DEFINED
	friend CLBPStringBV AFXAPI operator+(const CLBPString& string, const USHORT* lpsz);
	friend CLBPStringBV AFXAPI operator+(const USHORT* lpsz, const CLBPString& string);
	friend CLBPStringBV AFXAPI operator+(USHORT ch, const CLBPString& string);
	friend CLBPStringBV AFXAPI operator+(const CLBPString& string, USHORT ch);
#endif

	// string comparison
	bool operator==(const wchar_t* wsz) const;
	bool operator==(const char* sz) const;

	bool operator!=(const wchar_t* wsz) const;
	bool operator!=(const char* sz) const;

	bool operator<(const CLBPString&)const;
	bool operator>(const CLBPString&)const;
	bool operator<=(const CLBPString&)const;
	bool operator>=(const CLBPString&)const;

	friend bool AFXAPI operator == (const wchar_t * wsz, const CLBPString& s);
	friend bool AFXAPI operator != (const wchar_t * wsz, const CLBPString& s);
	friend bool AFXAPI operator == (const char * wsz, const CLBPString& s);
	friend bool AFXAPI operator != (const char * wsz, const CLBPString& s);

#ifdef _NATIVE_WCHAR_T_DEFINED
	bool operator==(const USHORT* wsz) const				{ return operator==((const wchar_t*)wsz); }
	bool operator!=(const USHORT* wsz) const					{ return operator!=((const wchar_t*)wsz); }
	friend bool AFXAPI operator == (const USHORT * wsz, const CLBPString& s)
	{ return ::operator==((const wchar_t*)wsz, s); }
	friend bool AFXAPI operator != (const USHORT * wsz, const CLBPString& s)
	{ return ::operator!=((const wchar_t*)wsz, s); }
#endif

	int Compare(const char* lpsz) const;
	int CompareNoCase(const char* lpsz) const;
	int Compare(const wchar_t* lpsz) const;
	int CompareNoCase(const wchar_t* lpsz) const;
	// NLS aware comparison, case sensitive
	int Collate(const wchar_t* lpsz) const;
	int CollateNoCase(const wchar_t* lpsz) const;
	int Collate(const char* lpsz) const;
	int CollateNoCase(const char* lpsz) const;

#ifdef _NATIVE_WCHAR_T_DEFINED
	__forceinline int Compare(const USHORT* lpsz) const		{ return Compare((const wchar_t*)lpsz); }
	__forceinline int CompareNoCase(const USHORT* lpsz) const	{ return CompareNoCase((const wchar_t*)lpsz); }
	__forceinline int Collate(const USHORT* lpsz) const		{ return Collate((const wchar_t*)lpsz); }
	__forceinline int CollateNoCase(const USHORT* lpsz) const	{ return CollateNoCase((const wchar_t*)lpsz); }
#endif

	// simple sub-string extraction

	CLBPStringBV Mid(int nFirst, int nCount) const;
	const wchar_t* Mid(int nFirst) const;
	CLBPStringBV Left(int nCount) const;
	const wchar_t* Right(int nCount) const;

	// optimized sub-string functions
	void Truncate(int nCount);
	void TruncateMid(int nPos);

	//  characters from beginning that are also in passed string
	CLBPStringBV SpanIncluding(const char* lpszCharSet) const;
	CLBPStringBV SpanIncluding(const wchar_t* lpszCharSet) const;
	CLBPStringBV SpanExcluding(const char* lpszCharSet) const;
	CLBPStringBV SpanExcluding(const wchar_t* lpszCharSet) const;

#ifdef _NATIVE_WCHAR_T_DEFINED
	CLBPStringBV SpanIncluding(const USHORT* lpszCharSet) const;
	CLBPStringBV SpanExcluding(const USHORT* lpszCharSet) const;
#endif

	// upper/lower/reverse conversion

	void MakeUpper();
	void MakeLower();
	void MakeReverse();

	// trimming whitespace (either side)

	void TrimRight();
	void TrimLeft();

	//URL encoding
	void URLEncode(bool bForANSI = false);
	void URLUnencode();

	bool IsURLEscapeSequence() const;
	bool NeedsURLEncode(bool bForANSI = false) const;

	// trimming anything (either side)

	// remove continuous occurrences of chTarget starting from right
	void TrimRight(char chTarget);
	void TrimRight(const char* lpszTargets);
	void TrimLeft(char chTarget);
	void TrimLeft(const char* lpszTargets);
	void TrimRight(wchar_t chTarget);
	void TrimRight(const wchar_t* lpszTargets);
	void TrimLeft(wchar_t chTarget);
	void TrimLeft(const wchar_t* lpszTargets);

#ifdef _NATIVE_WCHAR_T_DEFINED
	__forceinline void TrimRight(USHORT chTarget)			{ TrimRight((wchar_t)chTarget); }
	__forceinline void TrimRight(const USHORT* lpszTargets)	{ TrimRight((const wchar_t*)lpszTargets); }
	__forceinline void TrimLeft(USHORT chTarget)			{ TrimLeft((wchar_t)chTarget); }
	__forceinline void TrimLeft(const USHORT* lpszTargets)	{ TrimLeft((const wchar_t*)lpszTargets); }
#endif

	// advanced manipulation

	int Replace(char chOld, char chNew);
	int Replace(wchar_t chOld, wchar_t chNew);
	int Replace(const char* lpszOld, const char* lpszNew);
	int Replace(const wchar_t* lpszOld, const wchar_t* lpszNew);
	int Remove(char chRemove);
	int Remove(wchar_t chRemove);
	int Insert(int nIndex, char ch, int iCount = 1);
	int Insert(int nIndex, wchar_t ch, int iCount = 1);
	int Insert(int nIndex, const char* pstr);
	int Insert(int nIndex, const wchar_t* pstr);
	int Delete(int nIndex, int nCount = 1);

#ifdef _NATIVE_WCHAR_T_DEFINED
	__forceinline int Replace(USHORT chOld, USHORT chNew)					{ return Replace((wchar_t)chOld, (wchar_t)chNew); }
	__forceinline int Replace(const USHORT* lpszOld, const USHORT* lpszNew)	{ return Replace((const wchar_t*)lpszOld, (const wchar_t*)lpszNew); }
	__forceinline int Remove(USHORT chRemove)								{ return Remove((wchar_t)chRemove); }
	__forceinline int Insert(int nIndex, USHORT ch, int iCount = 1)			{ return Insert(nIndex, (wchar_t)ch, iCount); }
	__forceinline int Insert(int nIndex, const USHORT* pstr)				{ return Insert(nIndex, (const wchar_t*)pstr); }
#endif

	// searching

	// find character starting at left, -1 if not found
	int ReverseFind(char ch) const;
	int Find(char ch, int nStart) const;
	int FindOneOf(const char* lpszCharSet) const;
	__forceinline int Find(const char* lpszSub) const				{	return Find(CLBPString(lpszSub).AsWideString(), 0); }
	__forceinline int Find(const char* lpszSub, int nStart) const	{	return Find(CLBPString(lpszSub).AsWideString(), nStart);}
	__forceinline int Find(wchar_t ch) const							{	return Find(ch, 0);}
	int ReverseFind(wchar_t ch) const;
	int Find(char ch) const;

	__forceinline int Find(wchar_t wch, int nStart) const
	{
		const wchar_t* pstr = wcschr(m_wsz+nStart, wch);
		return pstr != NULL ? (int)(pstr-m_wsz) : -1;
	}

	int FindOneOf(const wchar_t* lpszCharSet) const;
	__forceinline int Find(const wchar_t* lpszSub) const				{	return Find(lpszSub, 0);}

	__forceinline int Find(const wchar_t* lpszSub, int nStart) const
	{
		const wchar_t* pstr = wcsstr(m_wsz+nStart, lpszSub);
		return pstr != NULL ? (int)(pstr-m_wsz) : -1;
	}

#ifdef _NATIVE_WCHAR_T_DEFINED
	__forceinline int Find(USHORT ch) const							{ return Find((wchar_t)ch); }
	__forceinline int ReverseFind(USHORT ch) const					{ return ReverseFind((wchar_t)ch); }
	__forceinline int Find(USHORT ch, int nStart) const				{ return Find((wchar_t)ch, nStart); }
	__forceinline int FindOneOf(const USHORT* lpszCharSet) const	{ return FindOneOf((const wchar_t*)lpszCharSet); }
	__forceinline int Find(const USHORT* lpszSub) const				{ return Find((const wchar_t*)lpszSub); }
	__forceinline int Find(const USHORT* lpszSub, int nStart) const	{ return Find((const wchar_t*)lpszSub, nStart); }
#endif

	//Simple versions

	__forceinline bool Contains(const CLBPString& sSub) const { return -1 != Find(sSub.AsWideString(), 0); }
	__forceinline bool Contains(const wchar_t ch) const { return -1 != Find(ch, 0); }
	bool ContainsNoCase(const CLBPString& sSub) const;

	bool StartsWith(const CLBPString& sSub) const;
	bool StartsWithNoCase(const CLBPString& sSub) const;

	bool EndsWith(const CLBPString& sSub) const;
	bool EndsWithNoCase(const CLBPString& sSub) const;

	//counting

	int Count(char ch) const;
	int Count(wchar_t ch) const;

#ifdef _NATIVE_WCHAR_T_DEFINED
	int Count(USHORT ch) const	{ return Count((wchar_t)ch); }
#endif

	// simple formatting

	// printf-like formatting using passed string
	void AFX_CDECL FormatV(const wchar_t* lpszFormat, va_list argList);
	void AFX_CDECL Format(const char* lpszFormat, ...);
	void AFX_CDECL Format(const wchar_t* lpszFormat, ...);

#ifdef _NATIVE_WCHAR_T_DEFINED
	void AFX_CDECL FormatV(const USHORT* lpszFormat, va_list argList) { FormatV((const wchar_t*)lpszFormat, argList); }
	void AFX_CDECL Format(const USHORT* lpszFormat, ...);
#endif

#if !defined LBPLIB_DISABLE_OLE
	BSTR AllocSysString() const;
	BSTR SetSysString(BSTR* pbstr) const;
#endif

	// Access to string implementation buffer as "C" character array

	// get pointer to modifiable buffer at least as long as nMinBufLength
	char*	GetMBCSBuffer(int nMinBufLength);
	WCHAR*	GetWideBuffer(int nMinBufLength);

	// release buffer, setting length to nNewLength (or to first nul if -1)
	void	ReleaseWideBuffer(int nNewLength = -1);
	void	ReleaseMBCSBuffer(int nNewLength = -1);
	
	char*  GetMBCSBufferSetLength(int nNewLength);
	WCHAR* GetWideBufferSetLength(int nNewLength);

	//Sets the string to the actual buffer passed on - no copy is made.
	void TakeOwnershipOfBuffer(wchar_t* wsz, int iLengthOfBuffer=-1);

	//Releases the string buffer to be owned by the caller.  String is set to empty.
	wchar_t* ReleaseOwnership(void);

	void SetAllowAutoReleaseOwnership(void) const { m_bAllowAutoReleaseOwnership = true; }

#ifdef _UNICODE
	WCHAR*	GetTBuffer(int nMinBufLength);
	WCHAR*	GetTBufferSetLength(int nNewLength);
	void	ReleaseTBuffer(int nNewLength = -1);
#else
	char*	GetTBuffer(int nMinBufLength);
	char*	GetTBufferSetLength(int nNewLength);
	void	ReleaseTBuffer(int nNewLength = -1);
#endif

	void	RandomCharacters(int iLength = 12);

public:
	bool	IsValidRealNumber() const;		//must not contain a decimal part - (ie 1.0 is not valid)
	bool	IsValidIntegerNumber() const;	//also accepts integers
	bool	IsValid4dPoint() const;			//reals must have decimal point "."
	bool	IsValid3dPoint() const;			//reals must have decimal point "."
	bool	IsValid2dPoint() const;			//reals must have decimal point "."
	bool	IsValidMatrix() const;			//similar to points, but with 16 doubles

public: // Clipboard
#if !defined LBPLIB_DISABLE_OLE
	bool	SetToClipboard(HWND hWnd=NULL) const;
	bool	GetFromClipboard(HWND hWnd=NULL);
#endif

public: // Windows specific
#if !defined LBPLIB_DISABLE_MFC_CONTROLS
	static	CLBPStringBV WindowText(HWND hwnd, UINT uBufferLength=300);
	static  CLBPStringBV WindowText(CWnd* pWnd, UINT uBufferLength=300);
#endif

public: // Conversion to Hex strings
	CLBPStringBV HexString();
	CLBPStringBV HexStringMBCS();

public: // Simple conversions
	static char		ConvertWideCharToMBCSChar(wchar_t w);
	static WCHAR	ConvertMBCSCharToWideChar(char w);

	void ConvertLineEndingsToLFs(void);   // CRLF -> LF ; CR -> LF
	void ConvertLineEndingsToCRLFs(void); // CR -> CRLF ; LF -> CRLF

public: //To make it easier to write reference-counted-agnostic code
#ifndef USE_REFERENCE_COUNTED_STRINGS
	CLBPString& EmptyStringForWrite(void)
	{
		Empty();
		return *this;
	}
#endif

protected:
	void InternalFormatV(const wchar_t* lpszFormat, va_list argList);

	void	SyncMBCS() const;	//Synchronise the MBCS string for output

	size_t	Allocate(size_t dwAlloc, bool bExact = false);	//Returns the actual amount allocated
	size_t	AllocateExact(size_t dwAlloc);
	inline	size_t	GetAllocation() const;

	bool	IsCommaDelimitedDoubleArray(int iDoubles) const;
	void	ConvertNonAsciiCharactersToEntities();
	int		FindFirstNonAsciiCharacter() const;
	void	Destroy();

#ifdef WIN64
	int GetLengthImpl() const;
#else
	__forceinline int GetLengthImpl() const { return (int)(m_dwLengthCache = ((INT_PTR)wcslen(m_wsz))); }
#endif

#ifdef _DEBUG
	bool OLD_IsValidIntegerNumber() const;
	bool OLD_IsValidRealNumber() const;
	bool OLD_IsCommaDelimitedDoubleArray(int iDoubles) const;
#endif

protected:
	static wchar_t* m_wszEmpty;
	wchar_t* m_wsz = nullptr;
	mutable char* m_sz = nullptr;
	DWORD_PTR m_dwSizeAllocation = 0;
	mutable INT_PTR m_dwLengthCache = -1;
	mutable bool m_bAllowAutoReleaseOwnership = false;
};

CLBPString FormatString(const wchar_t* sFormatString, ...);

void LBP_SetStringTestOptimizationsOn(void);

#ifdef USE_BYVALUE_OPTIMIZATION
//Special optimized class for returning by value
class CLBPStringBV : public CLBPString
{
public:
	CLBPStringBV();
	CLBPStringBV(const CLBPString&);
};
#endif


#ifdef USE_REFERENCE_COUNTED_STRINGS

class CLBPString_RC_Data
{
	CLBPString_RC_Data(const CLBPString& s)
		: m_iRefCount(1),
		  m_string(s)
	{
	}

	REFCOUNT_INT AddRef(void)
	{
		return InterlockedIncrement(&m_iRefCount);
	}

	REFCOUNT_INT Release(void)
	{	
		const REFCOUNT_INT iRef = InterlockedDecrement(&m_iRefCount);
		if (0 == iRef)
		{
			delete this;
		}
		return iRef;
	}

	volatile REFCOUNT_INT m_iRefCount;
	CLBPString m_string;

	friend class CLBPString_RC;
};

class CLBPString_RC
{
public:
	CLBPString_RC();
	CLBPString_RC(const CLBPString_RC& src);
	~CLBPString_RC();

	bool operator==(const wchar_t* other) { return String().operator==(other); }
	bool operator!=(const wchar_t* other) { return String().operator!=(other); }

	int CompareNoCase(const wchar_t* lpsz) const { return String().CompareNoCase(lpsz); }

	CLBPString_RC& operator = (const CLBPString_RC& src);

	int GetLength(void) const { return String().GetLength(); }
	bool IsEmpty(void) const { return String().IsEmpty(); }

	CLBPString& EmptyStringForWrite(void);
	
	void SetString(const CLBPString& s);

	//operator const wchar_t*(void) const { return String().Wide(); }
	//operator const char*(void) const { return String().Mbcs(); }
	operator const CLBPString&(void) const { return String(); }

	const wchar_t* Wide(void) const { return String().Wide(); }
	const char* Mbcs(void) const { return String().Mbcs(); }
	const wchar_t* AsWideString(void) const { return String().AsWideString(); }
	const char* AsMBCSString(void) const { return String().AsMBCSString(); }

	DWORD CRC(void) const { return String().CRC(); }

	bool	IsValidRealNumber() const		{ return String().IsValidRealNumber(); }
	bool	IsValidIntegerNumber() const	{ return String().IsValidIntegerNumber(); }
	bool	IsValid4dPoint() const			{ return String().IsValid4dPoint(); }	
	bool	IsValid3dPoint() const			{ return String().IsValid3dPoint(); }	
	bool	IsValid2dPoint() const			{ return String().IsValid2dPoint(); }	
	bool	IsValidMatrix() const			{ return String().IsValidMatrix(); }

	void Empty(void);

private:
	CLBPString_RC_Data* m_pData;

	static const CLBPString& _empty(void);

	//static CLBPString m_empty;

	const CLBPString& String(void) const { return m_pData ? m_pData->m_string : _empty(); }
};

#else

#define CLBPString_RC CLBPString

#endif // USE_REFERENCE_COUNTED_STRINGS
