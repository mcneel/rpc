// RPC.h : main header file for the RPC DLL.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h" // main symbols


class CRPCApp : public CWinApp
{
public:
	CRPCApp(void);
	virtual ~CRPCApp(void);

public:
	virtual BOOL InitInstance(void);
	virtual int ExitInstance(void);

	DECLARE_MESSAGE_MAP()
};
