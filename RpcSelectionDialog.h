#pragma once
#include "resource.h"
#include "RpcInstance.h"

class CRpcSelectionDialog : public CRhinoTabbedDockBarDialog, public CRpcInstance::IEditDialogCallback, public CRhinoEventWatcher
{
	DECLARE_DYNCREATE(CRpcSelectionDialog)

public:
	CRpcSelectionDialog();
	virtual ~CRpcSelectionDialog();

	static ON_UUID Id();
	static bool OpenPanelInDockBarWithOtherPanel(CRhinoDoc & doc, const ON_UUID & panelToOpen, const ON_UUID & otherPanel, bool makeSelectedPanel);

	// Required overrides
	const wchar_t* Caption() const override;
	ON_UUID TabId() const override;
	ON_UUID PlugInId() const override;
	enum { IDD = IDD_SELECTION};

private:
	void DoDataExchange(CDataExchange* pDX) override;
	BOOL OnInitDialog() override;
	void OnBeginCommand(const CRhinoCommand& command,const CRhinoCommandContext& context) override;
	// Inherited via IEditDialogCallback
	void OnRpcParameterChanged(void) override {};

	DECLARE_MESSAGE_MAP()
	afx_msg void OnButtonClickedSelection();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	void SetPosition();
	void Add(CRpcInstance& m_pRpcInstance);

private:
	CButton selectionButton;
	CStatic selectionText;
	bool coloured = false;

	//data to place the button and selection
	const int cxy = 32;
	const int x = 3;
	const int buttonY = 5;
	const int selectionY = 40;

};
