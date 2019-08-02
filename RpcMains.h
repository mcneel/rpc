#pragma once


class CRpcRdkPlugIn;
class CRpcClient;
class CRpcEventMachine;
class CRpcDragDropHandler;
class CRpcDocument;
class CRpcEventWatcher;
class CRPCPlugIn;
class CRpcPropertiesDlg;


class CRpcMains
{
public:
	CRpcMains(const CRPCPlugIn& plug);
	virtual ~CRpcMains(void);

public:
	bool Initialize(void);
	void Uninitialize(void);

public:
	const CRhRdkPlugIn& RdkPlugIn(void);
	CRpcClient& RpcClient(void);
	CRpcEventMachine& EventMachine(void) const;
	CRpcDocument& RpcDocument(void);
	CRpcEventWatcher& EventWatcher(void) const;
	CRpcPropertiesDlg& PropertiesDlg(void);

private:
	void CleanUp(void);
	void CreateCommands(void);

private:
	CRpcRdkPlugIn* m_pRdkPlugIn;
	CRpcClient* m_pRpcClient;
	CRpcDragDropHandler* m_pDragDropHandler;
	CRpcDocument* m_pRpcDocument;
	mutable CRpcEventMachine* m_pEventMachine;
	mutable CRpcEventWatcher* m_pEventWatcher;
	const CRPCPlugIn& m_PlugIn;
	CRpcPropertiesDlg* m_pRpcPropDlg;
};


CRpcMains& Mains(void);
