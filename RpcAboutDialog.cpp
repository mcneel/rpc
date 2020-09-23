#include "stdafx.h"
#include <afxdlgs.h>
#include "resource.h"
#include "RpcAboutDialog.h"
#include "RPCPlugIn.h"

IMPLEMENT_DYNAMIC(RpcAboutDialog, CDialog)

RpcAboutDialog::RpcAboutDialog(CWnd* pParent)
    : CDialog(IDD_ABOUT, pParent)
{}

RpcAboutDialog::~RpcAboutDialog()
{}

BEGIN_MESSAGE_MAP(RpcAboutDialog, CDialog)
    ON_WM_CLOSE()
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()
    ON_NOTIFY(NM_CLICK, IDC_ARCHSITE, &RpcAboutDialog::OnLinkClicked)
END_MESSAGE_MAP()

void RpcAboutDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_ARCHSITE, siteLink);
    DDX_Control(pDX, IDC_ABOUT_TEXT, editText);
}

wstring RpcAboutDialog::getDllVersion(const wchar_t* path)
{
    wstring version = L"1.0.0.0";

    DWORD handle = 0;
    DWORD verSize = GetFileVersionInfoSize(path, &handle);
    if (!verSize)
        return version;

    std::vector<uint8_t> verInfoBuffer(verSize);
    if (!GetFileVersionInfo(path, handle, verSize, verInfoBuffer.data()))
        return version;

    LPBYTE lpBuffer = nullptr;
    UINT size = 0;
    if (!VerQueryValueW(verInfoBuffer.data(), L"\\", (LPVOID*)&lpBuffer, &size)
        || !size)
        return version;

    VS_FIXEDFILEINFO* verInfo = (VS_FIXEDFILEINFO*)lpBuffer;
    if (verInfo->dwSignature == 0xfeef04bd)
    {
        version =
            to_wstring((verInfo->dwFileVersionMS >> 16) & 0xffff) + L"." +
            to_wstring((verInfo->dwFileVersionMS >> 0) & 0xffff) + L"." +
            to_wstring((verInfo->dwFileVersionLS >> 16) & 0xffff) + L"." +
            to_wstring((verInfo->dwFileVersionLS >> 0) & 0xffff);
    }
    return version;
}

void RpcAboutDialog::setVersion()
{
    wstring text = L"RPC Plugin " + getDllVersion(PlugIn().PlugInFileName())
        + L"\n\nRPC Render API\nVersion " + getDllVersion(PlugIn().getRpcApiFilename())
        + L"\n\nCopyright © 1998-2020\nArchVision Inc."
        + L"\n\nCheck out more information at";

    editText.SetWindowText(text.c_str());
}

BOOL RpcAboutDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    setVersion();
    siteLink.SetWindowText(L"<a href=\"https://www.archvision.com\">www.archvision.com</a>");

    return TRUE;
}

BOOL RpcAboutDialog::PreTranslateMessage(MSG * pMsg)
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

BOOL RpcAboutDialog::OnEraseBkgnd(CDC* pDC)
{
    CRect rect;
    GetClientRect(&rect);
    CBrush* myBrush = new CBrush(GetSysColor(COLOR_WINDOW));

    pDC->SelectObject(myBrush);
    BOOL bRes = pDC->PatBlt(0, 0, rect.Width(), rect.Height(), PATCOPY);

    return bRes;
}

HBRUSH RpcAboutDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

    if (pWnd->GetDlgCtrlID() != IDC_ABOUT_TEXT)
        pDC->SetTextColor(GetSysColor(COLOR_WINDOW));

    pDC->SetBkColor(GetSysColor(COLOR_WINDOW));

    return (HBRUSH)GetStockObject(NULL_BRUSH);
}

void RpcAboutDialog::OnLinkClicked(NMHDR* pNMHDR, LRESULT* pResult)
{
    PNMLINK pNMLink = (PNMLINK)pNMHDR;
    ShellExecuteW(NULL, L"open", pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);
    EndDialog(0);
    CDialog::OnClose();
}
