#include "StdAfx.h"
#include "RpcObjectUserData.h"
#include "RPCPlugIn.h"
#include "RpcObjectStorageStrings.h"
#include "RpcUtilities.h"
#include "RpcMains.h"

ON_OBJECT_IMPLEMENT(CRpcObjectUserData, ON_UserData, "F3E7353B-3C7F-45bc-9F71-32025D6E6C4A");


UUID CRpcObjectUserData::Id()
{
	// {F3E7353B-3C7F-45bc-9F71-32025D6E6C4A}
	static const GUID id = 
	{ 0xf3e7353b, 0x3c7f, 0x45bc, { 0x9f, 0x71, 0x32, 0x2, 0x5d, 0x6e, 0x6c, 0x4a } };
	return id;
}

CRpcObjectUserData::CRpcObjectUserData(void)
{
	m_userdata_uuid = Id();
	m_application_uuid = PlugIn().PlugInID();
	m_userdata_copycount = 1;
}

CRpcObjectUserData::CRpcObjectUserData(const CRpcObjectUserData& ud)
: CLBPRh_XMLUserData(ud)
{
	m_userdata_uuid = Id();
	m_application_uuid = PlugIn().PlugInID();
	*this = ud;
}

CRpcObjectUserData::~CRpcObjectUserData(void)
{

}

const CRpcObjectUserData& CRpcObjectUserData::operator = (const CRpcObjectUserData& ud)
{
	if(this != &ud)
	{
		CLBPRh_XMLUserData::operator = (ud);
	}

	return *this;
}

bool CRpcObjectUserData::operator==(const CRpcObjectUserData& src)
{
	if(this == &src)
		return true;

	return (XMLRoot().CRC() == src.XMLRoot().CRC()) ? true : false;
}

bool CRpcObjectUserData::operator!=(const CRpcObjectUserData& src)
{
	return (*this == src) ? false : true;
}

int CRpcObjectUserData::VersionNumber() const
{
	return RPC_USER_DATA_VERSION;
}

void CRpcObjectUserData::SetToDefaults(void) const
{
	Clear();

	CLBP_XMLProperty prop;
	prop.SetName(L"");

	// Main
	CLBP_XMLNode* pNodeMain = XMLRoot().AddChildNode(new CLBP_XMLNode(RPC_DATA));

	// Version
	CLBP_XMLNode* pNodeVersion = pNodeMain->AddChildNode(new CLBP_XMLNode(RPC_VERSION));
	prop.SetValue(Version());
	pNodeVersion->AddProperty(&prop);

	// Properties
	CLBP_XMLNode* pNodeProperties = pNodeMain->AddChildNode(new CLBP_XMLNode(RPC_PROPERTIES));

	CLBP_XMLNode* pNode = pNodeProperties->AddChildNode(new CLBP_XMLNode(RPC_DATA));
	prop.SetValue(CLBPBuffer());
	pNode->AddProperty(&prop);
}

void CRpcObjectUserData::ReportVersionError(void) const
{
	const CLBPString sMsg = _RhLocalizeString( L"This model was created with a more recent version of the RPC plugin.\nPlease upgrade your version of the RPC plugin.", 36081);
	::MessageBox(RhinoApp().MainWnd(), sMsg, L"RPC", MB_OK | MB_ICONINFORMATION);
}

bool CRpcObjectUserData::Archive(void) const
{
	return true;
}

bool CRpcObjectUserData::Write(ON_BinaryArchive& archive) const
{
	if(!archive.WriteInt(VersionNumber()))
		return false;

	return CLBPRh_XMLUserData::Write(archive);
}

bool CRpcObjectUserData::Read(ON_BinaryArchive& archive)
{
	int iVersion = 0;
	if(!archive.ReadInt(&iVersion))
		return false;

	if(iVersion > VersionNumber())
	{
		return false;
	}

	SetToDefaults();

	return CLBPRh_XMLUserData::Read(archive);
}

void CRpcObjectUserData::RpcData(CLBPBuffer& buf) const
{
	CLBP_XMLNode* pNodeMain = XMLRoot().GetNamedChild(RPC_DATA);
	ASSERT(NULL != pNodeMain);

	CLBP_XMLNode* pNodeProperties = pNodeMain->GetNamedChild(RPC_PROPERTIES);
	ASSERT(NULL != pNodeProperties);

	CLBP_XMLNode* pNode = pNodeProperties->GetNamedChild(RPC_DATA);
	ASSERT(NULL != pNode);

	CLBP_XMLProperty* pProp = pNode->GetNamedProperty(L"");
	ASSERT(NULL != pProp);

	buf = pProp->GetValue().AsBuffer();
}

void CRpcObjectUserData::SetRpcData(const CLBPBuffer& buf)
{
	CLBP_XMLNode* pNodeMain = XMLRoot().GetNamedChild(RPC_DATA);
	ASSERT(NULL != pNodeMain);

	CLBP_XMLNode* pNodeProperties = pNodeMain->GetNamedChild(RPC_PROPERTIES);
	ASSERT(NULL != pNodeProperties);

	CLBP_XMLNode* pNode = pNodeProperties->GetNamedChild(RPC_DATA);
	ASSERT(NULL != pNode);

	CLBP_XMLProperty* pProp = pNode->GetNamedProperty(L"");
	ASSERT(NULL != pProp);

	return pProp->SetValue(buf);
}
