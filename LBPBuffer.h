#pragma once

#ifndef LBPEXPORT
#define LBPEXPORT
#endif

#if defined (LBPLIB_APPLE_SPECIFIC)
#define REFCOUNT_INT int32_t
#endif

#if defined (LBPLIB_WINDOWS_SPECIFIC)
#define REFCOUNT_INT LONG
#endif

class CLBPBuffer;
#include "LBPString.h"

class LBPEXPORT CLBPInternalBuffer
{
public:
	CLBPInternalBuffer(const void* pBuffer, size_t size);
	CLBPInternalBuffer(const CLBPString& s);
	CLBPInternalBuffer(size_t size);	//Creates an allocated buffer containing random data
	CLBPInternalBuffer();
	~CLBPInternalBuffer();

	int	AddRef() { return ::InterlockedIncrement(&m_iRefCount); }
	int Release();

	void Set(const void* pBuffer, size_t size);
	void Set(size_t size);

	void Clear() { Set(0); }

	int Compare(const CLBPInternalBuffer& b) const;

	const CLBPInternalBuffer& operator = (const CLBPInternalBuffer& b);
	const CLBPInternalBuffer& operator = (const CLBPString& sBase64);

public:
	const CLBPString& Base64() const;
	void Base64(CLBPString& s) const;

	DWORD CRC(void) const;

private:
	BYTE* m_pBytes;
	size_t m_NumBytes;
	volatile REFCOUNT_INT m_iRefCount;

	mutable CLBPString m_sBase64;
	mutable bool m_bBase64Dirty = true;

	mutable DWORD m_dwCRC = 0;
	mutable bool m_bCRCDirty = true;

	friend class CLBPBuffer;
};

class LBPEXPORT CLBPBuffer
{
public:
	CLBPBuffer();
	CLBPBuffer(const void* pBuffer, size_t size);
	CLBPBuffer(const CLBPBuffer& src);
	CLBPBuffer(const CLBPString& buffer);
	CLBPBuffer(size_t size);	//Creates an allocated buffer containing random data
	~CLBPBuffer();

	void Clear() { Set(0); }

	static const WCHAR* Base64Prefix();

public:

	const CLBPBuffer& operator =  (const CLBPBuffer& b);
	const CLBPBuffer& operator =  (const CLBPString& sBase64);

public:
	const CLBPString& Base64() const;
	void Base64(CLBPString& s) const;

	DWORD CRC(void) const { return m_pBuffer->CRC(); }

public:
	void Set(const void* pBuffer, size_t size);
	void Set(size_t size);

	BYTE* GetBuffer(size_t size);
	void  ReleaseBuffer();

	int  Compare(const CLBPBuffer& b) const;
	
	bool operator == (const CLBPBuffer& b) const;
	bool operator >  (const CLBPBuffer& b) const;
	bool operator <  (const CLBPBuffer& b) const;

	const BYTE* Bytes(void) const	{ return NULL == m_pBuffer ? NULL : m_pBuffer->m_pBytes; }
	size_t NumBytes(void) const		{ return NULL == m_pBuffer ? 0 : m_pBuffer->m_NumBytes; }

private:
	CLBPInternalBuffer* m_pBuffer;
};

