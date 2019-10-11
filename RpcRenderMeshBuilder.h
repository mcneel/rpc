#pragma once


class CRpcRenderMeshBuilder
{
public:
	CRpcRenderMeshBuilder(const CRhinoDoc& doc, const RPCapi::Instance& rpc);
	~CRpcRenderMeshBuilder(void);

public:
	bool BuildOld(const ON_3dPoint& ptCamera, ON_SimpleArray<ON_Mesh*>& aMeshes,
			   ON_SimpleArray<CRhRdkBasicMaterial*>& aMaterials);
	bool BuildNew(ON_SimpleArray<ON_Mesh*>& aMeshes,
		ON_SimpleArray<CRhRdkBasicMaterial*>& aMaterials);

private:
	void Flush(ON_SimpleArray<ON_Mesh*>& aMeshes, ON_SimpleArray<CRhRdkBasicMaterial*>& aMaterials);

	void RpcMesh2RhinoMeshes(const RPCapi::Mesh& RpcMesh, const RPCapi::TextureMesh& RpcTextureMesh, ON_SimpleArray<ON_Mesh*>& aMeshes);

	void RpcTexture2RhinoMaterial(const ON_SimpleArray<RPCapi::Texture*>& aTextures, ON_SimpleArray<CRhRdkBasicMaterial*>& aMaterials);
	void RpcMaterial2RhinoMaterial(const ON_SimpleArray<RPCapi::Material*>& aRpcMaterials, ON_SimpleArray<CRhRdkBasicMaterial*>& aMaterials);

	template <typename T>
	bool Rgb2Material(T& RpcTexture, CRhRdkBasicMaterial& Material, CRhRdkMaterial::ChildSlotUsage slotType, const wchar_t* textureType);
	template <typename T>
	bool Alpha2Material(T& RpcTexture, CRhRdkBasicMaterial& Material);

	void SetColor(RPCapi::Material& aRpcMaterial, CRhRdkBasicMaterial& aMaterial);
	void SetTransparency(RPCapi::Material& aRpcMaterial, CRhRdkBasicMaterial& aMaterial);
	void SetGlossFinish(RPCapi::Material& aRpcMaterial, CRhRdkBasicMaterial& aMaterial);
	void SetReflectivity(RPCapi::Material& aRpcMaterial, CRhRdkBasicMaterial& aMaterial);
	void SetBump(RPCapi::Material& aRpcMaterial, CRhRdkBasicMaterial& aMaterial);
	void SetAlphaTransparency(RPCapi::Material& aRpcMaterial, CRhRdkBasicMaterial& aMaterial);
	void SetEmission(RPCapi::Material& aRpcMaterial, CRhRdkBasicMaterial& aMaterial);

	template <typename T>
	bool GetPrimValue(RPCapi::Param* param, T& value);
	RPCapi::Color* GetColor(RPCapi::Param* param);

private:
	const RPCapi::Instance& m_Rpc;
	const CRhinoDoc& m_doc;
};

