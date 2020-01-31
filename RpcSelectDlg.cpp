
#include "StdAfx.h"
#include "RpcSelectDlg.h"
#include "Resource.h"
#include "RpcInstance.h"
#include "RpcClient.h"
#include "RpcMains.h"

CRpcSelectDlg::CRpcSelectDlg(CRhinoDoc& doc, const CLBPString& sRpc)
{
	m_pSelInterface = NULL;
	m_pRpcInstance = new CRpcInstance(doc, sRpc);
	m_ret = IDCANCEL;
}

CRpcSelectDlg::~CRpcSelectDlg(void)
{
	if (nullptr != m_pSelInterface)
	{
		m_pSelInterface->registerCallback(this, false);
	}
	delete m_pSelInterface;
	delete m_pRpcInstance;
}

INT_PTR CRpcSelectDlg::DoModal(void)
{
	m_pSelInterface = dynamic_cast<RPCapiSI::SelectionInterface*>(Mains().RpcClient().RPCgetAPI()->newObject(RPCapiSI::SelectionInterface::OBJECT_CODE));
	if (nullptr == m_pSelInterface) return IDCLOSE;

	m_pSelInterface->registerCallback(this, true);
	m_pSelInterface->idSet(Mains().GetSelectedId());
	m_pSelInterface->show(RhinoApp().MainWnd(), RPCapiSI::SelectionInterface::THUMBS_PLUS, RPCapiSI::SelectionInterface::MODAL);
	Mains().SetSelectedId(m_pSelInterface->idGet());
	return m_ret;
}

void CRpcSelectDlg::RPCselectionInterfaceCallbackProc(const RPCapiSI::SelectionInterface *sw, int action, va_list args)
{
	if (RPCapiSI::SelectionInterface::SIcallback::ACTION_CODE_TYPE::WINDOW_CLOSING == action)
	{
		const int arg = va_arg(args, int);
		m_ret = (1==arg) ? IDOK : IDCANCEL;
	}
}

CLBPString CRpcSelectDlg::Selection(void) const
{
	if (NULL == m_pSelInterface)
		return L"";

	const RPCapi::ID* pId = m_pSelInterface->idGet();
	if (NULL == pId) return L"";

	m_pRpcInstance->SetId(pId);

	return m_pRpcInstance->FileName();
}
