#pragma once

class CRpcAvailBrowserDialog : public CDialog
{
	DECLARE_DYNAMIC(CRpcAvailBrowserDialog)

public:
	CRpcAvailBrowserDialog(CWnd* pParent = nullptr);
	virtual ~CRpcAvailBrowserDialog();

	BOOL OnInitDialog() override;
	BOOL PreTranslateMessage(MSG* pMsg) override;

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLinkClicked(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

protected:
	void DoDataExchange(CDataExchange* pDX) override;

	DECLARE_MESSAGE_MAP()

private:
	void setText();

private:
	CLinkCtrl           siteLink;
	CStatic             editText;
};

