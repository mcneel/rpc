#include "StdAfx.h"
#include "RpcEventWatcher.h"
#include "RpcMains.h"
#include "RpcDocument.h"
#include "RpcInstance.h"
#include "RpcObjectUserData.h"


CRpcEventWatcher::CRpcEventWatcher(void)
{
	m_bMergeDocument = false;
	m_bReferenceDocument = false;
	m_bOpeningDocument = false;
	m_bReadRpcData = false;
}

CRpcEventWatcher::~CRpcEventWatcher(void)
{
}

bool CRpcEventWatcher::IsMergingDocument(void) const
{
	return m_bMergeDocument;
}

bool CRpcEventWatcher::IsReferenceDocument(void) const
{
	return m_bReferenceDocument;
}

bool CRpcEventWatcher::IsOpeningDocument(void) const
{
	return m_bOpeningDocument;
}

void CRpcEventWatcher::SetReadingRpcData(void)
{
	m_bReadRpcData = true;
}

void CRpcEventWatcher::OnNewDocument(CRhinoDoc& doc)
{
	Mains().RpcDocument().Defaults();
}

void CRpcEventWatcher::OnBeginOpenDocument(CRhinoDoc& doc, const wchar_t* filename, BOOL bMerge, BOOL bReference)
{
	m_bMergeDocument = bMerge ? true : false;
	m_bReferenceDocument = bReference ? true : false;
	m_bOpeningDocument = true;
	m_bReadRpcData = false;
}

void CRpcEventWatcher::OnEndOpenDocument(CRhinoDoc& doc, const wchar_t* filename, BOOL bMerge, BOOL bReference)
{
	CRhinoObjectIterator it(doc, CRhinoObjectIterator::normal_or_locked_objects, CRhinoObjectIterator::active_and_reference_objects);

	for (auto pObject = it.First(); pObject; pObject = it.Next())
	{
		if(Mains().GetRPCInstanceTable().Lookup(pObject->Id()))
			continue;

		auto rpc = new CRpcInstance(doc, *pObject);

		if (!m_bReadRpcData)
			rpc->Replace(doc, true);
		else
			Mains().GetRPCInstanceTable().SetAt(pObject->Id(), rpc);
	}

	m_bOpeningDocument = false;

	if (!m_bReadRpcData)
	{
		Mains().RpcDocument().Defaults();
	}
}

void CRpcEventWatcher::OnAddObject(CRhinoDoc& doc, CRhinoObject& object)
{
	if (Mains().IsCopy())
	{
		if (Mains().GetRPCInstanceTable().Lookup(object.Id()))
			return;

		auto rpc = new CRpcInstance(doc, object);
		rpc->Replace(doc, true, &object);
	}
}

void CRpcEventWatcher::OnReplaceObject(CRhinoDoc & doc, CRhinoObject & old_object, CRhinoObject & new_object)
{
	const CRhinoObject& object = new_object.Attributes().m_uuid != ON_nil_uuid ? new_object : old_object;

	if (object.Attributes().GetUserData(CRpcObjectUserData::Id()))
		RhRdkCustomRenderMeshManager().OnRhinoObjectChanged(doc, &object);
}