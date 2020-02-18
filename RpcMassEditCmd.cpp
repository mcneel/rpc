#include "StdAfx.h"
#include "RpcMassEditCmd.h"
#include "RpcObject.h"

CRhinoCommand::result CRpcMassEditCmd::RunRpcCommand(const CRhinoCommandContext& context)
{
	CRhinoObjectIterator it(*context.Document(), CRhinoObjectIterator::normal_or_locked_objects, CRhinoObjectIterator::active_and_reference_objects);
	int counter = 0;

	for (auto pObject = it.First(); pObject; pObject = it.Next())
	{
		CRpcObject ro(pObject);

		if (ro.IsTagged())
		{
			counter++;
			pObject->Select();
		}
	}

	if (counter == 0)
	{
		RhinoMessageBox(L"There are no RPCs in the scene\n",L"",MB_OK);
		return cancel;
	}

	RhinoApp().RunScript(CRhinoDoc::NullRuntimeSerialNumber, L"_RpcEdit");
	context.Document()->UnselectAll();

	return success;
}

const wchar_t* CRpcMassEditCmd::EnglishCommandName()
{
	return L"RPCMassEdit";
}

UUID CRpcMassEditCmd::CommandUUID()
{
	static const UUID uuid =
	{
		// {c866d819-be71-4611-abb9-34422cf10951}
		0xc866d819, 0xbe71, 0x4611, { 0xab, 0xb9, 0x34, 0x42, 0x2c, 0xf1, 0x09, 0x51 }
	};

	return uuid;
}
