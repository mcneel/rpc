
#pragma once

#include "RpcCommand.h"

class CRpcEditCmd : public CRpcTestCommand
{
public:
	virtual CRhinoCommand::result RunRpcCommand(const CRhinoCommandContext& context);
	virtual const wchar_t * EnglishCommandName();
	virtual UUID CommandUUID();

private:
	bool GetRpcBlock(const CRhinoDoc& doc, ON_SimpleArray<ON_UUID>& aBlock);
};
