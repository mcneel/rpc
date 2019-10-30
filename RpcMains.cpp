
#include "stdafx.h"
#include "RpcMains.h"
#include "RPCApp.h"
#include "RpcCrmProvider.h"
#include "RpcRdkPlugIn.h"
#include "RpcClient.h"
#include "RpcUtilities.h"
#include "RpcEventMachine.h"
#include "RpcDragDropHandler.h"
#include "RpcDocument.h"
#include "RpcAnimationFrameRrp.h"
#include "RpcEventWatcher.h"
#include "RpcPropertiesDlg.h"
#include "RpcAddCmd.h"
#include "RpcEditCmd.h"
#include "RpcSetAnimationFrameCmd.h"

void CRpcMains::CreateCommands(void)
{
	static class CRpcAddCmd theRpcAddCmd;
	static class CRpcEditCmd theRpcEditCmd;
	static class CRpcSetAnimationFrameCmd theSetAnimationFrameCmd;
}


extern CRPCApp theApp;


CRpcMains& Mains(void)
{
	return PlugIn().Mains();
}


CRpcMains::CRpcMains(const CRPCPlugIn& plug)
: m_PlugIn(plug)
{
	m_pRpcClient = NULL;
	m_pRdkPlugIn = NULL;
	m_pEventMachine = NULL;
	m_pDragDropHandler = NULL;
	m_pRpcDocument = NULL;
	m_pEventWatcher = NULL;
	m_pRpcPropDlg = NULL;
	rpcTable = NULL;
}

CRpcMains::~CRpcMains(void)
{
	CleanUp();
}

void CRpcMains::CleanUp(void)
{
	if (m_pEventWatcher)
	{
		m_pEventWatcher->Enable(false);
		m_pEventWatcher->UnRegister();
		delete m_pEventWatcher;
		m_pEventWatcher = NULL;
	}

	if (m_pRdkPlugIn)
	{
		m_pRdkPlugIn->Uninitialize();
		delete m_pRdkPlugIn;
		m_pRdkPlugIn = NULL;
	}

	if (m_pRpcClient)
	{
		m_pRpcClient->Uninitialize();
		delete m_pRpcClient;
		m_pRpcClient = NULL;
	}

	if(m_pEventMachine)
	{
		m_pEventMachine->EnableEvents(false);
	}

	if (m_pRpcDocument)
	{
		delete m_pRpcDocument;
		m_pRpcDocument = NULL;
	}

	if (m_pRpcPropDlg)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		delete m_pRpcPropDlg;
		m_pRpcPropDlg = NULL;
	}

	if (m_pEventMachine)
	{
		delete m_pEventMachine;
		m_pEventMachine = NULL;
	}

	if (m_pDragDropHandler)
	{
		delete m_pDragDropHandler;
		m_pDragDropHandler = NULL;
	}

	if (rpcTable)
	{
		delete rpcTable;
		rpcTable = NULL;
	}
}

bool CRpcMains::Initialize(void)
{
	m_pRdkPlugIn = new CRpcRdkPlugIn;
	if (!m_pRdkPlugIn->Initialize())
	{
		RpcMessageBox(_RhLocalizeString( L"Failed to register RDK client.", 32814), MB_ICONERROR);
		CleanUp();
		return false;
	}

	RhRdkRegisteredPropertiesManager().Add(new CRpcAnimationFrameRrp);

	m_pRpcClient = new CRpcClient;
	if (!m_pRpcClient->Initialize())
	{
		RpcMessageBox(_RhLocalizeString( L"Failed to register RPC client.", 32815), MB_ICONERROR);
		CleanUp();
		return false;
	}

	rpcTable = new ON_SimpleUuidMap<CRpcInstance*>();
	EventWatcher();
	EventMachine();
	RpcDocument();

	m_pDragDropHandler = new CRpcDragDropHandler;
	m_pDragDropHandler->EnableRhinoDropTarget(true);
	m_pDragDropHandler->EnableAllowDropOnObject(false);
	m_pDragDropHandler->EnableAllowDropOnSubObject(false);
	m_pDragDropHandler->EnableAllowDropOnRhinoView(true);
	m_pDragDropHandler->EnableAllowDropOnRhinoLayerListControl(false);
	m_pDragDropHandler->EnableDeselectAllOnDrag(false);

	CreateCommands();

	return true;
}

void CRpcMains::Uninitialize(void)
{
	CleanUp();
}

const CRhRdkPlugIn& CRpcMains::RdkPlugIn()
{ 
	return *m_pRdkPlugIn; 
}

CRpcClient& CRpcMains::RpcClient(void)
{
	return *m_pRpcClient;
}

CRpcEventMachine& CRpcMains::EventMachine(void) const
{
	if (!m_pEventMachine)
	{
		m_pEventMachine = new CRpcEventMachine;
		m_pEventMachine->EnableEvents(true);
	}

	return *m_pEventMachine;
}

CRpcEventWatcher& CRpcMains::EventWatcher(void) const
{
	if (!m_pEventWatcher)
	{
		m_pEventWatcher = new CRpcEventWatcher;
		m_pEventWatcher->Register();
		m_pEventWatcher->Enable();
	}

	return *m_pEventWatcher;
}

CRpcDocument& CRpcMains::RpcDocument(void)
{
	if (!m_pRpcDocument)
	{
		m_pRpcDocument = new CRpcDocument;
	}

	return *m_pRpcDocument;
}

CRpcPropertiesDlg& CRpcMains::PropertiesDlg(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!m_pRpcPropDlg)
	{
		m_pRpcPropDlg = new CRpcPropertiesDlg;
	}

	return *m_pRpcPropDlg;
}

ON_SimpleUuidMap<CRpcInstance*>& CRpcMains::GetRPCInstanceTable()
{
	if (!rpcTable)
	{
		rpcTable = new ON_SimpleUuidMap<CRpcInstance*>();
	}

	return *rpcTable;
}
