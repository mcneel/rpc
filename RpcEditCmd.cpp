#include "StdAfx.h"
#include "RpcEditCmd.h"
#include "RpcObject.h"
#include "RpcInstance.h"
#include "RpcEditDlg.h"
#include "RpcUtilities.h"


const wchar_t * CRpcEditCmd::EnglishCommandName()
{
	return L"RPCEdit";
}

UUID CRpcEditCmd::CommandUUID()
{
	// {4386730C-5C6A-4427-B182-B31EB6F592B3}
	static const GUID id = 
	{ 0x4386730c, 0x5c6a, 0x4427, { 0xb1, 0x82, 0xb3, 0x1e, 0xb6, 0xf5, 0x92, 0xb3 } };
	return id;
}

bool CRpcEditCmd::GetRpcBlock(const CRhinoDoc& doc, ON_SimpleArray<const CRhinoInstanceObject*>& aBlock)
{
	CRhinoGetObject GetBlock;
	GetBlock.AcceptNothing();
	GetBlock.SetGeometryFilter(CRhinoGetObject::instance_reference);
	GetBlock.SetCommandPrompt(_RhLocalizeString( L"Select RPC(s)", 36077));
	GetBlock.EnableSubObjectSelect(false);
	GetBlock.EnablePreSelect(TRUE);
	GetBlock.GetObjects(1,0);
	if(CRhinoGet::object != GetBlock.Result())
		return false;

	for(int i=0; i<GetBlock.ObjectCount(); i++)
	{
		const CRhinoInstanceObject* pBlock = CRhinoInstanceObject::Cast(doc.LookupObject(GetBlock.Object(i).ObjectUuid()));
		if (NULL != pBlock)
		{
			aBlock.Append(pBlock);
		}
	}

	return (aBlock.Count() > 0) ? true : false;
}

CRhinoCommand::result CRpcEditCmd::RunRpcCommand(const CRhinoCommandContext& context)
{
	CRhinoDoc* pDoc = context.Document();
	if (NULL == pDoc)
		return failure;

	ON_SimpleArray<const CRhinoInstanceObject*> aBlock;
	if (!GetRpcBlock(*pDoc, aBlock))
		return cancel;

	ON_SimpleArray<CRpcInstance*> aRpc;
	ON_SimpleArray<int> aBlockIndex;
	for(int i=0; i<aBlock.Count(); i++)
	{
		CRpcInstance* pRpc = new CRpcInstance(*pDoc, *aBlock[i]);
		if (pRpc->IsValid())
		{
			RPCapi::Mesh* pRpcMesh = pRpc->Instance()->getEditMesh();
			if (NULL != pRpcMesh)
			{
				delete pRpcMesh;
				aRpc.Append(pRpc);
				aBlockIndex.Append(i);
			}
		}
	}

	if (aRpc.Count() <= 0)
	{
		RhinoApp().Print(_RhLocalizeString( L"No valid RPC(s) selected\n", 36078));
		return cancel;
	}
	
	CRpcEditDlg dlg(*pDoc, aRpc);
	if (!dlg.Edit())
	{
		for (int i=0; i<aRpc.Count(); i++)
		{
			delete aRpc[i];
		}

		return cancel;
	}

	for(int i=0; i<aRpc.Count(); i++)
	{
		CRhinoInstanceObject* pInstObj = aRpc[i]->Replace(*pDoc);
		if (NULL != pInstObj)
		{
			pInstObj->Select(true);
		}
	}
	
	for (int i=0; i<aRpc.Count(); i++)
	{
		delete aRpc[i];
	}

	IRhRdkCustomRenderMeshManager& crmm = ::RhRdkCustomRenderMeshManager();
	crmm.OnRhinoDocumentChanged(*pDoc);
	
	pDoc->Redraw();

	return success;
}
