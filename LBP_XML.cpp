
#include "stdafx.h"
#include "LBP_XML.h"
#include "LBPUnicodeTextFile.h"
#include "LBPColor.h"
#include "LBPException.h"
#include "LBP_UTF.h"
#include "LBPBase64.h"
#include "LBP_CRC32.h"
#include "LBPLibUtilities.h"

class CXMLCriticalSection
{
public:
	CXMLCriticalSection(CCriticalSection& cs) : m_CS(cs) { m_CS.Lock(); }
	~CXMLCriticalSection() { m_CS.Unlock(); }

private:
	CCriticalSection& m_CS;
};

bool CLBP_XMLNode::m_bAutoTypePropValue = false;

static const wchar_t* StringFromPropType(CLBPVariant::vt vt)
{
	switch (vt)
	{
	case CLBPVariant::vt_Integer:        return L"int";
	case CLBPVariant::vt_Float:          return L"float";
	case CLBPVariant::vt_Double:         return L"double";
	case CLBPVariant::vt_String:         return L"string";
	case CLBPVariant::vt_Bool:           return L"bool";
	case CLBPVariant::vt_Matrix:         return L"matrix";
	case CLBPVariant::vt_Uuid:           return L"uuid";
	case CLBPVariant::vt_Time:           return L"time";
	case CLBPVariant::vt_Buffer:         return L"buffer";
	case CLBPVariant::vt_4_double_color: return L"color";
	case CLBPVariant::vt_2_double_array: return L"2da";
	case CLBPVariant::vt_3_double_array: return L"3da";
	case CLBPVariant::vt_4_double_array: return L"4da";
	}

	return L"null";
}

static CLBPVariant::vt PropTypeFromString(const CLBPString& s)
{
	if (L"int"    == s) return CLBPVariant::vt_Integer;
	if (L"float"  == s) return CLBPVariant::vt_Float;
	if (L"double" == s) return CLBPVariant::vt_Double;
	if (L"string" == s) return CLBPVariant::vt_String;
	if (L"bool"   == s) return CLBPVariant::vt_Bool;
	if (L"matrix" == s) return CLBPVariant::vt_Matrix;
	if (L"uuid"   == s) return CLBPVariant::vt_Uuid;
	if (L"time"   == s) return CLBPVariant::vt_Time;
	if (L"buffer" == s) return CLBPVariant::vt_Buffer;
	if (L"color"  == s) return CLBPVariant::vt_4_double_color;
	if (L"2da"    == s) return CLBPVariant::vt_2_double_array;
	if (L"3da"    == s) return CLBPVariant::vt_3_double_array;
	if (L"4da"    == s) return CLBPVariant::vt_4_double_array;

	return CLBPVariant::vt_Null;
}

REFCOUNT_INT g_lNodeCount = 0;
int LBP_GetNodeCount() { return g_lNodeCount; }

REFCOUNT_INT g_lPropertyCount = 0;
int LBP_GetPropertyCount() { return g_lPropertyCount; }

#ifdef _DEBUG
	#define LBPXML_PROFILER_START XMLProfiler.StartWatch()
	#define LBPXML_PROFILER_STOP XMLProfiler.StopWatch()
#else
	#define LBPXML_PROFILER_START
	#define LBPXML_PROFILER_STOP
#endif

int g_iWarningsFlagCounter = 0;
bool ThrowWarningExceptions()
{
	return g_iWarningsFlagCounter > 0;
}

void AutoTypeVariant(CLBPVariant& v);

CLBP_XMLNode::CLBP_XMLNode(const CLBPString& sName)
{
	::InterlockedIncrement(&g_lNodeCount);

	SetTagName(sName);

	AddEmptyDefaultProperty();
}

CLBP_XMLNode::CLBP_XMLNode(const CLBP_XMLNode& src)
{
	::InterlockedIncrement(&g_lNodeCount);

	*this = src;
}

CLBP_XMLNode::~CLBP_XMLNode()
{
	InternalRemoveAllProperties();
	RemoveAllChildren();

	::InterlockedDecrement(&g_lNodeCount);
}

CLBP_XMLNode& CLBP_XMLNode::operator=(const CLBP_XMLNode& src)
{
	CXMLCriticalSection cs1(m_CS);
	CXMLCriticalSection cs2(src.m_CS);

	InternalRemoveAllProperties();
	RemoveAllChildren();

	m_sName = src.m_sName;

	// Copy in the properties.
	CLBP_XMLProperty* pProperty = NULL;
	CPropertyIterator pi = src.GetPropertyIterator();
	while (NULL != (pProperty = pi.GetNextProperty()))
	{
		InternalAddProperty(*pProperty); // This does a copy anyway - no need to call the copy constructor
	}

	// Copy in the children.
	CLBP_XMLNode* pChild = NULL;
	CChildIterator ci = src.GetChildIterator();

	while (NULL != (pChild = ci.GetNextChild()))
	{
		AddChildNode(new CLBP_XMLNode(*pChild));
	}

	return *this;
}

void CLBP_XMLNode::AssignFast(const CLBP_XMLNode& src)
{
	*this = src;
}

bool CLBP_XMLNode::MergeFrom(const CLBP_XMLNode& src)
{
	CXMLCriticalSection cs1(m_CS);
	CXMLCriticalSection cs2(src.m_CS);

	if (m_sName.operator != (src.TagName()))
		return false;

	//Copy in the parameters
	CLBP_XMLProperty* pProperty = NULL;

	CPropertyIterator pi = src.GetPropertyIterator();
	while(NULL != (pProperty = pi.GetNextProperty()))
	{
		//Replaces any that are already there.
		AddProperty(pProperty);
	}

	//Copy in the children
	CLBP_XMLNode* pChild = NULL;
	CChildIterator ci = src.GetChildIterator();

	BOOL bNeedToMerge = ChildrenCount() != 0;

	while (NULL != (pChild = ci.GetNextChild()))
	{
		CLBPString sName = pChild->TagName();
		CLBP_XMLNode* pLocalChildNode = bNeedToMerge ? GetNodeAtPath(pChild->TagName()) : NULL;

		if (pLocalChildNode)
		{
			pLocalChildNode->MergeFrom(*pChild);
		}
		else
		{
			AddChildNode(new CLBP_XMLNode(*pChild));
		}
	}

	return true;
}

void CLBP_XMLNode::Clear(void)
{
	SetTagName(L"");
	RemoveAllProperties();
	RemoveAllChildren();
}

void CLBP_XMLNode::AddEmptyDefaultProperty(void)
{
	static CLBP_XMLProperty empty_prop(L"");
	AddProperty(&empty_prop);
}

void CLBP_XMLNode::RemoveAllChildren(void)
{
	CXMLCriticalSection cs(m_CS);

	if (NULL != m_pFirstChild)
	{
		CChildIterator it(this);
		CLBP_XMLNode* pNode = NULL;
		do
		{
			pNode = it.GetNextChild();
			delete pNode;
		}
		while (NULL != pNode);

		m_pFirstChild = NULL;
		m_pLastChild = NULL;
	}
}

void CLBP_XMLNode::InternalRemoveAllProperties(void)
{
	if (NULL != m_pFirstProperty)
	{
		CLBP_XMLProperty* pProp = NULL;
		CPropertyIterator pit(this);
		do
		{
			pProp = pit.GetNextProperty();
			delete pProp;
		}
		while (NULL != pProp);

		m_pFirstProperty = NULL;
	}
}

CLBP_XMLNode* CLBP_XMLNode::InternalUnhookChild(CLBP_XMLNode& childNode)
{
	CLBP_XMLNode* pChildNode = &childNode;

	if (pChildNode->m_pParent != this)
		return NULL;

	if (m_pFirstChild == pChildNode)
	{
		if (m_pLastChild == m_pFirstChild)
		{
			m_pLastChild = pChildNode->m_pNextSibling;
		}

		m_pFirstChild = pChildNode->m_pNextSibling;

		return pChildNode;
	} 
	else 
	{
		//First the child which points to this one
		CLBP_XMLNode* pNode = m_pFirstChild;
		while (NULL != pNode)
		{
			if (pNode->m_pNextSibling == pChildNode)
			{
				pNode->m_pNextSibling = pChildNode->m_pNextSibling;

				if (NULL == pNode->m_pNextSibling)
				{
					m_pLastChild = pNode;
				}

				return pChildNode;
			}

			pNode = pNode->m_pNextSibling;
		}
	}

	return NULL;
}

void CLBP_XMLNode::RemoveAllProperties(void)
{
	CXMLCriticalSection cs(m_CS);

	InternalRemoveAllProperties();
	AddEmptyDefaultProperty();
}

const char* CLBP_XMLNode::TagNameMBCS(void) const
{
	CXMLCriticalSection cs(m_CS);

	return m_sName.AsMBCSString();
}

const WCHAR* CLBP_XMLNode::TagName(void) const
{
	CXMLCriticalSection cs(m_CS);

	return m_sName.AsWideString();
}

void CLBP_XMLNode::SetTagName(const CLBPString& sName)
{
	CXMLCriticalSection cs(m_CS);

	ASSERT(!ReadOnly());

	if (sName.IsEmpty())
	{
		m_sName.Empty();
	}
	else
	{
		CLBPString& s = m_sName.EmptyStringForWrite();
		s = sName;
		s.TrimLeft();
		s.TrimRight();
	}
}

bool CLBP_XMLNode::IsValidXMLName(const CLBPString& sTagName) // Static.
{
	const int iLength = sTagName.GetLength();
	for (int i = 0; i < iLength; i++)
	{
		const wchar_t wc = towlower(sTagName[i]);

		if ((wc >= L'a') && (wc <= L'z'))
			continue;

		if (wc == L'_')
			continue;

		if ((wc >= L'0') && (wc <= L'9'))
				continue;

		if (i > 0)
		{
			if (wc == L'-')
				continue;
		}

		OutputDebugString(_T("Invalid XML tag syntax - "));
		OutputDebugString(sTagName.T());
		OutputDebugString(_T("\n"));

		ASSERT(false);

		return false;
	}

	return true;
}

CLBP_XMLNode* CLBP_XMLNode::GetParent(void) const
{
	return m_pParent;
}

const CLBP_XMLNode& CLBP_XMLNode::TopmostParent(void) const
{
	CXMLCriticalSection cs(m_CS);

	const CLBP_XMLNode* p = this;
	while (p->m_pParent)
	{
		p = p->m_pParent;
	}

	return *p;
}

CLBP_XMLNode* CLBP_XMLNode::AddChildNode(CLBP_XMLNode* pNode)
{
	if (NULL == pNode)
		return NULL;

	CXMLCriticalSection cs(m_CS);

	ASSERT(!ReadOnly());

	if (NULL == m_pFirstChild)
	{
		//There are no children - just add one
		pNode->m_pNextSibling = NULL;
		m_pFirstChild = pNode;
		m_pLastChild = pNode;

	}
	else
	{
		//There are children - add one to the end
		ASSERT(m_pLastChild != NULL);
		m_pLastChild->m_pNextSibling = pNode;
		pNode->m_pNextSibling = NULL;
		m_pLastChild = pNode;
	}

	pNode->m_pParent = this;

	return pNode;
}

CLBP_XMLProperty* CLBP_XMLNode::AddProperty(const CLBP_XMLProperty* pProp)
{
	if (NULL == pProp)
		return NULL;

	return AddProperty(*pProp);
}

CLBP_XMLProperty* CLBP_XMLNode::AddProperty(const CLBP_XMLProperty& prop)
{
	CXMLCriticalSection cs(m_CS);

	ASSERT(!ReadOnly());

	if (ThrowWarningExceptions())
	{
		const CLBPString sName = prop.Name();
		if (sName.Contains(L"\n") || sName.Contains(L"\r"))
		{
			// The string contain LF or CR codes - this is likely to cause problems but
			// is still valid XML.
			throw CLBP_XMLException(CLBP_XMLException::eBadCharactersInString, sName);
		}

		const CLBPString sValue = prop.GetValue();
		if (sValue.Contains(L"\n") || sValue.Contains(L"\r"))
		{
			// The string contain LF or CR codes - this is likely to cause problems but
			// is still valid XML.
			throw CLBP_XMLException(CLBP_XMLException::eBadCharactersInString, sValue);
		}
	}

	InternalRemoveProperty(prop.Name());

	// Copy the property, then add it to the tree
	return InternalAddProperty(prop);
}

CLBP_XMLProperty* CLBP_XMLNode::AttachProperty(CLBP_XMLProperty* pProp)
{
	if (NULL == pProp)
		return NULL;

	CXMLCriticalSection cs(m_CS);

	ASSERT(!ReadOnly());

	InternalRemoveProperty(pProp->Name());

	pProp->m_pNextProperty = m_pFirstProperty;
	m_pFirstProperty = pProp;
	m_pFirstProperty->m_pOwner = this;

	return pProp;
}

bool CLBP_XMLNode::RemoveProperty(const CLBPString& sPropertyName)
{
	CXMLCriticalSection cs(m_CS);

	return InternalRemoveProperty(sPropertyName);
}

CLBP_XMLProperty* CLBP_XMLNode::InternalAddProperty(const CLBP_XMLProperty& prop)
{
	CLBP_XMLProperty* pProp = new CLBP_XMLProperty(prop);
	pProp->m_pOwner = this;
	pProp->m_pNextProperty = m_pFirstProperty;
	m_pFirstProperty = pProp;

	return pProp;
}

bool CLBP_XMLNode::InternalRemoveProperty(const CLBPString& sName)
{
	CLBP_XMLProperty* pPrevious = NULL;
	CLBP_XMLProperty* pProp = m_pFirstProperty;

	while (NULL != pProp)
	{
		if (pProp->Name().CompareNoCase(sName.AsWideString()) == 0)
		{
			if (NULL == pPrevious)
			{
				m_pFirstProperty = pProp->m_pNextProperty;
			}
			else
			{
				pPrevious->m_pNextProperty = pProp->m_pNextProperty;
			}

			delete pProp;
			return true;
		}

		pPrevious = pProp;
		pProp = pProp->m_pNextProperty;
	}

	return false;
}

void CLBP_XMLNode::Remove(void)
{
	CLBP_XMLNode* pParent = GetParent();
	if (NULL != pParent)
	{
		pParent->RemoveChild(this);
	}
	else
	{
		delete this;
	}
}

CLBP_XMLNode* CLBP_XMLNode::UnhookChild(CLBP_XMLNode* pChildNode)
{
	if (NULL == pChildNode)
		return NULL;

	CXMLCriticalSection cs(m_CS);

	ASSERT(!ReadOnly());

	return InternalUnhookChild(*pChildNode);
}

bool CLBP_XMLNode::RemoveChild(CLBP_XMLNode* pChildNode)
{
	if (NULL == pChildNode)
		return NULL;

	CXMLCriticalSection cs(m_CS);

	ASSERT(!ReadOnly());

	CLBP_XMLNode* pChild = InternalUnhookChild(*pChildNode);
	delete pChild;

	return pChild != NULL;
}

CLBP_XMLNode::CChildIterator CLBP_XMLNode::GetChildIterator() const
{
	return CChildIterator(this);
}

CLBP_XMLNode::CPropertyIterator CLBP_XMLNode::GetPropertyIterator(bool bAlphabetized) const
{
	return CPropertyIterator(this, bAlphabetized);
}

DWORD CLBP_XMLNode::PropertyCount() const
{
	CXMLCriticalSection cs(m_CS);

	CPropertyIterator it = GetPropertyIterator();

	DWORD iCount = 0;
	while (it.GetNextProperty())
	{
		iCount++;
	}

	return iCount;
}

DWORD CLBP_XMLNode::ChildrenCount() const
{
	CXMLCriticalSection cs(m_CS);

	CChildIterator it = GetChildIterator();

	DWORD iCount = 0;
	while (it.GetNextChild())
	{
		iCount++;
	}

	return iCount;
}

bool CLBP_XMLNode::HasDefaultProperty() const
{
	return true; // !!GetNamedProperty(L"");
}

int CLBP_XMLNode::GetNestedDepth() const
{
	CXMLCriticalSection cs(m_CS);

	int iDepth = 0;

	const CLBP_XMLNode* pNode = this;
	while (NULL != pNode->m_pParent)
	{
		pNode = pNode->m_pParent;
		iDepth++;
	}

	return iDepth;
}

static bool PrependNodeToStringAndRecurseParents(const CLBP_XMLNode* pNode, CLBPString& s)
{
	// Recursive function to pile up the path.

	if (NULL == pNode)
		return false;

	CLBP_XMLNode* pParent = pNode->GetParent();
	if (NULL != pParent)
	{
		//CLBPString sNodeName = pNode->TagName();
		//s = sNodeName + L"/" + s;
		s.Insert(0, L'/', 1);
		s.Insert(0, pNode->TagName());

		PrependNodeToStringAndRecurseParents(pParent, s);

		return true;
	}

	return false;
}

CLBPStringBV CLBP_XMLNode::GetPathFromRoot() const
{
	CXMLCriticalSection cs(m_CS);

	CLBPStringBV sPath(TagName());
	PrependNodeToStringAndRecurseParents(GetParent(), sPath);

	return sPath;
}

CLBP_XMLProperty& CLBP_XMLNode::GetDefaultProperty(void) const
{
	static CLBPString empty(L"");
	CLBP_XMLProperty* pProp = GetNamedProperty(empty);
	ASSERT(NULL != pProp); // Always created by constructor.

	return *pProp;
}

CLBP_XMLProperty* CLBP_XMLNode::GetNamedProperty(const wchar_t* wszName) const
{
	CXMLCriticalSection cs(m_CS);

	CPropertyIterator it = GetPropertyIterator();
	CLBP_XMLProperty* pProp = NULL;

	while (NULL != (pProp = it.GetNextProperty()))
	{
		if (_wcsicmp(wszName, pProp->Name()) == 0)
			return pProp;
	}

	return NULL;
}

CLBP_XMLNode* CLBP_XMLNode::GetNamedChild(const wchar_t* wszName) const
{
	CXMLCriticalSection cs(m_CS);

	CChildIterator it = GetChildIterator();

	CLBP_XMLNode* pNode = NULL;
	while (NULL != (pNode = it.GetNextChild()))
	{
		if (_wcsicmp(wszName, pNode->TagName()) == 0)
			return pNode;
	}

	return NULL;
}

CLBP_XMLNode* CLBP_XMLNode::GetNodeAtPathImpl(const wchar_t* wszPath, bool bCreate)
{
	//Forward slash "/" is the separator

	if (NULL == wszPath)
		return const_cast<CLBP_XMLNode*>(this);

	const wchar_t* p = wszPath;
	while (*p != 0)
	{
		if (!iswspace(*p) && *p!=L'/') 
			break;
		p++;
	}
	
	//The input string was empty
	if (*p == 0)
		return const_cast<CLBP_XMLNode*>(this);

	WCHAR wsz[260];
	wcsncpy(wsz, p, 260);

	//Now right trim out the whitespace
	const int iLength = (int)wcslen(wsz);

	for (int i = iLength - 1; i >= 0; i--)
	{
		if (iswspace(wsz[i]))
			wsz[i] = 0;
		else
			break;
	}

	//Check for a resultant empty string
	if (*wsz == 0)
		return const_cast<CLBP_XMLNode*>(this);

	const wchar_t* pstr = wcschr(wsz, L'/');
	const int iPos = pstr != NULL ? (int)(pstr-wsz) : -1;

	const wchar_t* pNext = NULL;

	if (-1 != iPos)
	{
		//sNext is the rest of the string that we're going to resurse through
		pNext = wsz + iPos + 1;
		wsz[iPos] = 0;
	}

	CChildIterator it = GetChildIterator();

	CLBP_XMLNode* pChild = NULL;
	while (NULL != (pChild = it.GetNextChild()))
	{
		if (_wcsicmp(wsz, pChild->TagName()) == 0)
		{
			return pChild->GetNodeAtPathImpl(pNext, bCreate);
		}
	}

	// The child was not found.

	if (bCreate)
	{
		return AddChildNode(new CLBP_XMLNode(wsz))->GetNodeAtPathImpl(pNext, bCreate);
	}

	return NULL;
}

LPVOID CLBP_XMLNode::LastReadBufferPointer() const
{
	return m_pLastReadBufferPointer;
}

void CLBP_XMLNode::SetLastReadBufferPointer(LPVOID p)
{
	m_pLastReadBufferPointer = p;
}

DWORD CLBP_XMLNode::ReadFromStreamNoThrow(const WCHAR* wszStream, bool bWarningsAsErrors, bool bValidateTags)
{
	// Added a simple check for an empty input stream to stop this thing from outputting first-chance exceptions
	// to the output line in normal operation and freaking out Dale.
	if (NULL == wszStream || 0 == *wszStream)
	{
		return LBP_XML_READ_ERROR;
	}

	try
	{
		return ReadFromStreamThrow(wszStream, bWarningsAsErrors, bValidateTags);
	}
	catch(...)
	{
		return LBP_XML_READ_ERROR;
	}
}

DWORD CLBP_XMLNode::ReadFromStreamThrow(const WCHAR* wszStream, bool bWarningsAsErrors, bool bValidateTags)
{
	if(bWarningsAsErrors) g_iWarningsFlagCounter++;

	Clear();
	//Search for the opening tag "<" character
	WCHAR* pBuffer = const_cast<WCHAR*>(wszStream);

	CLBPString tag;
	GetNextTag(tag, pBuffer, bValidateTags);

	SetLastReadBufferPointer((LPVOID)wszStream);

	if (!tag.IsEmpty())
	{
		GetPropertiesFromTag(tag);
	}

	if (tag.GetAtWide(tag.GetLength()-2) != L'/')
	{
		//This tag either has children, or a default property
		CLBPString sDefaultProperty;

		BOOL bClosingTag = FALSE;
		do
		{
			WCHAR* pStoreBuffer = pBuffer;

			//Get any crap between the tags and store as the default parameter

			WCHAR* pStart = pBuffer;

			while (*pStart != L'<' && *pStart != 0) pStart++;

			//These two lines account for 5% of the time
			//CLBPString s(pBuffer, (int)(pStart-pBuffer));

			//We ran off the end of the buffer - no idea why, but this is bad, so throw an exception
			//See http://mcneel.myjetbrains.com/youtrack/issue/RH-16605 for example
			if (0 == *pStart)
			{
				throw CLBP_XMLException(CLBP_XMLException::eMalformedTagOnRead, tag);
			}
			
			ASSERT(*pStart==L'<');

			if (NULL!=pStart && *pStart==L'<')
			{
				sDefaultProperty.Append(pBuffer, (int)(pStart-pBuffer));
			}

			pBuffer = pStart;
			GetNextTag(tag, pBuffer, bValidateTags);

			bClosingTag = IsClosingTag(tag);

			if (!bClosingTag)
			{
				CLBP_XMLNode* pNode = new CLBP_XMLNode(L"");
				AddChildNode(pNode);
				pBuffer = pStoreBuffer + pNode->ReadFromStreamThrow(pStoreBuffer, bWarningsAsErrors, bValidateTags);
			}
		}
		while (!bClosingTag);

		sDefaultProperty.TrimLeft();
		sDefaultProperty.TrimRight();
		sDefaultProperty.URLUnencode();

		if (!sDefaultProperty.IsEmpty())
		{
			CLBP_XMLProperty* pProp = new CLBP_XMLProperty;
			AttachProperty(pProp);

			const int pos = m_bAutoTypePropValue ? sDefaultProperty.Find(L":") : -1;
			if (pos > 0)
			{
				// The type is encoded in the value.
				const CLBPVariant::vt type = PropTypeFromString(sDefaultProperty.Left(pos));
				CLBPVariant& v = pProp->GetNonConstValue();
				v = sDefaultProperty.Mid(pos+1);
				v.SetTypePendingFlag(true);
				v.DoAutoTyping(type);
			}
			else
			{
				const int iLength = sDefaultProperty.GetLength();
				pProp->SetHugeStringValue(sDefaultProperty.ReleaseOwnership(), iLength);
				pProp->GetNonConstValue().SetTypePendingFlag(true);
			}
		}
	}

	if (bWarningsAsErrors) g_iWarningsFlagCounter--;

	TopmostParent().OnNodeReadFromStream(this);

	return (DWORD)(pBuffer-wszStream);
}

bool CLBP_XMLNode::GetPropertiesFromTag(const CLBPString& sTag)
{
	SetTagName(GetNameFromTag(sTag));

	CLBPString tag(sTag);
	tag.TrimLeft(L"</ ");
	tag.TrimRight(L">/ ");

	int iPos = tag.Find(L' ');
	if (iPos == -1)
		return true;	//No properties

	tag.TruncateMid(iPos+1);

	CLBPString sPropertyName, sPropertyValue;
	CLBPVariant vValue;

	//We are now at the start of the properties

	bool bClear = false;

	while (!tag.IsEmpty())
	{
		iPos = tag.Find(L'=');
		if (iPos != -1)
		{
			sPropertyName = tag.Left(iPos);
			sPropertyName.TrimLeft();
			
			//tag = tag.Mid(iPos);
			tag.TruncateMid(iPos);

			iPos = tag.Find(L'\"');

			if (-1 != iPos)
			{
				//0.061 - this bit
				tag.TruncateMid(iPos+1);
				iPos = tag.Find(L'\"');
				//

				if (-1 != iPos)
				{
					sPropertyValue = tag.Left(iPos);
					tag.TruncateMid(iPos+1);
					tag.TrimLeft();
					sPropertyValue.URLUnencode();

					const int pos = m_bAutoTypePropValue ? sPropertyValue.Find(L':') : -1;
					if (pos > 0)
					{
						// The type is encoded in the value.
						const CLBPVariant::vt type = PropTypeFromString(sPropertyValue.Left(pos));
						vValue = sPropertyValue.Mid(pos+1);
						vValue.SetTypePendingFlag(true);
						vValue.DoAutoTyping(type);
					}
					else
					{
						// This allows for lazy typing of the property
						vValue.SetTypePendingFlag(true);
						vValue = sPropertyValue;
//						AutoTypeVariant(vValue);            John commented this out because it made some of his strings turn into ints. But is this right?
					}

					AttachProperty(new CLBP_XMLProperty(sPropertyName, vValue));

					bClear = true;
				}
			}
		}

		if (!bClear)
		{
			throw CLBP_XMLException(CLBP_XMLException::eMalformedTagOnRead, sTag);
		}

		bClear = false;

		tag.TrimLeft();
	}

	return true;
}

bool CLBP_XMLNode::IsClosingTag(const CLBPString& sTag) const
{
	if (sTag.GetAtWide(1) != L'/')
		return false;

	if (GetNameFromTag(sTag).CompareNoCase(TagName()) == 0)
		return true;

	return false;
}

CLBPStringBV CLBP_XMLNode::GetNameFromTag(const CLBPString& sTag) // Static.
{
	CLBPStringBV tag(sTag);
	tag.TrimLeft(L"</ ");
	tag.TrimRight(L">/ ");

	const int iPos = tag.Find(L' ');
	if (iPos != -1)
	{
		tag.Truncate(iPos);
	}

	return tag;
}

bool CLBP_XMLNode::RecoverProperty(const CLBPString& tag, int iEqualSign, CLBPString& sProp) // Static.
{
	//Move left, looking for a space and ensuring every character is a valid name char
	ASSERT(tag.GetAtWide(iEqualSign) == L'=');

	int iLeftScan = iEqualSign-1;

	CLBPString sName;
	while(iLeftScan > 0 && tag.GetAtWide(iLeftScan)!=L' ')
	{
		sName = tag.GetAtWide(iLeftScan--) + sName;
	}

	if (!IsValidXMLName(sName) || sName.IsEmpty())
	{
		return false;
	}

	const int iStart = iEqualSign + 2;

	//Now try to recover the value
	if(tag.GetLength() <= iStart)
	{
		return false;
	}

	if(tag.GetAtWide(iEqualSign+1) != L'\"')
		return false;

	const int iSecondQuote = tag.Find(L'\"', iStart);
	if(iSecondQuote == -1)
		return false;

	CLBPString sValue = tag.Mid(iStart, iSecondQuote - iStart);

	sValue.URLEncode();

	sProp = sName + L"=\"" + sValue + L"\"";

	return true;
}

void CLBP_XMLNode::AttemptToFixTag(CLBPString& tag) // Static.
{
	// We're going to rebuild the tag from the name and the number of valid properties we find.
	const CLBPString sName = GetNameFromTag(tag);
	if (sName.IsEmpty() || tag.GetLength() < 2)
		return;

	const bool bSelfClosingTag = (0 == wcscmp(tag.Right(2), L"/>"));

	CLBPString sNewTag = L"<" + sName;

	// Now find all of the potential properties - looking for = signs.
	int iPos = 0;
	int iEqualSign = -1;
	CLBPString sProp;
	
	while (-1 != (iEqualSign = tag.Find(L'=', iPos)))
	{
		// Move the search past this one for next time around.
		iPos = iEqualSign + 1;

		if (RecoverProperty(tag, iEqualSign, sProp))
		{
			sNewTag += L" ";
			sNewTag += sProp;
		}
	}

	if (bSelfClosingTag)
	{
		sNewTag += L"/>";
	}
	else
	{
		sNewTag += L">";
	}

	tag = sNewTag;
}

void CLBP_XMLNode::GetNextTag(CLBPString& tag, WCHAR*& pBuffer, bool bValidateTag) // Static.
{
	WCHAR* pStart = pBuffer;
	while (*pStart != L'<')
	{
		if (0 == *pStart)
		{
			CLBP_XMLException e(CLBP_XMLException::eTagNotFound, pBuffer);
			throw(e);
		}
		pStart++;
	}

	while (pStart[1] == L'?')
	{
		// This is a nasty document description tag - need to look for the end tag and skip it.
		while (!(pStart[0]==L'?' && pStart[1]==L'>'))
		{
			if (0 == *pStart)
			{
				CLBP_XMLException e(CLBP_XMLException::eEndTagNotFound, pBuffer);
				throw(e);
			}

			pStart++;
		}

		while (*pStart != L'<') pStart++;
	}

	while (pStart[1] == L'!' && pStart[2] == L'-' && pStart[3] == L'-')
	{
		// This is a comment tag - need to look for the end tag and skip it.
		while (!(pStart[0]==L'-' && pStart[1]==L'-' && pStart[2]==L'>'))
		{
			if (0 == *pStart)
			{
				CLBP_XMLException e(CLBP_XMLException::eEndTagNotFound, pBuffer);
				throw(e);
			}

			pStart++;
		}

		while (*pStart != L'<') pStart++;
	}

	WCHAR* pEnd = pStart;
	while (*pEnd != L'>')
	{
		if (0 == *pEnd)
		{
			CLBP_XMLException e(CLBP_XMLException::eEndTagNotFound, pBuffer);
			throw(e);
		}
		pEnd++;
	}

	pBuffer = pEnd + 1;

	// Copy the tag into the ready-made string.
	const DWORD dwCharacters = (DWORD)(pEnd - pStart + 1);

	tag.Set(pStart, dwCharacters);

	if (bValidateTag)
	{
		try
		{
			AssertValidTag(tag);
		}
		catch (CLBP_XMLException&)
		{
			AttemptToFixTag(tag);
			AssertValidTag(tag);
		}
	}
}

void CLBP_XMLNode::AssertValidTag(const CLBPString& tag) // Static.
{
	//Check for an even number of quotes - odd means there are quotes in the strings

	const int iQuoteCount = tag.Count(L'\"');

	if ((iQuoteCount % 2) != 0)
	{
		//Odd number of quotes - boom!
		CLBP_XMLException e(CLBP_XMLException::eMalformedTagOnRead, tag);
		throw(e);
	}

	int iGTCount = tag.Count(L'<');
	int iLTCount = tag.Count(L'>');

	if (iGTCount != iLTCount)
	{
		//Bad tag format - even on nested tags, < and > should be equal
		CLBP_XMLException e(CLBP_XMLException::eMalformedTagOnRead, tag);
		throw(e);
	}

	//Check for lone ampersands
	int iPos = 0;
	int iFind = 0;
	do
	{
		iFind = tag.Find(L'&', iPos);
		if (iFind != -1)
		{
			iPos = iFind + 1;
			const int iSemicolon = tag.Find(L';', iFind);
			if (-1 == iSemicolon)
			{
				//Unterminated escape sequence
				throw CLBP_XMLException(CLBP_XMLException::eMalformedTagOnRead, tag);
			}

			CLBPString sEsc = tag.Mid(iPos-1, iSemicolon-iPos+2);
			if (!sEsc.IsURLEscapeSequence())
			{
				throw CLBP_XMLException(CLBP_XMLException::eMalformedTagOnRead, tag);
			}
		}

	}
	while (iFind != -1);
}

DWORD CLBP_XMLNode::WriteHeaderToStream(WCHAR* wszStream, DWORD cchMax, bool bIncludeFormatting, bool bForceLongFormat, bool bSortedProperties) const
{
	CXMLCriticalSection cs(m_CS);

	DWORD dwCharactersWritten = 0;
	const bool bWrite = (cchMax != 0);

	//Form the tag in a string

	CLBPString tag = L"<";
	tag += m_sName;

	//Start the count
	size_t iTagLength = tag.GetLength();

	if (bIncludeFormatting)
	{
		const int iDepth = GetNestedDepth();
		if (iDepth)
		{
			tag.Insert(0, L'\t', iDepth);
			iTagLength += iDepth;
		}
	}

	CPropertyIterator it = GetPropertyIterator(bSortedProperties);
	CLBP_XMLProperty* pProp = NULL;
	CLBPString sDefaultProperty;
	const CLBPString* psDefaultProperty = NULL;

	while (NULL != (pProp=it.GetNextProperty()))
	{
		if (pProp->IsDefaultProperty())
		{
			const auto& vDP = pProp->GetValue();

			if (vDP.NeedsURLEncode())
			{
				sDefaultProperty = vDP.AsString();
				sDefaultProperty.URLEncode();

				if (!sDefaultProperty.IsEmpty())
				{
					psDefaultProperty = &sDefaultProperty;
				}
			}
			else
			{
				const CLBPString& sDP = vDP.AsString();
				if (!sDP.IsEmpty())
				{
					psDefaultProperty = &sDP;
				}
			}
			continue;
		}

		if (bWrite)
		{
			tag += L' ';
			tag += pProp->Name();
			tag += L'=';
			tag += L'\"';
		}

		iTagLength += 3;
		iTagLength += pProp->Name().GetLength();

		const auto& vValue = pProp->GetValue();

		if (m_bAutoTypePropValue)
		{
			const CLBPString sType = StringFromPropType(pProp->GetValue().Type());
			if (bWrite) tag += sType + L":";
			iTagLength += sType.GetLength() + 1; // 1 for colon.
		}

		if (!vValue.NeedsURLEncode())
		{
			const auto& sValue = vValue.AsString();
			if (bWrite) tag += sValue;
			iTagLength += sValue.GetLength();
		}
		else
		{
			CLBPString sValueCopy = vValue.AsString();
			sValueCopy.URLEncode();
			if (bWrite) tag += sValueCopy;
			iTagLength += sValueCopy.GetLength();
		}

		if (bWrite) tag += L'\"'; // John added if (bWrite)
		iTagLength++;
	}

	if (NULL != psDefaultProperty || ChildrenCount() || bForceLongFormat)
	{
		const bool bType = m_bAutoTypePropValue && (NULL != psDefaultProperty);

		CLBPString sType;
		if (bType)
			sType = StringFromPropType(GetDefaultProperty().GetValue().Type());

		if (bWrite)
		{
			tag += L'>';
			if (bType) tag += sType + L':';

			if (psDefaultProperty)
				tag += *psDefaultProperty; // which might be nothing
		}

		iTagLength++;

		if (bType) iTagLength += sType.GetLength() + 1; // 1 for colon.

		iTagLength += (NULL == psDefaultProperty) ? 0 : psDefaultProperty->GetLength();

		if (ChildrenCount() && bIncludeFormatting)
		{
			if (bWrite)
			{
				tag += L'\r';
				tag += L'\n';
			}

			iTagLength += 2;
		}
	}

	// Write the tag itself + the default property
	if (bWrite)
	{
		const int iCopy = min((int)cchMax, (tag.GetLength() + 1)) * sizeof(WCHAR);
		memcpy(wszStream, tag.AsWideString(), iCopy);
	}

	dwCharactersWritten += (DWORD)iTagLength;

	ASSERT(!bWrite || (iTagLength == (size_t)tag.GetLength()));

	return dwCharactersWritten;
}

DWORD CLBP_XMLNode::WriteChildrenToStream(WCHAR* wszStream, DWORD cchMax, bool bIncludeFormatting, bool bForceLongFormat, bool bSortedProperties) const
{
	DWORD dwCharactersWritten = 0;
	bool bWrite = (0 != cchMax);

	CChildIterator it = GetChildIterator();
	CLBP_XMLNode* pChild = NULL;

	WCHAR* pStart = wszStream;

	while(NULL != (pChild = it.GetNextChild()))
	{
		const DWORD dw = pChild->WriteToStream(pStart, bWrite ? cchMax-dwCharactersWritten : 0, bIncludeFormatting, bForceLongFormat, bSortedProperties);
		dwCharactersWritten+=dw;
		pStart+=dw;
		bWrite = cchMax > dwCharactersWritten;
	}

	return dwCharactersWritten;
}

DWORD CLBP_XMLNode::WriteFooterToStream(WCHAR* wszStream, DWORD cchMax, bool bIncludeFormatting, bool bForceLongFormat) const
{
	const CLBPString& sDefaultPropertyValue = GetDefaultProperty().GetValue();

	const bool bDefaultProperty = !sDefaultPropertyValue.IsEmpty();

	const bool bWrite = 0 != cchMax;
	WCHAR* pStart = wszStream;

	DWORD dwCharactersWritten = 0;

	if (bDefaultProperty || ChildrenCount() || bForceLongFormat)
	{
		//Write the end tag

		CLBPString sEndTag = L"</";
		sEndTag+=m_sName;//TagName();
		sEndTag+=L'>';
		//sEndTag.Format("</%S>", TagName());

		if(bIncludeFormatting)
		{
			sEndTag+=L'\r';
			sEndTag+=L'\n';
		}


		if(bIncludeFormatting && ChildrenCount())
		{
			const int iDepth = GetNestedDepth();
			if(iDepth)
			{
				sEndTag.Insert(0, L'\t', iDepth);
			}
		}

		if(bWrite)
		{
			const int iCopy = min((int)(cchMax-dwCharactersWritten), (sEndTag.GetLength() + 1))*sizeof(WCHAR);

			memcpy(pStart, sEndTag.AsWideString(), iCopy);
			//wcsncpy(pStart, sEndTag.AsWideString(), cchMax-dwCharactersWritten);
		}

		dwCharactersWritten+=sEndTag.GetLength();
	}
	else
	{
		CLBPString tag = L"/>";

		if(bIncludeFormatting)
		{
			tag+=L'\r';
			tag+=L'\n';
		}

		if(bWrite)
		{
#if _MSC_VER < 1400
			wcscpy(wszStream, tag.AsWideString());
#else
			Checked::wcsncpy_s(wszStream, cchMax, tag.AsWideString(), _TRUNCATE);
#endif	
		}

		dwCharactersWritten += tag.GetLength();
	}

	return dwCharactersWritten;
}

CLBP_XMLNode::operator CLBPString() const
{
	return String();
}


CLBPStringBV CLBP_XMLNode::String(bool bIncludeFormatting, bool bForceLongFormat, bool bSortedProperties) const
{
	CLBPStringBV s;

	// Pretend to write the tree to get the buffer size that needs to be allocated.
	const DWORD dwNumCharsIncTerm = WriteToStream(NULL, 0, bIncludeFormatting, bForceLongFormat, bSortedProperties) + 1; // Add 1 for terminator.

	// Now really write the tree to the allocated buffer, including the terminator.
	WriteToStream(s.GetWideBufferSetLength(dwNumCharsIncTerm), dwNumCharsIncTerm, bIncludeFormatting, bForceLongFormat, bSortedProperties);
	s.ReleaseWideBuffer();

	return s;
}


DWORD CLBP_XMLNode::WriteToStream(WCHAR* wszStream, DWORD cchMax, bool bIncludeFormatting, bool bForceLongFormat, bool bSortedProperties) const
{
	CXMLCriticalSection cs(m_CS);

	DWORD dwCharactersWritten = 0;

	DWORD dw = WriteHeaderToStream(wszStream, cchMax, bIncludeFormatting, bForceLongFormat, bSortedProperties);

	dwCharactersWritten += dw;

	const bool bWrite = (cchMax != 0);
	if (bWrite)
		wszStream += dw;

	if (cchMax > dw) cchMax -= dw;
	else             cchMax = 0;

	dw = WriteChildrenToStream(wszStream, cchMax, bIncludeFormatting, bForceLongFormat, bSortedProperties);

	dwCharactersWritten += dw;
	if (bWrite)
		wszStream += dw;

	if (cchMax > dw) cchMax -= dw;
	else             cchMax = 0;

	dw = WriteFooterToStream(wszStream, cchMax, bIncludeFormatting, bForceLongFormat);

	dwCharactersWritten += dw;

	return dwCharactersWritten;
}

bool CLBP_XMLNode::WriteToSegmentedStream(CLBP_XMLSegmentedStream& segs, bool bIncludeFormatting, bool bForceLongFormat, bool bSortedProperties) const
{
	CXMLCriticalSection cs(m_CS);

	WCHAR* p;

	DWORD dw = WriteHeaderToStream(NULL, NULL, bIncludeFormatting, bForceLongFormat, bSortedProperties);
	segs.Append(p = new WCHAR[dw+1]);
	WriteHeaderToStream(p, dw+1, bIncludeFormatting, bForceLongFormat, bSortedProperties);

	WriteChildrenToSegmentedStream(segs, bIncludeFormatting, bForceLongFormat, bSortedProperties);

	dw = WriteFooterToStream(NULL, NULL, bIncludeFormatting, bForceLongFormat);
	segs.Append(p = new WCHAR[dw+1]);
	WriteFooterToStream(p, dw+1, bIncludeFormatting, bForceLongFormat);

	return true;
}

bool CLBP_XMLNode::WriteChildrenToSegmentedStream(CLBP_XMLSegmentedStream& segs, bool bIncludeFormatting, bool bForceLongFormat, bool bSortedProperties) const
{
	CXMLCriticalSection cs(m_CS);

	CChildIterator it = GetChildIterator();
	CLBP_XMLNode* pChild = NULL;

	while (NULL != (pChild = it.GetNextChild()))
	{
		pChild->WriteToSegmentedStream(segs, bIncludeFormatting, bForceLongFormat, bSortedProperties);
	}

	return true;
}

void CLBP_XMLNode::ReleaseSegmentedStream(CLBP_XMLSegmentedStream& segs) // Static.
{
	for (int i = 0; i < segs.Count(); i++)
	{
		delete[] segs[i];
	}
}

CLBP_XMLNode* CLBP_XMLNode::FirstChild(void) const
{
	return m_pFirstChild;
}

CLBP_XMLNode* CLBP_XMLNode::PrevSibling(void) const
{
	CXMLCriticalSection cs(m_CS);

	CLBP_XMLNode* pPrev = m_pParent->m_pFirstChild;
	if (pPrev == this)
		return NULL;

	while (pPrev->m_pNextSibling != this)
	{
		pPrev = pPrev->m_pNextSibling;
	}

	return pPrev;
}

CLBP_XMLNode* CLBP_XMLNode::NextSibling(void) const
{
	return m_pNextSibling;
}

void CLBP_XMLNode::MoveBefore(CLBP_XMLNode& other)
{
	if (&other == this)
		return;

	CXMLCriticalSection cs(m_CS);

	CLBP_XMLNode* pBeforeOther = other.PrevSibling();
	if (pBeforeOther == this)
		return;

	CLBP_XMLNode* pPrev = PrevSibling();
	if (NULL != pPrev)
	{
		pPrev->m_pNextSibling = m_pNextSibling;
	}
	else
	{
		// 'this' was the head; redirect the parent's first child.
		m_pParent->m_pFirstChild = m_pNextSibling;
	}

	m_pNextSibling = &other;

	if (NULL == pBeforeOther)
	{
		// 'other' was the head; redirect the parent's first child.
		m_pParent->m_pFirstChild = this;
	}
	else
	{
		pBeforeOther->m_pNextSibling = this;
	}
}

void CLBP_XMLNode::MoveAfter(CLBP_XMLNode& other)
{
	if (&other == this)
		return;

	CXMLCriticalSection cs(m_CS);

	CLBP_XMLNode* pPrev = PrevSibling();
	if (pPrev == &other)
		return;

	if (NULL != pPrev)
	{
		pPrev->m_pNextSibling = m_pNextSibling;
	}
	else
	{
		// 'this' was the head; redirect the parent's first child.
		m_pParent->m_pFirstChild = m_pNextSibling;
	}

	m_pNextSibling = other.m_pNextSibling;

	other.m_pNextSibling = this;
}

////////////////////////////////////////////////
//
// Node and Property Iterator Implementation
//
////////////////////////////////////////////////

CLBP_XMLNode::CChildIterator::CChildIterator(const CLBP_XMLNode* pParent)
{
	if (NULL != pParent)
	{
		m_pCurrent = pParent->m_pFirstChild;
	}
	else
	{
		m_pCurrent = NULL;
	}
}

CLBP_XMLNode* CLBP_XMLNode::CChildIterator::GetNextChild(void)
{
	CLBP_XMLNode* p = m_pCurrent;
	if (NULL != m_pCurrent)
	{
		m_pCurrent = m_pCurrent->m_pNextSibling;
	}

	return p;
}

CLBP_XMLNode::CPropertyIterator::CPropertyIterator(const CLBP_XMLNode* pParent, bool bAlphabetized)
{
	m_bSorted = bAlphabetized;
	m_apSortedProperties = 0;

	if (m_bSorted)
	{
		const int iPropertyCount = pParent->PropertyCount();

		if (iPropertyCount > 1)
		{
			m_pParent = pParent;
			m_iIndex = 0;
		}
		else
		{
			m_bSorted = false;
		}
	}

	if (!m_bSorted)
	{
		if (NULL != pParent)
		{
			m_pCurrent = pParent->m_pFirstProperty;
		}
		else
		{
			m_pCurrent = NULL;
		}
	}
}

CLBP_XMLNode::CPropertyIterator::~CPropertyIterator()
{
	delete m_apSortedProperties;
}

CLBP_XMLProperty* CLBP_XMLNode::CPropertyIterator::GetNextPropertySorted()
{
	//First time through, we build the list.
	if (m_iIndex == 0)
	{
		CPropertyIterator pi(m_pParent, false);
		CLBP_XMLProperty* p = NULL;

		m_apSortedProperties = new ON_ClassArray<CLBP_XMLProperty>;
		while(NULL != (p = pi.GetNextProperty()))
		{
			m_apSortedProperties->Append(*p);
		}
		m_apSortedProperties->QuickSort(ON_CompareIncreasing);
	}

	ASSERT(m_apSortedProperties != NULL);

#ifdef LBPRHLIB
	if (m_iIndex > m_apSortedProperties->Count()-1)
		return NULL;

#else
	if (m_iIndex > m_apSortedProperties->GetUpperBound())
		return NULL;

#endif
	return &(*m_apSortedProperties)[m_iIndex++];
}

////////////////////////////////////////////////
//
// Property Implementation
//
////////////////////////////////////////////////

#ifdef USE_REFERENCE_COUNTED_PROPERTY
CLBP_XMLProperty& CLBP_XMLProperty::operator = (const CLBP_XMLProperty& prop)
{
	if (NULL != prop.m_pData)
	{
		prop.m_pData->AddRef();
	}

	if (NULL != m_pData)
	{
		m_pData->Release();
	}

	m_pData = prop.m_pData;

	return *this;
}
#endif

CLBP_XMLPropertyData::CLBP_XMLPropertyData()
{
	CommonCtor();
}

CLBP_XMLPropertyData::CLBP_XMLPropertyData(const CLBP_XMLPropertyData& src)
{
	CommonCtor();

	m_sName = src.m_sName;
	m_value = src.m_value;
}

CLBP_XMLPropertyData::CLBP_XMLPropertyData(const CLBPVariant& value)
{
	CommonCtor();
	m_value = value;
}

CLBP_XMLPropertyData::CLBP_XMLPropertyData(const CLBPString& sName, const CLBPVariant& v)
{
	CommonCtor();
	SetName(sName);
	SetValue(v);
}

CLBP_XMLPropertyData::~CLBP_XMLPropertyData()
{
#ifdef USE_REFERENCE_COUNTED_PROPERTY
	ASSERT(m_iRefCount == 0);
#endif

	::InterlockedDecrement(&g_lPropertyCount);
}

void CLBP_XMLPropertyData::CommonCtor()
{
#ifdef USE_REFERENCE_COUNTED_PROPERTY
	m_iRefCount = 1;
#endif

	::InterlockedIncrement(&g_lPropertyCount);
}

const CLBPString& CLBP_XMLPropertyData::Name(void) const
{
	return m_sName;
}

const char* CLBP_XMLPropertyData::NameMBCS(void) const
{
	return m_sName.AsMBCSString();
}

void CLBP_XMLPropertyData::SetName(const CLBPString& sName)
{
	CLBPString& s = m_sName.EmptyStringForWrite();
	s = sName;
	s.TrimLeft();
	s.TrimRight();

/*#ifdef _DEBUG
	if(!CLBP_XMLNode::IsValidXMLName(m_sName))
	{
		throw CLBP_XMLException(CLBP_XMLException::eBadTagName, s);
	}
#endif*/
}

bool CLBP_XMLPropertyData::IsDefaultProperty(void) const
{
	return m_sName.IsEmpty();
}

void CLBP_XMLPropertyData::SetValue(const CLBPVariant& value)
{
	m_value = value;
}

//Takes ownership of the string
void CLBP_XMLPropertyData::SetHugeStringValue(wchar_t* wsz, int iLengthOfBuffer)
{
	//Ensure the value is set to a string
	m_value = L"";

	//Get direct access to the string
	CLBPString& s = const_cast<CLBPString&>(m_value.AsString());

	s.TakeOwnershipOfBuffer(wsz, iLengthOfBuffer);
}

const CLBPVariant& CLBP_XMLPropertyData::GetValue() const
{
	return m_value;
}

bool CLBP_XMLPropertyData::operator < (const CLBP_XMLPropertyData& prop) const
{
	if (prop.m_sName.IsEmpty())
		return false;

	if (m_sName.IsEmpty())
		return true;

	const int i = m_sName.CompareNoCase(prop.m_sName.Wide());

	return i < 0;
}

bool CLBP_XMLPropertyData::operator > (const CLBP_XMLPropertyData& prop) const
{
	if (m_sName.IsEmpty())
		return false;

	if (prop.m_sName.IsEmpty())
		return true;

	const int i = m_sName.CompareNoCase(prop.m_sName.Wide());

	return i > 0;
}

DWORD CLBP_XMLPropertyData::CRC(void) const
{
	return m_value.CRC() ^ m_sName.CRC();
}

////////////////////////////////////////////////
//
// ROOT NODE Implementation
//
////////////////////////////////////////////////

static const CLBPString sXMLRootNodeName(L"xml");
CLBP_XMLRootNode::CLBP_XMLRootNode() : CLBP_XMLNode(sXMLRootNodeName)
{
	m_wszBuffer = NULL;
}

CLBP_XMLRootNode::CLBP_XMLRootNode(const CLBP_XMLRootNode& src) : CLBP_XMLNode(L"xml")
{
	m_wszBuffer = NULL;

	*this = src;
}

CLBP_XMLRootNode::~CLBP_XMLRootNode()
{
	delete [] m_wszBuffer;
}

CLBP_XMLRootNode& CLBP_XMLRootNode::operator = (const CLBP_XMLRootNode& src)
{
	CLBP_XMLNode& node = *this;
	node = src;

	return *this;
}

void CLBP_XMLRootNode::ReadFromFile(const char* szPath, bool bWarningsAsErrors, bool bValidateTags)
{
	CLBPString sPath(szPath);
	ReadFromFile(sPath.AsWideString(), bWarningsAsErrors, bValidateTags);
}

void CLBP_XMLRootNode::ReadFromFile(const WCHAR* szPath, bool bWarningsAsErrors, bool bValidateTags)
{
	if (bWarningsAsErrors)
		g_iWarningsFlagCounter++;

	CLBPString sPath(szPath);

	CLBPString s;

	WIN32_FIND_DATA ffd = { 0 };
	HANDLE h = FindFirstFile(sPath, &ffd);
	if (h == INVALID_HANDLE_VALUE)
	{
		s.Format(L"The file was not found %S\n", sPath.Wide());
		throw CLBP_XMLException(CLBP_XMLException::eFileNotFound, s);
	}

	FindClose(h);

	CLBPUnicodeTextFile file;
	if (!file.Open(sPath, CFile::modeRead | CFile::shareDenyWrite))
	{
		s.Format(L"The file could not be read or was not Unicode %S\n", sPath.Wide());
		throw CLBP_XMLException(CLBP_XMLException::eBadFile, s);
	};

	CLBPString sFile;
	if (!file.ReadEverything(sFile))
	{
		s.Format(L"The file could not be read %S\n", sPath.Wide());
		throw CLBP_XMLException(CLBP_XMLException::eBadFile, s);
	}

	delete[] m_wszBuffer;
	const DWORD dwNumChars = sFile.GetLength();
	m_wszBuffer = new WCHAR[dwNumChars + 1];
	wcsncpy(m_wszBuffer, sFile.Wide(), dwNumChars);
	m_wszBuffer[dwNumChars] = 0;

	DWORD dwRead = 0;
	try
	{
		dwRead = ReadFromStreamThrow(m_wszBuffer, bWarningsAsErrors, bValidateTags);
	}
	catch (CLBP_XMLException& e)
	{
		delete [] m_wszBuffer;
		m_wszBuffer = NULL;
		throw e;
	}

	if (0 == dwRead)
	{
		s.Format(L"No valid XML was read from the file %S\n", sPath.Wide());
		throw CLBP_XMLException(CLBP_XMLException::eNoValidXML, s);
	}

	if (bWarningsAsErrors)
		g_iWarningsFlagCounter--;
}

bool CLBP_XMLRootNode::WriteToFile(const char* szPath, bool bIncludeFormatting, bool bUTF8, bool bSortedProperties) const
{
	CLBPString sPath(szPath);
	return WriteToFile(sPath.AsWideString(), bIncludeFormatting, bUTF8, bSortedProperties);
}

// 15th June 2018 John Croudy, https://mcneel.myjetbrains.com/youtrack/issue/RH-46786
// Length is in Chars. Multiply by size of the char for writing to the file.

bool CLBP_XMLRootNode::WriteToFile(const WCHAR* wszPath, bool bIncludeFormatting, bool bUTF8, bool bSortedProperties) const
{
	CLBPString sPath(wszPath);

	CLBPUnicodeTextFile file(bUTF8);
	if (!file.Open(sPath.T(), CFile::modeCreate | CFile::modeWrite))
		return false;

	CLBP_XMLSegmentedStream segs;
	WriteToSegmentedStream(segs, bIncludeFormatting, false, bSortedProperties);

	for(int i=0;i<segs.Count();i++)
	{
		if (bUTF8)
		{
			CLBP_UTF utf;
			utf.Set(segs[i]);
			const auto* szUTF8 = utf.UTF8();
			const auto lengthInChars = LBP_UTF_LengthInChars<LBP_UTF8_CHAR>(szUTF8);
			file.Write(szUTF8, (UINT)(lengthInChars * sizeof(LBP_UTF8_CHAR)));
		}
		else
		{
			CLBP_UTF utf;
			utf.Set(segs[i]);
			const auto* szUTF16 = utf.UTF16();
			const auto lengthInChars =  LBP_UTF_LengthInChars<LBP_UTF16_CHAR>(szUTF16);
			file.Write(szUTF16, (UINT)(lengthInChars * sizeof(LBP_UTF16_CHAR)));
		}
	}

	ReleaseSegmentedStream(segs);

	return true;
}

#ifdef USE_REFERENCE_COUNTED_ROOTNODE

CLBP_XMLRootNode_RC::CLBP_XMLRootNode_RC()
{
	m_pData = new CLBP_XMLRootNode_RC_Data;
}

CLBP_XMLRootNode_RC::CLBP_XMLRootNode_RC(const CLBP_XMLRootNode_RC& src)
	:
	m_pData(NULL)
{
	*this = src;
}

CLBP_XMLRootNode_RC::~CLBP_XMLRootNode_RC()
{
	if (NULL != m_pData)
	{
		m_pData->Release();
	}
}

CLBP_XMLRootNode_RC& CLBP_XMLRootNode_RC::operator = (const CLBP_XMLRootNode_RC& src)
{
	if (NULL != src.m_pData)
	{
		src.m_pData->AddRef();
	}

	if (NULL != m_pData)
	{
		m_pData->Release();
	}

	m_pData = src.m_pData;

	return *this;
}

#endif



bool CLBP_XMLNode::RecurseChildren(LBPXML_ScanTreeCallback pCallback, void* pv) const
{
	CChildIterator it = GetChildIterator();

	CLBP_XMLNode* pChild = NULL;
	while (NULL != (pChild = it.GetNextChild()))
	{
		if (!pCallback(pChild, pv))
			return false;

		if (!pChild->RecurseChildren(pCallback, pv))
			return false;
	}

	return true;
}
typedef ON_SimpleArray<int> IntArray;


DWORD CLBP_XMLNode::CRC(void) const
{
	DWORD crc = m_sName.CRC();

	int iCount = 0;

	IntArray propCRCs;
	IntArray childCRCs;

	int iPropCount = 0;
	int iChildCount = 0;

	CPropertyIterator pi = GetPropertyIterator();
	while (CLBP_XMLProperty* pProp = pi.GetNextProperty())
	{
		propCRCs.Append(pProp->CRC());
		iPropCount++;
	}

	CChildIterator ci = GetChildIterator();
	while (CLBP_XMLNode* pNode = ci.GetNextChild())
	{
		childCRCs.Append(pNode->CRC());
		iChildCount++;
	}


	propCRCs.QuickSort(ON_CompareIncreasing);
	childCRCs.QuickSort(ON_CompareIncreasing);

	ASSERT(iChildCount == childCRCs.Count());
	ASSERT(iPropCount  == propCRCs.Count());

	int i=0;
	for (i=0;i<propCRCs.Count();i++)
	{
		crc ^= _rotl(propCRCs[i], iCount++);
	}
	for (i=0;i<childCRCs.Count();i++)
	{
		crc ^= _rotl(childCRCs[i], iCount++);
	}

	ASSERT(iCount == iChildCount + iPropCount);

	return crc;
}

static void NormalizeXML(CLBPString& sXML)
{
	// Normalizes XML in preparation for CRCing. The result in sOut is not XML.
	//
	// 1. Convert all text to lowercase.
	// 2. Convert white space to dots.
	// 3. Sort the lines.

	// Make it all lowercase.
	sXML.MakeLower();

	// Convert white space to dots.
	WCHAR* p = sXML.GetWideBuffer(0);
	while (0 != *p)
	{
		if ((*p == L' ') || (*p == L'\t'))
			 *p =  L'.';
		p++;
	}
	sXML.ReleaseWideBuffer();

	// Pre-count the lines so that we know what to allocate
	const int iLines = sXML.Count(L'\n') + 1;  //+1 for the last line - possibly with no cr

	ON_ClassArray<CLBPString> aItems(iLines);

	// Isolate each line and add it to a sortable array.
	const WCHAR* pLineScale = sXML.Wide();
	const WCHAR* pLast = pLineScale;

	while (0 != *pLineScale)
	{
		if (*pLineScale++ == L'\n')
		{
			const CLBPString line(pLast, (DWORD)(pLineScale-pLast));

			aItems.Append(line);
			pLast = pLineScale;
		}
	}

	//Now sort the array.  This could be further optimized by making it into an array of pointers
	//to strings
	aItems.QuickSort(ON_CompareIncreasing);

	const int iLineCount = aItems.Count();

	//Everything is going to be spat out into the same string, so we need to empty it
	//without reducing the allocation so that it doesn't end up doing hundreds of REALLOCs
	sXML.Empty();

	// Build the output string by concatenating the array items.
	for (int i = 0; i < iLineCount; i++)
	{
		sXML += aItems[i];
	}
}

DWORD CLBP_XMLNode::ComputeCRC(void) const
{
	CLBPString s = String(true, false, true);

	NormalizeXML(s);

	return CLBP_CRC32::Calculate(s.Wide(), s.GetLength() * sizeof(WCHAR));
}

CLBP_XMLException::CLBP_XMLException() : CLBPRunTimeException(L"")
{
	m_error = eNoProblems;
}

CLBP_XMLException::CLBP_XMLException(lbpxml_error_code error, const WCHAR* wszExtraInfo)
 : CLBPRunTimeException(L"")
{
	m_error = error;
	m_sExtraInfo = wszExtraInfo;

	m_sMessage.Format("Type: %S, Context: %S", What(), Context());

#ifdef _DEBUG

	CLBPString sError = wszExtraInfo;
	OutputDebugString(sError);
#endif
}

CLBP_XMLException::lbpxml_error_code CLBP_XMLException::GetErrorCode() const
{
	return m_error;
}

const WCHAR* CLBP_XMLException::What() const
{
	switch(m_error)
	{
	case eNoProblems:
		return L"OK";
	case eMalformedTagOnRead:
		return L"Malformed tag on read";
	case eTagNotFound:
		return L"Tag not found";
	case eEndTagNotFound:
		return L"End tag not found";
	case eBadFile:
		return L"Bad File";
	case eFileNotFound:
		return L"File Not Found";
	case eBadType:
		return L"Bad Type";
	case eNoValidXML:
		return L"No Valid XML";
	case eBadTagName:
		return L"Bad Tag Name";
	case eBadCharactersInString:
		return L"Bad Characters in String";
	}

	return L"Unknown error";
}

const WCHAR* CLBP_XMLException::Context() const
{
	return m_sExtraInfo;
}

/////////////////////////////////////////////////////////////////
//
//	Variant implementation
//
/////////////////////////////////////////////////////////////////

CLBPVariant::CLBPVariant()
{
	m_type = vt_String;
	m_bTypePending = false;
	m_pBuffer = NULL;
}

CLBPVariant::CLBPVariant(int iValue)
{
	m_pBuffer = NULL;
	SetValue(iValue);
}

CLBPVariant::CLBPVariant(double dValue)
{
	m_pBuffer = NULL;
	SetValue(dValue);
}

CLBPVariant::CLBPVariant(float fValue)
{
	m_pBuffer = NULL;
	SetValue(fValue);
}

CLBPVariant::CLBPVariant(const CLBPString& s)
{
	m_pBuffer = NULL;
	SetValue(s);
}

CLBPVariant::CLBPVariant(const double* point, vt_arraytype at)
{
	m_pBuffer = NULL;
	SetValue(point, at);
}

CLBPVariant::CLBPVariant(const float* point, vt_arraytype at)
{
	m_pBuffer = NULL;
	SetValue(point, at);
}

CLBPVariant::CLBPVariant(const WCHAR* wsz)
{
	m_pBuffer = NULL;
	CLBPString s(wsz);
	SetValue(s);
}

CLBPVariant::CLBPVariant(const char* sz)
{
	m_pBuffer = NULL;
	CLBPString s(sz);
	SetValue(s);
}

CLBPVariant::CLBPVariant(const CLBPColor& c)
{
	m_pBuffer = NULL;
	SetValue(c);
}

CLBPVariant::CLBPVariant(bool b)
{
	m_pBuffer = NULL;
	SetValue(b);
}

CLBPVariant::CLBPVariant(const UUID& uuid)
{
	m_pBuffer = NULL;
	SetValue(uuid);
}

CLBPVariant::CLBPVariant(const CTime& time)
{
	m_pBuffer = NULL;
	SetValue(time);
}

CLBPVariant::CLBPVariant(const void* pBuffer, size_t size)
{
	m_pBuffer = NULL;
	SetValue(pBuffer, size);
}

CLBPVariant::CLBPVariant(const CLBPBuffer& buffer)
{
	m_pBuffer = NULL;
	SetValue(buffer);
}

CLBPVariant::CLBPVariant(const CLBPVariant& src)
{
	m_pBuffer = NULL;
	*this = src;
}

CLBPBuffer& CLBPVariant::Buffer() const
{
	if(!m_pBuffer)
	{
		m_pBuffer = new CLBPBuffer;
	}
	return *m_pBuffer;
}

void CLBPVariant::BufferClear()
{
	if(m_pBuffer)
	{
		delete m_pBuffer;
		m_pBuffer = NULL;
	}
}

CLBPVariant& CLBPVariant::operator=(const CLBPVariant& src)
{
	m_type = src.Type();
	m_bTypePending = src.TypePending();

	if(m_type != vt_Buffer)
	{
		BufferClear();
	}

	switch(m_type)
	{
	case vt_String:
		m_sVal.EmptyStringForWrite() = src.AsString();
		break;
	case vt_Double:
		m_dVal = src.AsDouble();
		break;
	case vt_Float:
		m_fVal = src.AsFloat();
		break;
	case vt_Integer:
		m_iVal = src.AsInteger();
		break;
	case vt_3_double_array:
		src.As3dPoint(m_aVal);
		break;
	case vt_2_double_array:
		src.As2dPoint(m_aVal);
		break;
	case vt_4_double_array:
	case vt_4_double_color:
		src.As4dPoint(m_aVal);
		break;
	case vt_Bool:
		m_bVal = src.AsBool();
		break;
	case vt_Matrix:
		src.AsMatrix(m_matrix);
		break;
	case vt_Null:
		break;
	case vt_Uuid:
		m_uuidVal = src.AsUuid();
		break;
	case vt_Time:
		m_timeVal = (time_t)src.AsTime().GetTime();
		break;
	case vt_Buffer:
		Buffer() = src.AsBuffer();
		break;
	default:
		throw CLBP_XMLException(CLBP_XMLException::eBadType, L"Source has unknown type");
	}

	//The above calls can reset the type pending flag on the source
	//make sure the source is set back to the original condition.
	src.SetTypePendingFlag(m_bTypePending);

	return *this;
}

CLBPVariant::~CLBPVariant()
{
	delete m_pBuffer;
}

CLBPVariant::vt CLBPVariant::Type() const
{
	return m_type;
}

bool CLBPVariant::IsNull() const
{
	return vt_Null == m_type;
}

void CLBPVariant::SetNull()
{
	m_type = vt_Null;
}

bool CLBPVariant::NeedsURLEncode(bool bForAnsi) const
{
	switch (Type())
	{
	case vt_Buffer:
	case vt_Integer:
	case vt_Float:
	case vt_Double:
	case vt_Bool:
	case vt_Null:
	case vt_Uuid:
	case vt_2_double_array:
	case vt_3_double_array:
	case vt_4_double_array:
	case vt_4_double_color:
	case vt_Matrix:
		return false;
	default:
		return AsString().NeedsURLEncode(bForAnsi);
	}
}

void CLBPVariant::SetValue(int v)
{
	BufferClear();
	m_type = vt_Integer;
	m_iVal = v;
	m_bTypePending = false;
}

void CLBPVariant::SetValue(double v)
{
	BufferClear();
	m_type = vt_Double;
	m_dVal = v;
	m_bTypePending = false;
}

void CLBPVariant::SetValue(float v)
{
	BufferClear();
	m_type = vt_Float;
	m_fVal = v;
	m_bTypePending = false;
}

void CLBPVariant::SetValue(const CLBPString& s)
{
	BufferClear();
	m_type = vt_String;
	m_sVal.EmptyStringForWrite() = s;
	m_bTypePending = false;
}

void CLBPVariant::SetValue(const double* p, vt_arraytype at)
{
	BufferClear();

	if(vt_array_16 == at)
	{
		m_type = vt_Matrix;
		for(int i=0; i<16; i++)
		{
			m_matrix[i] = p[i];
		}
	}

	if(vt_array_3 == at)
	{
		m_type = vt_3_double_array;
		for(int i=0; i<3; i++)
		{
			m_aVal[i] = p[i];
		}
	}

	if(vt_array_4 == at)
	{
		m_type = vt_4_double_array;
		for(int i=0; i<4; i++)
		{
			m_aVal[i] = p[i];
		}
	}

	if(vt_array_2 == at)
	{
		m_type = vt_2_double_array;
		for(int i=0; i<2; i++)
		{
			m_aVal[i] = p[i];
		}
	}

	m_bTypePending = false;
}

void CLBPVariant::SetValue(const float* p, vt_arraytype at)
{
	BufferClear();
	if(vt_array_16 == at)
	{
		m_type = vt_Matrix;
		for(int i=0; i<16; i++)
		{
			m_matrix[i] = (double)p[i];
		}
	}

	if(vt_array_3 == at)
	{
		m_type = vt_3_double_array;
		for(int i=0; i<3; i++)
		{
			m_aVal[i] = (double)p[i];
		}
	}

	if(vt_array_4 == at)
	{
		m_type = vt_4_double_array;
		for(int i=0; i<4; i++)
		{
			m_aVal[i] = (double)p[i];
		}
	}

	if(vt_array_2 == at)
	{
		m_type = vt_2_double_array;
		for(int i=0; i<2; i++)
		{
			m_aVal[i] = (double)p[i];
		}
	}

	m_bTypePending = false;
}

void CLBPVariant::SetValue(const CLBPColor& c)
{
	BufferClear();
	m_type = vt_4_double_color;

	m_aVal[0] = c.DRed();
	m_aVal[1] = c.DGreen();
	m_aVal[2] = c.DBlue();
	m_aVal[3] = c.DAlpha();

	m_bTypePending = false;
}

void CLBPVariant::SetValue(bool b)
{
	BufferClear();
	m_type = vt_Bool;
	m_bVal = b;
	m_bTypePending = false;
}

void CLBPVariant::SetValue(const UUID& uuid)
{
	BufferClear();
	m_type = vt_Uuid;
	m_uuidVal = uuid;
	m_bTypePending = false;
}

void CLBPVariant::SetValue(const CTime& time)
{
	BufferClear();
	m_type = vt_Time;
	m_timeVal = (time_t)time.GetTime();
	m_bTypePending = false;
}

void CLBPVariant::SetValue(const void* pBuffer, size_t size)
{
	BufferClear();
	m_type = vt_Buffer;
	m_bTypePending = false;

	Buffer() = CLBPBuffer(pBuffer, size);
}

void CLBPVariant::SetValue(const CLBPBuffer& buffer)
{
	m_type = vt_Buffer;
	m_bTypePending = false;

	Buffer() = buffer;
}

int CLBPVariant::AsInteger() const
{
	DoAutoTyping(vt_Integer);
	switch(Type())
	{
	case vt_String:
		//Sort out the BOOL case
		if(m_sVal.CompareNoCase(L"true") ==0) return 1;
		if(m_sVal.CompareNoCase(L"t")==0) return true;
		return LBP_wtoi(m_sVal.Wide());
	case vt_Double:
		return (int)m_dVal;
	case vt_Float:
		return (int)m_fVal;
	case vt_Integer:
		return m_iVal;
	case vt_Bool:
		return m_bVal ? 1 : 0;
	default:
		return 0;
	}
}

bool CLBPVariant::AsBool() const
{
	DoAutoTyping(vt_Bool);
	switch(Type())
	{
	case vt_String:
		if(m_sVal.CompareNoCase(L"true")==0) return true;
		if(m_sVal.CompareNoCase(L"t")==0) return true;
		return LBP_wtoi(m_sVal.Wide()) != 0;
	case vt_Integer:
		return m_iVal != 0;
	case vt_Bool:
		return m_bVal;
	default:
		return false;
	}
}

double CLBPVariant::AsDouble() const
{
	DoAutoTyping(vt_Double);
	switch(Type())
	{
	case vt_String:
		return LBP_wtof(m_sVal.Wide());
	case vt_Double:
		return m_dVal;
	case vt_Float:
		return m_fVal;
	case vt_Integer:
		return (double)m_iVal;
	case vt_Bool:
		return m_bVal ? 1.0 : 0.0;
	default:
		return 0.0;
	}
}

float CLBPVariant::AsFloat() const
{
	DoAutoTyping(vt_Float);
	switch(Type())
	{
	case vt_String:
		return (float)LBP_wtof(m_sVal.Wide());
	case vt_Double:
		return (float)m_dVal;
	case vt_Float:
		return m_fVal;
	case vt_Integer:
		return (float)m_iVal;
	case vt_Bool:
		return m_bVal ? 1.0f : 0.0f;
	default:
		return 0.0f;
	}
}

const double* CLBPVariant::As3dPoint() const
{
	DoAutoTyping(vt_3_double_array);
	switch(Type())
	{
	case vt_2_double_array:
		m_aVal[2] = 0.0;
	case vt_3_double_array:
	case vt_4_double_color:
	case vt_4_double_array:
		return m_aVal;
		break;
	case vt_String:
		if(m_sVal.IsValid3dPoint())
		{
			String2Point(3);
			return m_aVal;
		}
	default:
		{
			static double d3[3];
			d3[0] = 0.0;
			d3[1] = 0.0;
			d3[2] = 0.0;
			return d3;
		}
	}
}

const double* CLBPVariant::As4dPoint() const
{
	DoAutoTyping(vt_4_double_array);
	switch(Type())
	{
	case vt_2_double_array:
		m_aVal[2] = 0.0;
	case vt_3_double_array:
		m_aVal[3] = 0.0;
	case vt_4_double_array:
	case vt_4_double_color:
		return m_aVal;
		break;
	case vt_String:
		if(m_sVal.IsValid4dPoint())
		{
			String2Point(4);
			return m_aVal;
		}
	default:
		{
			static double d4[4];
			d4[0] = 0.0;
			d4[1] = 0.0;
			d4[2] = 0.0;
			d4[3] = 0.0;
			return d4;
		}
	}
}

bool CLBPVariant::As3dPoint(double* /*[3]*/d) const
{
	DoAutoTyping(vt_3_double_array);

	double* pd = NULL;

	switch(Type())
	{
	case vt_2_double_array:
		m_aVal[2] = 0.0;
	case vt_3_double_array:
	case vt_4_double_color:
	case vt_4_double_array:
		pd = m_aVal;
		break;

	case vt_String:
		if(m_sVal.IsValid3dPoint())
		{
			String2Point(3);
			pd = m_aVal;
		}
	}

	if(NULL == pd) return false;

	for(int i=0;i<3;i++)
	{
		d[i] = pd[i];
	}

	return true;
}

bool CLBPVariant::As4dPoint(double* /*[4]*/d) const
{
	DoAutoTyping(vt_4_double_array);

	double* pd = NULL;

	switch(Type())
	{
	case vt_2_double_array:
		m_aVal[2] = 0.0;
	case vt_3_double_array:
		m_aVal[3] = 0.0;
	case vt_4_double_color:
	case vt_4_double_array:
		pd = m_aVal;
		break;

	case vt_String:
		if(m_sVal.IsValid4dPoint())
		{
			String2Point(4);
			pd = m_aVal;
		}
	}

	if(NULL == pd) return false;

	for(int i=0;i<4;i++)
	{
		d[i] = pd[i];
	}

	return true;
}

bool CLBPVariant::As3dPoint(float* /*[3]*/f) const
{
	double d[3];
	bool b = As3dPoint(d);
	if(!b) return false;

	for(int i=0;i<3;i++)
	{
		f[i] = (float)d[i];
	}

	return true;
}

bool CLBPVariant::As4dPoint(float* /*[4]*/f) const
{
	double d[4];
	bool b = As4dPoint(d);
	if(!b) return false;

	for(int i=0;i<4;i++)
	{
		f[i] = (float)d[i];
	}

	return true;
}

const double* CLBPVariant::As2dPoint() const
{
	DoAutoTyping(vt_2_double_array);
	switch(Type())
	{
	case vt_2_double_array:
	case vt_3_double_array:
	case vt_4_double_color:
	case vt_4_double_array:
		return m_aVal;
		break;
	case vt_String:
		if(m_sVal.IsValid2dPoint())
		{
			String2Point(2);
			return m_aVal;
		}
	default:
		{
			static double d3[3];
			d3[0] = 0.0;
			d3[1] = 0.0;
			d3[2] = 0.0;
			return d3;
		}
	}
}

bool CLBPVariant::As2dPoint(double* /*[2]*/d) const
{
	DoAutoTyping(vt_2_double_array);

	double* pd = NULL;

	switch(Type())
	{
	case vt_2_double_array:
	case vt_3_double_array:
	case vt_4_double_color:
	case vt_4_double_array:
		pd = m_aVal;
		break;

	case vt_String:
		if(m_sVal.IsValid2dPoint())
		{
			String2Point(2);
			pd = m_aVal;
		}
	}

	if(NULL == pd) return false;

	for(int i=0;i<2;i++)
	{
		d[i] = pd[i];
	}

	return true;
}

bool CLBPVariant::As2dPoint(float* /*[2]*/f) const
{
	double d[2];
	bool b = As2dPoint(d);
	if(!b) return false;

	for(int i=0;i<2;i++)
	{
		f[i] = (float)d[i];
	}

	return true;
}

const double* CLBPVariant::AsMatrix() const
{
	DoAutoTyping(vt_Matrix);
	switch(Type())
	{
	case vt_Matrix:
		return m_matrix;
		break;
	case vt_String:
		if(m_sVal.IsValidMatrix())
		{
			String2Point(16);
			return m_matrix;
		}
	default:
		{
			static double d[16];
			for(int i=0;i<16;i++)
			{
				d[i] = 0.0;
			}
			return d;
		}
	}
}

bool CLBPVariant::AsMatrix(double* /*[16]*/d) const
{
	DoAutoTyping(vt_Matrix);

	double* pd = NULL;

	switch(Type())
	{
	case vt_Matrix:
		pd = m_matrix;
		break;
	case vt_String:
		if(m_sVal.IsValidMatrix())
		{
			String2Point(16);
			pd = m_matrix;
		}
		break;
	default:
		return false;
	}

	if(NULL == pd) return false;

	for(int i=0;i<16;i++)
	{
		d[i] = pd[i];
	}

	return true;
}

bool CLBPVariant::AsMatrix(float* /*[16]*/f) const
{
	double d[16];
	bool b = AsMatrix(d);
	if(!b) return false;

	for(int i=0;i<16;i++)
	{
		f[i] = (float)d[i];
	}

	return true;
}

CLBPColor CLBPVariant::AsColor() const
{
	DoAutoTyping(vt_4_double_color);

	switch(Type())
	{
	case vt_String:
		String2Point(4);
	case vt_4_double_color:
		CLBPColor c(m_aVal[0], m_aVal[1], m_aVal[2], m_aVal[3]);
		return c;
	}

	return CLBPColor();
}

UUID CLBPVariant::AsUuid() const
{
	DoAutoTyping(vt_Uuid);
	switch(Type())
	{
	case vt_String:
		return CLBP_UUID(m_sVal);
	case vt_Uuid:
		return m_uuidVal;
	default:
		static const UUID uuidNil = { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };
		return uuidNil;
	}
}

CTime CLBPVariant::AsTime() const
{
	DoAutoTyping(vt_Time);

	switch(Type())
	{
	case vt_String:
		return CLBPTime(m_sVal);
	case vt_Time:
		return m_timeVal;
	}

	return 0;
}

const CLBPBuffer CLBPVariant::AsBuffer() const
{
	DoAutoTyping(vt_Buffer);

	switch(Type())
	{
	case vt_String:
		//Who owns the resultant buffer?
		return CLBPBuffer(m_sVal);
	case vt_Buffer:
		return Buffer();
	}

	return CLBPBuffer();
}

void DoubleArrayString(const double* array, int iCount, CLBPString& s)
{
	s.Empty();
	s.GetWideBuffer(20*iCount);
	s.ReleaseWideBuffer();

	CLBPString sThis;

	for(int i=0;i<iCount;i++)
	{
		sThis.Set(array[i]);
		s+=sThis;
		s+=L',';
	}
	s.TrimRight(L',');
}

const CLBPString& CLBPVariant::AsString() const
{
	DoAutoTyping(vt_String);

	if (Type() == vt_String)
	{
		return m_sVal;
	}

	CLBPString& s = m_sVal.EmptyStringForWrite();

	switch (Type())
	{
	case vt_Double:
		s.Set(m_dVal);
		return m_sVal;

	case vt_Float:
		s.Set(m_fVal);
		return m_sVal;

	case vt_Integer:
		s.Set(m_iVal);
		return m_sVal;

	case vt_3_double_array:
		DoubleArrayString(m_aVal, 3, s);
		return m_sVal;

	case vt_4_double_array:
	case vt_4_double_color:
		DoubleArrayString(m_aVal, 4, s);
		return m_sVal;

	case vt_2_double_array:
		DoubleArrayString(m_aVal, 2, s);
		return m_sVal;

	case vt_Matrix:
		DoubleArrayString(m_aVal, 16, s);
		return m_sVal;

	case vt_Bool:
		s = m_bVal ? L"true" : L"false";
		return m_sVal;

	case vt_Uuid:
		s = CLBP_UUID(m_uuidVal).String();
		return m_sVal;

	case vt_Time:
		{
		CLBPTime time(m_timeVal);
		s = time.AsString();
		return m_sVal;
		}
	case vt_Buffer:
		Buffer().Base64(s);
		return m_sVal;

	default:
		{
		static CLBPString s;
		s.Empty();
		return s;
		}
	}
}

CLBPVariant::operator double() const
{
	return AsDouble();
}

CLBPVariant::operator float() const
{
	return AsFloat();
}

CLBPVariant::operator int() const
{
	return AsInteger();
}

CLBPVariant::operator const CLBPString&() const
{
	return AsString();
}

CLBPVariant::operator const double*() const
{
	return As3dPoint();
}

CLBPVariant::operator bool() const
{
	return AsBool();
}

CLBPVariant::operator UUID() const
{
	return AsUuid();
}

CLBPVariant::operator CLBPColor() const
{
	return AsColor();
}

CLBPVariant::operator CTime() const
{
	return AsTime();
}

CLBPVariant::operator CLBPBuffer() const
{
	return AsBuffer();
}

static CLBPString ___sZeros = L",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,";

void CLBPVariant::String2Point(int iValues) const
{
	
	CLBPString s = (const CLBPString&)m_sVal + ___sZeros;

	for (int i = 0; i < iValues; i++)
	{
		const double d = LBP_wtof(s.Wide());

		s = s.Mid(s.Find(L',') + 1);

		if (16 == iValues)
		{
			m_matrix[i] = d;
		}
		else
		{
			m_aVal[i] = d;
		}
	}
}

bool CLBPVariant::IsEmpty() const
{
	return AsString().IsEmpty() ? true : false;
}

bool CLBPVariant::TypePending() const
{
	return m_bTypePending;
}

void CLBPVariant::SetTypePendingFlag(bool bTypePending) const
{
	m_bTypePending = bTypePending;
}

CLBPStringBV CLBPVariant::TypeAsString() const
{
	switch(Type())
	{
	case vt_Double:
		return CLBPStringBV(L"double");
	case vt_Float:
		return CLBPStringBV(L"float");
	case vt_Integer:
		return CLBPStringBV(L"integer");
	case vt_3_double_array:
		return CLBPStringBV(L"3-double-array");
	case vt_4_double_array:
		return CLBPStringBV(L"4-double-array");
	case vt_2_double_array:
		return CLBPStringBV(L"2-double-array");
	case vt_4_double_color:
		return CLBPStringBV(L"color");
	case vt_Matrix:
		return CLBPStringBV(L"matrix-4x4");
	case vt_Bool:
		return CLBPStringBV(L"bool");
	case vt_Uuid:
		return CLBPStringBV(L"uuid");
	case vt_String:
		return CLBPStringBV(L"string");
	case vt_Time:
		return CLBPStringBV(L"time");
	case vt_Buffer:
		return CLBPStringBV(L"buffer");
	}

	return CLBPStringBV(L"null");
}

DWORD CLBPVariant::CRC(void) const
{
	const CLBPString& s = AsString();
	return s.CRC();
}

void CLBPVariant::DoAutoTyping(vt type) const
{
	//Only do automatic typing if the flag is set
	if(!m_bTypePending) return;

	m_bTypePending = false; // Must be first.

	//This is only meant to work if the value is a string - ie, it has just
	//been read out of the XML stream.
	if(Type() != vt_String)
	{
		return;
	}

	//This requires a pretty big const hack - since we are changing the variant
	//pretty dramatically, but we aren't actually changing its value

	CLBPVariant* pThis = const_cast<CLBPVariant*>(this);

	switch(type)
	{
	case vt_String:
		break;
	case vt_Double:
		pThis->SetValue(AsDouble());
		break;
	case vt_Float:
		pThis->SetValue(AsFloat());
		break;
	case vt_Integer:
		pThis->SetValue(AsInteger());
		break;
	case vt_4_double_array:
		pThis->SetValue(As4dPoint(), vt_array_4);
		break;
	case vt_3_double_array:
		pThis->SetValue(As3dPoint(), vt_array_3);
		break;
	case vt_2_double_array:
		pThis->SetValue(As2dPoint(), vt_array_2);
		break;
	case vt_4_double_color:
		pThis->SetValue(AsColor());
		break;
	case vt_Matrix:
		pThis->SetValue(AsMatrix(), vt_array_16);
		break;
	case vt_Bool:
		pThis->SetValue(AsBool());
		break;
	case vt_Uuid:
		pThis->SetValue(AsUuid());
		break;
	case vt_Time:
		pThis->SetValue(AsTime());
		break;
	case vt_Buffer:
		pThis->SetValue(AsBuffer());
		break;
	}

	//Otherwise we assume the conversion is not supported and just go on with life.
}

void AutoTypeVariant(CLBPVariant& v)
{
	//Used by the XML reader to try to invent sensible types
	//for variants read in from the stream
	if(v.Type() != CLBPVariant::vt_String)
		return;	//The variant already has a type

	CLBPString s = v.AsString();
	v.SetTypePendingFlag(true);

	static const CLBPString sBase64Prefix = CLBPBuffer::Base64Prefix();
	static const int iBase64PrefixLength = sBase64Prefix.GetLength();

	if(s == L"true" || s == L"false")
	{
		v.AsBool();
		return;
	}

	if(s.Left(iBase64PrefixLength) == sBase64Prefix.Wide())
	{
		v.AsBuffer();
		return;
	}

	if(CLBPTime::IsValidTime(s))
	{
		v.AsTime();
		return;
	}

	if(s.IsValidIntegerNumber())
	{
		v.AsInteger();
		return;
	}

	if(s.IsValidMatrix())
	{
		v.AsMatrix();
		return;
	}

	if(s.IsValid4dPoint())
	{
		v.As4dPoint();
		return;
	}

	if(s.IsValid3dPoint())
	{
		v.As3dPoint();
		return;
	}

	if(s.IsValid2dPoint())
	{
		v.As2dPoint();
		return;
	}

	if(s.IsValidRealNumber())
	{
		v.AsDouble();
		return;
	}

	CLBP_UUID uuid(s);
	if(!uuid.IsNil())
	{
		v.AsUuid();
		return;
	}
}

void CLBP_XMLNode::OnNodeReadFromStream(const CLBP_XMLNode* pNode) const 
{
}

#ifdef _DEBUG

void CLBP_XMLNode::SetReadOnly(bool b) const
{
}

#endif
