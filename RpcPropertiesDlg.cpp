
#include "stdafx.h"
#include "RpcPropertiesDlg.h"
#include "RpcObject.h"
#include "RpcInstance.h"
#include "RpcClient.h"
#include "RPCPlugIn.h"
#include "RpcUtilities.h"
#include "LBPRhObjectSelection.h"
#include "RpcMains.h"

#define RPC_PARAM_CHANGED 005
#define UPDATE_UI 006

#pragma warning(disable : 4355) // 'this' used in base member initialization list.

CRpcPropertiesDlg::CRpcPropertiesDlg(CWnd*)
	:
	m_Resize(this),
	TRhinoPropertiesPanelPage<CRhinoDialog>(IDD, IDI_PROP_RPC, false)
{
	m_iRpcParamChangedTimer = 0;
	m_iUpdateUiTimer = 0;
	m_bSelectionChangeByUi = false;
	m_rcRpcUiWnd.SetRectEmpty();
}

void CRpcPropertiesDlg::KillUI(void)
{
	m_btMassEditButton.ShowWindow(SW_HIDE);

	m_rcRpcUiWnd.SetRectEmpty();
}

void CRpcPropertiesDlg::OnDestroy()
{
	Enable(false);
	UnRegister();
	KillUI();
	__base_class::OnDestroy();
}

const wchar_t* CRpcPropertiesDlg::EnglishTitle() const
{
	return L"RPC"; 
}

const wchar_t* CRpcPropertiesDlg::LocalTitle() const
{
	return _RhLocalizeString( L"RPC", 35155); 
}

BEGIN_MESSAGE_MAP(CRpcPropertiesDlg, __base_class)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_BN_CLICKED(ID_BUTTON_EDIT, OnButtonClickedEditMass)
END_MESSAGE_MAP()

void CRpcPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	__base_class::DoDataExchange(pDX);
	DDX_Control(pDX, ID_BUTTON_EDIT, m_btMassEditButton); 
}

BOOL CRpcPropertiesDlg::OnInitDialog() 
{
	__super::OnInitDialog();

	Register();
	Enable();

	m_Resize.Add(&m_btMassEditButton, CRhinoUiDialogItemResizer::resize_locktopleft | CRhinoUiDialogItemResizer::resize_locktopright);
	
	return TRUE;
}

void CRpcPropertiesDlg::UpdatePage(IRhinoPropertiesPanelPageEventArgs&)
{
	if (m_bSelectionChangeByUi)
		return;

	if (0 == m_iUpdateUiTimer)
	{
		m_iUpdateUiTimer = SetTimer(UPDATE_UI, 100, nullptr);
	}
}

bool CRpcPropertiesDlg::IncludeInNavigationControl(IRhinoPropertiesPanelPageEventArgs&) const
{
	CLBPRhObjectSelection sel(this, false);
	if (sel.Count() < 1)
		return false;

	for (auto pObject = sel.First(); pObject; pObject = sel.Next())
	{
		CRpcObject ro(pObject);
		if (ro.IsTagged())
			return true;
	}

	return false;
}

CRhinoCommand::result CRpcPropertiesDlg::RunScript(IRhinoPropertiesPanelPageEventArgs&)
{
	return CRhinoCommand::success;
}

void CRpcPropertiesDlg::UpdateParameterEditor(void)
{
	KillUI();

	CLBPRhObjectSelection sel(this, ON::instance_reference);
	if (sel.Count() < 1)
		return;

	ON_SimpleArray<const CRhinoObject*> aSelectedRpcs;

	for (auto pObject = sel.First(); pObject; pObject = sel.Next())
	{
		CRpcObject ro(pObject);
		if (ro.IsTagged())
		{
			aSelectedRpcs.Append(pObject);
		}
	}

	const int iSelected = aSelectedRpcs.Count();
	if (iSelected == 0)
	{
		return;
	}
	else if (iSelected == 1)
		{
			const auto pDoc = aSelectedRpcs[0]->Document();
			if (!pDoc)
				return;

			if (auto rpc = Mains().GetRPCInstanceTable().Lookup(uuid))
			{
				if (uuid != aSelectedRpcs[0]->Id())
					(*rpc)->KillEditUi();
			}

			uuid = aSelectedRpcs[0]->Id();
		}
	else
	{
		if (auto rpc = Mains().GetRPCInstanceTable().Lookup(uuid))
			(*rpc)->KillEditUi();
	}

	CreateRpcUI((iSelected > 1) ? true : false);

	SetControlPositionAndSize();
}

void CRpcPropertiesDlg::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	if (!::IsWindow(GetSafeHwnd()))
		return;

	SetControlPositionAndSize();
}

static const int iBorder = 15;

CRect CRpcPropertiesDlg::HackRpcUiRect(void)
{
	CRect rcOut;
	rcOut.SetRectEmpty();

	HWND hWnd = ::FindWindowEx(GetSafeHwnd(), nullptr, L"#32770", nullptr);
	CWnd* pWndUI = CWnd::FromHandle(hWnd);
	if (pWndUI)
	{
		CRect rc;
		pWndUI->GetClientRect(rc);

		rcOut.SetRect(0, 0, rc.Width(), rc.Height()+iBorder);
	}

	return rcOut;
}

void CRpcPropertiesDlg::SetControlPositionAndSize(void)
{
	if (!m_rcRpcUiWnd.IsRectEmpty() && !m_rcRpcUiWnd.IsRectNull())
	{
		CRect rcHolderUI;
		GetClientRect(rcHolderUI);

		if (rcHolderUI.Height() < m_rcRpcUiWnd.Height())
		{
			rcHolderUI.SetRect(0, 0, rcHolderUI.Width(), m_rcRpcUiWnd.Height());
			MoveWindow(rcHolderUI);
			//RecalcStackedDialogScrollBars();
		}
	}

	m_Resize.Resize(this);
}

void CRpcPropertiesDlg::CreateRpcUI(bool bMultipleSelection)
{
	if (bMultipleSelection)
	{
		m_btMassEditButton.ShowWindow(SW_SHOW);
	}
	else
	{
		if (auto rpc = *Mains().GetRPCInstanceTable().Lookup(uuid))
		{
			if (rpc->EditUi(GetSafeHwnd(), this))
				m_rcRpcUiWnd = HackRpcUiRect();
		}
	}
}

void CRpcPropertiesDlg::OnTimer(UINT_PTR nIDEvent) 
{
	if (RPC_PARAM_CHANGED == nIDEvent)
	{
		KillTimer(m_iRpcParamChangedTimer);
		m_iRpcParamChangedTimer = 0;
	}
	else
	if (UPDATE_UI == nIDEvent)
	{
		KillTimer(m_iUpdateUiTimer);
		m_iUpdateUiTimer = 0;
		UpdateParameterEditor();
	}
	
	__super::OnTimer(nIDEvent);
}

void CRpcPropertiesDlg::OnRpcParameterChanged()
{
	if (!IsWindow(GetSafeHwnd()))
		return;

	if (!IsWindowVisible())
		return;

	auto rpc = (*Mains().GetRPCInstanceTable().Lookup(uuid));

	if (!rpc)
		return;

	const auto pRhinoDoc = CRhinoDoc::FromRuntimeSerialNumber(rpc->Document());

	if (!pRhinoDoc)
		return;

	m_bSelectionChangeByUi = true;
	const CRhinoInstanceObject* pBlock = rpc->Replace(*pRhinoDoc);

	if (pBlock)
	{
		pBlock->Select();
		RhinoApp().RunScript(CRhinoDoc::NullRuntimeSerialNumber, L"SetRedrawOn");
	}

	m_bSelectionChangeByUi = false;
}

void CRpcPropertiesDlg::OnButtonClickedEditMass()
{
	RhinoApp().RunScript( CRhinoDoc::NullRuntimeSerialNumber, L"_RpcEdit");
	UpdateParameterEditor();
}

BOOL CRpcPropertiesDlg::PreTranslateMessage(MSG* pMsg) 
{
	if ((pMsg) && (WM_KEYDOWN == pMsg->message))
	{
		if (VK_RETURN == pMsg->wParam)
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);

			return TRUE;
		}
    }

	return __super::PreTranslateMessage(pMsg);
}

void CRpcPropertiesDlg::OnBeginCommand(const CRhinoCommand& command, const CRhinoCommandContext& context)
{
	UpdateParameterEditor();
}
