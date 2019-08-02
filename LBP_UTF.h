
#pragma once

#include "LBPString.h"

#define LBP_UTF8_CHAR  unsigned char
#define LBP_UTF16_CHAR unsigned short
#define LBP_UTF32_CHAR unsigned int

template<typename T> int LBP_UTF_LengthInChars(const T* wsz)
{
	int count = 0;
	auto* p = wsz;
	while (*p++ != 0)
	{
		count++;
	}
	return count;
}

class CLBP_UTF
{
public:
	CLBP_UTF();
	virtual ~CLBP_UTF();

	/** Get contents as a string. */
	CLBPString String(void) const;

	/** Set contents to a specific string. */
	void SetString(const CLBPString& s);

	/** Set contents from a zero-terminated wchar_t string. */
	void Set(const wchar_t * sz);

	/** Set contents from a zero-terminated UTF8 string. */
	void SetUTF8(const LBP_UTF8_CHAR * sz);

	/** Set contents from a zero-terminated UTF16 string. */
	void SetUTF16(const LBP_UTF16_CHAR * wsz);

	/** Set contents from a zero-terminated UTF32 string. */
	void SetUTF32(const LBP_UTF32_CHAR * dwsz);

	/** Returns the zero-terminated UTF8 representation of the
		contents or NULL if the contents are invalid. */
	const LBP_UTF8_CHAR* UTF8(void) const;

	/** Returns the zero-terminated UTF16 representation of the
		contents or NULL if the contents are invalid. */
	const LBP_UTF16_CHAR* UTF16(void) const;

	/** Returns the zero-terminated UTF32 representation of the
		contents or NULL if the contents are invalid. */
	const LBP_UTF32_CHAR* UTF32(void) const;

	/** Returns the zero-terminated wchar_t* representation of the
		contents or NULL if the contents are invalid. */
	const wchar_t* AsWide(void) const;

	// 18th June 2018 John Croudy, https://mcneel.myjetbrains.com/youtrack/issue/RH-46786
	// This clears ALL THREE buffers.
	void ClearUTFBuffers(void);

	static bool IsValidCodePoint(LBP_UTF32_CHAR dwCodePoint);

private:
	class Impl;
	Impl * m_pImpl;
};
