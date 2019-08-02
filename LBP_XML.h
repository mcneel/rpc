
#pragma once

class CLBP_XMLRootNode;
class CLBP_XMLNode;
class CLBP_XMLProperty;
class CLBPColor;

#ifndef LBPEXPORT
#define LBPEXPORT
#endif

#ifdef _DEBUG
#define USE_REFERENCE_COUNTED_ROOTNODE
#define USE_REFERENCE_COUNTED_PROPERTY
#endif

#include "LBPException.h"
#include "LBPString.h"
#include "LBP_UUID.h"
#include "LBPLibUtilities.h"

#ifndef __AFXTEMPL_H__
#pragma message("XML classes require afxtempl.h in your PCH")                                                                                
#endif

#if defined (LBPLIB_APPLE_SPECIFIC)
#define REFCOUNT_INT int32_t
#define InterlockedIncrement OSAtomicIncrement32
#define InterlockedDecrement OSAtomicDecrement32
#endif

#if defined (LBPLIB_WINDOWS_SPECIFIC)
#define REFCOUNT_INT LONG
#endif

typedef bool (*LBPXML_ScanTreeCallback) (CLBP_XMLNode*, void* dwUser);

class LBPEXPORT CLBP_XMLException : public CLBPRunTimeException
{
public:

	typedef enum taglbpxml_error_code
	{
		eNoProblems,
		eMalformedTagOnRead,
		eTagNotFound,
		eEndTagNotFound,
		eBadFile,
		eFileNotFound,
		eBadType,
		eNoValidXML,
		eBadTagName,
		eBadCharactersInString

	} lbpxml_error_code;

	CLBP_XMLException();
	CLBP_XMLException(lbpxml_error_code error, const WCHAR* wszExtraInfo = NULL);

	lbpxml_error_code	GetErrorCode() const;
	const WCHAR*		What() const;
	const WCHAR*		Context() const;

protected:
	lbpxml_error_code	m_error;
	CLBPString			m_sExtraInfo;
};

int LBPEXPORT LBP_GetNodeCount(void);
int LBPEXPORT LBP_GetPropertyCount(void);

class LBPEXPORT CLBP_XMLNode
{
public:
	CLBP_XMLNode(const CLBPString& sName); // Always creates empty default property.
	CLBP_XMLNode(const CLBP_XMLNode&);     // Copy constructor.
	virtual ~CLBP_XMLNode();

	CLBP_XMLNode& operator=(const CLBP_XMLNode&);

	void AssignFast(const CLBP_XMLNode& src); // DEPRECATED - just calls operator =

	bool MergeFrom(const CLBP_XMLNode& src);  // src node must have the same name

	CLBP_XMLNode* FirstChild(void) const;
	CLBP_XMLNode* PrevSibling(void) const;
	CLBP_XMLNode* NextSibling(void) const;

public: // Tag ID
	const char* TagNameMBCS(void) const;
	const WCHAR* TagName(void) const;
	void SetTagName(const CLBPString& sName);

	CLBP_XMLNode* GetParent(void) const;
	const CLBP_XMLNode& TopmostParent(void) const;

	DWORD CRC(void) const;

	static bool IsValidXMLName(const CLBPString& sTagName);

public: // Children scan
	bool RecurseChildren(LBPXML_ScanTreeCallback, void* pvData) const;

public: // Change data
	CLBP_XMLNode*	  AddChildNode(CLBP_XMLNode* pNode);              // The ownership transfers to the tree - do not delete this pointer.
	CLBP_XMLProperty* AddProperty(const CLBP_XMLProperty* pProperty); // The property object is copied - you should delete it.
	CLBP_XMLProperty* AddProperty(const CLBP_XMLProperty& prop);      // John added this; it's handier than the pointer one.
	CLBP_XMLProperty* AttachProperty(CLBP_XMLProperty* pProp);        // This takes ownership of the property you made on the heap.

	// 19th June 2018 John Croudy, https://mcneel.myjetbrains.com/youtrack/issue/RH-46772 -- Crashes on the Mac.
	//void SafeRemove(void); // John added this; removes and deletes this node. Safe to call even if 'this' is NULL.

	void Remove(void); // Removes and deletes this node.
	bool RemoveProperty(const CLBPString& sPropertyName);
	bool RemoveChild(CLBP_XMLNode* pChildNode);
	CLBP_XMLNode* UnhookChild(CLBP_XMLNode* pChildNode); // Same as RemoveChild, but doesn't delete the child - instead it passes ownership to the caller.

	void RemoveAllChildren(void);
	void RemoveAllProperties(void);
		 
	void Clear(void); // Gets rid of all data on this node.
		 
	void MoveBefore(CLBP_XMLNode& other);
	void MoveAfter (CLBP_XMLNode& other);

public: // Serialization
	DWORD	ReadFromStreamThrow(const WCHAR* wszStream, bool bWarningsAsErrors = false, bool bValidateTags = true);	//Throws CLBP_XMLException
	
#define LBP_XML_READ_ERROR 0xFFFFFFFF
	//Returns LBP_XML_READ_ERROR if it fails
	DWORD ReadFromStreamNoThrow(const WCHAR* wszStream, bool bWarningsAsErrors = false, bool bValidateTags = true);

	LPVOID LastReadBufferPointer(void) const;

	//This function is called on the top-most node during the reading process.
	virtual void OnNodeReadFromStream(const CLBP_XMLNode* pNode) const;

	CLBPStringBV String(bool bIncludeFormatting = true, bool bForceLongFormat = false, bool bSortedProperties = false) const;
	operator CLBPString() const;

	//Input NULL for cchMax to return the number of characters which will be written
	DWORD WriteToStream(WCHAR* wszStream, DWORD cchMax, bool bIncludeFormatting = true, bool bForceLongFormat = false, bool bSortedProperties = false) const;
	DWORD WriteHeaderToStream(WCHAR* wszStream, DWORD cchMax, bool bIncludeFormatting = true, bool bForceLongFormat = false, bool bSortedProperties = false) const;
	DWORD WriteChildrenToStream(WCHAR* wszStream, DWORD cchMax, bool bIncludeFormatting = true, bool bForceLongFormat = false, bool bSortedProperties = false) const;
	DWORD WriteFooterToStream(WCHAR* wszStream, DWORD cchMax, bool bIncludeFormatting = true, bool bForceLongFormat = false) const;

	typedef ON_SimpleArray<WCHAR*> CLBP_XMLSegmentedStream;

	bool WriteToSegmentedStream(CLBP_XMLSegmentedStream&, bool bIncludeFormatting = true, bool bForceLongFormat = false, bool bSortedProperties = false) const;
	bool WriteChildrenToSegmentedStream (CLBP_XMLSegmentedStream&, bool bIncludeFormatting = true, bool bForceLongFormat = false, bool bSortedProperties = false) const;
	static void	ReleaseSegmentedStream(CLBP_XMLSegmentedStream&);

public: // Utilities
	DWORD PropertyCount(void) const;
	DWORD ChildrenCount(void) const;
	bool  HasDefaultProperty(void) const; // DEPRECATED - Always returns true.
	int	  GetNestedDepth(void) const;

	DWORD ComputeCRC(void) const; // This function is a bug factory. There is probably a better way. Try to find it.

	// Use this when you are looking for a node that is only one child below - non-recursive, but fast.
	CLBP_XMLNode* GetNamedChild(const wchar_t* wszName) const;

	CLBP_XMLProperty* GetNamedProperty(const wchar_t* wszName) const;

	CLBP_XMLProperty& GetDefaultProperty(void) const;

	// Gets at nodes deep into the tree using a slash delimited path - ie "child/grandchild/great-grandchild"
	// There's no checking for multiple nodes with the same name at each level of the tree, so if you use this
	// stuff, you have to make sure you have unique node names at each level.

	// relative to the current node, use "/" as a separator
	__forceinline CLBP_XMLNode* GetNodeAtPath(const CLBPString& s) const	{ return const_cast<CLBP_XMLNode*>(this)->GetNodeAtPathImpl(s); }
	__forceinline CLBP_XMLNode* CreateNodeAtPath(const CLBPString& sPath)	{ return GetNodeAtPathImpl(sPath, true); }
	CLBPStringBV GetPathFromRoot(void) const;

public: // Iteration helpers
	class LBPEXPORT CChildIterator
	{
	public:
		CChildIterator(const CLBP_XMLNode* pParent);

	public:
		CLBP_XMLNode* GetNextChild(void);

	protected:
		CLBP_XMLNode* m_pCurrent;
	};

	class LBPEXPORT CPropertyIterator
	{
	public:
		CPropertyIterator(const CLBP_XMLNode* pParent, bool bSorted = false);
		~CPropertyIterator();

	public:
		__forceinline CLBP_XMLProperty* GetNextProperty();

	protected:
		//Iterator type
		bool m_bSorted;

		//Fast, out of order access
		CLBP_XMLProperty* m_pCurrent;

		//Slower, presorted access
		const CLBP_XMLNode* m_pParent;
#ifdef LBPRHLIB
		ON_ClassArray<CLBP_XMLProperty>* m_apSortedProperties;
#else
		CArray<CLBP_XMLProperty, CLBP_XMLProperty&>* m_apSortedProperties;
#endif
		int m_iIndex;

	private:
		CLBP_XMLProperty* GetNextPropertySorted();
	};

	CChildIterator GetChildIterator(void) const;
	CPropertyIterator GetPropertyIterator(bool bAlphabetized = false) const;

	static bool AutoTypePropValue(void) { return m_bAutoTypePropValue; }
	static void SetAutoTypePropValue(bool b=true) { m_bAutoTypePropValue = b; }

private:
	static bool m_bAutoTypePropValue;

private:
	bool GetPropertiesFromTag(const CLBPString& sTag);
	bool IsClosingTag(const CLBPString& sTag) const;
	void SetLastReadBufferPointer(LPVOID pv);
	CLBP_XMLNode* GetNodeAtPathImpl(const wchar_t* wszPath, bool bCreate=false); 
	void AddEmptyDefaultProperty(void);
	CLBP_XMLProperty* InternalAddProperty(const CLBP_XMLProperty& prop);
	bool InternalRemoveProperty(const CLBPString& sName);
	CLBP_XMLNode* InternalUnhookChild(CLBP_XMLNode& childNode);
	void InternalRemoveAllProperties(void);

private:
	static CLBPStringBV GetNameFromTag(const CLBPString& sTag);
	static void AssertValidTag(const CLBPString& sTag);
	static void AttemptToFixTag(CLBPString& tag);
	static void GetNextTag(CLBPString& tag, WCHAR*& pBuffer, bool bValidateTag);
	static bool RecoverProperty(const CLBPString& tag, int iEqualSign, CLBPString& sProp);

private:
	CLBPString_RC m_sName;
	CLBP_XMLNode* m_pNextSibling = nullptr;
	CLBP_XMLNode* m_pParent = nullptr;
	CLBP_XMLNode* m_pFirstChild = nullptr;
	CLBP_XMLNode* m_pLastChild = nullptr;
	CLBP_XMLProperty* m_pFirstProperty = nullptr;
	void* m_pLastReadBufferPointer = nullptr;
	mutable CCriticalSection m_CS;

#ifdef _DEBUG
public:
	virtual bool ReadOnly(void) const { return false; }
	virtual void SetReadOnly(bool b) const;
#endif

protected:
	WCHAR* m_wszBuffer = nullptr; // Here to keep the fixed allocator happy.

	friend class CChildIterator;
	friend class CPropertyIterator;
	friend class CLBP_XMLRootNode;
};

class LBPEXPORT CLBP_XMLRootNode : public CLBP_XMLNode
{
public:
	CLBP_XMLRootNode();
	CLBP_XMLRootNode(const CLBP_XMLRootNode&);
	virtual ~CLBP_XMLRootNode();

	CLBP_XMLRootNode& operator=(const CLBP_XMLRootNode&);

public: // To make it easier to write reference-counted-agnostic code.
#ifndef USE_REFERENCE_COUNTED_ROOTNODE
	      CLBP_XMLRootNode& Node(void)       { return *this; }
	const CLBP_XMLRootNode& Node(void) const { return *this; }
#endif

	void ReadFromFile(const WCHAR* szPath, bool bWarningsAsErrors = false, bool bValidateTags = true);	//Throws CLBP_XMLException
	void ReadFromFile(const char*  szPath, bool bWarningsAsErrors = false, bool bValidateTags = true);	//Throws CLBP_XMLException

	bool WriteToFile(const WCHAR* szPath, bool bIncludeFormatting = true, bool bUTF8 = false, bool bSortedProperties = false) const;
	bool WriteToFile(const char*  szPath, bool bIncludeFormatting = true, bool bUTF8 = false, bool bSortedProperties = false) const;
};

#ifdef USE_REFERENCE_COUNTED_ROOTNODE

class CLBP_XMLRootNode_RC;
class CLBP_XMLRootNode_RC_Data
{
	CLBP_XMLRootNode_RC_Data() : m_iRefCount(1) { }
	CLBP_XMLRootNode_RC_Data(const CLBP_XMLRootNode& n) : m_iRefCount(1), m_node(n) { }
	CLBP_XMLRootNode_RC_Data(const CLBP_XMLRootNode_RC_Data& data) : m_iRefCount(1), m_node(data.m_node) { }

	REFCOUNT_INT AddRef(void)
	{
		return InterlockedIncrement(&m_iRefCount);
	}

	REFCOUNT_INT Release(void)
	{	
		const REFCOUNT_INT iRef = InterlockedDecrement(&m_iRefCount);
		if (0 == iRef)
		{
			delete this;
		}
		return iRef;
	}

	volatile REFCOUNT_INT m_iRefCount;
	CLBP_XMLRootNode m_node;

	friend class CLBP_XMLRootNode_RC;

	const CLBP_XMLRootNode_RC& operator=(const CLBP_XMLRootNode_RC&);
};

class LBPEXPORT CLBP_XMLRootNode_RC
{
public:
	CLBP_XMLRootNode_RC();
	CLBP_XMLRootNode_RC(const CLBP_XMLRootNode_RC&);
	virtual ~CLBP_XMLRootNode_RC();

	CLBP_XMLRootNode& Node(void)		 { CopyOnWrite(); return m_pData->m_node; }
	const CLBP_XMLRootNode& NodeForRead(void) const { return m_pData->m_node; }
	CLBP_XMLRootNode& NodeForWrite(void) { CopyOnWrite(); return m_pData->m_node; }

	void Clear(void)                         { CopyOnWrite(); m_pData->m_node.Clear(); }
	void SetTagName(const CLBPString& sName) { CopyOnWrite(); m_pData->m_node.SetTagName(sName); }

	CLBP_XMLRootNode_RC& operator = (const CLBP_XMLRootNode_RC&);

	bool WriteToFile(const WCHAR* szPath, bool bIncludeFormatting = true, bool bUTF8 = false, bool bSortedProperties = false) const
		{ return m_pData->m_node.WriteToFile(szPath, bIncludeFormatting, bUTF8, bSortedProperties); }

	bool WriteToFile(const char*  szPath, bool bIncludeFormatting = true, bool bUTF8 = false, bool bSortedProperties = false) const
		{ return m_pData->m_node.WriteToFile(szPath, bIncludeFormatting, bUTF8, bSortedProperties); }

private:
	CLBP_XMLRootNode_RC_Data* m_pData;

	void CopyOnWrite(void)
	{
		if (m_pData->m_iRefCount > 1)
		{
			CLBP_XMLRootNode_RC_Data* pData = new CLBP_XMLRootNode_RC_Data(*m_pData);
			m_pData->Release();
			m_pData = pData;
		}
	}
};

#else

#define CLBP_XMLRootNode_RC CLBP_XMLRootNode

#endif

#include "LBPTime.h"
#include "LBPBuffer.h"

//Extra cool variant that handles conversions on the fly
//Getters will throw an exception if the conversion is not possible.

class LBPEXPORT CLBPVariant
{
public:
	enum vt
	{
			vt_Integer			= 6,
			vt_Float			= 16,
			vt_Double			= 17,
			vt_3_double_array	= 19,   // vector xyz 1 double per axis
			vt_String			= 20,
			vt_Bool				= 21,
			vt_Matrix			= 22,	// 4x4 double precision matrix
			vt_Null				= 23,
			vt_2_double_array	= 24,
			vt_4_double_color	= 25,
			vt_Uuid				= 26,
			vt_Time				= 27,
			vt_Buffer			= 28,
			vt_4_double_array	= 29

	};

	enum vt_arraytype
	{
		vt_array_2	=	2,
		vt_array_3	=	3,
		vt_array_4  =   4,
		vt_array_16 =	16,		
	};

public:
	CLBPVariant();	//creates an empty string type
	CLBPVariant(int iValue);
	CLBPVariant(double iValue);
	CLBPVariant(float iValue);
	CLBPVariant(const CLBPString& s);
	CLBPVariant(const double* point, vt_arraytype at = vt_array_3);
	CLBPVariant(const float* point, vt_arraytype at = vt_array_3);
	CLBPVariant(const WCHAR* wsz);
	CLBPVariant(const char* sz);
	CLBPVariant(const CLBPColor& c);
	CLBPVariant(bool  b);
	CLBPVariant(const UUID& uuid);
	CLBPVariant(const CTime& time);
	CLBPVariant(const void* pBuffer, size_t size);
	CLBPVariant(const CLBPBuffer& buffer);

	CLBPVariant(const CLBPVariant& src);
	virtual ~CLBPVariant();

	CLBPVariant& operator=(const CLBPVariant& src);

	DWORD CRC(void) const;

public:
	 vt				Type() const;
	 CLBPStringBV	TypeAsString() const;
	 bool			IsEmpty() const;
	 bool			IsNull() const;

	 //Optimized version of CLBPString::NeedsURLEncode - always use this if possible
	 //because it knows from the type of the variant whether URL encoded output is even possible.
	 bool			NeedsURLEncode(bool bForAnsi = false) const;

protected:
    vt m_type;
	mutable CLBPString_RC m_sVal;
	union
	{
		mutable int			m_iVal;
        mutable double      m_dVal;
		mutable double		m_aVal[4];
		mutable bool		m_bVal;
		mutable float		m_fVal;
		mutable double		m_matrix[16];
		mutable UUID		m_uuidVal;
		mutable time_t		m_timeVal;
	};
	mutable bool		m_bTypePending;
	mutable CLBPBuffer*	m_pBuffer;

	CLBPBuffer&			Buffer() const;
	void				BufferClear();

public:
	void	SetNull();
	void	SetValue( int v );
	void	SetValue( double v );
	void	SetValue( float v);
	void	SetValue( const CLBPString& string);
	void	SetValue( const double* p, vt_arraytype at);
	void	SetValue( const float* p, vt_arraytype at);
	void	SetValue( const CLBPColor& c);
	void	SetValue( bool b);
	void	SetValue( const UUID& uuid);
	void	SetValue( const CTime& time);
	void	SetValue( const void* pBuffer, size_t size);
	void	SetValue( const CLBPBuffer& buffer);

	int					AsInteger() const;
	double				AsDouble() const;
	float				AsFloat() const;
	const double*		As3dPoint() const;
	bool				As3dPoint(double* /*[3]*/) const;
	bool				As3dPoint(float* /*[3]*/) const;
	const double*		As4dPoint() const;
	bool				As4dPoint(double* /*[4]*/) const;
	bool				As4dPoint(float* /*[4]*/) const;
	const double*		As2dPoint() const;
	bool				As2dPoint(double* /*[2]*/) const;
	bool				As2dPoint(float* /*[2]*/) const;
	const CLBPString&	AsString() const;
	bool				AsBool() const;
	CLBPColor			AsColor() const;
	const double*		AsMatrix() const;
	bool				AsMatrix(double* /*[16]*/) const;
	bool				AsMatrix(float* /*[16]*/) const;
	UUID				AsUuid() const;
	CTime				AsTime() const;
	const CLBPBuffer	AsBuffer() const;

	operator double() const;
	operator float() const;
	operator int() const;
	operator const CLBPString&() const;
	operator const double*() const;	//unsafe - check the type first
	operator bool() const;
	operator CLBPColor() const;
	operator UUID() const;
	operator CTime() const;
	operator CLBPBuffer() const;

public:
	bool	TypePending() const;
	void	SetTypePendingFlag(bool bTypePending) const;

protected:
	void String2Point(int iValues) const;

	friend class CLBP_XMLNode;
	void DoAutoTyping(vt) const;
};

class LBPEXPORT CLBP_XMLPropertyData
{
public:
	CLBP_XMLPropertyData();	
	CLBP_XMLPropertyData(const CLBPVariant& sValue);
	CLBP_XMLPropertyData(const CLBPString& sName, const CLBPVariant& value);
	~CLBP_XMLPropertyData();

public:
	const char* NameMBCS(void) const;
	const CLBPString& Name(void) const;
	void SetName(const CLBPString& sName);
	DWORD CRC(void) const;
	bool operator < (const CLBP_XMLPropertyData& prop) const;
	bool operator > (const CLBP_XMLPropertyData& prop) const;
	bool IsDefaultProperty(void) const;
	const CLBPVariant& GetValue(void) const;
	void SetValue(const CLBPVariant& value);
	void SetHugeStringValue(wchar_t* wsz, int iLengthOfBuffer=-1);

private:
	CLBPString_RC m_sName;
	CLBPVariant   m_value;

#ifdef USE_REFERENCE_COUNTED_PROPERTY
	volatile REFCOUNT_INT m_iRefCount;

	REFCOUNT_INT AddRef()
	{
		return ::InterlockedIncrement(&m_iRefCount);
	}

	REFCOUNT_INT Release()
	{	
		const REFCOUNT_INT iRef = ::InterlockedDecrement(&m_iRefCount);
		if (0 == iRef)
		{
			delete this;
		}
		return iRef;
	}
#endif
	CLBP_XMLPropertyData(const CLBP_XMLPropertyData& src);

protected:
	void CommonCtor();
	const CLBP_XMLPropertyData& operator = (const CLBP_XMLPropertyData& d) { m_sName = d.m_sName; m_value = d.m_value; return *this; }
	friend class CLBP_XMLProperty;
};

#ifdef USE_REFERENCE_COUNTED_PROPERTY
	#define LBP_XMLPROPERTY_BASE
#else
	#define LBP_XMLPROPERTY_BASE  : public CLBP_XMLPropertyData
#endif

class CLBP_XMLProperty LBP_XMLPROPERTY_BASE
{
public:
#ifdef USE_REFERENCE_COUNTED_PROPERTY
	CLBP_XMLProperty() : m_pOwner(NULL), m_pNextProperty(NULL)
		{ m_pData = new CLBP_XMLPropertyData; }

	CLBP_XMLProperty(const CLBPVariant& sValue) : m_pOwner(NULL), m_pNextProperty(NULL)
		{ m_pData = new CLBP_XMLPropertyData(sValue); }

	CLBP_XMLProperty(const CLBPString& sName, const CLBPVariant& value) : m_pOwner(NULL), m_pNextProperty(NULL)
		{ m_pData = new CLBP_XMLPropertyData(sName, value); }

	CLBP_XMLProperty(const CLBP_XMLProperty& prop) : m_pOwner(NULL), m_pNextProperty(NULL)
		{ prop.m_pData->AddRef(); m_pData = prop.m_pData; }

	~CLBP_XMLProperty()	{ m_pData->Release(); }
#else
	CLBP_XMLProperty() : m_pOwner(NULL), m_pNextProperty(NULL) { }
	CLBP_XMLProperty(const CLBPVariant& sValue) : CLBP_XMLPropertyData(sValue), m_pOwner(NULL), m_pNextProperty(NULL) { }
	CLBP_XMLProperty(const CLBPString& sName, const CLBPVariant& value) : CLBP_XMLPropertyData(sName, value), m_pOwner(NULL), m_pNextProperty(NULL) { }
	CLBP_XMLProperty(const CLBP_XMLProperty& prop) : CLBP_XMLPropertyData(prop), m_pOwner(NULL), m_pNextProperty(NULL) { }
#endif

public:
#ifdef USE_REFERENCE_COUNTED_PROPERTY
	const char*		NameMBCS() const								{ return m_pData->NameMBCS(); }
	const CLBPString& Name() const									{ return m_pData->Name(); }
	void			SetName(const CLBPString& sName)				{ CopyOnWrite(); m_pData->SetName(sName); }
	DWORD			CRC(void) const									{ return m_pData->CRC(); }
	bool			operator < (const CLBP_XMLProperty& prop) const	{ return m_pData->operator < (*prop.m_pData); }
	bool			operator > (const CLBP_XMLProperty& prop) const	{ return m_pData->operator > (*prop.m_pData); }
	CLBP_XMLProperty& operator = (const CLBP_XMLProperty& prop);
	bool			IsDefaultProperty() const						{ return m_pData->IsDefaultProperty(); }
	const CLBPVariant& GetValue() const								{ return m_pData->GetValue(); }
	void			SetValue(const CLBPVariant& value)				{ CopyOnWrite(); m_pData->SetValue(value); }
	void			SetHugeStringValue(wchar_t* wsz, int iLen=-1)	{ CopyOnWrite(); m_pData->SetHugeStringValue(wsz, iLen); }
	CLBPVariant&	GetNonConstValue()								{ CopyOnWrite(); return m_pData->m_value; }
#else
	CLBPVariant&	GetNonConstValue()								{ return const_cast<CLBPVariant&>(GetValue()); }
#endif

private:
	friend class CLBP_XMLNode::CPropertyIterator;
	CLBP_XMLProperty* m_pNextProperty;
	CLBP_XMLNode* m_pOwner;

#ifdef USE_REFERENCE_COUNTED_PROPERTY
	CLBP_XMLPropertyData* m_pData;

	void CopyOnWrite()
	{
		ASSERT((NULL == m_pOwner) || !m_pOwner->ReadOnly());
		if (m_pData->m_iRefCount > 1)
		{
			CLBP_XMLPropertyData* pData = new CLBP_XMLPropertyData(*m_pData);
			m_pData->Release();
			m_pData = pData;
		}
	}
#endif

	friend class CLBP_XMLNode;
};

__forceinline CLBP_XMLProperty* CLBP_XMLNode::CPropertyIterator::GetNextProperty()
{
	if (m_bSorted) 
		return GetNextPropertySorted();

	CLBP_XMLProperty* p = m_pCurrent;
	if (NULL != p)
	{
		m_pCurrent = p->m_pNextProperty;
	}

	return p;
}
