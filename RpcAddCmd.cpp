
#include "StdAfx.h"
#include "RpcAddCmd.h"
#include "RpcObject.h"
#include "RpcInstance.h"
#include "RpcMains.h"
#include "RpcSelectDlg.h"
#include "RpcFileDlg.h"
#include "RpcUtilities.h"
#include "RpcEditDlg.h"

const wchar_t * CRpcAddCmd::EnglishCommandName()
{
	return L"RPC";
}

UUID CRpcAddCmd::CommandUUID()
{
	static const UUID uuid = 
	{
		// {49DABEB1-A89E-41a7-9020-D52526CB3AB7}
		0x49dabeb1, 0xa89e, 0x41a7, { 0x90, 0x20, 0xd5, 0x25, 0x26, 0xcb, 0x3a, 0xb7 }
	};

	return uuid;
}

CRhinoCommand::result CRpcAddCmd::RunRpcCommand(const CRhinoCommandContext& context)
{
	CRhinoDoc* pDoc = context.Document();
	if (!pDoc)
		return failure;

	pDoc->UnselectAll();
	
	CLBPString sRpc;
	if (!GetRpcFileName(*pDoc, sRpc))
		return cancel;

	CRpcInstance* rpc = new CRpcInstance(*pDoc, sRpc);
	if (!rpc->IsValid())
	{
		delete rpc;
		return failure;
	}

	CRhinoGetPoint gp;
	gp.SetCommandPrompt(_RhLocalizeString( L"RPC base point", 36075));
	if (CRhinoGet::point != gp.GetPoint())
		return cancel;

	const ON_3dPoint ptInsertion = gp.Point();

	CRhinoInstanceObject* pBlock = rpc->AddToDocument(*pDoc, ptInsertion);
	if (!pBlock)
		return cancel;

	pBlock->Select();

	const ON_Xform xfromBlock = pBlock->InstanceXform();

	ON_3dPoint ptOfRotation;
	ptOfRotation.Zero();
	ptOfRotation.Transform(xfromBlock);

	const double dScale = pBlock->BoundingBox().Diagonal().Length() / 2.0;

	ON_3dPoint ptFirstRef;
	ptFirstRef = ON_3dVector::UnitVector(1);
	ptFirstRef *= dScale;
	ptFirstRef.Transform(xfromBlock);

	CLBPString sRotateCmd;
	sRotateCmd.Format(L"_Rotate %g,%g,%g %g,%g,%g", ptOfRotation.x, ptOfRotation.y, ptOfRotation.z,
													ptFirstRef.x, ptFirstRef.y, ptFirstRef.z);

	RhinoApp().RunScript( context.m_rhino_doc_sn, sRotateCmd.Wide());
	
	IRhRdkCustomRenderMeshManager& crmm = ::RhRdkCustomRenderMeshManager();
	crmm.OnRhinoDocumentChanged(*pDoc);

	pDoc->Redraw();

	return success;
}

bool CRpcAddCmd::GetRpcFileName(CRhinoDoc& doc, CLBPString& sRpc)
{
	CWnd* pParentWnd = CWnd::FromHandle(RhinoApp().MainWnd());

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CRpcAdvancedFileDialog dlg(pParentWnd);

	CRpcSelectDlg dlgRpcSelect(doc, (const wchar_t*)dlg.GetPathName());
	if (dlgRpcSelect.DoModal() != IDOK)
	{
		return false;
	}

	CLBPString sSel = dlgRpcSelect.Selection();
	if (sSel.IsEmpty()) 
		return false;

	sRpc = sSel;
	return true;
}
