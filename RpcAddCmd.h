#pragma once


#include "RpcCommand.h"


class CRpcAddCmd : public CRpcCommand
{
public:
	virtual CRhinoCommand::result RunRpcCommand(const CRhinoCommandContext& context);
	virtual const wchar_t * EnglishCommandName();
	virtual UUID CommandUUID();

private:
	bool GetRpcFileName(CRhinoDoc& doc, CLBPString& sRpc);
};
