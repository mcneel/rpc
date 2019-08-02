
#include "stdafx.h"
#include "LBPBase64.h"
#include "LBPString.h"

static const int DecodeTab[128] =
{
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1,  0, -1, -1,
	-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
};

CLBPBase64::CLBPBase64()
{
}

CLBPBase64::~CLBPBase64()
{
}

static bool ReadEncodedByte(int& pos, const WCHAR* wsz, BYTE& byteOut)
{
	wchar_t w = wsz[pos];
	if (w >= 128)
		w  = 128; // Will fail validation.

	byteOut = (BYTE)w;

	while (byteOut != 0)
	{
		pos++;

		// Check that the byte is a valid Base64 character.
		// The Base64 specification requires garbled data to be ignored.
		if ((byteOut < 128) && (DecodeTab[byteOut] >= 0))
			return true;

		// Move on to the next byte and keep trying.
		w = wsz[pos];
		if (w >= 128)
			w  = 128; // Will fail validation.

		byteOut = (BYTE)w;
	}

	return false; // End of data.
}

static size_t InternalDecode(BYTE * pBufferOut, const CLBPString& sBase64In, size_t maxLength)
{
	UINT uBytesWritten = 0;

	int iResultSize = 3;
	BYTE a, b, c, d, aResult[3];

	const WCHAR* wsz = sBase64In.AsWideString();

	int pos = 0;
	while (ReadEncodedByte(pos, wsz, a))
	{
		ReadEncodedByte(pos, wsz, b);
		ReadEncodedByte(pos, wsz, c);
		ReadEncodedByte(pos, wsz, d);

		if ('=' == d) // Handle padding character(s).
		{
			iResultSize = ('=' == c) ? 1 : 2;
		}

		if (pBufferOut != NULL)
		{
			const ULONG val = (DecodeTab[a] << 18) | (DecodeTab[b] << 12) |
							  (DecodeTab[c] <<  6) |  DecodeTab[d];

			aResult[0] = (BYTE)((val & 0xFF0000) >> 16);
			aResult[1] = (BYTE)((val & 0x00FF00) >>  8);
			aResult[2] = (BYTE)((val));

			memcpy(pBufferOut, aResult, iResultSize);

			pBufferOut += iResultSize;

			uBytesWritten += iResultSize;

			if (size_t(uBytesWritten) >= maxLength)
				break;
		}
		else
		{
			uBytesWritten += iResultSize;
		}
	}

	return size_t(uBytesWritten);
}

size_t CLBPBase64::Decode(BYTE* pBufferOut, const CLBPString& sBase64In) const
{
	return InternalDecode(pBufferOut, sBase64In, UINT_MAX);
}

size_t CLBPBase64::Decode(BYTE* pBufferOut, const CLBPString& sBase64In, size_t maxLength) const
{
	return InternalDecode(pBufferOut, sBase64In, maxLength);
}

static const WCHAR* EncodeTab = L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

bool CLBPBase64::Encode(const BYTE* pBufferIn, size_t bufNumBytes, CLBPString& sBase64Out, bool bAppend) const
{
	// Base64 is about 133% of the data size. Use 150% plus 4 for padding.
	const size_t bigBufNumBytes = MulDiv((int)bufNumBytes, 150, 100) + 4;

	const int iExistingLen = bAppend ? sBase64Out.GetLength() : 0;
	const size_t outNumBytes = iExistingLen + bigBufNumBytes;
	WCHAR* out = sBase64Out.GetWideBufferSetLength((int)outNumBytes);
	if (NULL == out)
		return false;

	out += iExistingLen;

	const BYTE * p = pBufferIn;
	const BYTE * pEnd = p + bufNumBytes;

	BYTE a, b, c;
	while (p < pEnd)
	{
		out[2] = out[3] = L'=';

		a = *p++;

		out[0] = EncodeTab[a >> 2];

		a = (BYTE)((a & 0x03) << 4);
		if (p < pEnd)
		{
			b = *p++;
			out[1] = EncodeTab[a | (b >> 4)];

			b = (BYTE)((b & 0x0F) << 2);
			if (p < pEnd)
			{
				c = *p++;
				out[2] = EncodeTab[b | (c >> 6)];
				out[3] = EncodeTab[c & 0x3F];
			}
			else
			{
				out[2] = EncodeTab[b];
			}
		}
		else
		{
			out[1] = EncodeTab[a];
		}

		out += 4;
	}

	out[0] = 0;

	sBase64Out.ReleaseWideBuffer();

	return true;
}
