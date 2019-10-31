#include "StdAfx.h"
#include "RpcDashboardCmd.h"

const wchar_t * CRpcDashboardCmd::EnglishCommandName()
{
	return L"RPCDashboard";
}

UUID CRpcDashboardCmd::CommandUUID()
{
	static const UUID uuid =
	{
		// {93AAD440-6162-4070-97EB-09087AE44CDC}
		0x93aad440, 0x6162, 0x4070, { 0x97, 0xeb, 0x09, 0x08, 0x7a, 0xe4, 0x4c, 0xdc }
	};

	return uuid;
}

CRhinoCommand::result CRpcDashboardCmd::RunRpcCommand(const CRhinoCommandContext& context)
{
	const HKEY root = HKEY_LOCAL_MACHINE;
	const std::wstring name = L"Path";
	std::wstring key = L"Software\\WOW6432Node\\ArchVision\\Dashboard";
	HKEY hKey;

	if (RegOpenKeyEx(root, key.c_str(), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		key = L"Software\\ArchVision\\Dashboard";

		if (RegOpenKeyEx(root, key.c_str(), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
			throw "Could not open registry key";
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

	ShellExecute(NULL, L"open", L"Dashboard.exe", NULL, (LPCWSTR)value.c_str(), SW_SHOWNORMAL);
	return success;
}