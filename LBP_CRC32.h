
#pragma once

#include "LBPSimpleByteArray.h"

class CLBP_CRC32
{
public:
	CLBP_CRC32(size_t iInitialSize = 100);
	~CLBP_CRC32();

	static DWORD Calculate(const void * pBuffer, size_t iBufferNumBytes);

	void  SmartBegin(void);
	DWORD SmartEnd(void) const;

	// Strings not declared inline - slightly more complicated.
	void SmartAdd(const WCHAR * sz);
	void SmartAdd(const char * sz);

	void SetDecimalPlaces(int places);

	void SmartAdd(const BYTE* pBytes, const size_t size)
	{
		const DWORD dwBufferCRC = Calculate(pBytes, size);
		SmartAddImpl(reinterpret_cast<const BYTE*>(&dwBufferCRC), sizeof(DWORD));
	}

	void SmartAdd(const int v)      { SmartAddImpl(reinterpret_cast<const BYTE*>(&v), sizeof(v)); }
	void SmartAdd(const float v)    { const int i = Chop(v); SmartAddImpl(reinterpret_cast<const BYTE*>(&i), sizeof(i)); }
	void SmartAdd(const double v)   { const int i = Chop(v); SmartAddImpl(reinterpret_cast<const BYTE*>(&i), sizeof(i)); }
    void SmartAdd(const UUID& v)    { SmartAddImpl(reinterpret_cast<const BYTE*>(&v), sizeof(v)); }
    
#ifdef LBPRHLIB
	void SmartAdd(const ON_Interval& in);
	void SmartAdd(const ON_Xform& x);
    void SmartAdd(const ON_Color& c) { SmartAddImpl(reinterpret_cast<const BYTE*>(&c), sizeof(c)); }
#endif

protected:
	inline int Chop(float f)  { f = ((f + 1e-5f) *  float(m_dChop)) + 0.5f; return (int)f; }
	inline int Chop(double d) { d = ((d + 1e-10) * double(m_dChop)) + 0.5;  return (int)d; }

private:
	void SmartAddImpl(const BYTE* pBytes, const size_t size);

private:
	static const DWORD m_Table[256];
	CLBPSimpleByteArray* m_pArray;
	double m_dChop;
};
