
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

void CRpcCrmProvider::CustomRender(const CRhinoInstanceObject& pBlock, ON_SimpleArray<ON_Mesh*>& aMeshes, ON_SimpleArray<CRhRdkBasicMaterial*>& aMaterials) const
{
	const int iInstanceDefintionId = pBlock.InstanceDefinition()->Index();
	CRhinoMaterialTable& matTable = RhinoApp().ActiveDoc()->m_material_table;
	CRhinoInstanceDefinitionTable& defTable = RhinoApp().ActiveDoc()->m_instance_definition_table;
	ObjectArray objects;

	for (int i = 0; i < aMeshes.Count(); i++)
	{
		ON_Mesh* pRhinoMesh = aMeshes[i];
		CRhRdkBasicMaterial* pRdkMaterial = aMaterials[i];

		CRhinoMeshObject* rhinoMesh = new CRhinoMeshObject;
		rhinoMesh->SetMesh(pRhinoMesh);

		ON_Material material = pRdkMaterial->SimulatedMaterial();
		int matIndex = matTable.AddMaterial(material);

		ON_3dmObjectAttributes attr;
		attr.SetMaterialSource(ON::material_from_object);
		attr.m_material_index = matIndex;
		rhinoMesh->ModifyAttributes(attr);

		objects.Append((CRhinoObject*)rhinoMesh);
	}

	defTable.ModifyInstanceDefinitionGeometry(iInstanceDefintionId, objects, true);
}

void CRpcCrmProvider::RhinoRender(const CRhinoInstanceObject& pBlock,ON_SimpleArray<ON_Mesh*>& aMeshes,
	ON_SimpleArray<CRhRdkBasicMaterial*>& aMaterials, IRhRdkCustomRenderMeshes& crmInOut) const
{
	for (int i = 0; i < aMeshes.Count(); i++)
	{
		ON_Mesh* pRhinoMesh = aMeshes[i];
		pRhinoMesh->Transform(pBlock.InstanceXform());

		crmInOut.Add(pRhinoMesh, aMaterials[i]);
	}
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
	if (pAttributes)
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

			//Rhino Render ID = {5DC0192D-73DC-44F5-9141-8E72542E792D}
			constexpr UUID RhinoRenderID = { 0x5dc0192d ,0x73dc, 0x44f5, { 0x91, 0x41, 0x8e, 0x72, 0x54, 0x2e, 0x79, 0x2d} };

			ON_SimpleArray<ON_Mesh*> aMeshes;
			ON_SimpleArray<CRhRdkBasicMaterial*> aMaterials;

			CRpcRenderMeshBuilder mb(doc, *pRpcInstance);
			
			if (pRpcInstance->hasMaterials())
				mb.BuildNew(aMeshes, aMaterials);
			else
				mb.BuildOld(ptCamera, aMeshes, aMaterials);

			CRhRdkRenderPlugIn* plug = FindCurrentRenderPlugIn();
			CRhinoPlugIn& rend = plug->RhinoPlugIn();

			if (rend.PlugInID()==RhinoRenderID)
			{
				RhinoRender(*pBlock, aMeshes, aMaterials, crmInOut);
			}
			else
			{
				CustomRender(*pBlock, aMeshes, aMaterials);
			}
		}
	}

	RhinoApp().ActiveView()->EnableDrawing(false);

	return true;
}