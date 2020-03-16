#include "stdafx.h"
#include <afxdlgs.h>
#include "resource.h"
#include "RpcAvailBrowserDialog.h"

IMPLEMENT_DYNAMIC(CRpcAvailBrowserDialog, CDialog)

CRpcAvailBrowserDialog::CRpcAvailBrowserDialog(CWnd* pParent)
	: CDialog(IDD_AVAIL_BROWSER, pParent)
{}

CRpcAvailBrowserDialog::~CRpcAvailBrowserDialog()
{}

BEGIN_MESSAGE_MAP(CRpcAvailBrowserDialog, CDialog)
	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_NOTIFY(NM_CLICK, IDC_DOWNLOADS, &CRpcAvailBrowserDialog::OnLinkClicked)
END_MESSAGE_MAP()

void CRpcAvailBrowserDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_DOWNLOADS, siteLink);
	DDX_Control(pDX, IDC_AVAILBROW_CONTENT, editText);
}

void CRpcAvailBrowserDialog::setText()
{
	wstring text = L"RPC plugin components missing!";
	text += L"\n\n- AVAIL Browser";
	text += L"\n\nThe ArchVision-AVAIL apps are\nrequired to access, download\nand manage the RPC content";
	text += L"\n\n\nDownload the AVAIL Browser";

	editText.SetWindowText(text.c_str());
}

BOOL CRpcAvailBrowserDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	setText();
	siteLink.SetWindowText(L"<a href=\"https://www.archvision.com/downloads\">www.archvision.com/downloads</a>");

	return TRUE;
}

BOOL CRpcAvailBrowserDialog::PreTranslateMessage(MSG* pMsg)
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

BOOL CRpcAvailBrowserDialog::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);
	CBrush* myBrush = new CBrush(GetSysColor(COLOR_WINDOW));

	pDC->SelectObject(myBrush);
	BOOL bRes = pDC->PatBlt(0, 0, rect.Width(), rect.Height(), PATCOPY);

	return bRes;
}

HBRUSH CRpcAvailBrowserDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID() != IDC_AVAILBROW_CONTENT)
		pDC->SetTextColor(GetSysColor(COLOR_WINDOW));

	pDC->SetBkColor(GetSysColor(COLOR_WINDOW));

	return (HBRUSH)GetStockObject(NULL_BRUSH);
}

void CRpcAvailBrowserDialog::OnLinkClicked(NMHDR* pNMHDR, LRESULT* pResult)
{
	PNMLINK pNMLink = (PNMLINK)pNMHDR;
	ShellExecuteW(NULL, L"open", pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);
	EndDialog(0);
	CDialog::OnClose();
}
