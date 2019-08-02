
#include "stdafx.h"

#ifdef LBPRHLIB

#include "LBPRh_XMLUserData.h"
//#include "LBPRhUtilities.h"
#include "LBP_XML.h"
#include "LBP_UTF.h"
#include "ILBPRhWrapper.h"

#ifdef RHINO_V6_READY
#define ON_BOOL32 bool
#endif

ON_BOOL32 CLBPRh_XMLUserData::Read(ON_BinaryArchive& archive)
{
	// Read the version number.
	int iVersion = 0;
	if (!archive.ReadInt(&iVersion))
		return FALSE;

	// Cannot load user data with a version number greater than mine.
	if (iVersion > Version())
	{
		ReportVersionError();
		return FALSE;
	}

	// Clear any existing XML data.
	XMLRootForWrite().Clear();

	if (1 == iVersion)
	{
		// Original version.
//		LBPRH_TRACE("RHLIB: Begin version 1 (uncompressed) loading\n");

		// Read the archive into a string.
		ON_wString s;
		if (!archive.ReadString(s))
			return FALSE;

		// Read the string into the XML root.
		XMLRootForWrite().ReadFromStreamNoThrow(s);
	}
	else
	{
		// UTF8 version.
//		LBPRH_TRACE("RHLIB: Begin version 2 (UTF8) loading\n");

		// Read the length of the UTF8 buffer from the archive.
		int iLength = 0;
		if (!archive.ReadInt(&iLength))
			return FALSE;

		// Read the UTF8 buffer from the archive.
		BYTE* pBuffer = new BYTE[iLength+1];
		bool bOk = archive.ReadChar(iLength, pBuffer);
		if (bOk)
		{
			// Convert the UTF8 data to UTF16 and read it into the root node.
			CLBP_UTF u;
			pBuffer[iLength] = 0; // Terminator.
			u.SetUTF8(pBuffer);
			XMLRootForWrite().ReadFromStreamNoThrow(u.AsWide());
		}
		delete[] pBuffer;

		if (!bOk)
			return FALSE;
	}

//	LBPRH_TRACE("RHLIB: Loading complete\n");

	return TRUE;
}

ON_BOOL32 CLBPRh_XMLUserData::Write(ON_BinaryArchive& archive) const
{
//	LBPRH_TRACE("RHLIB: Begin version 2 (UTF8) saving\n");

	// Write the version number to the archive.
	if (!archive.WriteInt(Version()))
		return FALSE;

	// Convert the XML string to UTF8.
	CLBP_UTF u;
	u.SetString(XMLRootForRead().String());
	const BYTE* pUTF8 = u.UTF8();

	// Write the length of the UTF8 buffer to the archive.
	const int iLength = (int)(strlen(reinterpret_cast<const char*>(pUTF8)));
	if (!archive.WriteInt(iLength))
		return FALSE;

	// Write the UTF8 buffer to the archive.
	if (!archive.WriteChar(iLength, pUTF8))
		return FALSE;

//	LBPRH_TRACE("RHLIB: Saving complete\n");

	return TRUE;
}

#ifdef RHINO_V6_READY
#undef ON_BOOL32
#endif

#endif
