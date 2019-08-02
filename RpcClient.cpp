
#include "StdAfx.h"
#include "RpcClient.h"
#include "RPCPlugIn.h"
#include "LBPFileMgr2.h"

const int NUM_METADATA = 9;

CRpcClient::CRpcClient(void)
{
	m_pRpcApi = nullptr;
	m_RpcPaths = nullptr;
	m_hRpcApiHandle = nullptr;

	m_metaKeys = new RPCapi::TStringArg[NUM_METADATA];
	m_metaValues = new RPCapi::Param*[NUM_METADATA];
	m_numMetaValues = 0;
}

CRpcClient::~CRpcClient(void)
{
	Flush();
}

void CRpcClient::Flush(void)
{
	if (0 != m_metaKeys)
	{
		delete[] m_metaKeys;
		m_metaKeys = 0;
	}

	if (0 != m_metaValues)
	{
		for (int i = 0; i < m_numMetaValues; i++)
			delete m_metaValues[i];
		delete[] m_metaValues;
		m_metaValues = 0;
		m_numMetaValues = 0;
	}

	if (nullptr != m_RpcPaths)
	{
		delete[] m_RpcPaths;
		m_RpcPaths = nullptr;
	}

	if (nullptr != m_pRpcApi)
	{
		delete m_pRpcApi;
		m_pRpcApi = nullptr;
	}

	if (nullptr != m_hRpcApiHandle)
	{
		FreeLibrary(m_hRpcApiHandle);
		m_hRpcApiHandle = nullptr;
	}
}

#ifdef _DEBUG
void* track_memory_allocate(size_t size, const char *file, int line)
{
	return malloc(size);
}

void track_memory_deallocate(void *ptr, const char *file, int line)
{
	free(ptr);
}
#endif

bool CRpcClient::Initialize(void)
{
	const CLBPString sFullPathToPlugIn = PlugIn().PlugInFileName();
	CLBPString sPathOnly = CLBPFileMgr2::GetPathOnly(sFullPathToPlugIn);
	CLBPFileMgr2::RemoveTrailingSlash(sPathOnly);
	
	const CLBPString sRpcApiDll = sPathOnly + L"\\RPCapi.dll";

	m_hRpcApiHandle = LoadLibrary(sRpcApiDll);
	if (nullptr != m_hRpcApiHandle)
	{

//#ifdef _DEBUG
//		RPCsetMemoryAllocationFuncType  allocDealloc = (RPCsetMemoryAllocationFuncType)GetProcAddress(m_hRpcApiHandle, "RPCsetMemoryAllocation");
//		allocDealloc(track_memory_allocate, track_memory_deallocate);
//#endif //_DEBUG

		RPCapi::RPCgetAPIFuncType getAPI = (RPCapi::RPCgetAPIFuncType)GetProcAddress(m_hRpcApiHandle, "RPCgetAPI");
		m_pRpcApi = getAPI(NULL);
		m_pRpcApi->setClient(this);
	}

	return Valid();
}

bool CRpcClient::Uninitialize(void)
{
	Flush();
	return true;
}

bool CRpcClient::Valid(void) const
{
	return nullptr != m_pRpcApi;
}

RPCapi& CRpcClient::RPC(void) const
{
	ASSERT(Valid());
	return *m_pRpcApi;
}

TStringArg CRpcClient::RPCgetAboutString(void)
{
	return "Rhino RPC plugin";
}

RPCapi* CRpcClient::RPCgetAPI(void)
{
	return m_pRpcApi;
}

const RPCapi::License* CRpcClient::RPCgetLicense(void)
{
	return m_pRpcApi->getLicense();
}

TStringArg* CRpcClient::RPCgetPaths(bool start, int &numPaths)
{
	static HCURSOR prevCursor;
	if (!start)
	{
		SetCursor(prevCursor);
		return nullptr;
	}

	HCURSOR hCursor = (HCURSOR)::LoadImage(NULL, IDC_WAIT, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
	prevCursor = SetCursor(hCursor);

	numPaths = m_aRpcPath.Count();

	if (nullptr == m_RpcPaths)
	{
		if (numPaths > 0)
		{
			m_RpcPaths = new TStringArg[numPaths];
			
			for(int i=0; i<numPaths; i++)
			{
				m_RpcPaths[i] = m_aRpcPath[i].Wide();
			}
		}
	}

	return m_RpcPaths;
}

int CRpcClient::RPCgetMode()
{
	return RPCapi::Client::Mode::EDIT;
}

TStringArg CRpcClient::RPCiniPath(void)
{
	return NULL;
}

void CRpcClient::RPClicenseChange(bool licensed, bool acm)
{

}

void CRpcClient::RPCpluginMetadata(int &num, const TStringArg*& keys, const RPCapi::Param**& values)
{
	//int i = 0;
	//m_metaKeys[i].init(RPCapi::TString::ACHAR, "author");
	//RPCapi::TString *apiString = dynamic_cast<RPCapi::TString *>
	//	(m_pRpcApi->newObject(RPCapi::ObjectCodes::STRING));
	//apiString->setA("ArchVision");
	//m_metaValues[i] = apiString;
	//++i;
	//m_metaKeys[i].init(RPCapi::TString::ACHAR, "license");
	//RPCapi::License *apiLicense = dynamic_cast<RPCapi::License *>
	//	(m_pRpcApi->newObject(RPCapi::ObjectCodes::LICENSE));
	//apiLicense->set(true, 1, RPCapi::License::COMMERCIAL);
	//m_metaValues[i] = apiLicense;
	//++i;
	//RPCapi::PrimP<int> *pint;
	//m_metaKeys[i].init(RPCapi::TString::ACHAR, "archSubscription4");
	//pint = dynamic_cast<RPCapi::PrimP<int> *>
	//	(m_pRpcApi->newObject(RPCapi::ObjectCodes::PRIM_INT));
	//pint->setValue(1);
	//m_metaValues[i] = pint;
	//++i;
	//m_metaKeys[i].init(RPCapi::TString::ACHAR, "archSubscription5");
	//pint = dynamic_cast<RPCapi::PrimP<int> *>
	//	(m_pRpcApi->newObject(RPCapi::ObjectCodes::PRIM_INT));
	//pint->setValue(1);
	//m_metaValues[i] = pint;
	//++i;
	//m_metaKeys[i].init(RPCapi::TString::ACHAR, "archSubscription6");
	//pint = dynamic_cast<RPCapi::PrimP<int> *>
	//	(m_pRpcApi->newObject(RPCapi::ObjectCodes::PRIM_INT));
	//pint->setValue(1);
	//m_metaValues[i] = pint;
	//++i;
	//m_metaKeys[i].init(RPCapi::TString::ACHAR, "archSubscription7");
	//pint = dynamic_cast<RPCapi::PrimP<int> *>
	//	(m_pRpcApi->newObject(RPCapi::ObjectCodes::PRIM_INT));
	//pint->setValue(1);
	//m_metaValues[i] = pint;
	//++i;
	//m_metaKeys[i].init(RPCapi::TString::ACHAR, "archSubscription8");
	//pint = dynamic_cast<RPCapi::PrimP<int> *>
	//	(m_pRpcApi->newObject(RPCapi::ObjectCodes::PRIM_INT));
	//pint->setValue(1);
	//m_metaValues[i] = pint;
	//++i;
	//m_metaKeys[i].init(RPCapi::TString::ACHAR, "archSubscription9");
	//pint = dynamic_cast<RPCapi::PrimP<int> *>
	//	(m_pRpcApi->newObject(RPCapi::ObjectCodes::PRIM_INT));
	//pint->setValue(1);
	//m_metaValues[i] = pint;
	//++i;
	//m_metaKeys[i].init(RPCapi::TString::ACHAR, "archSoftwareLicense");
	//pint = dynamic_cast<RPCapi::PrimP<int> *>
	//	(m_pRpcApi->newObject(RPCapi::ObjectCodes::PRIM_INT));
	//pint->setValue(1);
	//m_metaValues[i] = pint;
	//m_numMetaValues = num = NUM_METADATA;
	//keys = m_metaKeys;
	//values = (const RPCapi::Param **)m_metaValues;
}

void CRpcClient::RPCcontentMetadata(int req, int &numsets, const int *&setsize, const TStringArg **&keys,
									const RPCapi::Param ***&values)
{
	numsets = 0;
	setsize = NULL;
	keys = NULL;
	values = NULL;
}

TStringArg CRpcClient::RPCpluginId(void)
{
	return L"20-1cfc-4d06-70d84-2ed4";
}

bool CRpcClient::RPCuserMessage(int msgType, bool ret, const TString &title, const TString &msg)
{
	return false;
}

void CRpcClient::OnRpcInstanceWillBeCreated(UINT idDoc, const CLBPString& sRpc)
{
	CLBPString sRpcPath = CLBPFileMgr2::GetPathOnly(sRpc);
	CLBPFileMgr2::RemoveTrailingSlash(sRpcPath);

	if (!CRhinoFileUtilities::FileExists(sRpc.Wide()))
	{
		ON_wString sPathFound;
		if (!RhRdkFindFile(idDoc, sRpc, sPathFound))
		{
			return;
		}
		
		sRpcPath = sPathFound;
	}
	
	if (!IsRpcPathInList(sRpcPath))
	{
		CLBPFileMgr2::RemoveTrailingSlash(sRpcPath);
		AddRpcPathToList(sRpcPath);
	}
}

bool CRpcClient::IsRpcPathInList(const CLBPString& sPath) const
{
	for (int i=0; i<m_aRpcPath.Count(); i++)
	{
		if (0 == sPath.CompareNoCase(m_aRpcPath[i].Wide()))
			return true;
	}

	return false;
}

void CRpcClient::AddRpcPathToList(const CLBPString& sPath)
{
	m_aRpcPath.Append(sPath);

	if (nullptr != m_RpcPaths)
	{
		delete[] m_RpcPaths;
		m_RpcPaths = nullptr;
	}

	if (nullptr != m_pRpcApi)
	{
		m_pRpcApi->updatePaths();
	}
}
