
#include "stdafx.h"

#if !defined LBPLIB_DISABLE_MFC_CONTROLS

#include "LBPPreviewingFileDialog.h"
#include "LBPBitmapPreview.h"
#include "LBPPidlMgr.h"
#include "LBPLibUtilities.h"
#include "Resource.h"

bool CLBPPreviewingFileDialog::m_bShowPreview = true;

// 3 Feb. 2011 S. Baer
// This dialog will throw exceptions under VC2010 if bVistaStyle is set to true (which is the default unless overridden).
// Had to call a different constructor which is only available in VC2010 to get things to work.
CLBPPreviewingFileDialog::CLBPPreviewingFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
								   DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd)
#if defined(_MFC_VER) && _MFC_VER >= 0x0A00
:	CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd, 0, FALSE) // bVistaStyle = FALSE; dialog will downgrade on Vista upwards.
#else
:	CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
#endif
{
	m_ofn.Flags |= OFN_ENABLETEMPLATE | OFN_ENABLESIZING;

	m_pPreview = new CLBPBitmapPreview;

	m_iPreviewSize = 128;

#ifdef _DEBUG
	for (int i = 0; i < 4; i++)
	{
		//Set these up to bad values just to help debugging
		m_iControls[i] = -1;
	}
#endif
}

CLBPPreviewingFileDialog::~CLBPPreviewingFileDialog()
{
	delete m_pPreview;
}

INT_PTR CLBPPreviewingFileDialog::DoModal()
{
	m_ofn.hInstance = ResourceInstance();
	
	for(int i=0;i<4;i++)
	{
		m_iControls[i] = ResourceID(i);
	}

	SetTemplate(NULL, m_iControls[0]);

	return CFileDialog::DoModal();
}

HINSTANCE CLBPPreviewingFileDialog::ResourceInstance() const
{
	return NULL;
}

DWORD CLBPPreviewingFileDialog::ResourceID(DWORD OriginalID) const
{
	switch (OriginalID)
	{
	case 0: return IDD_PREVIEWING_FILE_DIALOG;
	case 1: return IDC_CHECK_PREVIEW;
	case 2: return IDC_STATIC_PREVIEW;
	case 3: return IDC_PROGRESS1;
	}

	return (DWORD)-1;
}

BEGIN_MESSAGE_MAP(CLBPPreviewingFileDialog, CFileDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CLBPPreviewingFileDialog::OnCommand(WPARAM wParam,LPARAM lParam)
{
	DWORD iID = LOWORD(wParam);
	DWORD iCommand = HIWORD(wParam);

	if(ResourceID(1) == iID)	//Preview check box
	{
		if(BN_CLICKED == iCommand)
		{
			OnCheckPreview();
			return TRUE;
		}
	}

	//Not handled - pass on to next window
	return CFileDialog::OnCommand(wParam, lParam);
}

void CLBPPreviewingFileDialog::DoDataExchange(CDataExchange* pDX)
{
	CFileDialog::DoDataExchange(pDX);
	
	// We can't do DDX/DDV if we're not using our own custom template.
	DDX_Control(pDX, m_iControls[2], m_static);
	DDX_Control(pDX, m_iControls[1], m_checkPreview);
	DDX_Control(pDX, m_iControls[3], m_progress);
}

int CLBPPreviewingFileDialog::PreviewSize(void) const
{
	return m_iPreviewSize;
}

void CLBPPreviewingFileDialog::SetPreviewSize(int iSize)
{
	m_iPreviewSize = iSize;
}

BOOL CLBPPreviewingFileDialog::OnInitDialog()
{
	CFileDialog::OnInitDialog();

	m_checkPreview.SetCheck(ShowPreviewOn());

	m_progress.SetRange(0, 100);

	m_pPreview->CreateWnd(CRect(0, 0, 0, 0), this, 12345);

	m_pPreview->SetSunken();

	return TRUE;
}

void CLBPPreviewingFileDialog::OnSize(UINT nType, int cx, int cy) 
{
	const int size = 2 + m_iPreviewSize + 2;

	CRect rectStatic;
	m_static.GetWindowRect(rectStatic);
	ScreenToClient(rectStatic);
	rectStatic.right = rectStatic.left + size;
	rectStatic.bottom = rectStatic.top + size;
	m_pPreview->MoveWindow(rectStatic);

	CRect rectProgress = rectStatic;
	m_progress.GetWindowRect(rectProgress);
	ScreenToClient(rectProgress);
	rectProgress.right = rectProgress.left + size;
	m_progress.MoveWindow(rectProgress);

	CRect rectCheck = rectProgress;
	rectCheck.top = rectProgress.bottom + D2PY(4);
	rectCheck.bottom = rectCheck.top + D2PY(12);
	m_checkPreview.MoveWindow(rectCheck);
}

IShellBrowser* CLBPPreviewingFileDialog::GetBrowserInterface(void) const
{
	// Get the IShellBrowser interface.

	DWORD_PTR dwResult = 0;
	if (0 == ::SendMessageTimeout(GetParent()->GetSafeHwnd(), WM_USER+7, NULL, NULL, SMTO_NORMAL, 100, &dwResult))
		return NULL;

	return (IShellBrowser*)dwResult;
}

HRESULT CLBPPreviewingFileDialog::GetSelectionObject(IDataObject ** ppDO, bool bSelected)
{
	*ppDO = NULL;

	IShellBrowser* pSB = GetBrowserInterface();
	if (NULL == pSB)
		return E_FAIL;

	IShellView* pSV = NULL;
	HRESULT result = pSB->QueryActiveShellView(&pSV);
	if (FAILED(result) || (NULL == pSV))
		return E_FAIL;

	IDataObject* pDO = NULL;
	result = pSV->GetItemObject(bSelected ? SVGIO_SELECTION : SVGIO_ALLVIEW, IID_IDataObject, (LPVOID*)&pDO);

	pSV->Release();

	if (FAILED(result) || (NULL == pDO))
		return E_FAIL;

	*ppDO = pDO;

	return S_OK;
}

HRESULT CLBPPreviewingFileDialog::GetFolderViewObject(IFolderView ** ppFV)
{
	*ppFV = NULL;

	IShellBrowser* pSB = GetBrowserInterface();
	if (NULL == pSB)
		return E_FAIL;

	IShellView* pSV = NULL;
	HRESULT result = pSB->QueryActiveShellView(&pSV);
	if (FAILED(result) || (NULL == pSV))
		return E_FAIL;

	result = pSV->QueryInterface(IID_IFolderView, (LPVOID*)ppFV);

	return result;
}

LPITEMIDLIST CLBPPreviewingFileDialog::GetSelectionPidl(LPDATAOBJECT pDO)
{
	FORMATETC formatEtc = { 0 };
	formatEtc.lindex = -1;
	formatEtc.tymed = TYMED_HGLOBAL;
	formatEtc.dwAspect = DVASPECT_CONTENT;
	formatEtc.cfFormat = (CLIPFORMAT)::RegisterClipboardFormat(CFSTR_SHELLIDLIST);

	HRESULT result = pDO->QueryGetData(&formatEtc);
	if (FAILED(result))
		return NULL;

	STGMEDIUM stg = { 0 };
	result = pDO->GetData(&formatEtc, &stg);
	if (FAILED(result))
		return NULL;

	LPITEMIDLIST pidl = NULL;

	if (stg.tymed == TYMED_HGLOBAL)
	{
		LPIDA pida = (LPIDA)::GlobalLock(stg.hGlobal);
		if (pida->cidl > 0)
		{
			// Single selection.
			LPCITEMIDLIST pidlFolder = (LPCITEMIDLIST)(((LPBYTE)pida)+(pida)->aoffset[0]);
			LPCITEMIDLIST pidlFile   = (LPCITEMIDLIST)(((LPBYTE)pida)+(pida)->aoffset[1]);
			if (pidlFolder && pidlFile)
			{
				CLBPPidlMgr pm;
				pidl = pm.Concatenate(pidlFolder, pidlFile);
			}
		}

		::GlobalUnlock(stg.hGlobal);
	}

	ReleaseStgMedium(&stg);

	return pidl;
}

void CLBPPreviewingFileDialog::OnFileNameChange()
{
	CFileDialog::OnFileNameChange();

	m_progress.SetPos(0);

	m_pPreview->SetBitmap(NULL);

	if (ShowPreviewOn())
	{
		const HBITMAP hBitmap = GetPreviewBitmap(CSize(m_iPreviewSize, m_iPreviewSize), m_progress);

		m_pPreview->SetBitmap(hBitmap);

		m_progress.SetPos(NULL == hBitmap ? 0 : 100);
	}
}

HBITMAP CLBPPreviewingFileDialog::GetPreviewBitmap(const CSize& size, CProgressCtrl& progress)
{
	IDataObject* pDO = NULL;
	if (FAILED(GetSelectionObject(&pDO, true))) 
		return NULL;

	m_progress.SetPos(10);

	LPITEMIDLIST pidl = GetSelectionPidl(pDO);

	pDO->Release();

	if (NULL == pidl) 
		return NULL;

	m_progress.SetPos(20);

	const HBITMAP hBitmap = ExtractThumbnailImage(size, pidl);

	m_progress.SetPos(80);

	CLBPPidlMgr pm;
	pm.Delete(pidl);

	return hBitmap;
}

HBITMAP CLBPPreviewingFileDialog::ExtractThumbnailImage(const CSize& size, LPCITEMIDLIST pidl)
{
	IShellFolder* pSFDesktop = NULL;
	HRESULT result = SHGetDesktopFolder(&pSFDesktop);
	if (FAILED(result))
		return NULL;

	CLBPPidlMgr pm;
	LPITEMIDLIST pidlFolder = pm.Truncate(pidl);

	IShellFolder* pSF = NULL;

	const int iLength = pm.GetPidlLength(pidlFolder);
	if (iLength > 0)
	{
		result = pSFDesktop->BindToObject(pidlFolder, NULL, IID_IShellFolder, (LPVOID*)&pSF);
		pSFDesktop->Release();
	}
	else
	{
		// The object is on the desktop itself.
		result = S_OK;
		pSF = pSFDesktop;
	}

	pm.Delete(pidlFolder);

	if (FAILED(result))
		return NULL;

	LPCITEMIDLIST cpidlItem = pm.GetLastItem(pidl);

	SFGAOF s = SFGAO_FOLDER | SFGAO_LINK;
	result = pSF->GetAttributesOf(1, &cpidlItem, &s);
	if (FAILED(result))
		return NULL;

	if (0 != (s & SFGAO_FOLDER))
		return NULL;

	if (0 != (s & SFGAO_LINK))
		return NULL;

	LPITEMIDLIST pidlItem = pm.Copy(pm.GetLastItem(pidl));

	IExtractImage* pIEI = NULL;
	result = pSF->GetUIObjectOf(NULL, 1, (LPCITEMIDLIST*)&pidlItem, IID_IExtractImage, NULL, (LPVOID*)&pIEI);
	pm.Delete(pidlItem);
	pSF->Release();

	if (FAILED(result))
		return NULL;

	// 25th February 2016, John Croudy. RH-33114: Use IEIFLAG_QUALITY to prevent use of a JPEG's internal thumbnail.
	WCHAR wszBuffer[MAX_PATH];
	DWORD dwPriority = 1;
	DWORD dwFlags = IEIFLAG_CACHE | IEIFLAG_QUALITY;
	result = pIEI->GetLocation(wszBuffer, MAX_PATH, &dwPriority, &size, 24, &dwFlags);

	if (FAILED(result))
	{
		pIEI->Release();
		return NULL;
	}

	HBITMAP hBitmap = NULL;
	result = pIEI->Extract(&hBitmap);
	pIEI->Release();

	if (FAILED(result))
		return NULL;

	return hBitmap;
}

void CLBPPreviewingFileDialog::OnFolderChange()
{
	CFileDialog::OnFolderChange();

	OnFileNameChange();
}

void CLBPPreviewingFileDialog::OnCheckPreview()
{
	const bool bShowPreview = (0 == m_checkPreview.GetCheck()) ? false : true;
	SetShowPreviewOn(bShowPreview);

	OnFileNameChange();
}

#endif
