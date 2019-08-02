
#pragma once

class CLBPString;

class CLBPBase64
{
public:
	CLBPBase64();
	virtual ~CLBPBase64();

	// Decode a base64 string.
	// pBufferOut must be large enough to accomodate the decoded data. It is safe to use the
	// length of sBase64in because this Base64 string will always be about 33% bigger than the
	// data it was created from. Returns the number of bytes written to pBufferOut.
	// If pBufferOut is NULL, the function simply calculates the exact required buffer size.
	size_t Decode(BYTE* pBufferOut, const CLBPString& sBase64in) const;

	// The same as Decode() above, but stops when uMaxLength bytes have been decoded.
	// Added 13.04.2010 to allow reading of data that was saved with garbage at the end
	// due to a bug in the base 64 cache.
	size_t Decode(BYTE* pBufferOut, const CLBPString& sBase64in, size_t maxLength) const;

	// Encode data to a base64 string. If bAppend is true, the encoded string is appended to sBase64out.
	// Otherwise sBase64out is replaced with the encoded string.
	// Returns true if ok, false if unable to allocate the output buffer.
	bool Encode(const BYTE* pBufferIn, size_t dwBufNumBytes, CLBPString& sBase64out, bool bAppend) const;
};
