///////////////////////////////////////////////////////////////////////////////
// SampleTabbedDockBarDialog.cpp : implementation file

#include "stdafx.h"
#include "RpcSelectionDialog.h"
#include "RpcMains.h"
#include "RpcClient.h"
#include "RpcPropertiesDlg.h"
#include "RpcFileDlg.h"
#include "RpcUtilities.h"
#include "RPCPlugIn.h"
#include "RPCApp.h"

IMPLEMENT_DYNCREATE(CRpcSelectionDialog, CRhinoTabbedDockBarDialog)

CRpcSelectionDialog::CRpcSelectionDialog()
	: CRhinoTabbedDockBarDialog()
{
}

CRpcSelectionDialog::~CRpcSelectionDialog()
{
}

void CRpcSelectionDialog::DoDataExchange(CDataExchange* pDX)
{
	CRhinoTabbedDockBarDialog::DoDataExchange(pDX);
	DDX_Control(pDX, ID_BUTTON_PLACE, selectionButton);
	DDX_Control(pDX, IDC_TEXT, selectionText);
	
	selectionButton.SetWindowPos(CWnd::FromHandle(GetSafeHwnd()), x, buttonY, cxy, cxy, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREPOSITION);
	HICON hIcn = (HICON)LoadImage(
		AfxGetApp()->m_hInstance,
		MAKEINTRESOURCE(IDI_SEL),
		IMAGE_ICON,
		cxy, cxy, // use actual size
		LR_DEFAULTCOLOR
	);
	selectionButton.SetIcon(hIcn);
}

const wchar_t* CRpcSelectionDialog::Caption() const
{
	return L"RPC";
}

ON_UUID CRpcSelectionDialog::TabId() const
{
	return Id();
}

ON_UUID CRpcSelectionDialog::Id()
{
	// {E1BDB83A-6FD9-4C7D-87BE-9DC9AC79B578}
	static const GUID SampleObjectManagerDialog_UUID =
	{ 0xe1bdb83a, 0x6fd9, 0x4c7d, { 0x87, 0xbe, 0x9d, 0xc9, 0xac, 0x79, 0xb5, 0x78 } };
	return SampleObjectManagerDialog_UUID;
}

ON_UUID CRpcSelectionDialog::PlugInId() const
{
	return PlugIn().PlugInID();
}

BOOL CRpcSelectionDialog::OnInitDialog()
{
	CRhinoTabbedDockBarDialog::OnInitDialog();

	CRhinoEventWatcher::Register();
	Enable();

	return TRUE;
}

void CRpcSelectionDialog::OnBeginCommand(const CRhinoCommand & command, const CRhinoCommandContext & context)
{
	if (wcscmp(command.EnglishCommandName(), L"RPCSelect") != 0)
		return;

	OnButtonClickedSelection();
}

bool CRpcSelectionDialog::OpenPanelInDockBarWithOtherPanel(CRhinoDoc& doc, const ON_UUID& panelToOpen, const ON_UUID& otherPanel, bool makeSelectedPanel)
{
	ON_SimpleArray<CRhinoUiDockBar*> dockbars;
	CRhinoTabbedDockBarDialog::DockBarsForTab(doc, otherPanel, dockbars);

	if (!dockbars)
		return true;

	return CRhinoTabbedDockBarDialog::OpenTabOnDockBar(dockbars[0], doc, panelToOpen, makeSelectedPanel);
}

BEGIN_MESSAGE_MAP(CRpcSelectionDialog, CRhinoTabbedDockBarDialog)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(ID_BUTTON_PLACE, OnButtonClickedSelection)
END_MESSAGE_MAP()

BOOL CRpcSelectionDialog::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);

	//color of the selection
	CBrush* myBrush = new CBrush(GetSysColor(COLOR_3DFACE));

	if (!coloured)
	{
		//color of the panel
		ON_Color tabBkColor = RhinoApp().AppSettings().UiPaintColorSettings().m_tab_background;
		myBrush = new CBrush(tabBkColor);
	}

	pDC->SelectObject(myBrush);
	BOOL bRes = pDC->PatBlt(0, 0, rect.Width(), rect.Height(), PATCOPY);
	
	return bRes;
}

void CRpcSelectionDialog::OnButtonClickedSelection()
{
	CRhinoTabbedDockBarDialog::ShowDockbarTab(*RhinoApp().ActiveDoc(), Id(), true, true, &uuidPanelObjectProps);
	CWnd* pParentWnd = CWnd::FromHandle(RhinoApp().MainWnd());
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CRpcAdvancedFileDialog dlg(pParentWnd);
	CRpcInstance* m_pRpcInstance = new CRpcInstance(*RhinoApp().ActiveDoc(), (const wchar_t*)dlg.GetPathName());

	if (Mains().GetSelectedId())
		m_pRpcInstance->SetId(Mains().GetSelectedId());

	selectionText.ShowWindow(SW_HIDE);
	selectionButton.EnableWindow(false);
	coloured = true;
	Invalidate();
	Add(*m_pRpcInstance);
}

void CRpcSelectionDialog::SetPosition()
{
	CWnd *parent = GetWindow(GW_CHILD);
	while (parent)
	{
		CWnd *child = parent->GetWindow(GW_CHILD);

		if (child)
		{
			parent->SetWindowPos(CWnd::FromHandle(GetSafeHwnd()), x, selectionY, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREPOSITION);
			return;
		}

		parent = parent->GetNextWindow(GW_HWNDNEXT);
	}
}

void CRpcSelectionDialog::Add(CRpcInstance & m_pRpcInstance)
{
	m_pRpcInstance.EditSelUI(GetSafeHwnd(), this);
	SetPosition();
	CRhinoGetPoint gp;
	gp.SetCommandPrompt(_RhLocalizeString(L"RPC base point", 36075));
	if (CRhinoGet::point != gp.GetPoint())
	{
		m_pRpcInstance.KillEditSelUi();
		coloured = false;
		Invalidate();
		selectionText.ShowWindow(SW_SHOW);
		selectionButton.EnableWindow(true);
		RhinoApp().ActiveDoc()->UnselectAll();
		return;
	}

	const ON_3dPoint ptInsertion = gp.Point();
	CRpcInstance* rpc = new CRpcInstance(*RhinoApp().ActiveDoc(), m_pRpcInstance.FileName());
	CRhinoInstanceObject* pBlock = rpc->AddToDocument(*RhinoApp().ActiveDoc(), ptInsertion);
	if (!pBlock)
		return;

	RhinoApp().ActiveDoc()->UnselectAll();
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

	RhinoApp().RunScript(RhinoApp().ActiveDoc()->TargetDocSerialNumber(), sRotateCmd.Wide());
	Mains().SetSelectedId(m_pRpcInstance.Id());
	Add(m_pRpcInstance);
	Mains().SetIsCopy(false);
}
