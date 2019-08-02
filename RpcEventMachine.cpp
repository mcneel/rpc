#include "StdAfx.h"
#include "RpcEventMachine.h"


#define EVENT_BEGIN   if (m_bEnabled) { for (int i = 0; i < m_aEventSinks.GetSize(); i++) { if (m_aEventSinks[i] != NULL) {
#define EVENT_CALL(f) m_aEventSinks[i]->f
#define EVENT_END     } } } CleanUp();


CRpcEventMachine::CRpcEventMachine()
{
	m_bEnabled = true;
}

void CRpcEventMachine::RegisterEventSink(CRpcEventSinkBase* pEventSink)
{
	m_aEventSinks.Add(pEventSink);
}

void CRpcEventMachine::UnregisterEventSink(CRpcEventSinkBase* pEventSink)
{
	const int iIndex = EventSinkIndex(pEventSink);
	if (iIndex >= 0)
	{
		m_aEventSinks[iIndex] = NULL;
	}
}

void CRpcEventMachine::EnableEvents(bool bEnabled)
{
	m_bEnabled = bEnabled;
}

int CRpcEventMachine::EventSinkIndex(const CRpcEventSinkBase* pEventSink)
{
	for (int i = 0; i < m_aEventSinks.GetSize(); i++)
	{
		if (m_aEventSinks[i] == pEventSink)
			return i;
	}

	return -1;
}

void CRpcEventMachine::CleanUp(void)
{
	int iIndex = 0;
	while (iIndex < m_aEventSinks.GetSize())
	{
		if (NULL == m_aEventSinks[iIndex])
		{
			m_aEventSinks.RemoveAt(iIndex);
		}
		else
		{
			iIndex++;
		}
	}
}

void CRpcEventMachine::OnRpcParameterChanged(const RPCapi::ID* id)
{
	EVENT_BEGIN;
	EVENT_CALL(OnRpcParameterChanged(id));
	EVENT_END;
}

void CRpcEventMachine::OnAnimationFrameChanged(CRhinoDoc& doc, int iOldFrame, int iNewFrame)
{
	EVENT_BEGIN;
	EVENT_CALL(OnAnimationFrameChanged(doc, iOldFrame, iNewFrame));
	EVENT_END;
}
