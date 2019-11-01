#include "StdAfx.h"
#include "RpcEditDlg.h"
#include "RpcInstance.h"
#include "RpcClient.h"
#include "RpcMains.h"
#include "Resource.h"


CRpcEditDlg::CRpcEditDlg(const CRhinoDoc& doc, ON_SimpleArray<ON_UUID>& aRpc)
: m_aRpc(aRpc),
  m_doc(doc)
{

}

CRpcEditDlg::~CRpcEditDlg(void)
{

}

ON_SimpleArray<ON_UUID>& CRpcEditDlg::RPC(void)
{
	return m_aRpc;
}

bool CRpcEditDlg::Edit(void)
{
	const int iCount = m_aRpc.Count();

	if (iCount <= 0)
	{
		return false;
	}
	else
	{
		RPCapi::MassEditInterface* pEditInterface = dynamic_cast<RPCapi::MassEditInterface *>
				(Mains().RpcClient().RPCgetAPI()->newObject(RPCapi::ObjectCodes::MASS_EDIT_INTERFACE));
		
		if (!pEditInterface)
			return false;

		for (int i = 0; i < iCount; i++)
		{
			if(auto rpc = Mains().GetRPCInstanceTable().Lookup(m_aRpc[i]))
				pEditInterface->addInstance((*rpc)->Instance());
		}

		const double dScale = ON::UnitScale(ON::LengthUnitSystem::Inches, m_doc.ModelUnits());
		pEditInterface->setUnits(RPCapi::Units::LINEAR_UNITS, dScale);

		const int iRet = pEditInterface->show(RhinoApp().MainWnd(), 0);

		pEditInterface->hide();
		delete pEditInterface;

		return (IDOK == iRet) ? true : false;
	}
}
