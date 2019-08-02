#pragma once


#include "RpcObjectUserData.h"


class CRpcObjectUserDataWrapper : public CLBPRhObjectWrapper<CRpcObjectUserData>
{
public:
	CRpcObjectUserDataWrapper(const CRhinoObject * pRhinoObject, lbprh_wrapper_eType type = lwt_attributes);
	~CRpcObjectUserDataWrapper(void);

public:
	virtual const CRpcObjectUserData* UserData(eLBPRhWrapper_DefaultUse du = lbprh_dont_use_defaults) const;
	virtual CRpcObjectUserData*	UserDataToModify(void);
};
