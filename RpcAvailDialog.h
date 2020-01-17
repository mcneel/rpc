#pragma once

class RpcAvailDialog : public CDialog
{
	DECLARE_DYNAMIC(RpcAvailDialog)

public:
    RpcAvailDialog(CWnd* pParent = nullptr);
	virtual ~RpcAvailDialog();

	BOOL OnInitDialog() override;
	BOOL PreTranslateMessage(MSG* pMsg) override;

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLinkClicked(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

protected:
	void DoDataExchange(CDataExchange* pDX) override;

	DECLARE_MESSAGE_MAP()

private:
	CLinkCtrl           editLink;
};

