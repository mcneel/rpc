#pragma once


class CRpcCommandBase : public CRhinoScriptCommand
{
protected:
	CRpcCommandBase(bool bTest) : CRhinoScriptCommand(false, false, 0, bTest) {}
public:
	virtual CRhinoCommand::result RunRpcCommand(const CRhinoCommandContext&) = 0;

protected:
	virtual CRhinoCommand::result RunCommand( const CRhinoCommandContext& );
};

class CRpcTestCommand : public CRpcCommandBase
{
protected:
	CRpcTestCommand() : CRpcCommandBase(true) {}
};

class CRpcCommand : public CRpcCommandBase
{
protected:
	CRpcCommand() : CRpcCommandBase(false) {}
};