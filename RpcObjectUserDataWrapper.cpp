
#include "StdAfx.h"
#include "RpcObjectUserDataWrapper.h"

CRpcObjectUserDataWrapper::CRpcObjectUserDataWrapper(const CRhinoObject * pRhinoObject, lbprh_wrapper_eType type)
:CLBPRhObjectWrapper<CRpcObjectUserData>(pRhinoObject, type)
{

}

CRpcObjectUserDataWrapper::~CRpcObjectUserDataWrapper()
{

}

const CRpcObjectUserData* CRpcObjectUserDataWrapper::UserData(eLBPRhWrapper_DefaultUse du) const
{
	return __super::UserData(du);
}

CRpcObjectUserData*	CRpcObjectUserDataWrapper::UserDataToModify(void)
{
	return __super::UserDataToModify();
}
