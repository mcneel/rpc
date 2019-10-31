
#include "StdAfx.h"
#include "RpcSelectionDialog.h"

#pragma region RPC command
class CRpcDockbar : public CRhinoCommand
{
public:
	CRpcDockbar() = default;
	~CRpcDockbar() = default;

	UUID CommandUUID() override
	{
		// {1CB91610-1762-4C93-8582-1D50BFC43152}
		static const GUID CRpcDockbar_UUID =
		{ 0x1CB91610, 0x1762, 0x4C93, { 0x85, 0x82, 0x1D, 0x50, 0xBF, 0xC4, 0x31, 0x52 } };
		return CRpcDockbar_UUID;
	}

	const wchar_t* EnglishCommandName() override { return L"RPCSelect"; }

	CRhinoCommand::result RunCommand(const CRhinoCommandContext& context) override;
};

static class CRpcDockbar theSampleDockBarDialogCommand;

CRhinoCommand::result CRpcDockbar::RunCommand(const CRhinoCommandContext& context)
{
	ON_UUID tabId = CRpcSelectionDialog::Id();

	if (context.IsInteractive())
	{
		CRhinoTabbedDockBarDialog::ShowDockbarTab(context.m_doc, tabId, true,true, &uuidPanelLayers);
	}
	else
	{
		bool bVisible = CRhinoTabbedDockBarDialog::IsTabVisible(context.m_doc, tabId);

		ON_wString str;
		str.Format(L"%ls is %ls. New value", LocalCommandName(), bVisible ? L"visible" : L"hidden");

		CRhinoGetOption go;
		go.SetCommandPrompt(str);
		int h_option = go.AddCommandOption(RHCMDOPTNAME(L"Hide"));
		int s_option = go.AddCommandOption(RHCMDOPTNAME(L"Show"));
		int t_option = go.AddCommandOption(RHCMDOPTNAME(L"Toggle"));
		go.GetOption();
		if (go.CommandResult() != CRhinoCommand::success)
			return go.CommandResult();

		const CRhinoCommandOption* option = go.Option();
		if (0 == option)
			return CRhinoCommand::failure;

		int option_index = option->m_option_index;

		if (h_option == option_index && bVisible)
			CRhinoTabbedDockBarDialog::ShowDockbarTab(context.m_doc, tabId, false, false, nullptr);
		else if (s_option == option_index && !bVisible)
			CRhinoTabbedDockBarDialog::ShowDockbarTab(context.m_doc, tabId, true, true, nullptr);
		else if (t_option == option_index)
			CRhinoTabbedDockBarDialog::ShowDockbarTab(context.m_doc, tabId, !bVisible, !bVisible, nullptr);
	}

	return CRhinoCommand::success;
}

#pragma endregion