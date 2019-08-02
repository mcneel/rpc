
#include "stdafx.h"
#include "LBP_UUID.h"
#include "LBP_CRC32.h"

#pragma comment (lib, "rpcrt4.lib")

CLBP_UUID::CLBP_UUID()
{
	// The UUID must be initialized to zero.
	memset((UUID*)this, 0, sizeof(UUID));
}

CLBP_UUID::CLBP_UUID(const UUID& uuid)
{
	*(UUID*)this = uuid;
}

CLBP_UUID::CLBP_UUID(const CLBPString& s)
{
	SetAsString(s);
}

CLBP_UUID::CLBP_UUID(const char * sz)
{
	SetAsString(sz);
}

CLBP_UUID::CLBP_UUID(const WCHAR * wsz)
{
	SetAsString(wsz);
}

void CLBP_UUID::Create(void)
{
	UuidCreate((UUID*)this);
}

bool CLBP_UUID::IsNil(void) const
{
#ifdef LBPRHLIB
    return ON_UuidIsNil((UUID&)*this);
#else
	RPC_STATUS s;
	return UuidIsNil((UUID*)this, &s) ? true : false;
#endif
}

int CLBP_UUID::Compare(const UUID& uuid) const
{
#ifdef LBPRHLIB
    return ON_UuidCompare((UUID*)this, (UUID*)&uuid);
#else
	RPC_STATUS s;
	return UuidCompare((UUID*)this, (UUID*)&uuid, &s);
#endif
}

int CLBP_UUID::Compare(const char * sz) const
{
#ifdef LBPRHLIB
    const ON_UUID u = ON_UuidFromString(sz);
    return Compare(u);
#else
    UUID u;
	UuidFromStringA((unsigned char *)sz, &u);
	return Compare(u);
#endif
}

int CLBP_UUID::Compare(const WCHAR * wsz) const
{
#ifdef LBPRHLIB
    const ON_UUID u = ON_UuidFromString(wsz);
    return Compare(u);
#else
    UUID u;
	UuidFromStringW(reinterpret_cast<USHORT*>(CLBPString(wsz).AsNonConstWideString()), &u);
	return Compare(u);
#endif
}

CLBP_UUID& CLBP_UUID::operator = (const UUID& uuid)
{
	*(UUID*)this = uuid;

	return *this;
}

CLBP_UUID& CLBP_UUID::operator = (const char * sz)
{
	SetAsString(sz);

	return *this;
}

CLBP_UUID& CLBP_UUID::operator = (const WCHAR * wsz)
{
	SetAsString(wsz);

	return *this;
}

CLBP_UUID& CLBP_UUID::operator = (const CLBPString& s)
{
	SetAsString(s);

	return *this;
}

bool CLBP_UUID::operator == (const char * sz) const
{
	return sz == *this;
}

bool CLBP_UUID::operator == (const WCHAR * wsz) const
{
	return wsz == *this;
}

bool CLBP_UUID::operator == (const UUID& uuid) const
{
#ifdef LBPRHLIB
    return 0==ON_UuidCompare(this, &uuid);
#else
	return IsEqualGUID(*this, uuid) ? true : false;
#endif
}

bool CLBP_UUID::operator != (const char * sz) const
{
	return !operator==(sz);
}

bool CLBP_UUID::operator != (const WCHAR * wsz) const
{
	return !operator==(wsz);
}

bool CLBP_UUID::operator != (const UUID& uuid) const
{
	return !operator==(uuid);
}

bool operator == (const char * sz, const UUID& uuid)
{
#ifdef LBPRHLIB
    const ON_UUID u = ON_UuidFromString(sz);
    return InlineIsEqualGUID(u, uuid) ? true : false;
#else
    UUID u;
	UuidFromStringA((UCHAR*)sz, &u);
	return IsEqualGUID(u, uuid) ? true : false;
#endif
}

bool operator == (const WCHAR * wsz, const UUID& uuid)
{
#ifdef LBPRHLIB
    const ON_UUID u = ON_UuidFromString(wsz);
    return InlineIsEqualGUID(u, uuid) ? true : false;
#else
    UUID u;
	UuidFromStringW(reinterpret_cast<USHORT*>(CLBPString(wsz).AsNonConstWideString()), &u);
	return IsEqualGUID(u, uuid) ? true : false;
#endif
}

bool operator != (const char * sz, const UUID& uuid)
{
	return !(operator == (sz, uuid));
}

bool operator != (const WCHAR * wsz, const UUID& uuid)
{
	return !(operator == (wsz, uuid));
}

CLBPString CLBP_UUID::String(int numChars) const
{
#ifndef LBPRHLIB
	USHORT* pString = NULL;
	UuidToStringW((UUID*)this, &pString);

	ASSERT(wcslen((WCHAR*)pString) == ciNumChars);

	CLBPString s((WCHAR*)pString, ciNumChars);

	RpcStringFreeW(&pString);
#else
	wchar_t str[ciNumChars+1];
	ON_UuidToString((UUID&)*this, str);

	ASSERT(wcslen(str) == ciNumChars);

	CLBPString s(str, ciNumChars);
#endif

	s.MakeUpper();

	if (numChars < ciNumChars)
	{
		s = s.Left(max(0, numChars)) + L"...";
	}

	return s;
}

CLBPString CLBP_UUID::RegistryFormat(int numChars) const
{
	return L"{" + String(numChars) + L"}";
}

void CLBP_UUID::SetAsString(const char* sz)
{
	SetAsString(CLBPString(sz).Wide());
}

void CLBP_UUID::SetAsString(const WCHAR* wsz)
{
	ASSERT(NULL != wsz);
	if (NULL == wsz)
		return;

	if (*wsz == L'{')
	{
		ASSERT(wcslen(wsz) == 38);

		// Support for registry style strings
		CLBPString szNew(wsz+1, 37);
		szNew = szNew.Left(szNew.GetLength()-1);
		SetAsString(szNew.Wide());
		return;
	}

#ifdef LBPRHLIB
    *this = ON_UuidFromString(wsz);
#else
	if (UuidFromStringW((unsigned short *)wsz, this) != RPC_S_OK)
	{
		UuidCreateNil((UUID*)this);
	}
#endif
}

void CLBP_UUID::SetAsString(const CLBPString& s)
{
	SetAsString(s.AsWideString());
}

bool CLBP_UUID::CreateMashFromString(const char* sz)
{
	CLBPString s(sz);

	// Example UUID
	// {1B6FE7D6-A246-4134-A32B-88B2966C76D3} - 32 Hex characters

	// Maximum length of string is 16 - we need to hash the last bit if it's longer.
	// Hash will take up 8 hex characters which means if the string is longer than 16
	// we need to shorten it to 12.

	CLBPString sHex;

	if (s.GetLength() > 16)
	{
		const int iLen = (int)strlen(s.AsMBCSString());

		const DWORD dw = CLBP_CRC32::Calculate((const void*)(s.AsMBCSString()), iLen);

		s = s.Left(12);

		sHex = s.HexStringMBCS();

		CLBPString s2;
		s2.Format(L"%X", dw);
		sHex += s2;
	} 
	else
	if (s.GetLength() < 16)
	{
		s += L"00000000000000000";
		s = s.Left(16);
		sHex = s.HexStringMBCS();
	} 
	else 
	{
		sHex = s.HexStringMBCS();
	}

	CLBPString sUUID;
	sUUID.Format("%S-%S-%S-%S-%S", sHex.Left(8)  .Wide(),
	                               sHex.Mid(8,4) .Wide(),
	                               sHex.Mid(12,4).Wide(),
	                               sHex.Mid(16,4).Wide(),
	                               sHex.Right(12));
	sUUID.MakeUpper();
	SetAsString(sUUID);

	return true;
}

DWORD CLBP_UUID::Hash(void) const
{
	return CLBP_CRC32::Calculate(this, sizeof(UUID));
}

bool CLBP_UUID::IsValidUuid(const WCHAR* wsz) // Static.
{
	// Format is XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX.

	const int len = int(wcslen(wsz));
	if (len != ciNumChars)
		return false;

	for (int i = 0; i < len; i++)
	{
		const wchar_t c = wsz[i];

		if ((8 == i) || (13 == i) || (18 == i) || (23 == i))
		{
			if ('-' != c)
				return false;
		}
		else
		if ( ! (((c >= '0') && (c <= '9')) || ((c >= 'A') && (c <= 'F')) || ((c >= 'a') && (c <= 'f'))))
			return false;
	}

	return true;
}
