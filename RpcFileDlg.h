#pragma once

#include "LBPPreviewingFileDialog.h"


class CRpcFileDlg : public CLBPPreviewingFileDialog
{
public:
	CRpcFileDlg(BOOL bOpenFileDialog, LPCTSTR lpszDefExt = NULL, LPCTSTR lpszFileName = NULL,
					  DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, LPCTSTR lpszFilter = NULL,
					  CWnd* pParentWnd = NULL);
	virtual ~CRpcFileDlg(void);

protected:
	virtual BOOL OnInitDialog();

public:
	virtual HINSTANCE ResourceInstance() const;
	virtual DWORD ResourceID(DWORD dwWhich) const;
};


class CRpcAdvancedFileDialog : public CRpcFileDlg
{
public:
	CRpcAdvancedFileDialog(CWnd* pParent);

	bool Advanced(void) const { return m_bAdvanced; }

protected:
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnAdvancedButtonClicked();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()

	void SetAdvancedButtonPosition(void);

private:
	CRhinoUiBitmapButton m_button_advanced;
	bool m_bAdvanced;
};
