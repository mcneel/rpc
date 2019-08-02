#pragma once

class CRpcClient : public RPCapi::Client
{
public:
	CRpcClient(void);
	~CRpcClient(void);

public:
	bool Initialize(void);
	bool Uninitialize(void);

public:
	bool Valid(void) const;
	RPCapi& RPC(void) const;

	void OnRpcInstanceWillBeCreated(UINT idDoc, const CLBPString& sRpc);

public:
	// RPCapi::Client overrides
	virtual TStringArg RPCgetAboutString(void);
	virtual RPCapi* RPCgetAPI(void);
	virtual const RPCapi::License* RPCgetLicense(void);
	virtual TStringArg* RPCgetPaths(bool start, int &numPaths);
	virtual int RPCgetMode();
	virtual TStringArg RPCiniPath(void);
	virtual void RPClicenseChange(bool licensed, bool acm);
	virtual void RPCpluginMetadata(int &num, const TStringArg*& keys, const RPCapi::Param**& values);
	virtual void RPCcontentMetadata(int req, int &numsets, const int *&setsize, const TStringArg **&keys, const RPCapi::Param ***&values);
	virtual TStringArg RPCpluginId(void);
	virtual bool RPCuserMessage(int msgType, bool ret, const TString &title, const TString &msg);

private:
	bool IsRpcPathInList(const CLBPString& sPath) const;
	void AddRpcPathToList(const CLBPString& sPath);

	void Flush(void);
	
private:
	mutable RPCapi* m_pRpcApi;
	RPCapi::TStringArg* m_RpcPaths;
	ON_ClassArray<CLBPString> m_aRpcPath;
	HMODULE m_hRpcApiHandle;

	RPCapi::TStringArg *m_metaKeys;
	RPCapi::Param **m_metaValues;
	int m_numMetaValues;
};
