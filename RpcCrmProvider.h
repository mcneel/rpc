
#pragma once

#include "RPCPlugIn.h"

class CRpcCrmProvider : public CRhRdkCustomRenderMeshProvider
{
public:
	
	CRpcCrmProvider(void);
	virtual ~CRpcCrmProvider(void);

public:
	virtual UUID ProviderId(void) const override;
	virtual UUID PlugInId(void) const override;
	virtual ON_wString Name(void) const override;

	virtual bool IsViewDependent(void) const override { return true; }

	virtual bool WillBuildCustomMesh(const ON_Viewport& vp, const CRhinoObject* pObject, const CRhinoDoc& doc, 
	             const UUID& uuidRequestingPlugIn, const CDisplayPipelineAttributes* pAttributes) const override;

	virtual ON_BoundingBox BoundingBox(const ON_Viewport& vp, const CRhinoObject* pObject,  const CRhinoDoc& doc, 
		         const UUID& uuidRequestingPlugIn, const CDisplayPipelineAttributes* pAttributes) const override;

	virtual bool BuildCustomMeshes(const ON_Viewport& vp, const UUID& uuidRequestingPlugIn, const CRhinoDoc& doc, 
	             IRhRdkCustomRenderMeshes& crmInOut, const CDisplayPipelineAttributes* pAttributes, bool bWillBuildCustomMeshCheck) const override;

	virtual bool IsRequestingPlugInDependent(void) const override { return false; }
	virtual bool IsPreviewAndStandardSameMesh(void) const override { return false; }

	virtual CRhRdkVariant GetParameter(const CRhinoObject& object, const wchar_t* wszParamName) const override { return CRhRdkVariant(); }
	virtual void SetParameter(const CRhinoObject& object, const wchar_t* wszParamName, const CRhRdkVariant& value) override { }
};
