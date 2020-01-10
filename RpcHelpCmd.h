#pragma once
#include "RpcCommand.h"

class CRpcHelpCmd : public CRpcTestCommand
{
public:
	CRhinoCommand::result RunRpcCommand(const CRhinoCommandContext& context) override;
	const wchar_t * EnglishCommandName() override;
	UUID CommandUUID() override;
};
class CRpcSiteCmd : public CRpcTestCommand
{
public:
	CRhinoCommand::result RunRpcCommand(const CRhinoCommandContext& context) override;
	const wchar_t * EnglishCommandName() override;
	UUID CommandUUID() override;
};

