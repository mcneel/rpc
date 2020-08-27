#include "StdAfx.h"
#include "RpcAboutCmd.h"
#include "RpcAboutDialog.h"

const wchar_t* CRpcAboutCmd::EnglishCommandName()
{
	return L"RPCAbout";
}

UUID CRpcAboutCmd::CommandUUID()
{
	static const UUID uuid =
	{
		// {3a226b56-553a-48ac-836b-133bde94b4a1}
		0x3a226b56, 0x553a, 0x48ac, { 0x83, 0x6b, 0x13, 0x3b, 0xde, 0x94, 0xb4, 0xa1 }
	};

	return uuid;
}

CRhinoCommand::result CRpcAboutCmd::RunRpcCommand(const CRhinoCommandContext& context)
{
	RpcAboutDialog dlg(CWnd::GetActiveWindow());

	if (dlg.DoModal() <= 0)
		return success;
}
