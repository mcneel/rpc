
#pragma once

#define LBPBP_SUNKEN	0
#define LBPBP_RAISED	1
#define LBPBP_OUTLINED	2

#define WM_LBPBP_LBUTTONUP WM_USER+0x400
#define WM_LBPBP_RBUTTONUP WM_USER+0x401

#ifndef LBPEXPORT
#define LBPEXPORT
#endif

class LBPEXPORT CLBPBitmapPreview : public CWnd
{
public:
	CLBPBitmapPreview();
	virtual ~CLBPBitmapPreview();

public:
	bool CreateWnd(const RECT& rc, CWnd* pParent, UINT id);

	// Takes ownership of the HBITMAP.
	void SetBitmap(HBITMAP hBitmap);

	bool HasBitmap(void) const;

	void SetOutlined(void);
	void SetRaised(void);
	void SetSunken(void);

	void SetTooltipText(LPCTSTR lpszText, BOOL bActivate = TRUE);
	void ActivateTooltip(BOOL bEnable = TRUE);
	void InitToolTip(void);
	void DrawEmpty(CDC* pDC, const RECT*);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	void SafeRedraw(void);
	virtual BOOL OnDrawImage(CDC* pDC, const RECT*); // Called from drawing code - return TRUE if overridden.

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

protected:
	HBITMAP m_hBitmap;
	int m_iBorderType;
	CToolTipCtrl m_ToolTip;
	DWORD m_dwToolTipStyle;

public:
	CString m_sDescription;
	CString m_sToolTip;
};
