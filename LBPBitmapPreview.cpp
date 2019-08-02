
#include "stdafx.h"
#include "LBPBitmapPreview.h"

#if !defined LBPLIB_DISABLE_MFC_CONTROLS

CLBPBitmapPreview::CLBPBitmapPreview()
{
	m_hBitmap = NULL;

	m_iBorderType = LBPBP_SUNKEN;

	m_ToolTip.m_hWnd = NULL;

	m_dwToolTipStyle = TTS_ALWAYSTIP;
}

CLBPBitmapPreview::~CLBPBitmapPreview()
{
	if (NULL != m_hBitmap)
	{
		::DeleteObject(m_hBitmap);
	}
}

BEGIN_MESSAGE_MAP(CLBPBitmapPreview, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

bool CLBPBitmapPreview::HasBitmap(void) const
{
	return NULL != m_hBitmap;
}

BOOL CLBPBitmapPreview::OnEraseBkgnd(CDC* pDC) 
{
	if (nullptr == pDC)
		return false;

	CDC& dc = *pDC;

	EnableToolTips(TRUE);

	CRect rc;
	GetClientRect(rc);

	if (m_iBorderType == LBPBP_RAISED)
	{
		dc.DrawEdge(rc, EDGE_RAISED, BF_RECT | BF_ADJUST | BF_SOFT | BF_MIDDLE);
	}
	else
	if (m_iBorderType == LBPBP_SUNKEN)
	{
		dc.DrawEdge(rc, EDGE_SUNKEN, BF_RECT | BF_ADJUST | BF_SOFT | BF_MIDDLE);
	}
	else
	{
		dc.DrawEdge(rc, EDGE_RAISED, BF_RECT | BF_ADJUST | BF_FLAT | BF_MIDDLE);
	}

	if (OnDrawImage(&dc, rc))
		return FALSE;

	CBitmap* pBmp = CBitmap::FromHandle(m_hBitmap);
	if (NULL == pBmp)
	{
		DrawEmpty(&dc, rc);
		return FALSE;
	}

	BITMAP bm = { 0 };
	if (pBmp->GetBitmap(&bm))
	{
		CDC dcBmp;
		dcBmp.CreateCompatibleDC(&dc);
		CBitmap* pBmpOld = dcBmp.SelectObject(pBmp); 

		dc.IntersectClipRect(rc);

		const int x = rc.left + (rc.Width()  - bm.bmWidth)  / 2;
		const int y = rc.top  + (rc.Height() - bm.bmHeight) / 2;
		dc.BitBlt(x, y, bm.bmWidth, bm.bmHeight, &dcBmp, 0, 0, SRCCOPY);

		dcBmp.SelectObject(pBmpOld); 
	}
	
	return FALSE;
}

BOOL CLBPBitmapPreview::OnDrawImage(CDC* pDC, const RECT* rc)
{
	return FALSE;
}

void CLBPBitmapPreview::OnPaint() 
{
	// Nothing to paint - all done in OnEraseBackground.
	CPaintDC dc(this);
}

void CLBPBitmapPreview::SafeRedraw(void)
{
	if (IsWindow(GetSafeHwnd()))
	{
		RedrawWindow();	
	}
}

void CLBPBitmapPreview::SetBitmap(HBITMAP hBitmap)
{
	if (NULL != m_hBitmap)
	{
		::DeleteObject(m_hBitmap);
		m_hBitmap = NULL;
	}

	if (NULL == hBitmap)
	{
		ActivateTooltip(FALSE);
		m_sDescription.Empty();
	}
	else
	{
		m_hBitmap = hBitmap;
//		SetTooltipText(m_sToolTip);
	}

	SafeRedraw();
}

void CLBPBitmapPreview::SetSunken(void)
{	
	m_iBorderType = LBPBP_SUNKEN;
	SafeRedraw();
}

void CLBPBitmapPreview::SetRaised(void)
{
	m_iBorderType = LBPBP_RAISED;
	SafeRedraw();
}

void CLBPBitmapPreview::SetOutlined(void)
{
	m_iBorderType = LBPBP_OUTLINED;
	SafeRedraw();
}

bool CLBPBitmapPreview::CreateWnd(const RECT& rc, CWnd *pParent, UINT id)
{
	CString s = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS,
									::LoadCursor(NULL, IDC_ARROW), (HBRUSH)::GetStockObject(LTGRAY_BRUSH), NULL);

//	EnableToolTips(TRUE);
	InitToolTip();

	return CWnd::Create(s, _T("LBPBitmapPreview"), WS_CHILD | WS_VISIBLE, rc, pParent, id) ? true : false;
}

void CLBPBitmapPreview::OnLButtonUp(UINT nFlags, CPoint point) 
{
	GetParent()->SendMessage(WM_LBPBP_LBUTTONUP, GetDlgCtrlID(), NULL);
	
	CWnd::OnLButtonUp(nFlags, point);
}

void CLBPBitmapPreview::OnRButtonUp(UINT nFlags, CPoint point) 
{
	GetParent()->SendMessage(WM_LBPBP_RBUTTONUP, GetDlgCtrlID(), NULL);
	
	CWnd::OnRButtonUp(nFlags, point);
}

BOOL CLBPBitmapPreview::PreTranslateMessage(MSG* pMsg) 
{
	InitToolTip();
//	m_ToolTip.RelayEvent(pMsg);
	
	if (pMsg->message == WM_LBUTTONDBLCLK)
		pMsg->message = WM_LBUTTONDOWN;

	return CWnd::PreTranslateMessage(pMsg);
}

void CLBPBitmapPreview::OnMouseMove(UINT nFlags, CPoint point)
{
	InitToolTip();

/*	if (m_ToolTip.m_hWnd)
    {
		UINT_PTR uId;
		
		TOOLINFO ti;
		ti.cbSize = sizeof(TOOLINFO);
		ti.hwnd   = GetSafeHwnd();
		uId = GetDlgCtrlID();
		ti.uId    = uId;
		ti.rect.left   = point.x;
		ti.rect.right  = point.x+1;
		ti.rect.top    = point.y;
		ti.rect.bottom = point.y+1;
		ti.uFlags = TTF_SUBCLASS;
		m_ToolTip.SendMessage(TTM_NEWTOOLRECT,   0, (LPARAM)&ti);
    }*/

	MSG msg;
	msg.hwnd = GetSafeHwnd();
	msg.time = 0;
	msg.message = WM_MOUSEMOVE;
	msg.wParam = nFlags;
	msg.lParam = MAKELPARAM(point.x, point.y);
	m_ToolTip.RelayEvent(&msg);

	CWnd::OnMouseMove(nFlags, point);
}

void CLBPBitmapPreview::InitToolTip(void)
{
	if (m_ToolTip.m_hWnd == NULL)
	{
		if (m_ToolTip.Create(this, m_dwToolTipStyle))	
		{
			m_ToolTip.Activate(FALSE);
			m_ToolTip.SendMessage(TTM_SETMAXTIPWIDTH, 0, 400);
//			m_ToolTip.SetWindowPos(&(CWnd::wndTopMost),0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
		}
	} 
}

void CLBPBitmapPreview::SetTooltipText(LPCTSTR lpszText, BOOL bActivate)
{
	if (lpszText == NULL)
		return;

	InitToolTip();

	if (!m_ToolTip.GetSafeHwnd())
		return;

	// If there is no tooltip defined then add it.
	if (m_ToolTip.GetToolCount() == 0)
	{
		CRect rectBtn; 
		GetClientRect(rectBtn);
		m_ToolTip.AddTool(this, lpszText, rectBtn, 1);
	}

	// Set text for tooltip.
	m_ToolTip.UpdateTipText(lpszText, this, 1);
	m_ToolTip.Activate(bActivate);
}

void CLBPBitmapPreview::ActivateTooltip(BOOL bActivate)
{
	if (m_ToolTip.GetToolCount() == 0)
		return;

	// Activate tooltip.
	m_ToolTip.Activate(bActivate);
}

void CLBPBitmapPreview::DrawEmpty(CDC* pDC, const RECT* pRC)
{
/*	pDC->FillSolidRect(pRC, RGB(255, 255, 255));

//	pDC->Rectangle(pRC);
	pDC->MoveTo(pRC->top, pRC->left);
	pDC->LineTo(pRC->bottom, pRC->right);

	pDC->MoveTo(pRC->bottom, pRC->left);
	pDC->LineTo(pRC->top, pRC->right);
*/
}

#endif
