
#include "stdafx.h"
#include "LBP_UTF.h"

enum
{
	eSurrogateLead  = 0x00D800,
	eSurrogateTrail = 0x00DC00,
	eSurrogateEnd   = 0x00DFFF,
	eMaxCodePoint   = 0x10FFFD,
};

class CLBP_UTF::Impl
{
public:
	Impl();
	~Impl();

	const LBP_UTF8_CHAR * UTF8(void) const;
	const LBP_UTF16_CHAR* UTF16(void) const;
	const LBP_UTF32_CHAR* UTF32(void) const;
	void SetUTF8(const  LBP_UTF8_CHAR* sz);
	void SetUTF16(const LBP_UTF16_CHAR* wsz);
	void SetUTF32(const LBP_UTF32_CHAR* dwsz);

	void ClearUTF8Buffer(void);
	void ClearUTF16Buffer(void);
	void ClearUTF32Buffer(void);

private:
	void Clear(void);

	const LBP_UTF32_CHAR* UTF8toUTF32(const LBP_UTF8_CHAR* sz) const;
	const LBP_UTF8_CHAR * UTF32toUTF8(const LBP_UTF32_CHAR* dwsz) const;
	const LBP_UTF32_CHAR* UTF16toUTF32(const LBP_UTF16_CHAR* wsz) const;
	const LBP_UTF16_CHAR* UTF32toUTF16(const LBP_UTF32_CHAR* dwsz) const;

	int  SizeUTF8Sequence(LBP_UTF8_CHAR iLead) const;
	int  SizeUTF8Sequence(LBP_UTF32_CHAR dwCodePoint) const;
	int  CountUTF8Sequences(const LBP_UTF8_CHAR* sz) const;
	bool SurrogatePairToCodePoint(LBP_UTF32_CHAR& dwCodePointOut, LBP_UTF16_CHAR wCharLead, LBP_UTF16_CHAR wCharTrail) const;
	bool SurrogatePairFromCodePoint(LBP_UTF32_CHAR dwCodePoint, LBP_UTF16_CHAR& wCharLeadOut, LBP_UTF16_CHAR& wCharTrailOut) const;

private: // UTF8 storage.
	mutable LBP_UTF8_CHAR * m_aUTF8;
	mutable int m_iSizeUTF8;

private: // UTF16 storage.
	mutable LBP_UTF16_CHAR * m_aUTF16;
	mutable int m_iSizeUTF16;

private: // UTF32 storage.
	mutable LBP_UTF32_CHAR * m_aUTF32;
	mutable int m_iSizeUTF32;
};

CLBP_UTF::Impl::Impl()
{
	m_aUTF32 = nullptr;
	m_iSizeUTF32 = 0;

	m_aUTF8 = nullptr;
	m_iSizeUTF8 = 0;

	m_aUTF16 = nullptr;
	m_iSizeUTF16 = 0;
}

CLBP_UTF::Impl::~Impl()
{
	Clear();
}

void CLBP_UTF::Impl::ClearUTF32Buffer(void)
{
	delete[] m_aUTF32;
	m_aUTF32 = nullptr;
	m_iSizeUTF32 = 0;
}

void CLBP_UTF::Impl::ClearUTF16Buffer(void)
{

	delete[] m_aUTF16;
	m_aUTF16 = nullptr;
	m_iSizeUTF16 = 0;
}

void CLBP_UTF::Impl::ClearUTF8Buffer(void)
{
	delete[] m_aUTF8;
	m_aUTF8 = nullptr;
	m_iSizeUTF8 = 0;
}

void CLBP_UTF::Impl::Clear(void)
{
	ClearUTF8Buffer();
	ClearUTF16Buffer();
	ClearUTF32Buffer();
}

const LBP_UTF8_CHAR* CLBP_UTF::Impl::UTF8(void) const
{
	if (0 == m_iSizeUTF8)
	{
		if (m_iSizeUTF16 > 0)
			UTF16toUTF32(m_aUTF16);

		if (m_iSizeUTF32 > 0)
			UTF32toUTF8(m_aUTF32);
	}

	return m_aUTF8;
}

const LBP_UTF16_CHAR* CLBP_UTF::Impl::UTF16(void) const
{
	if (0 == m_iSizeUTF16)
	{
		if (m_iSizeUTF8 > 0)
			UTF8toUTF32(m_aUTF8);

		if (m_iSizeUTF32 > 0)
			UTF32toUTF16(m_aUTF32);
	}

	return m_aUTF16;
}

const LBP_UTF32_CHAR* CLBP_UTF::Impl::UTF32(void) const
{
	if (0 == m_iSizeUTF32)
	{
		if (m_iSizeUTF16 > 0)
			UTF16toUTF32(m_aUTF16);
		else
		if (m_iSizeUTF8 > 0)
			UTF8toUTF32(m_aUTF8);
	}

	return m_aUTF32;
}

void CLBP_UTF::Impl::SetUTF8(const LBP_UTF8_CHAR* sz)
{
	if (sz == m_aUTF8)
		return;

	Clear();

	const int numChars = LBP_UTF_LengthInChars<LBP_UTF8_CHAR>(sz);

	const size_t numCharsIncTerm = numChars + 1;

	m_aUTF8 = new LBP_UTF8_CHAR[numCharsIncTerm];
	memcpy(m_aUTF8, sz, numCharsIncTerm * sizeof(LBP_UTF8_CHAR));

	m_iSizeUTF8 = numChars;
}

void CLBP_UTF::Impl::SetUTF16(const LBP_UTF16_CHAR* wsz)
{
	if (wsz == m_aUTF16)
		return;

	Clear();

	const int numChars = LBP_UTF_LengthInChars<LBP_UTF16_CHAR>(wsz);

	const size_t numCharsIncTerm = numChars + 1;

	m_aUTF16 = new LBP_UTF16_CHAR[numCharsIncTerm];
	memcpy(m_aUTF16, wsz, numCharsIncTerm * sizeof(LBP_UTF16_CHAR));

	// 15th June 2018 John Croudy, https://mcneel.myjetbrains.com/youtrack/issue/RH-46786
	// When this was changed from a string to a buffer, this line was left out.
	// This didn't actually cause RH-46786, but it still needs fixing.
	m_iSizeUTF16 = numChars;
}

void CLBP_UTF::Impl::SetUTF32(const LBP_UTF32_CHAR* dwsz)
{
	if (dwsz == m_aUTF32)
		return;

	Clear();

	const int numChars = LBP_UTF_LengthInChars<LBP_UTF32_CHAR>(dwsz);

	const size_t numCharsIncTerm = numChars + 1;

	m_aUTF32 = new LBP_UTF32_CHAR[numCharsIncTerm];
	memcpy(m_aUTF32, dwsz, numCharsIncTerm * sizeof(LBP_UTF32_CHAR));

	m_iSizeUTF32 = numChars;
}

const LBP_UTF8_CHAR* CLBP_UTF::Impl::UTF32toUTF8(const LBP_UTF32_CHAR * dwsz) const
{
	if (NULL == dwsz)
		return NULL;

	for (int i = 0; i < m_iSizeUTF32; i++)
	{
		const int iSeqSize = SizeUTF8Sequence(dwsz[i]);
		if (0 == iSeqSize)
			return NULL;

		m_iSizeUTF8 += iSeqSize;
	}

	delete[] m_aUTF8;
	m_aUTF8 = new LBP_UTF8_CHAR[m_iSizeUTF8 + 1];

	LBP_UTF8_CHAR* pSequence = m_aUTF8;

	for (int i = 0; i < m_iSizeUTF32; i++)
	{
		const LBP_UTF32_CHAR dwCodePoint = dwsz[i];

		const int iSeqSize = SizeUTF8Sequence(dwCodePoint);
		switch (iSeqSize)
		{
		case 1:
			*pSequence++ = (LBP_UTF8_CHAR)dwCodePoint;
			break;

		case 2:
			*pSequence++ = (LBP_UTF8_CHAR)(0xC0 | ((dwCodePoint >>  6)       ));
			*pSequence++ = (LBP_UTF8_CHAR)(0x80 | ((dwCodePoint      ) & 0x3F));
			break;

		case 3:
			*pSequence++ = (LBP_UTF8_CHAR)(0xE0 | ((dwCodePoint >> 12)       ));
			*pSequence++ = (LBP_UTF8_CHAR)(0x80 | ((dwCodePoint >>  6) & 0x3F));
			*pSequence++ = (LBP_UTF8_CHAR)(0x80 | ((dwCodePoint      ) & 0x3F));
			break;

		case 4:
			*pSequence++ = (LBP_UTF8_CHAR)(0xF0 | ((dwCodePoint >> 18)       ));
			*pSequence++ = (LBP_UTF8_CHAR)(0x80 | ((dwCodePoint >> 12) & 0x3F));
			*pSequence++ = (LBP_UTF8_CHAR)(0x80 | ((dwCodePoint >>  6) & 0x3F));
			*pSequence++ = (LBP_UTF8_CHAR)(0x80 | ((dwCodePoint      ) & 0x3F));
		}
	}

	*pSequence = 0; // Terminator.

	return m_aUTF8;
}

const LBP_UTF32_CHAR* CLBP_UTF::Impl::UTF8toUTF32(const LBP_UTF8_CHAR* sz) const
{
	if (NULL == sz)
		return NULL;

	m_iSizeUTF32 = CountUTF8Sequences(sz);

	delete[] m_aUTF32;
	m_aUTF32 = new LBP_UTF32_CHAR[m_iSizeUTF32 + 1];

	LBP_UTF32_CHAR* pCodePoint = m_aUTF32;

	LBP_UTF8_CHAR byte2, byte3, byte4;

	for (int i = 0; i < m_iSizeUTF32; i++)
	{
		const LBP_UTF8_CHAR byteLead = *sz++;

		const int iSeqSize = SizeUTF8Sequence(byteLead);
		switch (iSeqSize)
		{
		case 1:
			*pCodePoint++ = byteLead;
			break;

		case 2:
			byte2 = *sz++ & 0x3F;
			*pCodePoint++ = ((byteLead & 0x1F) << 6) | byte2;
			break;

		case 3:
			byte2 = *sz++ & 0x3F;
			byte3 = *sz++ & 0x3F;
			*pCodePoint++ = ((byteLead & 0x0F) << 12) | (byte2 << 6) | byte3;
			break;

		case 4:
			byte2 = *sz++ & 0x3F;
			byte3 = *sz++ & 0x3F;
			byte4 = *sz++ & 0x3F;
			*pCodePoint++ = ((byteLead & 0x07) << 18) | (byte2 << 12) | (byte3 << 6) | byte4;
		}
	}

	*pCodePoint = 0; // Terminator.

	return m_aUTF32;
}

const LBP_UTF32_CHAR* CLBP_UTF::Impl::UTF16toUTF32(const LBP_UTF16_CHAR * wsz) const
{
	if (NULL == wsz)
		return NULL;

	// Create UTF32 buffer. This requires the same number of characters
	// as UTF16, or fewer if UTF16 contains one or more surrogate pairs, but it
	// will never require more.
	const size_t lengthInChars = LBP_UTF_LengthInChars<LBP_UTF16_CHAR>(wsz);

	delete[] m_aUTF32;
	m_aUTF32 = new LBP_UTF32_CHAR[lengthInChars + 1];

	LBP_UTF32_CHAR* pCodePoint = m_aUTF32;

	m_iSizeUTF32 = 0;

	while (0 != *wsz)
	{
		const LBP_UTF16_CHAR wCharLead = *wsz++;

		LBP_UTF32_CHAR dwCodePoint = wCharLead;

		if ((wCharLead >= eSurrogateLead) && (wCharLead < eSurrogateTrail))
		{
			if (0 == *wsz)
				return NULL; // Trailing surrogate missing.

			const LBP_UTF16_CHAR wCharTrail = *wsz++;
			if (!SurrogatePairToCodePoint(dwCodePoint, wCharLead, wCharTrail))
				return NULL;
		}
		else
		if ((wCharLead >= eSurrogateTrail) && (wCharLead <= eSurrogateEnd))
			return NULL; // Invalid surrogate.

		*pCodePoint++ = dwCodePoint;

		m_iSizeUTF32++;
	}

	*pCodePoint = 0; // Terminator.

	return m_aUTF32;
}

const LBP_UTF16_CHAR* CLBP_UTF::Impl::UTF32toUTF16(const LBP_UTF32_CHAR * dwsz) const
{
	if (NULL == dwsz)
		return NULL;

	LBP_UTF32_CHAR dwCodePoint = 0;

	int iCount = 0;

	const LBP_UTF32_CHAR* dwszTmp = dwsz;
	while (0 != (dwCodePoint = *dwszTmp++))
	{
		if (!IsValidCodePoint(dwCodePoint))
			return NULL;

		if (dwCodePoint >= 0x10000)
			iCount++;

		iCount++;
	}

	if (0 == iCount)
		return NULL;

	const size_t numCharsIncTerm = iCount + 1;

	delete[] m_aUTF16;
	m_aUTF16 = new LBP_UTF16_CHAR[numCharsIncTerm];

	int iIndex = 0;
	while (0 != (dwCodePoint = *dwsz++))
	{
		if (dwCodePoint < 0x10000)
		{
			m_aUTF16[iIndex++] = (LBP_UTF16_CHAR)dwCodePoint;
		}
		else
		{
			// Too big for 16 bits so make a surrogate pair.
			LBP_UTF16_CHAR wCharLead = 0, wCharTrail = 0;
			if (!SurrogatePairFromCodePoint(dwCodePoint, wCharLead, wCharTrail))
				return NULL;

			m_aUTF16[iIndex++] = wCharLead;
			m_aUTF16[iIndex++] = wCharTrail;
		}
	}

	// 15th June 2018 John Croudy, https://mcneel.myjetbrains.com/youtrack/issue/RH-46786
	// When this was changed from a string to a buffer, this line was left out.
	// This didn't actually cause RH-46786, but it still needs fixing.
	m_iSizeUTF16 = iIndex;

	m_aUTF16[iIndex] = 0; // Terminator.

	ASSERT(iCount == iIndex);

	return m_aUTF16;
}

int CLBP_UTF::Impl::CountUTF8Sequences(const LBP_UTF8_CHAR* sz) const
{
	int iCount = 0, iIndex = 0;

	const int iSizeUTF8 = (int)strlen((const char*)sz);
	while (iIndex < iSizeUTF8)
	{
		const int iSeqSize = SizeUTF8Sequence(sz[iIndex]);
		if (0 == iSeqSize)
			return -1;

		iIndex += iSeqSize;
		iCount++;
	}

	return iCount;
}

int CLBP_UTF::Impl::SizeUTF8Sequence(LBP_UTF8_CHAR byteLead) const
{
	if (byteLead >= 0xF0)
		return 4;

	if (byteLead >= 0xE0)
		return 3;

	if (byteLead >= 0xC0)
		return 2;

	// What about invalid lead bytes? return 0;

	return 1;
}

int CLBP_UTF::Impl::SizeUTF8Sequence(LBP_UTF32_CHAR dwCodePoint) const
{
	if (!IsValidCodePoint(dwCodePoint))
		return 0;

	if (dwCodePoint < 0x007F)
		return 1;

	if (dwCodePoint < 0x07FF)
		return 2;

	if (dwCodePoint < 0xFFFF)
		return 3;

	return 4;
}

bool CLBP_UTF::Impl::SurrogatePairToCodePoint(LBP_UTF32_CHAR& dwCodePointOut, LBP_UTF16_CHAR wLead, LBP_UTF16_CHAR wTrail) const
{
//	TODO: if wLead or wTrail are invalid surrogates, return false.

	const LBP_UTF32_CHAR hi = wLead  & 0x03FF;
	const LBP_UTF32_CHAR lo = wTrail & 0x03FF;

	dwCodePointOut = ((hi << 10) | lo) + 0x10000;

	return true;
}

bool CLBP_UTF::Impl::SurrogatePairFromCodePoint(LBP_UTF32_CHAR dwCodePoint, LBP_UTF16_CHAR& wCharLeadOut, LBP_UTF16_CHAR& wCharTrailOut) const
{
	if (dwCodePoint < 0x10000)
		return false;

	const LBP_UTF32_CHAR a = dwCodePoint - 0x10000;
	wCharLeadOut  = (LBP_UTF16_CHAR)(eSurrogateLead  | (a >> 10));
	wCharTrailOut = (LBP_UTF16_CHAR)(eSurrogateTrail | (a & 0x3FF));

	return true;
}

//---------------------------------------------------------------------------------------

CLBP_UTF::CLBP_UTF()
{
	m_pImpl = new Impl;
}

CLBP_UTF::~CLBP_UTF()
{
	delete m_pImpl;
}

CLBPString CLBP_UTF::String(void) const
{
#ifdef RHINO_APPLE
	return (const wchar_t*)m_pImpl->UTF32();
#else
	return (const wchar_t*)m_pImpl->UTF16();
#endif
}

void CLBP_UTF::SetString(const CLBPString& s)
{
#ifdef RHINO_APPLE
	m_pImpl->SetUTF32((const LBP_UTF32_CHAR*)s.Wide());
#else
	m_pImpl->SetUTF16((const LBP_UTF16_CHAR*)s.Wide());
#endif
}

const wchar_t* CLBP_UTF::AsWide(void) const
{
#ifdef RHINO_APPLE
	return (const wchar_t*)UTF32();
#else
	return (const wchar_t*)UTF16();
#endif
}

const LBP_UTF8_CHAR* CLBP_UTF::UTF8(void) const
{
	return m_pImpl->UTF8();
}

const LBP_UTF16_CHAR* CLBP_UTF::UTF16(void) const
{
	return m_pImpl->UTF16();
}

const LBP_UTF32_CHAR* CLBP_UTF::UTF32(void) const
{
	return m_pImpl->UTF32();
}

void CLBP_UTF::Set(const wchar_t* sz)
{
#ifdef RHINO_APPLE
	SetUTF32((const LBP_UTF32_CHAR*)sz);
#else
	SetUTF16((const LBP_UTF16_CHAR*)sz);
#endif
}

void CLBP_UTF::SetUTF8(const LBP_UTF8_CHAR* sz)
{
	m_pImpl->SetUTF8(sz);
}

void CLBP_UTF::SetUTF16(const LBP_UTF16_CHAR* wsz)
{
	m_pImpl->SetUTF16(wsz);
}

void CLBP_UTF::SetUTF32(const LBP_UTF32_CHAR* dwsz)
{
	m_pImpl->SetUTF32(dwsz);
}

void CLBP_UTF::ClearUTFBuffers(void)
{
	m_pImpl->ClearUTF8Buffer();
	m_pImpl->ClearUTF16Buffer();
	m_pImpl->ClearUTF32Buffer();
}

bool CLBP_UTF::IsValidCodePoint(LBP_UTF32_CHAR dwCodePoint)
{
	if ((dwCodePoint >= eSurrogateLead) && (dwCodePoint <= eSurrogateEnd))
		return false; // Code point is not allowed to be within surrogate range.

	if (dwCodePoint > eMaxCodePoint)
		return false; // Code point out of range.

	return true;
}
