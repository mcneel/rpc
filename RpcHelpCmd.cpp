#include "StdAfx.h"
#include "RpcHelpCmd.h"

const wchar_t * CRpcHelpCmd::EnglishCommandName()
{
	return L"RPCHelp";
}

UUID CRpcHelpCmd::CommandUUID()
{
	static const UUID uuid =
	{
		// {6EB9EF5B-3E7A-4D34-A82F-133E393831A1}
		0x6eb9ef5b, 0x3e7a, 0x4d34, { 0xa8, 0x2f, 0x13, 0x3e, 0x39, 0x38, 0x31, 0xa1 }
	};

	return uuid;
}

CRhinoCommand::result CRpcHelpCmd::RunRpcCommand(const CRhinoCommandContext& context)
{
	const TCHAR *arcHelp = _T("http://helpcenter.archvision.com");
	ShellExecute(0, 0, arcHelp, 0, 0, SW_SHOW);
	return success;
}

CRhinoCommand::result CRpcSiteCmd::RunRpcCommand(const CRhinoCommandContext & context)
{
	const TCHAR *arcSite = _T("http://archvision.com/");
	ShellExecute(0, 0, arcSite, 0, 0, SW_SHOW);
	return success;
}

const wchar_t * CRpcSiteCmd::EnglishCommandName()
{
	return L"RPCSite";
}

UUID CRpcSiteCmd::CommandUUID()
{
	static const UUID uuid =
	{
		// {6EB9EF5B-4E7A-4D34-A82F-133E313831A1}
		0x6eb9ef5b, 0x4e7a, 0x4d34, { 0xa8, 0x2f, 0x13, 0x3e, 0x31, 0x38, 0x31, 0xa1 }
	};

	return uuid;
}
