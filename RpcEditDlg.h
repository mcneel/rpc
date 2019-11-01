#pragma once


class CRpcInstance;


class CRpcEditDlg
{
public:
	CRpcEditDlg(const CRhinoDoc& doc, ON_SimpleArray<ON_UUID>& aRpc);
	~CRpcEditDlg(void);

public:
	bool Edit(void);

public:
	ON_SimpleArray<ON_UUID>& RPC(void);

private:
	ON_SimpleArray<ON_UUID>& m_aRpc;
	const CRhinoDoc& m_doc;
};
