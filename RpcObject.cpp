#include "StdAfx.h"
#include "RpcObject.h"
#include "RpcObjectUserDataWrapper.h"


CLBPRhWrapperMap<CRpcObjectUserDataWrapper> g_mapRpcObject;

CRpcObject::CRpcObject(const CRhinoObject * pRhinoObject)
{
	Construct(pRhinoObject);
}

CRpcObject::~CRpcObject(void)
{
	if (NULL != m_pWrapperDirect)
	{
		delete m_pWrapperDirect->OriginalObject();
	}

	delete m_pWrapper;
	delete m_pWrapperDirect;
	delete m_pDefaultObject;
}

void CRpcObject::Construct(const CRhinoObject* pObject)
{
	m_pWrapperDirect = NULL;

	if (NULL == pObject)
	{
		m_pDefaultObject = new CRhinoPointObject;
		m_pWrapper = new CRpcObjectUserDataWrapperRef(g_mapRpcObject, m_pDefaultObject, lwt_attributes);	
	}
	else
	{
		m_pDefaultObject = NULL;
		m_pWrapper = new CRpcObjectUserDataWrapperRef(g_mapRpcObject, pObject, lwt_attributes);
	}
}

CRpcObjectUserDataWrapper& CRpcObject::Wrapper() 
{
	if(NULL != m_pWrapperDirect)
	{
		return *m_pWrapperDirect;
	}

	return m_pWrapper->Wrapper();
}

const CRpcObjectUserDataWrapper& CRpcObject::Wrapper() const 
{
	if(NULL != m_pWrapperDirect)
	{
		return *m_pWrapperDirect;
	}

	return m_pWrapper->Wrapper();
}

UUID CRpcObject::ObjectId(void) const
{
	return Wrapper().OriginalUuid();
}

bool CRpcObject::IsValid() const
{
	return (Wrapper().OriginalObject() == NULL) ? false : true;
}

bool CRpcObject::Tag(const CLBPBuffer& buf)
{
	if (IsTagged())
	{
		Untag();
	}

	CRpcObjectUserData* pData = Wrapper().UserDataToModify();
	if (NULL == pData) return false;

	pData->SetRpcData(buf);

	return true;
}

bool CRpcObject::Untag(void)
{
	return Wrapper().RemoveData();
}

bool CRpcObject::IsTagged(void) const
{
	return  Wrapper().UserDataPresent();
}

bool CRpcObject::RpcData(CLBPBuffer& buf) const
{
	if (!IsTagged())
		return false;

	const CRpcObjectUserData* pData = Wrapper().UserData();
	if (NULL == pData) return false;

	pData->RpcData(buf);

	return true;	
}

void CRpcObject::SetRpcData(const CLBPBuffer& buf)
{
	if (!IsTagged())
		return;

	CRpcObjectUserData* pData = Wrapper().UserDataToModify();
	if (NULL == pData) return;

	pData->SetRpcData(buf);
}
