
#include "stdafx.h"
#include "RpcCrmProvider.h"
#include "RpcObject.h"
#include "RpcMains.h"
#include "RpcInstance.h"
#include "RpcUtilities.h"
#include "RpcRenderMeshBuilder.h"
#include "RpcRdkPlugIn.h"

CRpcCrmProvider::CRpcCrmProvider(void)
{
}

CRpcCrmProvider::~CRpcCrmProvider(void)
{
}

UUID CRpcCrmProvider::ProviderId(void) const
{
	// {70D82ED4-1CFC-4d06-8E20-01C76D262923}
	static const GUID id = { 0x70d82ed4, 0x1cfc, 0x4d06, { 0x8e, 0x20, 0x1, 0xc7, 0x6d, 0x26, 0x29, 0x23 } };
	return id;
}

UUID CRpcCrmProvider::PlugInId(void) const
{
	return Mains().RdkPlugIn().PlugInId();
}

ON_wString CRpcCrmProvider::Name(void) const
{
	return L"RPC";
}

bool CRpcCrmProvider::WillBuildCustomMesh(const ON_Viewport& vp, const CRhinoObject* pObject, const CRhinoDoc& /*doc*/, 
                                          const UUID& uuidRequestingPlugIn, const CDisplayPipelineAttributes* pAttributes) const
{
	if (nullptr != pAttributes)	
		return false;

	CRpcObject ro(pObject);
	return ro.IsTagged();
}

ON_BoundingBox CRpcCrmProvider::BoundingBox(const ON_Viewport& vp, const CRhinoObject* pObject, const CRhinoDoc& doc, const UUID& uuidRequestingPlugIn, const CDisplayPipelineAttributes* pAttributes) const
{
	return ::RMPBoundingBoxImpl(*this, vp, pObject, doc, uuidRequestingPlugIn, pAttributes);
}

bool CRpcCrmProvider::BuildCustomMeshes(const ON_Viewport& vp, const UUID& uuidRequestingPlugIn, const CRhinoDoc& doc, IRhRdkCustomRenderMeshes& crmInOut, const CDisplayPipelineAttributes* pAttributes, bool bWillBuildCustomMeshCheck) const
{
	if(bWillBuildCustomMeshCheck && !WillBuildCustomMesh(vp, crmInOut.Object(), doc, uuidRequestingPlugIn, pAttributes))
		return false;

	const CRhinoObject* pObject = crmInOut.Object();
	if (NULL == pObject) return false;

	const CRhinoInstanceObject* pBlock = CRhinoInstanceObject::Cast(pObject);
	if (NULL == pBlock) return false;

	ON_Xform xformInstance = pBlock->InstanceXform();
	xformInstance.Invert();

	ON_3dPoint ptCamera = vp.CameraLocation();

	ptCamera.Transform(xformInstance);

	crmInOut.SetAutoDeleteMeshesOn();
	crmInOut.SetAutoDeleteMaterialsOn();
			
	CRpcInstance rpc(doc, *pBlock);
	if (rpc.IsValid())
	{
		const RPCapi::Instance* pRpcInstance = rpc.Instance();
		if (NULL != pRpcInstance)
		{
			// DebugSaveTexturesToRoot(*pRpcInstance, ptCamera);

			ON_SimpleArray<ON_Mesh*> aMeshes;
			ON_SimpleArray<CRhRdkBasicMaterial*> aMaterials;

			CRpcRenderMeshBuilder mb(doc, *pRpcInstance);
			if (mb.Build(ptCamera, aMeshes, aMaterials))
			{
				for (int i=0; i<aMeshes.Count(); i++)
				{
					ON_Mesh* pRhinoMesh = aMeshes[i];
					pRhinoMesh->Transform(pBlock->InstanceXform());

					CRhRdkBasicMaterial* pRdkMaterial = NULL;

					pRdkMaterial = aMaterials[i];

					crmInOut.Add(pRhinoMesh, pRdkMaterial);
				}
			}
		}
	}

	return true;
}
