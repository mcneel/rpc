
#pragma once

#ifndef LBPRHLIB
#error RHLIB header included in non-Rhino compile
#endif

class CLBPVariant;
class CLBP_XMLNode;
class CLBP_XMLRootNode;
class CLBP_XMLProperty;

#include "ILBPRhWrapper.h"
#include "LBPRh_XMLUserData.h"
//#include "LBPRhUtilities.h"
#include "LBP_XML.h"
#include "LBP_UTF.h"

template <class BASE, class ARCHIVE>
class TLBPRh_XMLUserData : public BASE
{
public:
	TLBPRh_XMLUserData()
	{
		m_pXMLRoot = new CLBP_XMLRootNode_RC;
		m_wt = lbprh_wt_none;
	}

	TLBPRh_XMLUserData(const TLBPRh_XMLUserData<BASE, ARCHIVE>& ud) : BASE(ud)
	{
		m_pXMLRoot = new CLBP_XMLRootNode_RC;
		m_wt = lbprh_wt_none;
	}

	virtual ~TLBPRh_XMLUserData()
	{
		delete m_pXMLRoot;
	}
	
	TLBPRh_XMLUserData& operator = (const TLBPRh_XMLUserData<BASE, ARCHIVE>& ud)
	{
		BASE::operator = (ud); // CRITICAL - Be sure to call base class.
		*m_pXMLRoot = *ud.m_pXMLRoot;

		return *this;
	}

	virtual void AssignFast(const TLBPRh_XMLUserData<BASE, ARCHIVE>& ud)
	{
#ifdef USE_REFERENCE_COUNTED_ROOTNODE
		operator = (ud);
#else
		BASE::operator = (ud); // CRITICAL - Be sure to call base class.
		m_pXMLRoot->AssignFast(*ud.m_pXMLRoot);
#endif
	}

	CLBP_XMLRootNode& XMLRoot(void) const { return m_pXMLRoot->Node();	}

#ifdef USE_REFERENCE_COUNTED_ROOTNODE
	const CLBP_XMLRootNode& XMLRootForRead(void) const { return m_pXMLRoot->NodeForRead(); } // TEMP for compatibility.
	CLBP_XMLRootNode& XMLRootForWrite(void)      const { return m_pXMLRoot->NodeForWrite(); } // TEMP for compatibility.
#else
	const CLBP_XMLRootNode& XMLRootForRead(void) const { return m_pXMLRoot->Node(); } // TEMP for compatibility.
	CLBP_XMLRootNode& XMLRootForWrite(void)      const { return m_pXMLRoot->Node(); } // TEMP for compatibility.
#endif

	CLBP_XMLProperty * Property(const WCHAR* wszXMLPath, const WCHAR* wszPropertyName) const
	{
		CLBP_XMLProperty* pProp = InternalProperty(wszXMLPath, wszPropertyName);
		if (NULL == pProp)
		{
			// Failed to get the property. This probably means that the XML strings have been changed
			// and this is old user data. Reset to defaults.
			SetToDefaults();

			// *-NOTE-* This can still return NULL if the path and/or property were not set
			// as one of the defaults.
			return InternalProperty(wszXMLPath, wszPropertyName);
		}

		return pProp;
	}

	CLBPVariant Value(const WCHAR * wszXMLPath, const WCHAR * wszPropertyName = L"") const
	{
		const CLBP_XMLProperty* pProp = Property(wszXMLPath, wszPropertyName);
		if (pProp != NULL)
		{
			return pProp->GetValue();
		}

		throw CException();
	}

	void SetValue(const WCHAR * wszXMLPath, const WCHAR * wszPropertyName, const CLBPVariant& value)
	{
		CLBP_XMLProperty* pProp = Property(wszXMLPath, wszPropertyName);
		if (pProp != NULL)
		{
			pProp->SetValue(value);
		}
		else
		{
			throw CException();
		}
	}
	void SetValue(const WCHAR * wszXMLPath, const CLBPVariant& value)
	{
		SetValue(wszXMLPath, L"", value);
	}

	void Clear(void) const
	{
		m_pXMLRoot->Clear();
		m_pXMLRoot->SetTagName(L"xml");
	}

	int Version(void) const { return 2; }
	void Dump(const TCHAR* szFileName) const { m_pXMLRoot->WriteToFile(szFileName); }

protected:	
	CLBP_XMLProperty* InternalProperty(const WCHAR * wszXMLPath, const WCHAR * wszPropertyName) const
	{
		const CLBP_XMLNode* pNode = m_pXMLRoot->Node().GetNodeAtPath(wszXMLPath);
		if (NULL == pNode)
			return NULL;

		return pNode->GetNamedProperty(wszPropertyName);
	}

#ifdef RHINO_V6_READY
#define ON_BOOL32 bool
#endif

	virtual ON_BOOL32 Archive(void) const { return true; }

#ifdef RHINO_V6_READY
#undef ON_BOOL32
#endif

protected:
	virtual void SetToDefaults() const = 0;
	virtual void ReportVersionError(void) const = 0;

public:
	void SetToDefaultsImpl(eLBPRhWrapperType wt) const
	{
		m_wt = wt;
		SetToDefaults();
	}

	eLBPRhWrapperType WrapperType(void) const { return m_wt; }

private:
	CLBP_XMLRootNode_RC* m_pXMLRoot;
	mutable eLBPRhWrapperType m_wt;

	class CException : public CLBPProgramException
	{
	public:
		CException() : CLBPProgramException(L"Property not found in user data") { }

		virtual void Report(void) const
		{ 
			::RhinoMessageBox(m_sMessage, L"Error", MB_ICONSTOP);
		}
	};

	friend class CLBPRh_XMLUserData;
	friend class CLBPRhRdkContent_XMLUserData;
};

class CLBPRh_XMLUserData : public TLBPRh_XMLUserData<ON_UserData, ON_BinaryArchive>
{
	typedef TLBPRh_XMLUserData<ON_UserData, ON_BinaryArchive> _super;
public:
	CLBPRh_XMLUserData()
	{
		m_userdata_copycount = 1;
		m_userdata_uuid = ON_nil_uuid;
	}

	CLBPRh_XMLUserData(const CLBPRh_XMLUserData& ud) : TLBPRh_XMLUserData<ON_UserData, ON_BinaryArchive>(ud)
	{
		m_userdata_copycount = ud.m_userdata_copycount;
		m_userdata_uuid = ud.m_userdata_uuid;
	}

	CLBPRh_XMLUserData& operator= (const CLBPRh_XMLUserData& ud)
	{
		__super::operator = (ud); // CRITICAL - Be sure to call base class.

		m_userdata_uuid = ud.m_userdata_uuid;

		return *this;		
	}

	virtual void AssignFast(const TLBPRh_XMLUserData<ON_UserData, ON_BinaryArchive>& ud)
	{
		__super::AssignFast(ud);

		m_userdata_uuid = ud.m_userdata_uuid;
	}

	virtual bool Write(ON_BinaryArchive&) const override;
	virtual bool Read(ON_BinaryArchive&) override;
};

#ifdef RHINO_V6_READY
#undef ON_BOOL32
#endif
