
#pragma once

class CRpcInstance;

class CRpcSelectDlg : public RPCapiSI::SelectionInterface::SIcallback
{
public:
	CRpcSelectDlg(CRhinoDoc& doc, const CLBPString& sRpc);
	virtual ~CRpcSelectDlg(void);

public:
	INT_PTR DoModal(void);
	CLBPString Selection(void) const;

private:
	void CreateRpcUI(void);
	virtual void RPCselectionInterfaceCallbackProc(const RPCapiSI::SelectionInterface *sw, int action, va_list args);

private:
	CRpcInstance* m_pRpcInstance;
	RPCapiSI::SelectionInterface* m_pSelInterface;
	INT_PTR m_ret;
};
