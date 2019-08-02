
#pragma once

#include "RpcEventSink.h"

class CRpcRdkPlugIn : public CRhRdkPlugIn, public CRpcEventSink
{
public:
	CRpcRdkPlugIn();
	virtual ~CRpcRdkPlugIn();

public:
	virtual void RegisterExtensions(void) const override;
	virtual UUID PlugInId(void) const override;
	virtual CRhinoPlugIn& RhinoPlugIn(void) const override;
	virtual bool SupportsFeature(const UUID&) const;

public:
	virtual void OnAnimationFrameChanged(CRhinoDoc& doc, int iOldFrame, int iNewFrame) override;
};
