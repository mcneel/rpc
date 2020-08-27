#include "StdAfx.h"
#include "RpcEventWatcher.h"
#include "RpcMains.h"
#include "RpcObject.h"
#include "RpcDocument.h"
#include "RpcInstance.h"
#include "RPCPlugIn.h"
#include "RpcObjectUserData.h"


CRpcEventWatcher::CRpcEventWatcher(void) : CRhinoOnTransformObject(PlugIn().PlugInID())
{
	m_bMergeDocument = false;
	m_bReferenceDocument = false;
	m_bOpeningDocument = false;
	m_bReadRpcData = false;
	isCopy = false;
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
        if (!CRpcObject(pObject).IsTagged())
            continue;

		if (Mains().GetRPCInstanceTable().Lookup(pObject->Id()))
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
    if (!CRpcObject(&object).IsTagged())
        return;

	if (Mains().GetRPCInstanceTable().Lookup(object.Id()))
		return;

	if (isCopy)
	{
		auto rpc = new CRpcInstance(doc, object);
		rpc->Replace(doc, true, &object);
		isCopy = false;
	}
	else if (doc.ActiveCommand() && wcscmp(doc.ActiveCommand()->EnglishCommandName(), L"Paste") != 0)
	{
		auto rpc = new CRpcInstance(doc, object);
		Mains().GetRPCInstanceTable().SetAt(object.Id(), rpc);
	}
}

void CRpcEventWatcher::OnDeleteObject(CRhinoDoc& doc, CRhinoObject& object)
{
	if (doc.ActiveCommand() && wcscmp(doc.ActiveCommand()->EnglishCommandName(), L"Delete") != 0)
		return;

	const CRhinoInstanceObject* pBlock = CRhinoInstanceObject::Cast(&object);

	if (!pBlock)
		return;

	CRhinoLayerTable& layerTable = doc.m_layer_table;
	int objIndex = object.Attributes().m_layer_index;

	if (layerTable.DeleteLayer(objIndex, false));
	Mains().GetRPCInstanceTable().Remove(object.Id());

	if (Mains().GetRPCInstanceTable().Count() == 0)
	{
		constexpr int NotFoundIndex = -2;
		constexpr int MultipleFoundIndex = -1;
		constexpr auto RpcLayer = L"RPC Assets";
		int assetIndex = layerTable.FindLayerFromName(RpcLayer, false, false, NotFoundIndex, MultipleFoundIndex);
		layerTable.DeleteLayer(assetIndex, true);
	}
}

void CRpcEventWatcher::OnReplaceObject(CRhinoDoc & doc, CRhinoObject & old_object, CRhinoObject & new_object)
{
	const CRhinoObject& object = new_object.Attributes().m_uuid != ON_nil_uuid ? new_object : old_object;

	if (object.Attributes().GetUserData(CRpcObjectUserData::Id()))
		RhRdkCustomRenderMeshManager().OnRhinoObjectChanged(doc, &object);
}

void CRpcEventWatcher::OnUnDeleteObject(CRhinoDoc& doc, CRhinoObject& object)
{
	auto rpc = new CRpcInstance(doc, object);
	Mains().GetRPCInstanceTable().SetAt(object.Id(), rpc);
}

void CRpcEventWatcher::Notify(const CRhinoOnTransformObject::CParameters& params)
{
	isCopy = params.m_bCopy;
}
