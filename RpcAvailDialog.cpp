#include "stdafx.h"
#include <afxdlgs.h>
#include "resource.h"
#include "RpcAvailDialog.h"

IMPLEMENT_DYNAMIC(RpcAvailDialog, CDialog)

RpcAvailDialog::RpcAvailDialog(CWnd* pParent)
	: CDialog(IDD_AVAIL, pParent)
{}

RpcAvailDialog::~RpcAvailDialog()
{}

BEGIN_MESSAGE_MAP(RpcAvailDialog, CDialog)
	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_NOTIFY(NM_CLICK, IDC_DOWNLOADS, &RpcAvailDialog::OnLinkClicked)
END_MESSAGE_MAP()

void RpcAvailDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_DOWNLOADS, editLink);
}

BOOL RpcAvailDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_DOWNLOADS)->SetWindowText(
		L"<a href=\"https://www.archvision.com/downloads\">www.archvision.com/downloads</a>");

	return TRUE;
}

BOOL RpcAvailDialog::PreTranslateMessage(MSG * pMsg)
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

BOOL RpcAvailDialog::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);
	CBrush* myBrush = new CBrush(GetSysColor(COLOR_WINDOW));

	pDC->SelectObject(myBrush);
	BOOL bRes = pDC->PatBlt(0, 0, rect.Width(), rect.Height(), PATCOPY);

	return bRes;
}

HBRUSH RpcAvailDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID() != IDC_TEXT &&
		pWnd->GetDlgCtrlID() != IDC_TEXT2 &&
		pWnd->GetDlgCtrlID() != IDC_TEXT3 &&
		pWnd->GetDlgCtrlID() != IDC_TEXT4 &&
		pWnd->GetDlgCtrlID() != IDC_TEXT5)
		pDC->SetTextColor(GetSysColor(COLOR_WINDOW));
	else if(pWnd->GetDlgCtrlID() == IDC_TEXT5)
		pDC->SetTextColor(RGB(255,0,0));

	pDC->SetBkColor(GetSysColor(COLOR_WINDOW));

	return (HBRUSH)GetStockObject(NULL_BRUSH);
}

void RpcAvailDialog::OnLinkClicked(NMHDR* pNMHDR, LRESULT* pResult)
{
	PNMLINK pNMLink = (PNMLINK)pNMHDR;
	ShellExecuteW(NULL, L"open", pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);
	EndDialog(0);
	CDialog::OnClose();
}
