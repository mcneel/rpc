#include "StdAfx.h"
#include "RpcUtilities.h"
#include "RpcInstance.h"


int RpcMessageBox(const CLBPString& sMessage, UINT uType)
{
	CRhRdkModalizer m;

	uType |= MB_SETFOREGROUND;

	return ::MessageBox(RhinoApp().MainWnd(), sMessage, L"RPC PlugIn", uType);
}

void RpcMesh2RhinoMesh(const RPCapi::Mesh& RpcMesh, ON_Mesh& RhinoMesh)
{
	const int iVertexCount = RpcMesh.getNumVerts();

	const RPCapi::Mesh::Vertex* pVerts = RpcMesh.getConstVertices();
	ASSERT(pVerts);

	RhinoMesh.m_V.SetCapacity(iVertexCount);
	for (int i=0;i<iVertexCount;i++)
	{
		const RPCapi::Mesh::Vertex* pVert = pVerts+i;
		const ON_3fPoint v(float(pVert->x), float(pVert->y), float(pVert->z));
		RhinoMesh.m_V.Append(v);
	}

	const int iFaceCount = RpcMesh.getNumFaces();
	const RPCapi::Mesh::Face* pFaces = RpcMesh.getConstFaces();

	RhinoMesh.m_F.SetCapacity(iFaceCount);
	for (int i=0;i<iFaceCount;i++)
	{
		const RPCapi::Mesh::Face* pFace = pFaces+i;
		
		ON_MeshFace face;
		face.vi[0] = pFace->v0;
		face.vi[1] = pFace->v1;
		face.vi[2] = pFace->v2;
		face.vi[3] = pFace->v2;

		RhinoMesh.m_F.Append(face);
	}

	RhinoMesh.ComputeVertexNormals();
}

bool IsRpcNameInUse(const ON_ClassArray<CLBPString>& aNames, const CLBPString& sRpcName)
{
	for(int i=0; i<aNames.Count(); i++)
	{
		if (0 == sRpcName.CompareNoCase(aNames[i].Wide()))
			return true;
	}

	return false;
}

CLBPString UnusedRpcName(const CLBPString& sRpcName)
{
	ON_ClassArray<CLBPString> aRpcNames;

	CRhinoObjectIterator oi;
	oi.SetObjectFilter(ON::instance_reference);
	for(const CRhinoObject* pObject = oi.First();
        NULL != pObject;
		pObject = oi.Next())
	{
		CLBPString& s = aRpcNames.AppendNew();
		s = pObject->Attributes().m_name;
	}

	int iIndex = 0;

	while(true)
	{
		CLBPString s;
		s.Format(L"%S_%d",sRpcName.Wide(), iIndex);

		if (!IsRpcNameInUse(aRpcNames, s))
			return s;

		iIndex++;
	}
}

CLBPString UnusedInstanceDefinitionName(CRhinoDoc& doc)
{
	CRhinoInstanceDefinitionTable& def_table = doc.m_instance_definition_table;
  ON_wString unused_idef_name;
  def_table.GetUnusedInstanceDefinitionName(L"*_RPC_",unused_idef_name);
	const CLBPString sDef = unused_idef_name;
  return sDef;
}

void DebugSaveTexturesToRoot(const RPCapi::Instance& rpc, const ON_3dPoint& ptCamera)
{
	RPCapi::Texture** Texture;
	RPCapi::TextureMesh* TextureMesh;
	int iTextures;
	if (!rpc.getTextures(ptCamera.x, ptCamera.y, ptCamera.z, iTextures, Texture, TextureMesh))
		return;

	for(int i = 0; i<iTextures; i++)
	{
		bool bRgb = Texture[i]->hasChannel(RPCapi::Texture::Channel::RGB);
		bool bAlpha = Texture[i]->hasChannel(RPCapi::Texture::Channel::ALPHA);

		int iWidth = 0;
		int iHeight = 0;
		int iBytes;
		unsigned char* pRGB = NULL;

		if (bRgb)
		{
			iBytes = Texture[i]->data(pRGB, false, RPCapi::Texture::Channel::RGB, RPCapi::Texture::Scale::SLOW, iWidth, iHeight);

			if (iBytes > 0)
			{
				pRGB = new BYTE[iBytes];

				VERIFY(iBytes = Texture[i]->data(pRGB, false, RPCapi::Texture::Channel::RGB, RPCapi::Texture::Scale::SLOW, iWidth, iHeight));
			}
			else
			{
				bRgb = false;
			}
		}

		int iAlphaWidth = 0;
		int iAlphaHeight = 0;
		int iAlphaBytes;
		unsigned char* pAlpha = NULL;

		if (bAlpha)
		{
			iAlphaBytes = Texture[i]->data(pAlpha, false, RPCapi::Texture::Channel::ALPHA,RPCapi::Texture::Scale::SLOW, iAlphaWidth, iAlphaHeight);
			
			if (iAlphaBytes > 0)
			{
				pAlpha = new BYTE[iAlphaBytes];
				VERIFY(iAlphaBytes == Texture[i]->data(pAlpha, false, RPCapi::Texture::Channel::ALPHA,RPCapi::Texture::Scale::SLOW, iAlphaWidth, iAlphaHeight));
			}
			else 
			{
				bAlpha = false;
			}
		}

		BYTE* rgb = pRGB;
		BYTE* alp = pAlpha;

		CRhinoDib rdRGB(iWidth, iHeight, 32);
		CRhinoDib rdAlpha(iWidth, iHeight, 32);

		for(int y=0; y<iHeight; y++)
		{
			for(int x=0; x<iWidth; x++)
			{
				if (bAlpha && NULL != alp)
				{
					rdAlpha.SetPixel(x, y, alp[0], alp[0], alp[0], 255);
					alp += 1;
				}

				if (bRgb && NULL != rgb)
				{
					rdRGB.SetPixel(x, y, rgb[0], rgb[1], rgb[2], 255);
					rgb += 3;
				}
			}
		}

		CLBPString sRGB;
		sRGB.Format(L"C:\\Text_%d.png",i);
		rdRGB.WriteToFile(sRGB);

		if (bAlpha)
		{
			CLBPString sAlpha;
			sAlpha.Format(L"C:\\Text_Alpha_%d.png",i);
			rdRGB.WriteToFile(sAlpha);
		}

		delete[] pRGB;
		delete[] pAlpha;
	}
}



bool UniteVertexNormals(ON_Mesh & mesh, ON_SimpleArray<int> & aSourceVertexIndexList, int iSourceVertexCount)
{
	ASSERT(mesh.QuadCount() == 0);
	ASSERT(mesh.HasVertexNormals());

	ON_3fVectorArray aSourceVertexNormals(iSourceVertexCount);
	aSourceVertexNormals.SetCount(iSourceVertexCount);
	aSourceVertexNormals.MemSet(0);

	for (int f = 0; f < mesh.FaceCount(); f++)
	{
		const ON_MeshFace & face = mesh.m_F[f];

		const int iAlpha = face.vi[0];
		const int iBeta = face.vi[1];
		const int iGamma = face.vi[2];

		const ON_3fVector vtA = mesh.m_V[iGamma] - mesh.m_V[iBeta];
		const double aSq = vtA.LengthSquared();
		const ON_3fVector vtB = mesh.m_V[iGamma] - mesh.m_V[iAlpha];
		const double bSq = vtB.LengthSquared();
		const ON_3fVector vtC = mesh.m_V[iBeta] - mesh.m_V[iAlpha];
		const double cSq = vtC.LengthSquared();
		const double a = sqrt(aSq);
		const double b = sqrt(bSq);
		const double c = sqrt(cSq);

		const double d2bc = 2.0 * b * c;
		const double d2ac = 2.0 * a * c;
		const double d2ab =	2.0 * a * b;

		if (d2bc <= 0.0 || d2ac <= 0.0 || d2ab <= 0.0)
			continue;

		const double cosAlpha = (bSq + cSq - aSq) / d2bc;
		const double cosBeta = (aSq + cSq - bSq) / d2ac;
		const double cosGamma = (aSq + bSq - cSq) / d2ab;
		const float alpha = (float)acos(min(1.0, max(-1.0, cosAlpha)));
		const float beta = (float)acos(min(1.0, max(-1.0, cosBeta)));
		const float gamma = (float)acos(min(1.0, max(-1.0, cosGamma)));

		ON_3fVector vtFaceNormal = ON_CrossProduct(vtC, vtB);
		if (false == vtFaceNormal.Unitize())
			return false;

		aSourceVertexNormals[aSourceVertexIndexList[iAlpha]] += alpha * vtFaceNormal;
		aSourceVertexNormals[aSourceVertexIndexList[iBeta]] += beta * vtFaceNormal;
		aSourceVertexNormals[aSourceVertexIndexList[iGamma]] += gamma * vtFaceNormal;
	}

	for (int n = 0; n < aSourceVertexNormals.Count(); n++)
	{
		if (false == aSourceVertexNormals[n].Unitize())
			aSourceVertexNormals[n].Set(0.0f, 0.0f, 0.0f);
	}

	for (int v = 0; v < mesh.VertexCount(); v++)
	{
		const ON_3fVector & vtNormal = aSourceVertexNormals[aSourceVertexIndexList[v]];

		if (!vtNormal.IsZero())
			mesh.m_N[v] = aSourceVertexNormals[aSourceVertexIndexList[v]];
	}

	return true;
}

CRhRdkBasicMaterial* CreateNewBasicMaterial(void)
{
	const CRhRdkContentFactory* pFactory = RhRdkContentFactoriesEx().FindFactory(uuidBasicMaterialType);

	if (!pFactory)
		return nullptr;

	CRhRdkContent* pContent = pFactory->NewContent();
	if (!pContent)
		return nullptr;

	if (!pContent->Initialize())
		return nullptr;

	CRhRdkBasicMaterial* pMaterial = dynamic_cast<CRhRdkBasicMaterial*>(pContent);

	if (!pMaterial)
	{
		pContent->Uninitialize();
		delete pContent;
		return nullptr;
	}

	return pMaterial;
}

CRhRdkMaterial* CreatePBMaterial(void)
{
	//5A8D7B9B - CDC9 - 49DE - 8C16 - 2EF64FB097AB
	UUID uuidPBMaterialType = { 0x5a8d7b9b, 0xcdc9, 0x49de, { 0x8c, 0x16, 0x2e, 0xcf6, 0x4f, 0xb0, 0x97, 0xab } };
	CRhRdkContent* pContent = ::RhRdkContentFactoriesEx().NewContentFromTypeEx(uuidPBMaterialType, RhinoApp().ActiveDoc());

	if (!pContent)
		return nullptr;

	CRhRdkMaterial* pMaterial = dynamic_cast<CRhRdkMaterial*>(pContent);

	if (!pMaterial)
	{
		delete pContent;
		return nullptr;
	}

	return pMaterial;
}

const wchar_t* _RhLocalizeString( const wchar_t* wsz, int nContext)
{
	return wsz;
}
