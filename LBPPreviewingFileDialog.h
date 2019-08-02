
#pragma once

#ifndef LBPEXPORT
#define LBPEXPORT
#endif

#if !defined LBPLIB_DISABLE_MFC_CONTROLS

class CLBPBitmapPreview;

class LBPEXPORT CLBPPreviewingFileDialog : public CFileDialog
{
public:
	CLBPPreviewingFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt=NULL, LPCTSTR lpszFileName=NULL,
	                         DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, LPCTSTR lpszFilter=NULL, CWnd* pParentWnd=NULL);
	virtual ~CLBPPreviewingFileDialog();

	int PreviewSize(void) const;
	void SetPreviewSize(int iSize);

	virtual INT_PTR DoModal();

	virtual bool ShowPreviewOn(void) const { return m_bShowPreview; }
	virtual void SetShowPreviewOn(bool b)  { m_bShowPreview = b; }

protected:
	afx_msg void OnCheckPreview();
	DECLARE_MESSAGE_MAP()

protected:
	// Override these functions to provide custom resource support (ie - not LBPLibResources.dll)
	virtual HINSTANCE ResourceInstance() const;

/*	case 0: return IDD_LBP_PREVIEWING_FILE_DIALOG;
	case 1: return IDC_CHECK_PREVIEW;
	case 2: return IDC_STATIC_PREVIEW;
	case 3: return IDC_PROGRESS1;
*/	virtual DWORD ResourceID(DWORD OriginalID) const;

protected:
	// Override this to get the preview bitmap in a different way than using the shell.
	virtual HBITMAP GetPreviewBitmap(const CSize& size, CProgressCtrl& progress);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnFileNameChange();
	virtual void OnFolderChange();
	virtual BOOL OnCommand(WPARAM wParam,LPARAM lParam);

	IShellBrowser* GetBrowserInterface(void) const;
	LPITEMIDLIST GetSelectionPidl(LPDATAOBJECT pDO);
	HRESULT GetFolderViewObject(IFolderView** ppFV);
	HRESULT GetSelectionObject(IDataObject** ppDO, bool bSelected);
	HBITMAP ExtractThumbnailImage(const CSize& size, LPCITEMIDLIST pidl);

	CButton& PreviewCheck(void) { return m_checkPreview; }

private:
	CStatic m_static;
	CButton m_checkPreview;
	int m_iPreviewSize;
	CProgressCtrl m_progress;
	CLBPBitmapPreview* m_pPreview;
	int m_iControls[4];

	static bool m_bShowPreview;
};

#endif
