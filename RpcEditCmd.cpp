#include "StdAfx.h"
#include "RpcEditCmd.h"
#include "RpcObject.h"
#include "RpcInstance.h"
#include "RpcEditDlg.h"
#include "RpcUtilities.h"
#include "RpcMains.h"


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

bool CRpcEditCmd::GetRpcBlock(const CRhinoDoc& doc, ON_SimpleArray<ON_UUID>& aBlock)
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

	for (int i = 0; i < GetBlock.ObjectCount(); i++)
	{
		aBlock.Append(GetBlock.Object(i).ObjectUuid());
	}

	return (aBlock.Count() > 0) ? true : false;
}

CRhinoCommand::result CRpcEditCmd::RunRpcCommand(const CRhinoCommandContext& context)
{
	CRhinoDoc* pDoc = context.Document();
	if (NULL == pDoc)
		return failure;

	ON_SimpleArray<ON_UUID> aBlock;
	if (!GetRpcBlock(*pDoc, aBlock))
		return cancel;

	if (aBlock.Count() <= 0)
	{
		RhinoApp().Print(_RhLocalizeString( L"No valid RPC(s) selected\n", 36078));
		return cancel;
	}
	
	CRpcEditDlg dlg(*pDoc, aBlock);
	if (!dlg.Edit())
	{
		return cancel;
	}

	for(int i=0; i < aBlock.Count(); i++)
	{
		auto rpc = (*Mains().GetRPCInstanceTable().Lookup(aBlock[i]));
		const CRhinoInstanceObject* pBlock = rpc->Replace(*pDoc);
		
		if (pBlock)
		{
			rpc->CopyFromRpc(*pBlock);
			pBlock->Select();
		}
	}

	IRhRdkCustomRenderMeshManager& crmm = ::RhRdkCustomRenderMeshManager();
	crmm.OnRhinoDocumentChanged(*pDoc);

	pDoc->Redraw();

	return success;
}
