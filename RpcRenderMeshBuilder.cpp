
#include "StdAfx.h"
#include "RpcRenderMeshBuilder.h"
#include "RpcUtilities.h"
#include "MaterialParams.h"

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
								  ON_SimpleArray<CRhRdkBasicMaterial*>& aMaterials)
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

void CRpcRenderMeshBuilder::RpcMaterial2RhinoMaterial(const ON_SimpleArray<RPCapi::Material*>& aRpcMaterials, ON_SimpleArray<CRhRdkBasicMaterial*>& aMaterials)
{
	for (int i = 0; i < aRpcMaterials.Count(); i++)
	{
		aMaterials.Append(CreateNewBasicMaterial());

		// Skipping of empty material.
		if (aRpcMaterials[i]->count() <= 1)
		{
			continue;
		}
		RPCapi::Material* mat = aRpcMaterials[i];

		SetColor(*mat, *aMaterials[i]);
	   
		
	}
}

bool CRpcRenderMeshBuilder::BuildNew(ON_SimpleArray<ON_Mesh*>& aMeshes,
	ON_SimpleArray<CRhRdkBasicMaterial*>& aMaterials)
{
	RPCapi::Mesh *pRpcMesh;
	RPCapi::TextureMesh *pTextureMesh;
	RPCapi::Material **pRpcMaterials = NULL;
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

	if (NULL != pRpcMaterials)
	{
		for (int i = 0; i < numMaterials; i++)
		{
			if (NULL != pRpcMaterials[i])
			{
				delete pRpcMaterials[i];
				pRpcMaterials[i] = NULL;
			}
		}
		delete[] pRpcMaterials;
	}

	delete pTextureMesh;
	delete pRpcMesh;

	return true;
}

bool CRpcRenderMeshBuilder::Build(const ON_3dPoint& ptCamera, ON_SimpleArray<ON_Mesh*>& aMeshes,
								  ON_SimpleArray<CRhRdkBasicMaterial*>& aMaterials)
{
	const RPCapi::Mesh* pRpcMesh = m_Rpc.getMesh(NULL, ptCamera.x, ptCamera.y, ptCamera.z);
	if (NULL == pRpcMesh) return false;

	const double dUnitsScale = ON::UnitScale(ON::LengthUnitSystem::Inches, m_doc.ModelUnits());
	ON_Xform xformUnitsScale = ON_Xform::DiagonalTransformation(dUnitsScale, dUnitsScale, dUnitsScale);

	RPCapi::Texture** Texture = NULL;
	RPCapi::TextureMesh* pTextureMesh = NULL;
	int iTextures = 0;

	if (!m_Rpc.getTextures(ptCamera.x, ptCamera.y, ptCamera.z, iTextures, Texture, pTextureMesh))
	{
		ON_Mesh* pRhinoMesh = new ON_Mesh;
		RpcMesh2RhinoMesh(*pRpcMesh, *pRhinoMesh);

		pRhinoMesh->Transform(xformUnitsScale);

		aMeshes.Append(pRhinoMesh);
		aMaterials.Append(NULL);

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

	if (NULL != Texture)
	{
		for (int i=0; i<iTextures; i++)
		{
			if (NULL != Texture[i])
			{
				delete Texture[i];
				Texture[i] = NULL;
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

			if (!IsDegenerateTriangle(*pRhinoMesh, face))
			{
				pRhinoMesh->m_F.Append(face);
				ASSERT(pRhinoMesh->IsValid());
			}
			else
			{
				iBadFaces++;
				pRhinoMesh->m_V.SetCount(iVertexCount);
				pRhinoMesh->m_T.SetCount(iVertexCount);
			}				
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

template <typename T>
bool CRpcRenderMeshBuilder::Rgb2Material(T& RpcTexture, CRhRdkBasicMaterial& Material, const wchar_t* textureType)
{
	int iWidth = 0;
	int iHeight = 0;
	int iBytes;
	unsigned char* pRGB = NULL;

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
			if (NULL != rgb)
			{
				rdRGB.SetPixel(x, y, rgb[0], rgb[1], rgb[2], 255);
				rgb += 3;
			}
		}
	}

	// TODO: [HERE] Possible dib ownership problem - check RDK SDK comments.
	CRhRdkTexture* pRdkTexture = RhRdkNewDibTexture(&rdRGB, Material.DocumentAssoc(), false, false);
	if (CTestRpcGamma::m_dGamma != 1.0)
	{
		pRdkTexture->SetAdjustmentGamma(CTestRpcGamma::m_dGamma);
	}

	CRhRdkBasicMaterial::CTextureSlot slot = Material.TextureSlot(CRhRdkMaterial::ChildSlotUsage::Diffuse);
	slot.m_bOn = true;
	slot.m_dAmount = 1.0;
	slot.m_bFilterOn = true;
	Material.SetTextureSlot(CRhRdkMaterial::ChildSlotUsage::Diffuse, slot);
	
	VERIFY(Material.SetChild(pRdkTexture, textureType));

	delete[] pRGB;

	return true;
}

bool CRpcRenderMeshBuilder::Alpha2Material(RPCapi::Texture& RpcTexture, CRhRdkBasicMaterial& Material)
{
	int iAlphaWidth = 0;
	int iAlphaHeight = 0;
	int iAlphaBytes;
	unsigned char* pAlpha = NULL;

	iAlphaBytes = RpcTexture.data(pAlpha, false, RPCapi::Texture::Channel::ALPHA,RPCapi::Texture::Scale::SLOW, iAlphaWidth, iAlphaHeight);
	
	if (iAlphaBytes > 0)
	{
		pAlpha = new BYTE[iAlphaBytes];
		VERIFY(iAlphaBytes == RpcTexture.data(pAlpha, false, RPCapi::Texture::Channel::ALPHA,RPCapi::Texture::Scale::SLOW, iAlphaWidth, iAlphaHeight));
	}
	else 
	{
		return false;
	}

	BYTE* alp = pAlpha;

	CRhinoDib rdAlpha(iAlphaWidth, iAlphaHeight, 32);

	for(int y=0; y<iAlphaHeight; y++)
	{
		for(int x=0; x<iAlphaWidth; x++)
		{
			if (NULL != alp)
			{
				rdAlpha.SetPixel(x, y, alp[0], alp[0], alp[0], 255);
				alp += 1;
			}
		}
	}

	// TODO: [HERE] Possible dib ownership problem - check RDK SDK comments.
	CRhRdkTexture* pRdkTextureAlpha = RhRdkNewDibTexture(&rdAlpha, Material.DocumentAssoc(), false, false);

	CRhRdkBasicMaterial::CTextureSlot slotAlpha = Material.TextureSlot(CRhRdkMaterial::ChildSlotUsage::Transparency);
	slotAlpha.m_bOn = true;
	slotAlpha.m_dAmount = 1.0;
	slotAlpha.m_bFilterOn = true;
	Material.SetTextureSlot(CRhRdkMaterial::ChildSlotUsage::Transparency, slotAlpha);

	VERIFY(Material.SetChild(pRdkTextureAlpha, RDK_BASIC_MAT_TRANSPARENCY_TEXTURE));

	delete[] pAlpha;

	return true;
}

void CRpcRenderMeshBuilder::SetColor(RPCapi::Material & aRpcMaterial, CRhRdkBasicMaterial & aMaterial)
{
	float baseWeight = GetPrimValue<float>(aRpcMaterial.get(PARAM_NAMES.at(MaterialParams::BASE_WEIGHT)));
	RPCapi::Color* pColor = GetColor(aRpcMaterial.get(PARAM_NAMES.at(MaterialParams::BASE_COLOR)));	
	ON_Color *color = new ON_Color();
	color->SetFractionalRGB(pColor->r *baseWeight, pColor->g*baseWeight, pColor->b*baseWeight);
	aMaterial.SetDiffuse(*color);


	RPCapi::Param* param = aRpcMaterial.get(MAP_NAMES.at(MaterialMaps::BASE_COLOR_MAP));

	if ((nullptr != param) && (param->typeCode() == RPCapi::ObjectCodes::TYPE_TEXMAP))
	{
		RPCapi::TextureMap* pMap = dynamic_cast<RPCapi::TextureMap*>(param);
		if (pMap == nullptr) {
			throw std::exception("Type conversion exception.");
		}
		const RPCapi::TStringArg MAP("map_name");
		RPCapi::Param *param = pMap->get(MAP);

		if (param == NULL)
		{
			throw std::bad_cast();
		}

		RPCapi::Image *image = dynamic_cast<RPCapi::Image*>(param);

		Rgb2Material(*image, aMaterial, RDK_BASIC_MAT_BITMAP_TEXTURE);
	}
}

template <typename T>
T CRpcRenderMeshBuilder::GetPrimValue(RPCapi::Param * param)
{
	if (param == nullptr)
	{
		throw std::exception("The parameter is null.");
	}

	RPCapi::PrimP<T>* tVal = dynamic_cast<RPCapi::PrimP<T>*>(param);

	if (tVal == nullptr)
	{
		throw std::exception("Type conversion exception.");
	}

	return tVal->getValue();
}

RPCapi::Color * CRpcRenderMeshBuilder::GetColor(RPCapi::Param * param)
{
	if (param == nullptr)
	{
		throw std::exception("The parameter is null.");
	}

	RPCapi::Color* color = dynamic_cast<RPCapi::Color*>(param);

	if (color == nullptr)
	{
		throw std::exception("Type conversion exception.");
	}

	return color;
}

void CRpcRenderMeshBuilder::RpcTexture2RhinoMaterial(const ON_SimpleArray<RPCapi::Texture*>& aTextures,
													 ON_SimpleArray<CRhRdkBasicMaterial*>& aMaterials)
{
	for(int i=0; i<aTextures.Count(); i++)
	{
		RPCapi::Texture* pRpcTexture = aTextures[i];
		CRhRdkBasicMaterial* pRdkMaterial = NULL;

		if (pRpcTexture->hasChannel(RPCapi::Texture::Channel::RGB))
		{
			pRdkMaterial = CreateNewBasicMaterial();
			if (NULL == pRdkMaterial) continue;

			pRdkMaterial->SetInstanceName(L"RpcSpecialMaterial");

			if (!Rgb2Material(*pRpcTexture, *pRdkMaterial, RDK_BASIC_MAT_BITMAP_TEXTURE))
			{
				pRdkMaterial->Uninitialize();
				delete pRdkMaterial;
				pRdkMaterial = NULL;
			}
		}

		if (pRpcTexture->hasChannel(RPCapi::Texture::Channel::ALPHA))
		{
			bool bRgbChannel;

			if (NULL == pRdkMaterial)
			{
				pRdkMaterial = CreateNewBasicMaterial();
				if (NULL == pRdkMaterial) continue;

				pRdkMaterial->SetInstanceName(L"RpcSpecialMaterial");
				bRgbChannel = false;
			}
			else
			{
				bRgbChannel = true;
			}

			if (!Alpha2Material(*pRpcTexture, *pRdkMaterial))
			{
				if (!bRgbChannel)
				{
					pRdkMaterial->Uninitialize();
					delete pRdkMaterial;
					pRdkMaterial = NULL;
				}
			}
		}

		aMaterials.Append(pRdkMaterial);
	}
}
