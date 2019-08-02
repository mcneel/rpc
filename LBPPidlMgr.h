// PidlMgr.h: interface for the CPidlMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LBPPIDLMGR_H__F27C435D_4A2E_4E97_9F62_67D19D99D792__INCLUDED_)
#define AFX_LBPPIDLMGR_H__F27C435D_4A2E_4E97_9F62_67D19D99D792__INCLUDED_

#pragma once

#ifndef LBPEXPORT
#define LBPEXPORT
#endif

#if !defined LBPLIB_DISABLE_SHELL



////////////////////////////////////////////////////
// CPidlMgr : Class for managing Pidls
class LBPEXPORT CLBPPidlMgr
{
public:
	static void				Delete(LPITEMIDLIST);
	static LPCITEMIDLIST	GetNextItem(LPCITEMIDLIST);
	static LPITEMIDLIST		Copy(LPCITEMIDLIST);
	static UINT				GetSize(LPCITEMIDLIST);
	static UINT				GetPidlLength(LPCITEMIDLIST);
	static LPCITEMIDLIST	GetLastItem(LPCITEMIDLIST pidl);
	static LPITEMIDLIST		Concatenate(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);
	static LPITEMIDLIST		Truncate(LPCITEMIDLIST pidl);
	static bool				IsEqual(LPCITEMIDLIST pidlFQ1, LPCITEMIDLIST pidlFQ2);	//Pidls must be fully qualified - ie, root at the desktop.
	static bool				IsDesktop(LPCITEMIDLIST pidl);
	static LPITEMIDLIST		GetDesktopPidl();
	static LPITEMIDLIST		PidlFromPath(LPCTSTR pszPath, HWND hOwner = 0);
	static LPITEMIDLIST		GetSimplePidl(LPCITEMIDLIST pidl, int iItem);
};

#endif

#endif // !defined(AFX_LBPPIDLMGR_H__F27C435D_4A2E_4E97_9F62_67D19D99D792__INCLUDED_)




