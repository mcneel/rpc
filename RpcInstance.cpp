#include "StdAfx.h"
#include "RpcInstance.h"
#include "RpcClient.h"
#include "Resource.h"
#include "RpcMains.h"
#include "Resource.h"
#include "RpcObject.h"
#include "RpcUtilities.h"
#include "RpcDocument.h"
#include <iomanip>


CRpcInstance::CRpcInstance(const CRhinoDoc& doc, const CLBPString& sFullPath)
{
	Construct(doc.RuntimeSerialNumber(), nullptr, sFullPath);
}

CRpcInstance::CRpcInstance(const CRhinoDoc& doc, const CRhinoObject& obj)
{
	Construct(doc.RuntimeSerialNumber(), &obj, L"");
}

CRpcInstance::~CRpcInstance()
{
	KillEditUi();
	delete m_pInstance;
}

void CRpcInstance::Construct(UINT idDoc, const CRhinoObject* pObject, const CLBPString& sRpcPath)
{
	m_pEditInterface = nullptr;
	m_pEditDlgCallback = nullptr;
	selectionInterface = nullptr;
	m_idObject = ON_nil_uuid;
	m_idDoc = idDoc;

	if (!sRpcPath.IsEmpty())
	{
		Mains().RpcClient().OnRpcInstanceWillBeCreated(m_idDoc, sRpcPath);
	}

	m_pInstance = dynamic_cast<RPCapi::Instance*>(Mains().RpcClient().RPCgetAPI()->newObject(RPCapi::ObjectCodes::INSTANCE));

	if (!m_pInstance) 
		return;

	m_pInstance->setClientInstance(this);

	if (!sRpcPath.IsEmpty())
	{
		if (!m_pInstance->setRPCFileName(sRpcPath.T()))
		{
			RPCapi::ParamList* pRpcFile = Mains().RpcClient().RPCgetAPI()->openRPCFile(sRpcPath.T());

			if (pRpcFile)
			{
				RPCapi::ID* pRpcId = dynamic_cast<RPCapi::ID*>(pRpcFile->get("/metadata/id"));
				delete pRpcFile;
				m_pInstance->setRpcId(pRpcId);
				delete pRpcId;
			}
		}
	}
	else
		if (pObject)
		{
			m_idObject = pObject->ModelObjectId();
			CopyToRpc(*pObject);
		}
		else
			return;
}

CRhinoMeshObject* CRpcInstance::CreateProxyMesh(CRhinoDoc& doc)
{
	if (!m_pInstance)
		return nullptr;

	RPCapi::Mesh* pRpcMesh = m_pInstance->getProxyMesh();

	if (!pRpcMesh)
	{
		RhinoApp().Print(_RhLocalizeString(L"RPC invalid: selected RPC has no mesh.\n", 36080));
		return nullptr;
	}

	const double dUnitsScale = ON::UnitScale(ON::LengthUnitSystem::Inches, doc.ModelUnits());
	ON_Xform xformUnitsScale = ON_Xform::DiagonalTransformation(dUnitsScale, dUnitsScale, dUnitsScale);

	ON_Mesh* pRhinoMesh = new  ON_Mesh;

	RpcMesh2RhinoMesh(*pRpcMesh, *pRhinoMesh);

	delete pRpcMesh;

	pRhinoMesh->Transform(xformUnitsScale);

	CRhinoMeshObject* pMesh = new CRhinoMeshObject;
	pMesh->SetMesh(pRhinoMesh);

	return pMesh;
}

bool CRpcInstance::IsValidRpc(const CLBPString& s) // static
{
	RPCapi* pApi = Mains().RpcClient().RPCgetAPI();
	if (!pApi) 
		return false;

	RPCapi::ParamList* pRpcFile = pApi->openRPCFile(s.T());
	if (!pRpcFile)
		return false;

	delete pRpcFile;
	return true;
}

bool CRpcInstance::IsValid(void) const
{
	RPCapi* pApi = Mains().RpcClient().RPCgetAPI();
	if (!pApi) 
		return false;

	CLBPString sPath = FileName();
	if (sPath.IsEmpty())
	{
		pApi->updatePaths();
		sPath = FileName();
	}

	RPCapi::ParamList* pRpcFile = pApi->openRPCFile(sPath.T());
	if (!pRpcFile)
		return false;

	delete pRpcFile;
	return true;
}

void CRpcInstance::SetId(const RPCapi::ID *id)
{
	if (!m_pInstance)
		return;

	m_pInstance->setRpcId(id);
}

const RPCapi::ID* CRpcInstance::Id(void) const
{
	if (!m_pInstance)
		return nullptr;

	return m_pInstance->getID();
}

CLBPString CRpcInstance::FileName(void) const
{
	if (!m_pInstance)
		return L"";

	const TString & s = m_pInstance->getRPCFileName();

	return s.getA();
}

CLBPString CRpcInstance::ObjectName(void) const
{
	const CRhinoObject* pObject = Object();

	if (!pObject)
	{
		m_sName = L"";
	}
	else
	{
		m_sName = pObject->Attributes().m_name;
	}

	return m_sName;
}

CLBPString CRpcInstance::RpcName(void) const
{
	if (!m_pInstance)
		return L"";

	TString* pName = m_pInstance->getName();
	if (!pName) return L"";

	CLBPString s = pName->getA();
	delete pName;

	return s;
}

RPCapi::Instance* CRpcInstance::Instance(void)
{
	return m_pInstance;
}

TStringArg CRpcInstance::RPCgetName(void)
{
	ObjectName();
	return m_sName.AsMBCSString();
}

RPCapi::Instance* CRpcInstance::RPCgetInstance(void)
{
	return m_pInstance;
}

void CRpcInstance::RPCgetPivot(double px, double py, double pz, double &distance, double &dx, double &dy, double &dz)
{

}

int CRpcInstance::RPCgetTime(void)
{
	return Mains().RpcDocument().AnimationFrame();
}

bool CRpcInstance::RPCisSelected(void)
{
	const CRhinoObject* pObject = Object();
	if (!pObject)
		return false;

	return (0 == pObject->IsSelected()) ? false : true;
}

void CRpcInstance::RPCparameterChangeNotification(bool newInstance, const TString **params, int num)
{
	if (m_pEditDlgCallback)
	{
		m_pEditDlgCallback->OnRpcParameterChanged();
	}
}

void CRpcInstance::RPCselect(bool select)
{
	const CRhinoObject* pObject = Object();

	if (pObject)
	{
		pObject->Select(select);
	}
}

bool CRpcInstance::Data(CLBPBuffer& buf) const
{
	if (!IsValid())
		return false;

	const int iSize = m_pInstance->size();
	char* buffer = new char[iSize + 1];
	buffer[iSize] = 0;

	stringstream streamOut(buffer);
	m_pInstance->toStream(streamOut, 0);

	string str = streamOut.str();
	const size_t strSize = str.size();

	buf.Set(str.c_str(), strSize);

	delete []buffer;

	return true;
}

void CRpcInstance::SetData(const CLBPBuffer& buf)
{
	if (!m_pInstance)
		return;

	const int iType = m_pInstance->typeCode();
	const size_t iSize = buf.NumBytes();
	const BYTE* pBytes = buf.Bytes();

	const char* sz = (const char*)pBytes;

	string str(sz, iSize);
	stringstream streamIn(str);
	m_pInstance->fromStream(streamIn, 0, iType);

	Mains().RpcClient().OnRpcInstanceWillBeCreated(m_idDoc, FileName());
}

bool CRpcInstance::CopyToRpc(const CRhinoObject& obj)
{
	CRpcObject ro(&obj);
	if (!ro.IsTagged())
		return false;

	CLBPBuffer buf;
	ro.RpcData(buf);

	SetData(buf);

	return true;
}

bool CRpcInstance::CopyFromRpc(const CRhinoObject& obj) const
{
	const CLBPString sRpc = FileName();
	if (sRpc.IsEmpty())
		return false;

	CLBPBuffer buf;
	Data(buf);

	CRpcObject ro(&obj);
	ro.Tag(buf);

	return true;
}

void CRpcInstance::KillEditUi(void)
{
	if (m_pEditInterface)
	{
		m_pEditInterface->hide();
		delete m_pEditInterface;
		m_pEditInterface = nullptr;
	}
}

void CRpcInstance::KillEditSelUi(void)
{
	if (selectionInterface)
	{
		selectionInterface->hide();
		delete selectionInterface;
		selectionInterface = nullptr;
	}
}

bool CRpcInstance::EditSelUI(HWND hWndParent, IEditDialogCallback* pCallback)
{
	if (!m_pInstance)
		return false;

	KillEditSelUi();

	selectionInterface = dynamic_cast<RPCapi::InstanceInterface *>(Mains().RpcClient().RPCgetAPI()->newObject(RPCapi::ObjectCodes::INSTANCE_INTERFACE));
	if (!selectionInterface)
		return false;

	selectionInterface->setInstance(m_pInstance);

	CRhinoDoc* pDoc = CRhinoDoc::FromRuntimeSerialNumber(m_idDoc);
	const double dScale = (pDoc) ? ON::UnitScale(ON::LengthUnitSystem::Inches, pDoc->ModelUnits()) : 1.0;
	selectionInterface->setUnits(RPCapi::Units::LINEAR_UNITS, dScale);
	selectionInterface->show(hWndParent, RPCapi::InstanceInterface::Window::SELECTION);
	return true;
}

bool CRpcInstance::EditUi(HWND hWndParent, IEditDialogCallback* pCallback)
{
	if (!m_pInstance)
		return false;

	m_pEditDlgCallback = pCallback;

	KillEditUi();

	m_pEditInterface = dynamic_cast<RPCapi::InstanceInterface *>(Mains().RpcClient().RPCgetAPI()->newObject(RPCapi::ObjectCodes::INSTANCE_INTERFACE));
	if (!m_pEditInterface) 
		return false;

	m_pEditInterface->setInstance(m_pInstance);

	CRhinoDoc* pDoc = CRhinoDoc::FromRuntimeSerialNumber(m_idDoc);
	const double dScale = (pDoc) ? ON::UnitScale(ON::LengthUnitSystem::Inches, pDoc->ModelUnits()) : 1.0;

	m_pEditInterface->setUnits(RPCapi::Units::LINEAR_UNITS, dScale);
	m_pEditInterface->show(hWndParent, RPCapi::InstanceInterface::Window::PARAMETERS);
	return true;
}

CRhinoInstanceObject* CRpcInstance::Replace(CRhinoDoc& doc)
{
	const CRhinoInstanceObject* pBlock = CRhinoInstanceObject::Cast(Object());
	if (!pBlock) 
		return nullptr;

	const int iInstanceDefintionId = pBlock->InstanceDefinition()->Index();
	ObjectArray objects;
	objects.Append(CreateProxyMesh(doc));

	CRhinoInstanceDefinitionTable& defTable = doc.m_instance_definition_table;
	defTable.ModifyInstanceDefinition(*pBlock->InstanceDefinition(), iInstanceDefintionId, ON_InstanceDefinition::all_idef_settings, true);
	defTable.ModifyInstanceDefinitionGeometry(iInstanceDefintionId, objects, true);

	return (CRhinoInstanceObject*) pBlock;
}

CRhinoInstanceObject* CRpcInstance::AddToDocument(CRhinoDoc& doc, const ON_3dPoint& pt)
{
    ON_Xform xform = ON_Xform::TranslationTransformation(pt - ON_origin);

	const CLBPString sName = UnusedRpcName(RpcName());

	return AddToDocument(doc, sName, xform);
}

const CRhinoObject* CRpcInstance::Object(void) const
{
	const CRhinoDoc* pDoc = CRhinoDoc::FromRuntimeSerialNumber(m_idDoc);

	if (pDoc)
		return  pDoc->LookupObject(m_idObject);

	return nullptr;
}

int CRpcInstance::CreateLayer(wstring& rpcName)
{
	constexpr int NotFoundIndex = -2;
	constexpr int MultipleFoundIndex = -1;
	constexpr auto RpcLayer = L"RPC Assets";
	wchar_t* categoryName = nullptr;
	wchar_t* contentName = nullptr;

	CRhinoLayerTable& layerTable = RhinoApp().ActiveDoc()->m_layer_table;
	int index = layerTable.FindLayerFromName(rpcName.c_str(), false, false, NotFoundIndex, MultipleFoundIndex);

	if (index >= 0)
	{
		return index;
	}

	if (auto rpcFile = m_pInstance->getRPCFile())
	{
		categoryName = dynamic_cast<RPCapi::TString*>(rpcFile->get("/metadata/categoryName"))->extractW();
		contentName = dynamic_cast<RPCapi::TString*>(rpcFile->get("/metadata/contentName"))->extractW();
		delete rpcFile;
	}

	int parentLayerIndex = layerTable.CurrentLayerIndex();

	if (layerTable.CurrentLayer().Name() != RpcLayer)
	{
		ON_Layer parentLayer;
		parentLayer.SetName(RpcLayer);

		parentLayerIndex = layerTable.AddLayer(parentLayer);
		layerTable.SetCurrentLayerIndex(parentLayerIndex);
	}

	const CRhinoLayer& layer = layerTable[parentLayerIndex];
	int check = layerTable.FindLayerFromName(categoryName, false, false, NotFoundIndex, MultipleFoundIndex);
	int childLayerIndex = check;

	if (check == NotFoundIndex)
	{
		ON_Layer childLayer;
		childLayer.SetParentLayerId(layer.Id());
		childLayer.SetName(categoryName);
		childLayerIndex = layerTable.AddLayer(childLayer);
	}

	const CRhinoLayer& subLayer = layerTable[childLayerIndex];
	int counter = 1;
	ON_Layer rpcLayer;
	rpcLayer.SetParentLayerId(subLayer.Id());
	wstring objectName;

	do
	{
		std::stringstream ss;
		ss << std::setw(3) << std::setfill('0') << counter;
		objectName = wstring(contentName) + L"_" + wstring(ss.str().begin(), ss.str().end());
		counter++;
	} while (layerTable.FindLayerFromName(objectName.c_str(), false, false, NotFoundIndex, MultipleFoundIndex) != NotFoundIndex);

	rpcName = objectName;
	rpcLayer.SetName(objectName.c_str());

	return layerTable.AddLayer(rpcLayer);
}

CRhinoInstanceObject* CRpcInstance::AddToDocument(CRhinoDoc& doc, const CLBPString& sName,
												  const ON_Xform& xform)
{
	CRhinoInstanceDefinitionTable& defTable = doc.m_instance_definition_table;

	const ON_wString sDefName = UnusedInstanceDefinitionName(doc).Wide();

	ON_InstanceDefinition idef;
	idef.SetName(sDefName);

	const int iIndex = defTable.AddInstanceDefinition(idef, CreateProxyMesh(doc));

	ON_3dmObjectAttributes* attr = new ON_3dmObjectAttributes();
	wstring rpcName = sName.Wide();
	attr->m_layer_index = CreateLayer(rpcName);
	attr->SetName(rpcName.c_str(), true);
	wstring objectName = L"*_RPC_" + wstring(rpcName);
	defTable.SetName(iIndex, objectName.c_str());

    CRhinoInstanceObject* pInstObj = defTable.AddInstanceObject(iIndex, xform, attr);

	if (pInstObj)
	{
		CopyFromRpc(*pInstObj);
		m_idObject = pInstObj->ModelObjectId();
		m_idDoc = doc.RuntimeSerialNumber();
	}

	Mains().GetRPCInstanceTable().SetAt(pInstObj->Id(), this);

	return pInstObj;
}
