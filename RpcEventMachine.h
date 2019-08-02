#pragma once


#include "RpcEventSink.h"


class CRpcEventSinkBaseArray : public CArray<CRpcEventSinkBase*, CRpcEventSinkBase*>
{
};

class CRpcEventMachine
{
public:
	CRpcEventMachine();
	virtual ~CRpcEventMachine() { }

	void RegisterEventSink(CRpcEventSinkBase* pEventSink);
	void UnregisterEventSink(CRpcEventSinkBase* pEventSink);

	void EnableEvents(bool bEnabled);

public:
	void OnRpcParameterChanged(const RPCapi::ID* id);
	void OnAnimationFrameChanged(CRhinoDoc& doc, int iOldFrame, int iNewFrame);

protected:
	int EventSinkIndex(const CRpcEventSinkBase* pEventSink);
	void CleanUp(void);

private:
	CRpcEventSinkBaseArray m_aEventSinks;
	bool m_bEnabled;
};
