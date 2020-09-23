// RPCPlugIn.h : main header file for the RPC plug-in.
//

#pragma once

// CRPCPlugIn
// See RPCPlugIn.cpp for the implementation of this class
//
class CRpcMains;


class CRPCPlugIn : public CRhinoUtilityPlugIn, IRhinoAddPropertiesPages
{
public:
  // CRPCPlugIn constructor. The constructor is called when the
  // plug-in is loaded and "thePlugIn" is constructed. Once the plug-in
  // is loaded, CRPCPlugIn::OnLoadPlugIn() is called. The
  // constructor should be simple and solid. Do anything that might fail in
  // CRPCPlugIn::OnLoadPlugIn().
  CRPCPlugIn();
  
  // CRPCPlugIn destructor. The destructor is called to destroy
  // "thePlugIn" when the plug-in is unloaded. Immediately before the
  // DLL is unloaded, CRPCPlugIn::OnUnloadPlugin() is called. Do
  // not do too much here. Be sure to clean up any memory you have allocated
  // with onmalloc(), onrealloc(), oncalloc(), or onstrdup().
  ~CRPCPlugIn() = default;

  // Required overrides
  
  // Plug-in name display string. This name is displayed by Rhino when
  // loading the plug-in, in the plug-in help menu, and in the Rhino
  // interface for managing plug-ins. 
  const wchar_t* PlugInName() const override;
  
  // Plug-in version display string. This name is displayed by Rhino
  // when loading the plug-in and in the Rhino interface for 
  // managing plug-ins.
  const wchar_t* PlugInVersion() const override;
  
  // Plug-in unique identifier. The identifier is used by Rhino for
  // managing plug-ins.
  GUID PlugInID() const override;
  
  // Additional overrides
  
  // Called after the plug-in is loaded and the constructor has been
  // run. This is a good place to perform any significant initialization,
  // license checking, and so on.  This function must return TRUE for
  // the plug-in to continue to load.  
  BOOL OnLoadPlugIn() override;
  
  // Called one time when plug-in is about to be unloaded. By this time,
  // Rhino's mainframe window has been destroyed, and some of the SDK
  // managers have been deleted. There is also no active document or active
  // view at this time. Thus, you should only be manipulating your own objects.
  // or tools here.  
  void OnUnloadPlugIn() override;

  //https://mcneel.myjetbrains.com/youtrack/issue/RH-60550
  plugin_load_time PlugInLoadTime() override { return load_plugin_when_needed_ignore_docked; }

  BOOL CallWriteDocument(const CRhinoFileWriteOptions& options) override;
  BOOL ReadDocument(CRhinoDoc& doc, ON_BinaryArchive& archive, const CRhinoFileReadOptions& options) override;
  BOOL WriteDocument(CRhinoDoc& doc, ON_BinaryArchive& archive, const CRhinoFileWriteOptions& options) override;
  const CLBPString getRpcApiFilename();

  CRpcMains& Mains(void);

private:
  void RefreshToolbar();

private:
    ON_wString m_plugin_version;

    // TODO: Add additional class information here
    CRpcMains* m_pMains = nullptr;

    // Inherited via IRhinoAddPropertiesPages
    ON_UUID PropertiesPlugInId() const override;
    void GetPropertiesPages(CRhinoPropertiesPanelPageCollection& collection) override;
};

// Return a reference to the one and only CRPCPlugIn object
CRPCPlugIn& PlugIn();
