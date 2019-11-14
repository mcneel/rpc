#pragma once

constexpr auto
PBR_BASE_COLOR = L"pbr-base-color",
PBR_METALLIC = L"pbr-metallic",
PBR_SPECULAR = L"pbr-specular",
PBR_SPECULAR_TINT = L"pbr-specular-tint",
PBR_ROUGHNESS = L"pbr-roughness",
PBR_ANISOTROPIC = L"pbr-anisotropic",
PBR_ANISOROTATION = L"pbr-anisotropic-rotation",
PBR_CLEARCOAT = L"pbr-clearcoat",
PBR_CLEARCOAT_ROUGH = L"pbr-clearcoat-roughness",
PBR_OPACITY_IOR = L"pbr-opacity-ior",
PBR_OPACITY = L"pbr-opacity",
PBR_OPACITY_ROUGH = L"pbr-opacity-roughness",
PBR_EMISSION = L"pbr-emission",
PBR_BUMP = L"pbr-bump",
PBR_DISPLACEMENT = L"pbr-displacement",
PBR_CLEARCOAT_BUMP = L"pbr-clearcoat-bump";

class CRpcRenderMeshBuilder
{
public:
	CRpcRenderMeshBuilder(const CRhinoDoc& doc, const RPCapi::Instance& rpc);
	~CRpcRenderMeshBuilder(void);

public:
	bool BuildOld(const ON_3dPoint& ptCamera, ON_SimpleArray<ON_Mesh*>& aMeshes,
			   ON_SimpleArray<CRhRdkMaterial*>& aMaterials);
	bool BuildNew(ON_SimpleArray<ON_Mesh*>& aMeshes,
		ON_SimpleArray<CRhRdkMaterial*>& aMaterials);

private:
	void Flush(ON_SimpleArray<ON_Mesh*>& aMeshes, ON_SimpleArray<CRhRdkMaterial*>& aMaterials);

	void RpcMesh2RhinoMeshes(const RPCapi::Mesh& RpcMesh, const RPCapi::TextureMesh& RpcTextureMesh, ON_SimpleArray<ON_Mesh*>& aMeshes);

	void RpcTexture2RhinoMaterial(const ON_SimpleArray<RPCapi::Texture*>& aTextures, ON_SimpleArray<CRhRdkMaterial*>& aMaterials);
	void RpcMaterial2RhinoMaterial(const ON_SimpleArray<RPCapi::Material*>& aRpcMaterials, ON_SimpleArray<CRhRdkMaterial*>& aMaterials);

	bool Rgb2Material(RPCapi::Texture& RpcTexture, CRhRdkMaterial& Material, const wchar_t* textureType, bool inverse);

	bool OldTexture2Material(RPCapi::Texture& RpcTexture, RPCapi::Texture::Channel::CHANNEL_CODE_T channel, CRhRdkBasicMaterial& Material,
		CRhRdkMaterial::ChildSlotUsage slotType, const wchar_t* textureType);

	template <typename T>
	void SetValue(CRhRdkMaterial& aMaterial, T& value, bool check,const wchar_t* paramName);
	void SetTexture(CRhRdkMaterial& aMaterial,RPCapi::Param* param, const wchar_t* paramName, bool inverse = false);
	void BaseMetalRoughness(RPCapi::Material& aRpcMaterial, CRhRdkMaterial& aMaterial);
	void Subsurface(RPCapi::Material& aRpcMaterial, CRhRdkMaterial& aMaterial);
	void Specularity(RPCapi::Material& aRpcMaterial, CRhRdkMaterial& aMaterial);
	void Anisotropy(RPCapi::Material& aRpcMaterial, CRhRdkMaterial& aMaterial);
	void Sheen(RPCapi::Material& aRpcMaterial, CRhRdkMaterial& aMaterial);
	void Clearcoat(RPCapi::Material& aRpcMaterial, CRhRdkMaterial& aMaterial);
	void Opacity(RPCapi::Material& aRpcMaterial, CRhRdkMaterial& aMaterial);
	void Emission(RPCapi::Material& aRpcMaterial, CRhRdkMaterial& aMaterial);
	void BumpDisplacement(RPCapi::Material& aRpcMaterial, CRhRdkMaterial& aMaterial);
	void AmbientOcclusion(RPCapi::Material& aRpcMaterial, CRhRdkMaterial& aMaterial);

	template <typename T>
	bool GetPrimValue(RPCapi::Param* param, T& value);
	RPCapi::Color* GetColor(RPCapi::Param* param);

private:
	const RPCapi::Instance& m_Rpc;
	const CRhinoDoc& m_doc;
};

