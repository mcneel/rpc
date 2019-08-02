
#include "stdafx.h"
#include "LBPBuffer.h"
#include "LBPString.h"
#include "LBPBase64.h"
#include "LBP_CRC32.h"

static const WCHAR* wszBase64Prefix = L"base64:";

const WCHAR* CLBPBuffer::Base64Prefix()
{
	return wszBase64Prefix;
}

CLBPInternalBuffer::CLBPInternalBuffer()
{
	m_pBytes = NULL;
	m_NumBytes = 0;
	m_iRefCount = 1;
}

CLBPInternalBuffer::CLBPInternalBuffer(const void* pBuffer, size_t size)
{
	m_pBytes = NULL;
	m_NumBytes = 0;
	m_iRefCount = 1;

	Set(pBuffer, size);
}

CLBPInternalBuffer::CLBPInternalBuffer(size_t size)
{
	m_pBytes = NULL;
	m_NumBytes = 0;
	m_iRefCount = 1;

	Set(size);
}

CLBPInternalBuffer::CLBPInternalBuffer(const CLBPString& sBase64)
{
	m_pBytes = NULL;
	m_NumBytes = 0;
	m_iRefCount = 1;

	*this = sBase64;
}

CLBPInternalBuffer::~CLBPInternalBuffer()
{
	delete[] m_pBytes;
}

int CLBPInternalBuffer::Release()
{
	const int iRefCount = ::InterlockedDecrement(&m_iRefCount);

	if (0 == iRefCount)
		delete this;

	return iRefCount;
}

void CLBPInternalBuffer::Set(const void* pBuffer, size_t size)
{
	Set(size);

	if (NULL != m_pBytes)
	{
		::CopyMemory(m_pBytes, pBuffer, size);
	}
}

void CLBPInternalBuffer::Set(size_t size)
{
	m_bBase64Dirty = true;
	m_bCRCDirty = true;

	if (m_NumBytes != size)
	{
		delete [] m_pBytes;
		m_NumBytes = size;
		m_pBytes = (size > 0) ? new BYTE[size] : NULL;
	}
}


int CLBPInternalBuffer::Compare(const CLBPInternalBuffer& ib) const
{
	if (m_NumBytes != ib.m_NumBytes)
		return (int)(m_NumBytes - ib.m_NumBytes);

	if (NULL == m_pBytes)
	{
		return (NULL == ib.m_pBytes) ? 0 : -1;
	}

	if (NULL == ib.m_pBytes)
		return +1;

	return memcmp(m_pBytes, ib.m_pBytes, m_NumBytes);
}

const CLBPInternalBuffer& CLBPInternalBuffer::operator = (const CLBPInternalBuffer& ib)
{
	Set(ib.m_pBytes, ib.m_NumBytes);
	return *this;
}

const CLBPInternalBuffer& CLBPInternalBuffer::operator = (const CLBPString& s)
{
	if (s.StartsWithNoCase(CLBPBuffer::Base64Prefix()) && s != CLBPBuffer::Base64Prefix())
	{
		const int iPrefixLen = CLBPString(CLBPBuffer::Base64Prefix()).GetLength();

		CLBPBase64 b;
		const int iSize = s.GetLength() - iPrefixLen; // Base64 is about 33% bigger than the resulting data.
		Set(iSize);
		m_NumBytes = b.Decode(m_pBytes, s.Mid(iPrefixLen));
	}
	else
	{
		Clear();
	}

	return *this;
}

const CLBPString& CLBPInternalBuffer::Base64() const
{
	if (m_bBase64Dirty)
	{
		Base64(m_sBase64);
	}

	return m_sBase64;
}

void CLBPInternalBuffer::Base64(CLBPString& s) const
{
	if (m_bBase64Dirty)
	{
		m_bBase64Dirty = false;

		s = CLBPBuffer::Base64Prefix();

		if (NULL != m_pBytes)
		{
			CLBPBase64 b;
			b.Encode(m_pBytes, m_NumBytes, s, true);
		}

		m_sBase64 = s;
	}
	else
	{
		s = m_sBase64;
	}
}

DWORD CLBPInternalBuffer::CRC(void) const
{
	if (m_bCRCDirty)
	{
		m_dwCRC = CLBP_CRC32::Calculate(m_pBytes, m_NumBytes);
		m_bCRCDirty = false;
	}

	return m_dwCRC;
}




CLBPBuffer::CLBPBuffer()
{
	m_pBuffer = new CLBPInternalBuffer();
}

CLBPBuffer::CLBPBuffer(const void* pBuffer, size_t size)
{
	m_pBuffer = new CLBPInternalBuffer(pBuffer, size);
}

CLBPBuffer::CLBPBuffer(size_t size)
{
	m_pBuffer = new CLBPInternalBuffer(size);
}

CLBPBuffer::CLBPBuffer(const CLBPBuffer& src)
{
	src.m_pBuffer->AddRef();
	m_pBuffer = src.m_pBuffer;
}

CLBPBuffer::CLBPBuffer(const CLBPString& sBase64)
{
	m_pBuffer = new CLBPInternalBuffer(sBase64);
}

const CLBPBuffer& CLBPBuffer::operator=(const CLBPBuffer& src)
{
	if (this == &src) 
		return *this;

	src.m_pBuffer->AddRef();

	ASSERT(m_pBuffer);
	m_pBuffer->Release();

	m_pBuffer = src.m_pBuffer;

	return *this;
}

const CLBPBuffer& CLBPBuffer::operator = (const CLBPString& sBase64)
{
	ASSERT(m_pBuffer);
	m_pBuffer->Release();

	m_pBuffer = new CLBPInternalBuffer(sBase64);
	return *this;
}

CLBPBuffer::~CLBPBuffer()
{
	ASSERT(m_pBuffer);
	m_pBuffer->Release();
}

int CLBPBuffer::Compare(const CLBPBuffer& b) const
{
	ASSERT(m_pBuffer);
	ASSERT(b.m_pBuffer);

	return m_pBuffer->Compare(*b.m_pBuffer);
}

void CLBPBuffer::Set(const void* pBuffer, size_t size)
{
	ASSERT(m_pBuffer);

	if (m_pBuffer->m_iRefCount > 1)
	{
		//We are not the only owners - need to clone up
		*this = CLBPBuffer(pBuffer, size);
	}
	else
	{
		m_pBuffer->Set(pBuffer, size);
	}
}

void CLBPBuffer::Set(size_t size)
{
	Set(NULL, size);
}

BYTE* CLBPBuffer::GetBuffer(size_t size)
{
	*this = CLBPBuffer(size);

	return m_pBuffer->m_pBytes;
}

bool CLBPBuffer::operator == (const CLBPBuffer& b) const
{
	return Compare(b) == 0;
}

bool CLBPBuffer::operator > (const CLBPBuffer& b) const
{
	return Compare(b) > 0;
}

bool CLBPBuffer::operator < (const CLBPBuffer& b) const
{
	return Compare(b) < 0;
}

const CLBPString& CLBPBuffer::Base64() const
{
	return m_pBuffer->Base64();
}

void CLBPBuffer::Base64(CLBPString& s) const
{
	m_pBuffer->Base64(s);
}
