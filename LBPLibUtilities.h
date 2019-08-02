
#pragma once

BOOL LBPIsDoubleEqual(double, double, int places=-1);
BOOL LBPIsFloatEqual(float, float);

#if defined LBPLIB_WINDOWS_SPECIFIC
_locale_t LBP_Locale(void);
inline double  LBP_wtof(const wchar_t* s)   { return _wtof_l  (s, LBP_Locale()); }
inline int     LBP_wtoi(const wchar_t* s)   { return _wtoi_l  (s, LBP_Locale()); }
#endif

#if defined LBPLIB_WINDOWS_SPECIFIC

// Measures a font's average character width and height,
// If pFont is null, it uses the Rhino UI font if LBPRHLIB is defined
// or the MS Shell Dlg font if LBPRHLIB is not defined.
CSize LBP_MeasureFont(CFont* pFont=NULL);

int LBP_DialogUnitsToPixelsX(int du, CFont* pFont=NULL);
int LBP_DialogUnitsToPixelsY(int du, CFont* pFont=NULL);

#define D2PX(x) LBP_DialogUnitsToPixelsX(x)
#define D2PY(y) LBP_DialogUnitsToPixelsY(y)

// Gets a font for use in dialogs. Matches the dialog template's FONT 8, "MS Shell Dlg", 400, 0, 0x1
CFont* LBP_GetDialogFont(void);

#endif

int CountSelectedObjects(const CRhinoDoc& doc);

#ifdef _DEBUG
void LBPRhAssert(bool bPredicate);
#else
inline void LBPRhAssert(bool) {}
#endif


unsigned int LBPRhDocRuntimeSerialNumber(const CRhinoDoc& doc);

CRhinoDoc* LBPRhDocFromRuntimeSerialNumber(unsigned int serial);

unsigned int LBPRhObjectDocSerialNumber(const CRhinoObject* pObject);
