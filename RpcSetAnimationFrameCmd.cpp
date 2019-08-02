#include "StdAfx.h"
#include "RpcSetAnimationFrameCmd.h"
#include "RPCPlugIn.h"
#include "RpcDocument.h"
#include "RpcMains.h"
#include "RpcUtilities.h"


const wchar_t * CRpcSetAnimationFrameCmd::EnglishCommandName()
{
	return L"RPCSetAnimationFrame";
}

UUID CRpcSetAnimationFrameCmd::CommandUUID()
{
	// {DEF313CB-2713-461d-9ED9-F5E021ADC2A1}
	static const GUID id = 
	{ 0xdef313cb, 0x2713, 0x461d, { 0x9e, 0xd9, 0xf5, 0xe0, 0x21, 0xad, 0xc2, 0xa1 } };
	return id;
}

CRhinoCommand::result CRpcSetAnimationFrameCmd::RunRpcCommand(const CRhinoCommandContext& context)
{
	const int iOldFrame = Mains().RpcDocument().AnimationFrame();

	CRhinoGetInteger gi;
	gi.SetLowerLimit(0);
	gi.SetDefaultInteger(iOldFrame);
	gi.SetCommandPrompt(_RhLocalizeString( L"Animation Frame", 36083));
	if (CRhinoGet::number != gi.GetInteger())
		return cancel;

	const int iNewFrame = gi.Number();

	if (iNewFrame == iOldFrame)
		return success;

	Mains().RpcDocument().SetAnimationFrame(context.m_doc, iNewFrame);

	return success;
}
