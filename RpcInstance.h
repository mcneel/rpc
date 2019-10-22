
#pragma once

class CRpcInstance : public RPCapi::ClientInstance
{
public:
	CRpcInstance(const CRhinoDoc& doc, const CLBPString& sFullPath);
	CRpcInstance(const CRhinoDoc& doc, const CRhinoObject& obj);
	bool IsCreated = false;
	
	virtual ~CRpcInstance();

public:
	bool IsValid(void) const;
	int CreateLayer(wstring& rpcName);

	CLBPString FileName(void) const;
	CLBPString ObjectName(void) const;
	CLBPString RpcName(void) const;

	void SetId(const RPCapi::ID *id);
	const RPCapi::ID* Id(void) const;

	bool Data(CLBPBuffer& buf) const;
	void SetData(const CLBPBuffer& buf);

	unsigned int Document(void) const { return m_idDoc; } 

	CRhinoInstanceObject* AddToDocument(CRhinoDoc& doc, const ON_3dPoint& pt);
	CRhinoInstanceObject* Replace(CRhinoDoc& doc);

	bool CopyToRpc(const CRhinoObject& obj);
	bool CopyFromRpc(const CRhinoObject& obj) const;

	class IEditDialogCallback
	{
	public:
		virtual void OnRpcParameterChanged(void) = 0;
	};

	bool EditUi(HWND hWndParent, IEditDialogCallback* pCallback = NULL);

public:
	RPCapi::Instance* Instance(void);

	static bool IsValidRpc(const CLBPString& s);
	
private:
	// RPCapi::ClientInstance overrides
	virtual RPCapi::Instance* RPCgetInstance(void);
	virtual TStringArg RPCgetName(void);
	virtual void RPCgetPivot(double px, double py, double pz, double &distance, double &dx, double &dy, double &dz);
	virtual int RPCgetTime(void);
	virtual bool RPCisSelected(void);
	virtual void RPCparameterChangeNotification(bool newInstance, const TString **params, int num);
	virtual void RPCselect(bool select);

private:
	void Construct(UINT idDoc, const CRhinoObject* pObject, const CLBPString& sRpcPath);
	void KillEditUi(void);

	CRhinoInstanceObject* AddToDocument(CRhinoDoc& doc, const CLBPString& sName, const ON_Xform& xform);
	const CRhinoObject* Object(void) const;

private:
	RPCapi::Instance* m_pInstance;
	UUID m_idObject;
	unsigned int m_idDoc;
	mutable CLBPString m_sName;
	RPCapi::InstanceInterface* m_pEditInterface;
	IEditDialogCallback* m_pEditDlgCallback;
	wchar_t *categoryName, *contentName;
};
