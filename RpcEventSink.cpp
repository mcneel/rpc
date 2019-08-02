#include "StdAfx.h"
#include "RpcEventSink.h"
#include "RPCPlugIn.h"
#include "RpcMains.h"
#include "RpcEventMachine.h"


CRpcEventSinkBase::CRpcEventSinkBase()
{

}

CRpcEventSinkBase::~CRpcEventSinkBase()
{

}


CRpcEventSink::CRpcEventSink()
{
	Mains().EventMachine().RegisterEventSink(this);
}

CRpcEventSink::~CRpcEventSink()
{
	Mains().EventMachine().UnregisterEventSink(this);
}
