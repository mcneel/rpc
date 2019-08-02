
#pragma once

#include "LBPString.h"

class CLBP_UUID : public UUID
{
public:
	CLBP_UUID(); // Initialised to nil uuid.
	CLBP_UUID(const UUID& uuid);
	CLBP_UUID(const char* sz);
	CLBP_UUID(const WCHAR* wsz);
	CLBP_UUID(const CLBPString& s);

	~CLBP_UUID() { } // No virtual functions are allowed in this class.

public: // UUID operations.
	void Create(void);
	bool IsNil(void) const;
	DWORD Hash(void) const;

	int Compare(const char* sz) const;
	int Compare(const WCHAR* wsz) const;
	int Compare(const UUID& uuid) const;

	CLBP_UUID& operator = (const char* sz);
	CLBP_UUID& operator = (const WCHAR* wsz);
	CLBP_UUID& operator = (const CLBPString& s);
	CLBP_UUID& operator = (const UUID& uuid);

	bool operator == (const char* sz)  const;
	bool operator == (const WCHAR* wsz) const;
	bool operator == (const UUID& uuid) const;

	bool operator != (const char* sz)  const;
	bool operator != (const WCHAR* wsz) const;
	bool operator != (const UUID& uuid) const;

	friend bool operator == (const char  * sz,  const UUID& uuid);
	friend bool operator == (const WCHAR * wsz, const UUID& uuid);
	friend bool operator != (const char  * sz,  const UUID& uuid);
	friend bool operator != (const WCHAR * wsz, const UUID& uuid);

public: // String operations.
	static const int ciNumChars = 36;
	CLBPString String(int numChars=ciNumChars) const;
	CLBPString RegistryFormat(int numChars=ciNumChars) const;
	operator CLBPString(void) const { return String(); }

	// Input any string. The UUID produced from the string will always be the same. Note - ASCII strings only.
	bool CreateMashFromString(const char* sz);

	// Returns true if string is a valid UUID.
	static bool IsValidUuid(const WCHAR* wsz);

protected:
	void SetAsString(const char* sz);
	void SetAsString(const WCHAR* wsz);
	void SetAsString(const CLBPString& s);
};
