#pragma once


class CRpcRenderMeshBuilder
{
public:
	CRpcRenderMeshBuilder(const CRhinoDoc& doc, const RPCapi::Instance& rpc);
	~CRpcRenderMeshBuilder(void);

public:
	bool Build(const ON_3dPoint& ptCamera, ON_SimpleArray<ON_Mesh*>& aMeshes,
			   ON_SimpleArray<CRhRdkBasicMaterial*>& aMaterials);

private:
	void Flush(ON_SimpleArray<ON_Mesh*>& aMeshes, ON_SimpleArray<CRhRdkBasicMaterial*>& aMaterials);

	void RpcMesh2RhinoMeshes(const RPCapi::Mesh& RpcMesh, const RPCapi::TextureMesh& RpcTextureMesh, ON_SimpleArray<ON_Mesh*>& aMeshes);

	void RpcTexture2RhinoMaterial(const ON_SimpleArray<RPCapi::Texture*>& aTextures, ON_SimpleArray<CRhRdkBasicMaterial*>& aMaterials);

	bool Rgb2Material(RPCapi::Texture& RpcTexture, CRhRdkBasicMaterial& Material);
	bool Alpha2Material(RPCapi::Texture& RpcTexture, CRhRdkBasicMaterial& Material);

private:
	const RPCapi::Instance& m_Rpc;
	const CRhinoDoc& m_doc;
};
