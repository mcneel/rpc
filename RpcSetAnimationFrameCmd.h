#pragma once


#include "RpcCommand.h"


class CRpcSetAnimationFrameCmd : public CRpcCommand
{
public:
	virtual CRhinoCommand::result RunRpcCommand(const CRhinoCommandContext& context);
	virtual const wchar_t * EnglishCommandName();
	virtual UUID CommandUUID();
};
