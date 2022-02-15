#pragma once

class RpcAboutDialog : public CDialog
{
    DECLARE_DYNAMIC(RpcAboutDialog)

public:
    RpcAboutDialog(CWnd* pParent = nullptr);
    virtual ~RpcAboutDialog();

    BOOL OnInitDialog() override;
    BOOL PreTranslateMessage(MSG* pMsg) override;
    
    wstring getVersionString();

public:
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnLinkClicked(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

protected:
    void DoDataExchange(CDataExchange* pDX) override;

    DECLARE_MESSAGE_MAP()

private:
    wstring getDllVersion(const wchar_t* path);
    void setVersion();

private:
    CLinkCtrl           siteLink;
    CStatic             editText;
};

