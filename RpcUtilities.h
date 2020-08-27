#pragma once


int RpcMessageBox(const CLBPString& sMessage, UINT uType);

void RpcMesh2RhinoMesh(const RPCapi::Mesh& RpcMesh, ON_Mesh& RhinoMesh);

CLBPString UnusedInstanceDefinitionName(CRhinoDoc& doc);

CLBPString UnusedRpcName(const CLBPString& sRpcName);

void DebugSaveTexturesToRoot(const RPCapi::Instance& rpc, const ON_3dPoint& ptCamera);

bool UniteVertexNormals(ON_Mesh & mesh, ON_SimpleArray<int> & aSourceVertexIndexList, int iSourceVertexCount);

CRhRdkBasicMaterial* CreateNewBasicMaterial(void);

CRhRdkMaterial* CreatePBMaterial(void);

#if defined (INSITU)
#define _RhLocalizeString RhLocalizeString
#else
inline const ON_2iPoint  P2P(const CPoint& p) {	return ON_2iPoint(p.x, p.y);}
const wchar_t* _RhLocalizeString( const wchar_t* wsz, int nContext);
#endif
