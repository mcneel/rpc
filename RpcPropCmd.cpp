#include "stdafx.h"
#include "RpcPropCmd.h"

CRhinoCommand::result CRpcPropCmd::RunRpcCommand(const CRhinoCommandContext& context)
{
	CRhinoTabbedDockBarDialog::ShowDockbarTab(context.m_doc, uuidPanelObjectProps, true, true, &uuidPanelObjectProps);

	return result::success;
}

const wchar_t* CRpcPropCmd::EnglishCommandName()
{
	return L"RpcOpenProp";
}

UUID CRpcPropCmd::CommandUUID()
{
	static const UUID uuid =
	{
		// {93AAD440-6162-4070-97EB-44087AE44CDC}
		0x93aad440, 0x6162, 0x4070, { 0x97, 0xeb, 0x44, 0x08, 0x7a, 0xe4, 0x4c, 0xdc }
	};

	return uuid;
}
