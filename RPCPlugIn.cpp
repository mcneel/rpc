// RPCPlugIn.cpp : defines the initialization routines for the plug-in.
//

#include "StdAfx.h"
#if defined INSITU
#include "../../../SDK/inc/rhinoSdkPlugInDeclare.h"
#else
#include "rhinoSdkPlugInDeclare.h"
#endif
#include "RPCPlugIn.h"
#include "Resource.h"
#include "RpcMains.h"
#include "RpcPropertiesDlg.h"
#include "RpcSelectionDialog.h"
#include "RpcEventWatcher.h"
#include "RpcDocument.h"
#include "LBPFileMgr2.h"

// The plug-in object must be constructed before any plug-in classes derived
// from CRhinoCommand. The #pragma init_seg(lib) ensures that this happens.
#pragma warning(push)
#pragma warning(disable : 4073)
#pragma init_seg(lib)
#pragma warning(pop)

// Rhino plug-in declaration
RHINO_PLUG_IN_DECLARE

// Rhino plug-in name
// Provide a short, friendly name for this plug-in.
RHINO_PLUG_IN_NAME(L"RPC");

// Rhino plug-in id
// Provide a unique uuid for this plug-in.
// {1F908FF5-4984-45A6-95F0-A81CE979A4D7}
RHINO_PLUG_IN_ID(L"1F908FF5-4984-45A6-95F0-A81CE979A4D7");

// Rhino plug-in version
// Provide a version number string for this plug-in.
RHINO_PLUG_IN_VERSION(__DATE__ "  " __TIME__)

// Rhino plug-in description
// Provide a description of this plug-in.
RHINO_PLUG_IN_DESCRIPTION(L"RPC plug-in for Rhinoceros®");

// Rhino plug-in icon resource id
// Provide an icon resource this plug-in.
// Icon resource should contain 16, 24, 32, 48, and 256-pixel image sizes.
//RHINO_PLUG_IN_ICON_RESOURCE_ID(IDI_ICON);

// Rhino plug-in developer declarations
// TODO: fill in the following developer declarations with
// your company information. Note, all of these declarations
// must be present or your plug-in will not load.
//
// When completed, delete the following #error directive.
RHINO_PLUG_IN_DEVELOPER_ORGANIZATION(L"Robert McNeel & Associates");
RHINO_PLUG_IN_DEVELOPER_ADDRESS     (L"3670 Woodland Park Avenue North\r\nSeattle WA 98103");
RHINO_PLUG_IN_DEVELOPER_COUNTRY     (L"United States");
RHINO_PLUG_IN_DEVELOPER_PHONE       (L"206-545-7000");
RHINO_PLUG_IN_DEVELOPER_FAX         (L"206-545-7321");
RHINO_PLUG_IN_DEVELOPER_EMAIL       (L"tech@mcneel.com");
RHINO_PLUG_IN_DEVELOPER_WEBSITE     (L"http://www.mcneel.com");
RHINO_PLUG_IN_UPDATE_URL            (L"http://www.mcneel.com");

// The one and only CRPCPlugIn object
static class CRPCPlugIn thePlugIn;

/////////////////////////////////////////////////////////////////////////////
// CRPCPlugIn definition

CRPCPlugIn& PlugIn()
{
	// Return a reference to the one and only CRPCPlugIn object
	return thePlugIn;
}

CRPCPlugIn::CRPCPlugIn()
{
	// Description:
	//   CRPCPlugIn constructor. The constructor is called when the
	//   plug-in is loaded and "thePlugIn" is constructed. Once the plug-in
	//   is loaded, CRPCPlugIn::OnLoadPlugIn() is called. The
	//   constructor should be simple and solid. Do anything that might fail in
	//   CRPCPlugIn::OnLoadPlugIn().

	// TODO: Add construction code here
	m_plugin_version = RhinoPlugInVersion();
	m_pMains = nullptr;
}

/////////////////////////////////////////////////////////////////////////////
// Required overrides

const wchar_t* CRPCPlugIn::PlugInName() const
{
	// Description:
	//   Plug-in name display string.  This name is displayed by Rhino when
	//   loading the plug-in, in the plug-in help menu, and in the Rhino
	//   interface for managing plug-ins.

	// TODO: Return a short, friendly name for the plug-in.
	return RhinoPlugInName();
}

const wchar_t* CRPCPlugIn::PlugInVersion() const
{
	// Description:
	//   Plug-in version display string. This name is displayed by Rhino
	//   when loading the plug-in and in the Rhino interface for managing
	//   plug-ins.

	// TODO: Return the version number of the plug-in.
	return m_plugin_version;
}

GUID CRPCPlugIn::PlugInID() const
{
	// Description:
	//   Plug-in unique identifier. The identifier is used by Rhino to
	//   manage the plug-ins.

	// TODO: Return a unique identifier for the plug-in.
	// {7F806339-3300-4992-B349-AFC72474696C}
	return ON_UuidFromString(RhinoPlugInId());
}

/////////////////////////////////////////////////////////////////////////////
// Additional overrides

BOOL CRPCPlugIn::OnLoadPlugIn()
{
	// Description:
	//   Called after the plug-in is loaded and the constructor has been
	//   run. This is a good place to perform any significant initialization,
	//   license checking, and so on.  This function must return TRUE for
	//   the plug-in to continue to load.

	// Remarks:
	//    Plug-ins are not loaded until after Rhino is started and a default document
	//    is created.  Because the default document already exists
	//    CRhinoEventWatcher::On????Document() functions are not called for the default
	//    document.  If you need to do any document initialization/synchronization then
	//    override this function and do it here.  It is not necessary to call
	//    CPlugIn::OnLoadPlugIn() from your derived class.

	// TODO: Add plug-in initialization code here.

	RefreshToolbar();

	m_pMains = new CRpcMains(*this);
	if (!m_pMains->Initialize())
	{
		delete m_pMains;
		m_pMains = nullptr;

		return FALSE;
	}
	CRpcSelectionDialog::CRhinoTabbedDockBarDialog::Register(
		RUNTIME_CLASS(CRpcSelectionDialog),
		CRpcSelectionDialog::IDD,
		IDI_PROP_RPC,
		AfxGetStaticModuleState()
	);

	CRpcSelectionDialog::OpenPanelInDockBarWithOtherPanel(*RhinoApp().ActiveDoc(), CRpcSelectionDialog::Id(), uuidPanelObjectProps, false);

	return TRUE;
}

void CRPCPlugIn::OnUnloadPlugIn()
{
	// Description:
	//    Called one time when plug-in is about to be unloaded. By this time,
	//    Rhino's mainframe window has been destroyed, and some of the SDK
	//    managers have been deleted. There is also no active document or active
	//    view at this time. Thus, you should only be manipulating your own objects.
	//    or tools here.

	// TODO: Add plug-in cleanup code here.
	if (nullptr != m_pMains)
	{
		m_pMains->Uninitialize();

		delete m_pMains;
		m_pMains = nullptr;
	}
}

CRpcMains& CRPCPlugIn::Mains(void)
{
	return *m_pMains;
}

void CRPCPlugIn::RefreshToolbar()
{
	ON_wString path;
	CRhinoFileUtilities::GetRhinoRoamingProfileDataFolder(path);
	path += L"UI\\Plug-ins\\";

	ON_wString plugin;
	GetPlugInFileName(plugin);

	ON_wString fname;
	ON_wString::SplitPath(plugin, nullptr, nullptr, &fname, nullptr);
	fname += L".rui";

	path += fname;

	if (CRhinoFileUtilities::FileExists(path))
		CRhinoFileUtilities::DeleteFile(path);
}

ON_UUID CRPCPlugIn::PropertiesPlugInId() const
{
	return ON_UuidFromString(L"334b4fce-860c-415b-89ff-3beaf65ebf52");
}

void CRPCPlugIn::GetPropertiesPages(CRhinoPropertiesPanelPageCollection& collection)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    auto page = new CRpcPropertiesDlg();

	if (page)
		collection.Add(page);
}

BOOL CRPCPlugIn::CallWriteDocument(const CRhinoFileWriteOptions& options)
{
	return TRUE;
}

BOOL CRPCPlugIn::ReadDocument(CRhinoDoc& doc, ON_BinaryArchive& archive, const CRhinoFileReadOptions& options)
{
	if (Mains().EventWatcher().IsReferenceDocument() || Mains().EventWatcher().IsMergingDocument())
		return TRUE;

	if (!Mains().RpcDocument().ReadFromArchive(doc, archive, options))
		return FALSE;

	Mains().EventWatcher().SetReadingRpcData();

	return TRUE;
}

BOOL CRPCPlugIn::WriteDocument(CRhinoDoc& doc, ON_BinaryArchive& archive, const CRhinoFileWriteOptions& options)
{
	if (!Mains().RpcDocument().WriteToArchive(doc, archive, options))
		return FALSE;

	return TRUE;
}

const CLBPString CRPCPlugIn::getRpcApiFilename()
{
	const CLBPString sFullPathToPlugIn = PlugIn().PlugInFileName();
	CLBPString sPathOnly = CLBPFileMgr2::GetPathOnly(sFullPathToPlugIn);
	CLBPFileMgr2::RemoveTrailingSlash(sPathOnly);

	const CLBPString sRpcApiDll = sPathOnly + L"\\RPCapi.dll";

	return sRpcApiDll;
}
