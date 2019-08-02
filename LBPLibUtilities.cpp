
#include "stdafx.h"
#include "LBPFileMgr2.h"
#include "LBPString.h"
#include "LBPLibUtilities.h"
#include "LBPRhException.h"


BOOL LBPIsDoubleEqual(double d1, double d2, int places)
{
	const double thresh = (places < 0) ? 1e-10 : (1.0 / pow(10.0, places));

	const double d3 = fabs(d1 - d2);

	if (d3 < thresh)
		return TRUE;

	return FALSE;
}

BOOL LBPIsFloatEqual(float f1, float f2)
{
	const float f3 = fabsf(f1 - f2);

	if (f3 < 1e-6)
		return TRUE;

	return FALSE;
}

#if defined LBPLIB_WINDOWS_SPECIFIC

_locale_t LBP_Locale(void)
{
	static _locale_t loc = NULL;

	if (NULL == loc)
	{
		loc = _create_locale(LC_ALL, "C");
	}

	return loc;
}

CFont* LBP_GetDialogFont(void)
{
#ifdef LBPRHLIB

	// 4th August 2017, John Croudy. Brian G says we should use this function for fonts.
	// Fixes https://mcneel.myjetbrains.com/youtrack/issue/RH-40635
	return RhinoApp().RhinoUiPaintManager().GetUiFont();

#else

	// 14th September 2015, John Croudy. Fixes RH-30857
	//
	// The problem is now obvious and I don't know why I didn't see it before.
	// To get the same font dimensions as a dialog, we have to measure MS Shell Dlg
	// and not DEFAULT_GUI_FONT, because although they are the same on English Windows,
	// they are not necessarily the same in other locales. We have to use the same
	// settings as all our dialogs:
	//
	// FONT 8,         "MS Shell Dlg", 400,      0,      0x1
	// FONT pointsize, "typeface",     weight, italic, charset
	// https://msdn.microsoft.com/en-us/library/windows/desktop/aa381013(v=vs.85).aspx
	//
	// Unfortunately this issue is far more complicated than you would think:
	// https://msdn.microsoft.com/en-us/library/windows/desktop/ff684173(v=vs.85).aspx

	static CFont font;

	if (font.GetSafeHandle() == NULL)
	{
		// http://www.winprog.org/tutorial/fonts.html
		const int pointSize = 8;
		HDC hDC = ::GetDC(NULL);
		const int logPixY = ::GetDeviceCaps(hDC, LOGPIXELSY);
		::ReleaseDC(NULL, hDC);

		LOGFONT lf = { 0 };
		lf.lfHeight = -MulDiv(pointSize, logPixY, 72);
		lf.lfWeight = 400;
		lf.lfCharSet = 0x1;
		Checked::wcsncpy_s(lf.lfFaceName, _countof(lf.lfFaceName), L"MS Shell Dlg", _TRUNCATE);
		font.CreateFontIndirect(&lf);
	}

	return &font;

#endif
}


/*	https://msdn.microsoft.com/en-us/library/windows/desktop/ms645475(v=vs.85).aspx

	For a dialog box that does not use the system font, the base units are the average width and height,
	in pixels, of the characters in the dialog's font. You can use the GetTextMetrics and GetTextExtentPoint32
	functions to calculate these values for a selected font. However, by using the MapDialogRect function, you
	can avoid errors that might result if your calculations differ from those performed by the system.
	Each horizontal base unit is equal to 4 horizontal dialog template units; each vertical base unit is equal
	to 8 vertical dialog template units.
*/

CSize LBP_MeasureFont(CFont* pFont)
{
	if (NULL == pFont)
	{
		pFont = LBP_GetDialogFont();
		if (NULL == pFont)
			return CSize(0, 0);
	}

	CDC dc;
	dc.CreateCompatibleDC(NULL);
	CFont* pOldFont = dc.SelectObject(pFont);

	//TEXTMETRIC tm = { 0 };
	//::GetTextMetrics(dc.GetSafeHdc(), &tm);

//	// Microsoft says that MapDialogRect() uses the average character width, but it doesn't.
//	// The only way I can get this to return the same width as MapDialogRect() is to add
//	// one to the average character width. How typical. "I don't know why I need to add 1".
//	return CSize(tm.tmAveCharWidth+1, tm.tmHeight);

	// 11th September 2015, John Croudy.
	//
	// https://support.microsoft.com/en-us/kb/125681
	//
	// The tmAveCharWidth field of the TEXTMETRIC structure only approximates the actual average character
	// width (usually it gives the width of the letter "x") and so the true average character width must
	// be calculated to match the value used by the system.

	// The letter "x"? Excuse me? Even Microsoft thinks it's OK to just use Roman letters for everything.
	// I knew I'd got the idea for LBP_Alphabet from somewhere. But what about Chinese fonts?
	// This is like the blind leading the blind. Welcome back, covert LBP_Alphabet. The letter "x", lol.

	CSize size;
	::GetTextExtentPoint32(dc.GetSafeHdc(), L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", 52, &size);

	dc.SelectObject(pOldFont);

	size.cx = (size.cx / 26 + 1) / 2;

	// 11th September 2015, John Croudy.
	// On English Windows, size.cy is the same as tm.tmHeight -- 13.
	// The previous code just used size.cy. $100 says that value is
	// 12 for Chinese Windows' DEFAULT_GUI_FONT (PMingLiu).
	// I can reproduce RH-30857 by setting size.cy to 12.

	// 14th September 2015, John Croudy.
	// Kelvin's test confirms that size.cy is 12 -- but so is tm.tmHeight.
	// So it doesn't matter which one you use -- which makes sense.
	//size.cy = tm.tmHeight;

	return size;
}

int LBP_DialogUnitsToPixelsX(int du, CFont* pFont)
{
#if LBPLIB_RHINOVER >= 6
	if ((NULL == pFont) && (du >= 0) && (du < 64))
	{
		static int aDU[64] = // Optimizes most common du values for default GUI font.
		{
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		};

		if (aDU[du] < 0)
		{
			const CSize size = LBP_MeasureFont(pFont);
			const int baseUnit = size.cx;
			aDU[du] = MulDiv(du, baseUnit, 4);
		}

		return aDU[du];
	}
#endif

	const CSize size = LBP_MeasureFont(pFont);
	const int baseUnit = size.cx;

	return MulDiv(du, baseUnit, 4);
}

int LBP_DialogUnitsToPixelsY(int du, CFont* pFont)
{
#if LBPLIB_RHINOVER >= 6
	if ((NULL == pFont) && (du >= 0) && (du < 64))
	{
		static int aDU[64] = // Optimizes most common du values for default GUI font.
		{
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		};

		if (aDU[du] < 0)
		{
			const CSize size = LBP_MeasureFont(pFont);
			const int baseUnit = size.cy;
			aDU[du] = MulDiv(du, baseUnit, 8);
		}

		return aDU[du];
	}
#endif

	const CSize size = LBP_MeasureFont(pFont);
	const int baseUnit = size.cy;

	return MulDiv(du, baseUnit, 8);
}
#endif


CLBPException::CLBPException(const wchar_t* sMess)
{
	m_sMessage = sMess;
}

CLBPRunTimeException::CLBPRunTimeException(const wchar_t* sMess)
	: CLBPException(sMess)
{
}

void CLBPRunTimeException::Report(void) const
{
	::MessageBox(NULL, m_sMessage.AsTString(), _T("Error"), MB_ICONSTOP);
}

static const CLBPString sProgramError = _T("**** Program Error ****\n\n");

CLBPProgramException::CLBPProgramException(const wchar_t* sMess)
	: CLBPException(sProgramError + sMess)
{
}

void CLBPRhRunTimeException::Report(void) const
{
	::RhinoMessageBox(m_sMessage, "Error", MB_ICONSTOP);

	RhinoApp().Print(m_sMessage.Wide());
}

#ifdef LBPRHLIB


int CountSelectedObjects(const CRhinoDoc& doc)
{
	CRhinoObjectIterator it(doc, CRhinoObjectIterator::normal_objects, CRhinoObjectIterator::active_and_reference_objects);

	it.EnableSelectedFilter();
	it.IncludeLights(false);

	int iCount = 0;

	CRhinoObject* pObject = it.First();
	while (pObject != NULL)
	{
		if (pObject->IsSelected())
			iCount++;

		pObject = it.Next();
	}

	return iCount;
}

#ifdef _DEBUG
void LBPRhAssert(bool bPredicate)
{
	if (bPredicate) return;
	::MessageBox(NULL, _T("LBPRhLib Assert"), _T("LBPRhLib"), MB_OK | MB_ICONEXCLAMATION);
	DebugBreak();
}
#endif

unsigned int LBPRhDocRuntimeSerialNumber(const CRhinoDoc& doc)
{
#ifdef RHINO_6_OR_LATER
	return doc.RuntimeSerialNumber(); // Rhino 6 or later.
#else
	return (unsigned int)doc.m_runtime_doc_serial_number;
#endif
}

CRhinoDoc* LBPRhDocFromRuntimeSerialNumber(unsigned int serial)
{
#ifdef RHINO_6_OR_LATER
	return CRhinoDoc::FromRuntimeSerialNumber(serial); // Rhino 6 or later.
#else
	return RhinoApp().GetDocument(serial);
#endif
}

unsigned int LBPRhObjectDocSerialNumber(const CRhinoObject* pObject)
{
	if (NULL == pObject)
		return 0;

#ifdef RHINO_6_OR_LATER
	return pObject->DocumentRuntimeSerialNumber(); // Rhino 6 or later.
#else
	const CRhinoDoc* pDoc = pObject->Document();
	return (NULL != pDoc) ? (unsigned int)pDoc->m_runtime_doc_serial_number : 0;
#endif
}

#endif

