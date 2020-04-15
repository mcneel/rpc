#pragma once

class CRpcInstance : public RPCapi::ClientInstance
{
public:
	class IEditDialogCallback
	{
	public:
		virtual void OnRpcParameterChanged(void) = 0;
	};

public:
	CRpcInstance(const CRhinoDoc& doc, const CLBPString& sFullPath);
	CRpcInstance(const CRhinoDoc& doc, const CRhinoObject& obj);

	virtual ~CRpcInstance();

public:
	bool IsValid() const;

	CLBPString FileName() const;
	CLBPString ObjectName() const;
	CLBPString RpcName() const;

	void SetId(const RPCapi::ID* id);
	const RPCapi::ID* Id() const;

	bool Data(CLBPBuffer& buf) const;
	void SetData(const CLBPBuffer& buf);

	unsigned int Document() const { return m_idDoc; }

	CRhinoInstanceObject* AddToDocument(CRhinoDoc& doc, const ON_3dPoint& pt);
	CRhinoInstanceObject* Replace(CRhinoDoc& doc, bool copied = false, const CRhinoObject* object = nullptr);
	void OnRpcInstanceChanged();

	bool CopyToRpc(const CRhinoObject& obj);
	bool CopyFromRpc(const CRhinoObject& obj) const;

	bool EditUi(HWND hWndParent, IEditDialogCallback* pCallback = nullptr);
	bool EditSelUI(HWND hWndParent, IEditDialogCallback* pCallback = nullptr);
	void KillEditUi();
	void KillEditSelUi();

	RPCapi::Instance* Instance();
	CRhinoMeshObject* CreateProxyMesh(CRhinoDoc& doc);
	static bool IsValidRpc(const CLBPString& s);

private:
	// RPCapi::ClientInstance implementation
	RPCapi::Instance* RPCgetInstance() override;
	TStringArg RPCgetName() override;
	void RPCgetPivot(double px, double py, double pz, double& distance, double& dx, double& dy, double& dz) override;
	int RPCgetTime() override;
	bool RPCisSelected() override;
	void RPCparameterChangeNotification(bool newInstance, const TString** params, int num) override;
	void RPCselect(bool select) override;

	void Construct(UINT idDoc, const CRhinoObject* pObject, const CLBPString& sRpcPath);
	const CRhinoObject* Object() const;
	CRhinoInstanceObject* AddToDocument(CRhinoDoc& doc, const CLBPString& sName, const ON_Xform& xform, int copiedLayer = 0);
	int CreateLayer(wstring& rpcName, int copiedLayer);
    void UpdatePrevUnitSystem();

private:
	RPCapi::Instance* m_pInstance;
	UUID m_idObject;
	unsigned int m_idDoc;
	mutable CLBPString m_sName;
	RPCapi::InstanceInterface* selectionInterface;
	RPCapi::InstanceInterface* m_pEditInterface;
	IEditDialogCallback* m_pEditDlgCallback;
    ON::LengthUnitSystem prevUnitSystem;
};
