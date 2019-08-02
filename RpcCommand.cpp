
#include "StdAfx.h"
#include "RpcCommand.h"

CRhinoCommand::result CRpcCommandBase::RunCommand(const CRhinoCommandContext& context)
{
	const CRhinoCommand::result result = RunRpcCommand(context);
	return result;
}
