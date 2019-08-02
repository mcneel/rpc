
#pragma once

class CRpcEventSinkBase
{
public:
	CRpcEventSinkBase();
	virtual ~CRpcEventSinkBase();

public:
	virtual void OnRpcParameterChanged(const RPCapi::ID* id) { }
	virtual void OnAnimationFrameChanged(CRhinoDoc& doc, int iOldFrame, int iNewFrame) { }
};

class CRpcEventSink : protected CRpcEventSinkBase
{
public:
	CRpcEventSink();
	virtual ~CRpcEventSink();
};
