
#include "StdAfx.h"
#include "RpcRenderMeshBuilder.h"
#include "RpcUtilities.h"
#include "MaterialParams.h"

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

static class CTestRpcGamma : public CRhinoTestCommand
{
	static double m_dGamma;

	virtual const wchar_t * EnglishCommandName() { return L"TestRPCGamma"; }
	virtual UUID CommandUUID()
	{
		// {F70A388B-7298-4506-ACFE-1FACD6106B1C}
		static const GUID uuid = { 0xf70a388b, 0x7298, 0x4506, { 0xac, 0xfe, 0x1f, 0xac, 0xd6, 0x10, 0x6b, 0x1c } };
		return uuid;
	}

	virtual CRhinoCommand::result RunCommand(const CRhinoCommandContext& context)
	{
		CRhinoGetNumber gi;
		gi.SetLowerLimit(0.0);
		gi.SetUpperLimit(5.0);
		gi.SetDefaultNumber(m_dGamma);

		gi.SetCommandPrompt(L"RPC texture gamma:");
		if (CRhinoGet::number != gi.GetNumber())
			return cancel;

		m_dGamma = gi.Number();

		return success;
	}

	friend class CRpcRenderMeshBuilder;
}
theTestRPCCommand;

double CTestRpcGamma::m_dGamma = 1.0;

CRpcRenderMeshBuilder::CRpcRenderMeshBuilder(const CRhinoDoc& doc, const RPCapi::Instance& rpc)
: m_Rpc(rpc),
  m_doc(doc)
{
}

CRpcRenderMeshBuilder::~CRpcRenderMeshBuilder(void)
{
}

void CRpcRenderMeshBuilder::Flush(ON_SimpleArray<ON_Mesh*>& aMeshes,
								  ON_SimpleArray<CRhRdkMaterial*>& aMaterials)
{
	for(int i=0; i<aMeshes.Count(); i++)
	{
		delete aMeshes[i];
	}

	aMeshes.Destroy();

	for(int i=0; i<aMaterials.Count(); i++)
	{
		delete aMaterials[i];
	}

	aMaterials.Destroy();
}

void CRpcRenderMeshBuilder::RpcMaterial2RhinoMaterial(const ON_SimpleArray<RPCapi::Material*>& aRpcMaterials, ON_SimpleArray<CRhRdkMaterial*>& aMaterials)
{
	for (int i = 0; i < aRpcMaterials.Count(); i++)
	{
		aMaterials.Append(CreatePBMaterial());

		// Skipping of empty material.
		if (aRpcMaterials[i]->count() <= 1)
		{
			continue;
		}

		RPCapi::Material* mat = aRpcMaterials[i];
		BaseMetalRoughness(*mat, *aMaterials[i]);
		Specularity(*mat, *aMaterials[i]);
		Opacity(*mat, *aMaterials[i]);
		Clearcoat(*mat, *aMaterials[i]);
		Emission(*mat, *aMaterials[i]);
		BumpDisplacement(*mat, *aMaterials[i]);
	}
}

bool CRpcRenderMeshBuilder::BuildNew(ON_SimpleArray<ON_Mesh*>& aMeshes,
	ON_SimpleArray<CRhRdkMaterial*>& aMaterials)
{
	RPCapi::Mesh *pRpcMesh;
	RPCapi::TextureMesh *pTextureMesh;
	RPCapi::Material **pRpcMaterials = nullptr;
	int numMaterials;

	const double dUnitsScale = ON::UnitScale(ON::LengthUnitSystem::Inches, m_doc.ModelUnits());
	ON_Xform xformUnitsScale = ON_Xform::DiagonalTransformation(dUnitsScale, dUnitsScale, dUnitsScale);

	numMaterials = m_Rpc.getRenderData(pRpcMesh, pRpcMaterials, pTextureMesh);
	
	if (numMaterials == 0)
	{
		return false;
	}

	ON_SimpleArray<RPCapi::Material*> aRpcMaterials;

	for (int i = 0; i < numMaterials; i++)
	{
		aRpcMaterials.Append(pRpcMaterials[i]);
		aMeshes.Append(new ON_Mesh);
	}

	RpcMesh2RhinoMeshes(*pRpcMesh, *pTextureMesh, aMeshes);
	RpcMaterial2RhinoMaterial(aRpcMaterials, aMaterials);

	for (int i = 0; i < aMeshes.Count(); i++)
	{
		aMeshes[i]->Transform(xformUnitsScale);
	}

	if (pRpcMaterials)
	{
		for (int i = 0; i < numMaterials; i++)
		{
			if (pRpcMaterials[i])
			{
				delete pRpcMaterials[i];
				pRpcMaterials[i] = nullptr;
			}
		}
		delete[] pRpcMaterials;
	}

	delete pTextureMesh;
	delete pRpcMesh;

	return true;
}

bool CRpcRenderMeshBuilder::BuildOld(const ON_3dPoint& ptCamera, ON_SimpleArray<ON_Mesh*>& aMeshes,
								  ON_SimpleArray<CRhRdkMaterial*>& aMaterials)
{
	const RPCapi::Mesh* pRpcMesh = m_Rpc.getMesh(nullptr, ptCamera.x, ptCamera.y, ptCamera.z);
	if (!pRpcMesh) 
		return false;

	const double dUnitsScale = ON::UnitScale(ON::LengthUnitSystem::Inches, m_doc.ModelUnits());
	ON_Xform xformUnitsScale = ON_Xform::DiagonalTransformation(dUnitsScale, dUnitsScale, dUnitsScale);

	RPCapi::Texture** Texture = nullptr;
	RPCapi::TextureMesh* pTextureMesh = nullptr;
	int iTextures = 0;

	if (!m_Rpc.getTextures(ptCamera.x, ptCamera.y, ptCamera.z, iTextures, Texture, pTextureMesh))
	{
		ON_Mesh* pRhinoMesh = new ON_Mesh;
		RpcMesh2RhinoMesh(*pRpcMesh, *pRhinoMesh);

		pRhinoMesh->Transform(xformUnitsScale);

		aMeshes.Append(pRhinoMesh);
		aMaterials.Append(nullptr);

		delete pRpcMesh;

		return true;
	}

	ON_SimpleArray<RPCapi::Texture*> aTextures;

	for (int i=0; i<iTextures; i++)
	{
		aTextures.Append(Texture[i]);
		aMeshes.Append(new ON_Mesh);
	}

	RpcMesh2RhinoMeshes(*pRpcMesh, *pTextureMesh, aMeshes);
	RpcTexture2RhinoMaterial(aTextures, aMaterials);

	for(int i=0; i<aMeshes.Count(); i++)
	{
		aMeshes[i]->Transform(xformUnitsScale);
	}

	if (Texture)
	{
		for (int i=0; i<iTextures; i++)
		{
			if (Texture[i])
			{
				delete Texture[i];
				Texture[i] = nullptr;
			}
		}
		delete[] Texture;
	}

	delete pTextureMesh;
	delete pRpcMesh;

	return true;
}

static void AddVertex(	int i,
						int iMeshFaceVertexIndex,
						int iTextureFaceVertexIndex,
						ON_SimpleMap<int,int>& map,
						ON_SimpleArray<int>& sourceVertexIndexList,
						//ON_SimpleMap<int,int>& tc_map,
						ON_MeshFace& on_face,
						ON_Mesh& on_mesh,
						const RPCapi::Mesh::Vertex* pVerts,
						const RPCapi::TextureMesh::Vertex* pTextureVerts						
						)
{
	int iONMeshVertexIndex = -1;
	on_face.vi[i] = -1;

	if (map.Lookup(iMeshFaceVertexIndex, iONMeshVertexIndex))
	{
		//ASSERT(tc_map.Lookup(iTextureFaceVertexIndex, iONMeshTCIndex)));
		//ASSERT(iONMeshVertexIndex == iONMeshTCIndex);

		const ON_2fPoint& t = on_mesh.m_T[iONMeshVertexIndex];
		const RPCapi::TextureMesh::Vertex& tv = pTextureVerts[iTextureFaceVertexIndex];

		if (LBPIsFloatEqual(t.x,(float)tv.x) && LBPIsFloatEqual(t.y,(float)tv.y))
		{
			on_face.vi[i] = iONMeshVertexIndex;
		}		
	}

	if (on_face.vi[i] == -1)
	{
		const int iIndex = on_mesh.m_V.Count();
		on_face.vi[i] = iIndex;

		ON_3fPoint& v = on_mesh.m_V.AppendNew();
		v.x = float(pVerts[iMeshFaceVertexIndex].x);
		v.y = float(pVerts[iMeshFaceVertexIndex].y);
		v.z = float(pVerts[iMeshFaceVertexIndex].z);
		
		ON_2fPoint& t = on_mesh.m_T.AppendNew();
		t.x = float(pTextureVerts[iTextureFaceVertexIndex].x);
		t.y = float(pTextureVerts[iTextureFaceVertexIndex].y);

		map.SetAt(iMeshFaceVertexIndex, iIndex);

		sourceVertexIndexList.Append(iMeshFaceVertexIndex);
	}
}

template<class FACE>
static bool IsMeshFaceValid(const FACE& face)
{
	return face.v0 != face.v1 && face.v1 != face.v2 && face.v2 != face.v0;
}

static bool IsDegenerateTriangle(const ON_Mesh& mesh, const ON_MeshFace& face)
{
	if (face.IsQuad()) return true;

	if (!face.IsValid(mesh.m_V.Count()))
		return true;

	const float fSmall = 0.000001f;
	
	if ((mesh.m_V[face.vi[0] ] - mesh.m_V[face.vi[1] ]).LengthSquared() < fSmall)
		return true;

	if ((mesh.m_V[face.vi[1] ] - mesh.m_V[face.vi[2] ]).LengthSquared() < fSmall)
		return true;

	if ((mesh.m_V[face.vi[2] ] - mesh.m_V[face.vi[0] ]).LengthSquared() < fSmall)
		return true;
	
	return false;	
}

void CRpcRenderMeshBuilder::RpcMesh2RhinoMeshes(const RPCapi::Mesh& RpcMesh,
												const RPCapi::TextureMesh& RpcTextureMesh,
												ON_SimpleArray<ON_Mesh*>& aMeshes)
{
	ON_SimpleMap<int,int>* pMaps = new ON_SimpleMap<int,int>[aMeshes.Count()];
	const int iFaceCount = RpcMesh.getNumFaces();
	const RPCapi::Mesh::Face* pFaces = RpcMesh.getConstFaces();

	ON_SimpleArray<int>* pSourceVertexIndexLists = new  ON_SimpleArray<int> [aMeshes.Count()];

	const int iVertexCount = RpcMesh.getNumVerts();
	const RPCapi::Mesh::Vertex* pVerts = RpcMesh.getConstVertices();

	const RPCapi::TextureMesh::Face* pTectureFaces = RpcTextureMesh.getConstFaces();
	const int iTextureFaceCount = RpcTextureMesh.getNumFaces();

	const RPCapi::TextureMesh::Vertex* pTextureVerts = RpcTextureMesh.getConstVertices();

	ASSERT(iFaceCount == iTextureFaceCount);
	int iBadFaces = 0;

	for (int i=0; i<iTextureFaceCount; i++)
	{
		const RPCapi::TextureMesh::Face* pTextureFace = pTectureFaces+i;
		const int iTextureIndex = pTextureFace->u.textureIndex;

		const RPCapi::Mesh::Face* pFace = pFaces+i;

		if (IsMeshFaceValid(*pFace) && IsMeshFaceValid(*pTextureFace))
		{
			ON_Mesh* pRhinoMesh = aMeshes[iTextureIndex];
			ON_SimpleMap<int,int>& map = pMaps[iTextureIndex];

			ON_SimpleArray<int> & sourceVertexIndexList = pSourceVertexIndexLists[iTextureIndex];

			ON_MeshFace face;// = pRhinoMesh->m_F.AppendNew();

			const int iVertexCount = pRhinoMesh->m_V.Count();

			AddVertex(0, pFace->v0, pTextureFace->v0, map, sourceVertexIndexList, face, *pRhinoMesh, pVerts, pTextureVerts);
			AddVertex(1, pFace->v1, pTextureFace->v1, map, sourceVertexIndexList, face, *pRhinoMesh, pVerts, pTextureVerts);
			AddVertex(2, pFace->v2, pTextureFace->v2, map, sourceVertexIndexList, face, *pRhinoMesh, pVerts, pTextureVerts);
			face.vi[3] = face.vi[2];

			pRhinoMesh->m_F.Append(face);
			ASSERT(pRhinoMesh->IsValid());
		}
		else
		{
			iBadFaces++;
		}
	}

	if (iBadFaces > 0)
	{
		RhinoApp().Print(_RhLocalizeString( L"Bad face count = %d\n", 36082), iBadFaces);
	}

	delete [] pMaps;

	for(int i=0; i<aMeshes.Count(); i++)
	{
		aMeshes[i]->ComputeVertexNormals();
		if (aMeshes[i]->m_N.Count() == aMeshes[i]->m_V.Count())
		{
			UniteVertexNormals(*aMeshes[i], pSourceVertexIndexLists[i], iVertexCount);
		}
	}

	delete [] pSourceVertexIndexLists;
}

bool CRpcRenderMeshBuilder::Rgb2Material(RPCapi::Texture& RpcTexture, CRhRdkMaterial& Material, const wchar_t* textureType, bool inverse)
{
	int iWidth = 0;
	int iHeight = 0;
	int iBytes;
	unsigned char* pRGB = nullptr;

	iBytes = RpcTexture.data(pRGB, false, RPCapi::Texture::Channel::RGB, RPCapi::Texture::Scale::SLOW, iWidth, iHeight);

	if (iBytes > 0)
	{
		pRGB = new BYTE[iBytes];

		VERIFY(iBytes = RpcTexture.data(pRGB, false, RPCapi::Texture::Channel::RGB, RPCapi::Texture::Scale::SLOW, iWidth, iHeight));
	}
	else
	{
		return false;
	}

	BYTE* rgb = pRGB;

	CRhinoDib rdRGB(iWidth, iHeight, 32);

	for(int y=0; y<iHeight; y++)
	{
		for(int x=0; x<iWidth; x++)
		{
			if (rgb)
			{
				rdRGB.SetPixel(x, y, rgb[0], rgb[1], rgb[2], 255);
				rgb += 3;
			}
		}
	}
	
	// TODO: [HERE] Possible dib ownership problem - check RDK SDK comments.
	CRhRdkTexture* pRdkTexture = RhRdkNewDibTexture(&rdRGB, Material.DocumentAssoc(), false, true);

	if (CTestRpcGamma::m_dGamma != 1.0)
	{
		pRdkTexture->SetAdjustmentGamma(CTestRpcGamma::m_dGamma);
	}

	pRdkTexture->SetAdjustmentInvert(inverse);

	VERIFY(Material.SetChild(pRdkTexture, textureType));
	Material.SetChildSlotOn(textureType, true);

	delete[] pRGB;

	return true;
}

bool CRpcRenderMeshBuilder::OldTexture2Material(RPCapi::Texture& RpcTexture, RPCapi::Texture::Channel::CHANNEL_CODE_T channel,
	CRhRdkBasicMaterial& Material, CRhRdkMaterial::ChildSlotUsage slotType, const wchar_t* textureType)
{
	int iWidth = 0;
	int iHeight = 0;
	int iBytes;
	unsigned char* pixels = nullptr;

	iBytes = RpcTexture.data(pixels, false, channel,RPCapi::Texture::Scale::SLOW, iWidth, iHeight);
	
	if (iBytes > 0)
	{
		pixels = new BYTE[iBytes];
		VERIFY(iBytes == RpcTexture.data(pixels, false, channel,RPCapi::Texture::Scale::SLOW, iWidth, iHeight));
	}
	else 
	{
		return false;
	}

	BYTE* alp = pixels;

	CRhinoDib rDib(iWidth, iHeight, 32);

	if (channel == RPCapi::Texture::Channel::RGB)
	{
		for (int y = 0; y < iHeight; y++)
		{
			for (int x = 0; x < iWidth; x++)
			{
				rDib.SetPixel(x, y, alp[0], alp[1], alp[2], 255);
				alp += 3;
			}
		}
	}
	else
	{
		for (int y = 0; y < iHeight; y++)
		{
			for (int x = 0; x < iWidth; x++)
			{
				rDib.SetPixel(x, y, alp[0], alp[0], alp[0], 255);
				alp += 1;
			}
		}
	}

	// TODO: [HERE] Possible dib ownership problem - check RDK SDK comments.
	CRhRdkTexture* pRdkTexture = RhRdkNewDibTexture(&rDib, Material.DocumentAssoc(), false, true);

	CRhRdkBasicMaterial::CTextureSlot slot = Material.TextureSlot(slotType);
	slot.m_bOn = true;
	slot.m_dAmount = 1.0;
	slot.m_bFilterOn = true;
	Material.SetTextureSlot(slotType, slot);

	VERIFY(Material.SetChild(pRdkTexture, textureType));

	delete[] pixels;

	return true;
}

template <typename T>
void CRpcRenderMeshBuilder::SetValue(CRhRdkMaterial & aMaterial, T& value, bool check, const wchar_t* paramName)
{
	CRhRdkVariant *data = new CRhRdkVariant(value);
	wstring checkBox(paramName);
	checkBox += L"-on";
	aMaterial.SetParameter(checkBox.c_str(), check);
	aMaterial.SetParameter(paramName, *data);
}

void CRpcRenderMeshBuilder::SetTexture(CRhRdkMaterial & aMaterial, RPCapi::Param * param, const wchar_t * paramName, bool inverse)
{
	if (!param || param->typeCode() != RPCapi::ObjectCodes::TYPE_TEXMAP)
		return;

	auto pMap = dynamic_cast<RPCapi::TextureMap*>(param);

	if (!pMap)
		return;

	const RPCapi::TStringArg MAP("map_name");

	param = pMap->get(MAP);

	auto image = dynamic_cast<RPCapi::Texture*>(param);

	if (!image)
		return;

	Rgb2Material(*image, aMaterial, paramName, inverse);
}

void CRpcRenderMeshBuilder::BaseMetalRoughness(RPCapi::Material& aRpcMaterial, CRhRdkMaterial& aMaterial)
{
	auto pColor = GetColor(aRpcMaterial.get(getParamName(MaterialParams::BASE_COLOR)));

	if (pColor)
	{
		ON_Color *color = new ON_Color();
		color->SetFractionalRGB(pColor->r, pColor->g, pColor->b);
		SetValue(aMaterial, *color, true, PBR_BASE_COLOR);
	}

	auto param = aRpcMaterial.get(getMapName(MaterialMaps::BASE_COLOR_MAP));
	SetTexture(aMaterial, param, PBR_BASE_COLOR);

	float metallic = 0.0f;
	GetPrimValue<float>(aRpcMaterial.get(getParamName(MaterialParams::METALNESS)), metallic);
	SetValue(aMaterial, metallic, metallic > 0.0f, PBR_METALLIC);
	param = aRpcMaterial.get(getMapName(MaterialMaps::METALNESS_MAP));
	SetTexture(aMaterial, param, PBR_METALLIC);

	bool roughnessInv = false;
	GetPrimValue<bool>(aRpcMaterial.get(getParamName(MaterialParams::ROUGHNESS_INVERSION)), roughnessInv);
	float roughness = 0.2f;
	GetPrimValue<float>(aRpcMaterial.get(getParamName(MaterialParams::ROUGHNESS)), roughness);

	if (roughnessInv)
		roughness = 1.0f - roughness;

	SetValue(aMaterial, roughness, roughness>0.0f, PBR_ROUGHNESS);

	param = aRpcMaterial.get(getMapName(MaterialMaps::ROUGHNESS_MAP));
	SetTexture(aMaterial, param, PBR_ROUGHNESS, roughnessInv);
}

void CRpcRenderMeshBuilder::Specularity(RPCapi::Material & aRpcMaterial, CRhRdkMaterial & aMaterial)
{
	float F0 = 1.0f;
	GetPrimValue<float>(aRpcMaterial.get(getParamName(MaterialParams::REFLECTIVITY)), F0);
	SetValue(aMaterial, F0, F0 > 0.0f, PBR_SPECULAR);

	auto param = aRpcMaterial.get(getMapName(MaterialMaps::REFLECTIVITY_MAP));
	SetTexture(aMaterial, param, PBR_SPECULAR);

	auto pColor = GetColor(aRpcMaterial.get(getParamName(MaterialParams::REFLECTION_COLOR)));

	if (pColor)
	{
		ON_Color *color = new ON_Color();
		color->SetFractionalRGB(pColor->r, pColor->g, pColor->b);
		double tint = color->Value();
		SetValue(aMaterial, tint, tint > 0.0f, PBR_SPECULAR_TINT);
	}

	param = aRpcMaterial.get(getMapName(MaterialMaps::REFL_COLOR_MAP));
	SetTexture(aMaterial, param, PBR_SPECULAR_TINT);
}

void CRpcRenderMeshBuilder::Anisotropy(RPCapi::Material & aRpcMaterial, CRhRdkMaterial & aMaterial)
{
	float amount = 0.0f;
	GetPrimValue<float>(aRpcMaterial.get(getParamName(MaterialParams::ANISOTROPY)), amount);
	SetValue(aMaterial, amount, amount > 0.0f, PBR_ANISOTROPIC);
	auto param = aRpcMaterial.get(getMapName(MaterialMaps::ANISOTROPY_MAP));
	SetTexture(aMaterial, param, PBR_ANISOTROPIC);

	float rotation = 0.0f;
	GetPrimValue<float>(aRpcMaterial.get(getParamName(MaterialParams::ANISOANGLE)), rotation);
	SetValue(aMaterial, rotation, rotation > 0.0f, PBR_ANISOROTATION);
	param = aRpcMaterial.get(getMapName(MaterialMaps::ANISO_ANGLE_MAP));
	SetTexture(aMaterial, param, PBR_ANISOROTATION);
}

void CRpcRenderMeshBuilder::Clearcoat(RPCapi::Material & aRpcMaterial, CRhRdkMaterial & aMaterial)
{
	float amount = 0.0f;
	GetPrimValue<float>(aRpcMaterial.get(getParamName(MaterialParams::COATING)), amount);
	SetValue(aMaterial, amount, amount > 0.0f, PBR_CLEARCOAT);
	auto param = aRpcMaterial.get(getMapName(MaterialMaps::COAT_MAP));
	SetTexture(aMaterial, param, PBR_CLEARCOAT);

	param = aRpcMaterial.get(getMapName(MaterialMaps::COAT_BUMP_MAP));
	SetTexture(aMaterial, param, PBR_CLEARCOAT_BUMP);

	bool roughnessInv = false;
	GetPrimValue<bool>(aRpcMaterial.get(getParamName(MaterialParams::COAT_ROUGHNESS_INV)), roughnessInv);
	float roughness = 0.0f;
	GetPrimValue<float>(aRpcMaterial.get(getParamName(MaterialParams::COAT_ROUGHNESS)), roughness);

	SetValue(aMaterial, roughness, roughness>0.0f, PBR_CLEARCOAT_ROUGH);
	param = aRpcMaterial.get(getMapName(MaterialMaps::COAT_ROUGH_MAP));
	SetTexture(aMaterial, param, PBR_CLEARCOAT_ROUGH, roughnessInv);
}

void CRpcRenderMeshBuilder::Opacity(RPCapi::Material & aRpcMaterial, CRhRdkMaterial & aMaterial)
{
	float amount = 1.0f;
	GetPrimValue<float>(aRpcMaterial.get(getParamName(MaterialParams::TRANSPARENCY)), amount);
	amount = 1.0f - amount;
	SetValue(aMaterial, amount, amount>0.0f, PBR_OPACITY);

	auto param = aRpcMaterial.get(getMapName(MaterialMaps::CUTOUT_MAP));

	if (!param)
		param = aRpcMaterial.get(getMapName(MaterialMaps::TRANSPARENCY_MAP));

	SetTexture(aMaterial, param, PBR_OPACITY);
	float ior = 1.52f;
	GetPrimValue<float>(aRpcMaterial.get(getParamName(MaterialParams::TRANSPARENCY_IOR)), ior);
	SetValue(aMaterial, ior, ior > 0.0f, PBR_OPACITY_IOR);
	param = aRpcMaterial.get(getMapName(MaterialMaps::TRANSPARENCY_IOR_MAP));
	SetTexture(aMaterial, param, PBR_OPACITY_IOR);

	bool transRoughLock = true;
	GetPrimValue<bool>(aRpcMaterial.get(getParamName(MaterialParams::TRANSPARENCY_ROUGHNESS_LOCK)), transRoughLock);
	bool transRoughInv = false;
	float opacityRoughness = 0.0f;

	if (transRoughLock)
	{
		GetPrimValue<float>(aRpcMaterial.get(getParamName(MaterialParams::ROUGHNESS)), opacityRoughness);
		GetPrimValue<bool>(aRpcMaterial.get(getParamName(MaterialParams::ROUGHNESS_INVERSION)), transRoughInv);
		param = aRpcMaterial.get(getMapName(MaterialMaps::ROUGHNESS_MAP));
	}
	else
	{
		GetPrimValue<float>(aRpcMaterial.get(getParamName(MaterialParams::TRANSPARENCY_ROUGHNESS)), opacityRoughness);
		param = aRpcMaterial.get(getMapName(MaterialMaps::TRANSPARENCY_ROUGH_MAP));
		GetPrimValue<bool>(aRpcMaterial.get(getParamName(MaterialParams::TRANSPARENCY_ROUGHNESS_INVERSION)), transRoughInv);
	}

	if (transRoughInv)
		opacityRoughness = 1.0f - opacityRoughness;

	SetValue(aMaterial, opacityRoughness, opacityRoughness > 0.0f, PBR_OPACITY_ROUGH);
	SetTexture(aMaterial, param, PBR_OPACITY_ROUGH, transRoughInv);
}

void CRpcRenderMeshBuilder::Emission(RPCapi::Material & aRpcMaterial, CRhRdkMaterial & aMaterial)
{
	auto pColor = GetColor(aRpcMaterial.get(getParamName(MaterialParams::EMISSION_COLOR)));

	if (pColor)
	{
		ON_Color *color = new ON_Color();
		color->SetFractionalRGB(pColor->r, pColor->g, pColor->b);
		SetValue(aMaterial, *color, true, PBR_EMISSION);
	}

	auto param = aRpcMaterial.get(getMapName(MaterialMaps::EMISSION_COLOR_MAP));
	SetTexture(aMaterial, param, PBR_EMISSION);
}

void CRpcRenderMeshBuilder::BumpDisplacement(RPCapi::Material & aRpcMaterial, CRhRdkMaterial & aMaterial)
{
	auto param = aRpcMaterial.get(getMapName(MaterialMaps::BUMP_MAP));
	SetTexture(aMaterial, param, PBR_BUMP);

	param = aRpcMaterial.get(getMapName(MaterialMaps::DISPLACEMENT_MAP));
	SetTexture(aMaterial, param, PBR_DISPLACEMENT);
}

template <typename T>
bool CRpcRenderMeshBuilder::GetPrimValue(RPCapi::Param* param, T& value)
{
	if (!param)
	{
		return false;
	}

	RPCapi::PrimP<T>* tVal = dynamic_cast<RPCapi::PrimP<T>*>(param);

	if (!tVal)
	{
		return false;
	}

	value = tVal->getValue();

	return true;
}

RPCapi::Color* CRpcRenderMeshBuilder::GetColor(RPCapi::Param* param)
{
	if (!param)
	{
		return nullptr;
	}

	auto color = dynamic_cast<RPCapi::Color*>(param);

	return color;
}

void CRpcRenderMeshBuilder::RpcTexture2RhinoMaterial(const ON_SimpleArray<RPCapi::Texture*>& aTextures,
													 ON_SimpleArray<CRhRdkMaterial*>& aMaterials)
{
	for(int i=0; i<aTextures.Count(); i++)
	{
		RPCapi::Texture* pRpcTexture = aTextures[i];
		CRhRdkBasicMaterial* pRdkMaterial = nullptr;

		if (pRpcTexture->hasChannel(RPCapi::Texture::Channel::RGB))
		{
			pRdkMaterial = CreateNewBasicMaterial();
			if (!pRdkMaterial)
				continue;

			pRdkMaterial->SetInstanceName(L"RpcSpecialMaterial");

			if (!OldTexture2Material(*pRpcTexture, RPCapi::Texture::Channel::RGB, *pRdkMaterial, CRhRdkMaterial::ChildSlotUsage::Diffuse, RDK_BASIC_MAT_BITMAP_TEXTURE))
			{
				pRdkMaterial->Uninitialize();
				delete pRdkMaterial;
				pRdkMaterial = nullptr;
			}
		}

		if (pRpcTexture->hasChannel(RPCapi::Texture::Channel::ALPHA))
		{
			bool bRgbChannel;

			if (!pRdkMaterial)
			{
				pRdkMaterial = CreateNewBasicMaterial();
				if (!pRdkMaterial) 
					continue;

				pRdkMaterial->SetInstanceName(L"RpcSpecialMaterial");
				bRgbChannel = false;
			}
			else
			{
				bRgbChannel = true;
			}

			if (!OldTexture2Material(*pRpcTexture, RPCapi::Texture::Channel::ALPHA, *pRdkMaterial, CRhRdkMaterial::ChildSlotUsage::Transparency, RDK_BASIC_MAT_TRANSPARENCY_TEXTURE))
			{
				if (!bRgbChannel)
				{
					pRdkMaterial->Uninitialize();
					delete pRdkMaterial;
					pRdkMaterial = nullptr;
				}
			}
		}

		aMaterials.Append(pRdkMaterial);
	}
}
