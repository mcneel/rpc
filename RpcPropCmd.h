#pragma once
#include "RpcCommand.h"

class CRpcPropCmd : public CRpcTestCommand

{
	CRhinoCommand::result RunRpcCommand(const CRhinoCommandContext& context) override;
	const wchar_t* EnglishCommandName() override;
	UUID CommandUUID() override;
};

