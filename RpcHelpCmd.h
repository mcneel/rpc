#pragma once
#include "RpcCommand.h"

class CRpcHelpCmd : public CRpcTestCommand
{
public:
	virtual CRhinoCommand::result RunRpcCommand(const CRhinoCommandContext& context);
	virtual const wchar_t * EnglishCommandName();
	virtual UUID CommandUUID();
};
class CRpcSiteCmd : public CRpcTestCommand
{
public:
	virtual CRhinoCommand::result RunRpcCommand(const CRhinoCommandContext& context);
	virtual const wchar_t * EnglishCommandName();
	virtual UUID CommandUUID();
};

