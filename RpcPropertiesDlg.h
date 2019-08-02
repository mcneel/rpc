
#pragma once

#include "resource.h"
#include "RpcEventSink.h"
#include "RpcInstance.h"

class CRpcPropertiesDlg : public TRhinoPropertiesPanelPage<CRhinoDialog>,
						  public CRpcInstance::IEditDialogCallback,
						  public CRhinoEventWatcher
{
public:
	CRpcPropertiesDlg(CWnd* pParent = nullptr);

private:
	enum { IDD = IDD_EDIT};

private:
	void SetControlPositionAndSize(void);
	void UpdateParameterEditor(void);
	void KillUI(void);
	void CreateRpcUI(bool bMultipleSelection);
	CRect HackRpcUiRect(void);

private:
	virtual void OnRpcParameterChanged(void);

protected:
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

	RHINO_PAGE_DECLARE
	RHINO_PROPERTIES_PANEL_PAGE_DECLARE

	virtual int Index(void) const override { return 7176; }
	virtual RhinoPropertiesPanelPageType PageType() const override { return RhinoPropertiesPanelPageType::Custom; }
	virtual void OnBeginCommand(const CRhinoCommand& command, const CRhinoCommandContext& context) override;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnButtonClickedEditMass();
	afx_msg void OnSize(UINT nType, int cx, int cy);

private:
	CRpcInstance* m_pRpcInstance;
	UINT_PTR m_iRpcParamChangedTimer;
	UINT_PTR m_iUpdateUiTimer;
	CButton m_btMassEditButton;
	CRhinoUiDialogItemResizer m_Resize;
	bool m_bSelectionChangeByUi;
	CRect m_rcRpcUiWnd;
};
