#include "StdAfx.h"
#include "RpcAnimationFrameRrp.h"
#include "RPCPlugIn.h"
#include "RpcMains.h"
#include "RpcRdkPlugIn.h"
#include "RpcInstance.h"
#include "RpcDocument.h"


UUID CRpcAnimationFrameRrp::PlugInId(void) const
{
	return PlugIn().PlugInID();
}

bool CRpcAnimationFrameRrp::SetValue(CRhinoDoc& doc, const CRhRdkVariant& value)
{
	if (!IsEnabledForDoc(doc))
		return false;

	const int iOldFrame = Mains().RpcDocument().AnimationFrame();
	
	if (iOldFrame == value.AsInteger())
		return true;

	Mains().RpcDocument().SetAnimationFrame(doc, value.AsInteger());

	CallPropertyChangedEvent(doc, iOldFrame);

	return true;
}

CRhRdkVariant CRpcAnimationFrameRrp::GetValue(const CRhinoDoc& doc) const
{
	if (!IsEnabledForDoc(doc))
		return CRhRdkVariant::Null();

	return Mains().RpcDocument().AnimationFrame();
}

bool CRpcAnimationFrameRrp::IsEnabledForDoc(const CRhinoDoc& doc) const 
{
	return IsEnabled();
}
