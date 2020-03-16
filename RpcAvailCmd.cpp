#include "StdAfx.h"
#include "RpcAvailDialog.h"
#include "RpcAvailCmd.h"
#include "RpcAvailBrowserDialog.h"
#include "RpcSelectionDialog.h"

const wchar_t* CRpcAvailCmd::EnglishCommandName()
{
	return L"RPCAvail";
}

UUID CRpcAvailCmd::CommandUUID()
{
	static const UUID uuid =
	{
		// {93AAD440-6162-4070-97EB-09087AE44CDC}
		0x93aad440, 0x6162, 0x4070, { 0x97, 0xeb, 0x09, 0x08, 0x7a, 0xe4, 0x4c, 0xdc }
	};

	return uuid;
}

CRhinoCommand::result CRpcAvailCmd::RunRpcCommand(const CRhinoCommandContext& context)
{
	const HKEY root = HKEY_LOCAL_MACHINE;
	const std::wstring name = L"Path";
	std::wstring key = L"Software\\ArchVision\\Beacon\\00F8D4D6-3919-409C-97FB-E3549884989F";
	HKEY hKey;

	if (RegOpenKeyEx(root, key.c_str(), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		RpcAvailDialog dlg(CWnd::GetActiveWindow());

		if (dlg.DoModal() <= 0)
			return success;
	}

	DWORD type, cbData;

	if (RegQueryValueEx(hKey, name.c_str(), NULL, &type, NULL, &cbData) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		throw "Could not read registry value";
	}

	if (type != REG_SZ)
	{
		RegCloseKey(hKey);
		throw "Incorrect registry value type";
	}

	std::wstring value(cbData / sizeof(wchar_t), L'\0');

	if (RegQueryValueEx(hKey, name.c_str(), NULL, NULL, reinterpret_cast<LPBYTE>(&value[0]), &cbData) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		throw "Could not read registry value";
	}

	RegCloseKey(hKey);
	size_t firstNull = value.find_first_of(L'\0');

	if (firstNull != string::npos)
	{
		value.resize(firstNull);
	}

	ShellExecute(NULL, L"open", L"AVAIL.exe", NULL, (LPCWSTR)value.c_str(), SW_SHOWNORMAL);
	return success;
}

CRhinoCommand::result CRpcAvailBrowserCmd::RunRpcCommand(const CRhinoCommandContext& context)
{
	//34F7245E-D7D0-4652-840E-AE4FF2CA75D1
	constexpr UUID AvailBrowserDockPanel =
	{
		0x34f7245e, 0xd7d0, 0x4652, { 0x84, 0x0e, 0xae, 0x4f, 0xf2, 0xca, 0x75, 0xd1 }
	};

	if (!CRpcSelectionDialog::OpenPanelInDockBarWithOtherPanel(*RhinoApp().ActiveDoc(), AvailBrowserDockPanel, uuidPanelObjectProps, true))
	{
		CRpcAvailBrowserDialog dlg(CWnd::GetActiveWindow());

		if (dlg.DoModal() <= 0)
			return success;
	}

	return success;
}

const wchar_t* CRpcAvailBrowserCmd::EnglishCommandName()
{
	return L"RPCAvailBrowser";
}

UUID CRpcAvailBrowserCmd::CommandUUID()
{
	static const UUID uuid =
	{
		// {8467ef3c-3945-4967-a6ce-25b849e23fde}
		0x8467ef3c, 0x3945, 0x4967, { 0xa6, 0xce, 0x25, 0xb8, 0x49, 0xe2, 0x3f, 0xde }
	};

	return uuid;
}