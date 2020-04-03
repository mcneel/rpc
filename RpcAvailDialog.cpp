#include "stdafx.h"
#include <afxdlgs.h>
#include "resource.h"
#include "RpcAvailDialog.h"

IMPLEMENT_DYNAMIC(CRpcAvailDialog, CDialog)

CRpcAvailDialog::CRpcAvailDialog(CWnd* pParent)
    : CDialog(IDD_AVAIL, pParent)
{}

CRpcAvailDialog::~CRpcAvailDialog()
{}

BEGIN_MESSAGE_MAP(CRpcAvailDialog, CDialog)
    ON_WM_CLOSE()
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()
    ON_NOTIFY(NM_CLICK, IDC_DOWNLOADS, &CRpcAvailDialog::OnLinkClicked)
END_MESSAGE_MAP()

void CRpcAvailDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_DOWNLOADS, siteLink);
    DDX_Control(pDX, IDC_AVAILBROW_CONTENT, editText);
}

void CRpcAvailDialog::setText()
{
    wstring text = L"RPC plugin components missing!";
    text += L"\n\n- AVAIL Browser";
    text += L"\n\nThe ArchVision-AVAIL apps are\nrequired to access, download\nand manage the RPC content";
    text += L"\n\n\nDownload the AVAIL Browser";

    editText.SetWindowText(text.c_str());
}

BOOL CRpcAvailDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    setText();
    siteLink.SetWindowText(L"<a href=\"https://www.archvision.com/downloads\">www.archvision.com/downloads</a>");

    return TRUE;
}

BOOL CRpcAvailDialog::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_ESCAPE)
        {
            EndDialog(0);
            CDialog::OnClose();

            return TRUE;
        }
    }

    return CWnd::PreTranslateMessage(pMsg);
}

BOOL CRpcAvailDialog::OnEraseBkgnd(CDC* pDC)
{
    CRect rect;
    GetClientRect(&rect);
    CBrush* myBrush = new CBrush(GetSysColor(COLOR_WINDOW));

    pDC->SelectObject(myBrush);
    BOOL bRes = pDC->PatBlt(0, 0, rect.Width(), rect.Height(), PATCOPY);

    return bRes;
}

HBRUSH CRpcAvailDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

    if (pWnd->GetDlgCtrlID() != IDC_AVAILBROW_CONTENT)
        pDC->SetTextColor(GetSysColor(COLOR_WINDOW));

    pDC->SetBkColor(GetSysColor(COLOR_WINDOW));

    return (HBRUSH)GetStockObject(NULL_BRUSH);
}

void CRpcAvailDialog::OnLinkClicked(NMHDR* pNMHDR, LRESULT* pResult)
{
    PNMLINK pNMLink = (PNMLINK)pNMHDR;
    ShellExecuteW(NULL, L"open", pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);
    EndDialog(0);
    CDialog::OnClose();
}
