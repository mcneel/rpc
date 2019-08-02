#include "StdAfx.h"
#include "RpcFileDlg.h"
#include "Resource.h"
#include "RpcUtilities.h"


CRpcFileDlg::CRpcFileDlg(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
                                     DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd)
: CLBPPreviewingFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{

}

CRpcFileDlg::~CRpcFileDlg(void)
{
}

BOOL CRpcFileDlg::OnInitDialog()
{
	CLBPPreviewingFileDialog::OnInitDialog();

		PreviewCheck().SetWindowText(_RhLocalizeString( L"Preview", 32810)); // RR #83932.

	return TRUE;
}

DWORD CRpcFileDlg::ResourceID(DWORD dwWhich) const
{
	switch (dwWhich)
	{
	case 1: return IDC_CHECK_PREVIEW;
	case 2:	return IDC_STATIC_PREVIEW;
	case 3: return IDC_PROGRESS1;
	}

	return IDD_FILE_DIALOG;
}

HINSTANCE CRpcFileDlg::ResourceInstance(void) const
{
	return AfxGetInstanceHandle();
}



BEGIN_MESSAGE_MAP(CRpcAdvancedFileDialog, CRpcFileDlg)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDCC1, OnAdvancedButtonClicked)
END_MESSAGE_MAP()

CRpcAdvancedFileDialog::CRpcAdvancedFileDialog(CWnd* pParent)
	:
	m_bAdvanced(false),
	CRpcFileDlg(true, L"*.rpc", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"RPC files (*.rpc)|*.rpc|All files (*.*)|*.*||", pParent)
{
}

BOOL CRpcAdvancedFileDialog::OnInitDialog()
{
	__super::OnInitDialog();

	const CRect rectNil(0, 0, 0, 0);

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	const DWORD dwStyle = WS_VISIBLE | WS_CHILD | WS_TABSTOP;
	m_button_advanced.Create(_RhLocalizeString( L"Ad&vanced...", 32811), dwStyle, rectNil, this, IDCC1);
	m_button_advanced.SetFont(GetFont());

	const HICON hIcon = (HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ACM), IMAGE_ICON, 32, 32, LR_SHARED);
	m_button_advanced.SetIcon(hIcon);

	SetAdvancedButtonPosition();

	return TRUE;
}

void CRpcAdvancedFileDialog::OnAdvancedButtonClicked()
{
	m_bAdvanced = true;

	::EndDialog(GetParent()->GetSafeHwnd(), IDCANCEL);
}

void CRpcAdvancedFileDialog::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	SetAdvancedButtonPosition();
}

void CRpcAdvancedFileDialog::SetAdvancedButtonPosition(void)
{
	CWnd* pWnd = GetDlgItem(ResourceID(1));
	if (NULL == pWnd)
		return;

	CRect rectW;
	pWnd->GetWindowRect(rectW);
	ScreenToClient(rectW);

	CRect rect;
	GetClientRect(rect);
	rect.top = rectW.bottom + 20;
	rect.bottom = rect.top + 60;
	rect.left = rectW.left;
	rect.right = rectW.right;
	m_button_advanced.MoveWindow(rect);
}
