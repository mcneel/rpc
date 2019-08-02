#pragma once


class CRpcInstance;


class CRpcEditDlg
{
public:
	CRpcEditDlg(const CRhinoDoc& doc, ON_SimpleArray<CRpcInstance*>& aRpc);
	~CRpcEditDlg(void);

public:
	bool Edit(void);

public:
	ON_SimpleArray<CRpcInstance*>& RPC(void);

private:
	ON_SimpleArray<CRpcInstance*>& m_aRpc;
	const CRhinoDoc& m_doc;
};
