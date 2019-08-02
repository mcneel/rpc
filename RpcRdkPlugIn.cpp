#include "StdAfx.h"
#include "RpcRdkPlugIn.h"
#include "RpcCrmProvider.h"
#include "RpcAnimationFrameRrp.h"


CRpcRdkPlugIn::CRpcRdkPlugIn(void)
{
}

CRpcRdkPlugIn::~CRpcRdkPlugIn(void)
{
}

void CRpcRdkPlugIn::RegisterExtensions(void) const
{
	AddExtension(new CRpcCrmProvider);
}

UUID CRpcRdkPlugIn::PlugInId(void) const
{
	return PlugIn().PlugInID();
}

CRhinoPlugIn& CRpcRdkPlugIn::RhinoPlugIn(void) const
{
	return PlugIn();
}

bool CRpcRdkPlugIn::SupportsFeature(const UUID&) const
{
	return false;
}

/*void CRpcRdkPlugIn::PlugInRdkVersion(int& iMajorVersion, int& iMinorVersion, int& iBetaRelease) const
{
	iMajorVersion = RDK_MAJOR_VERSION;
	iMinorVersion = RDK_MINOR_VERSION;
	iBetaRelease  = RDK_BETA_RELEASE;
}*/

void CRpcRdkPlugIn::OnAnimationFrameChanged(CRhinoDoc& doc, int iOldFrame, int iNewFrame)
{
	static bool bWorking = false;

	if (bWorking)
		return;

	IRhRdkRegisteredProperty* pProp = RhRdkRegisteredPropertiesManager().FindProperty(idAF);
	ASSERT(NULL != pProp);
	if (NULL == pProp) return;

	bWorking = true;

	CRpcAnimationFrameRrp* pRRP = static_cast<CRpcAnimationFrameRrp*>(pProp);
	pRRP->CallPropertyChangedEvent(doc, iOldFrame);

	bWorking = false;
}
