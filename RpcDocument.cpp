#include "StdAfx.h"
#include "RpcDocument.h"
#include "RpcMains.h"
#include "RpcEventMachine.h"
#include "RpcUtilities.h"

CRpcDocument::CRpcDocument(void)
{
	Defaults();
}

CRpcDocument::~CRpcDocument(void)
{
}

void CRpcDocument::Defaults(void)
{
	m_iAnimationFrame = 0;
}

int CRpcDocument::Version(void)
{
	return 1;
}

int CRpcDocument::AnimationFrame(void) const
{
	return m_iAnimationFrame;
}

void CRpcDocument::SetAnimationFrame(CRhinoDoc& doc, int iFrame)
{
	if (m_iAnimationFrame == iFrame)
		return;

	const int iOldFrame = m_iAnimationFrame;
	m_iAnimationFrame = iFrame;
	Mains().EventMachine().OnAnimationFrameChanged(doc, iOldFrame, iFrame);
}

bool CRpcDocument::ReadFromArchive(CRhinoDoc& doc, ON_BinaryArchive& archive, const CRhinoFileReadOptions& options)
{
	int major_version = 0;
	int minor_version = 0;
	if (!archive.BeginRead3dmChunk(TCODE_USER, &major_version, &minor_version))
		return false;

	int iVersion = 0;
	if (!archive.ReadInt(&iVersion))
	{
		archive.EndRead3dmChunk();
		return false;
	}

	if (iVersion > Version())
	{
		const CLBPString sMsg = _RhLocalizeString( L"This model was created with a more recent version of the RPC Plugin.", 36076);
		::MessageBox(RhinoApp().MainWnd(), sMsg, L"RPC", MB_OK|MB_ICONINFORMATION);
		archive.EndRead3dmChunk();
		return false;
	}

	bool bRet = true;

	if (bRet)	{bRet = archive.ReadInt(&m_iAnimationFrame);}

	if (!archive.EndRead3dmChunk())
		return false;

	return bRet;
}

bool CRpcDocument::WriteToArchive(CRhinoDoc& doc, ON_BinaryArchive& archive, const CRhinoFileWriteOptions& options)
{
	const int major_version = 1;
	const int minor_version = 0;
	archive.BeginWrite3dmChunk(TCODE_USER, major_version, minor_version);

	if (!archive.WriteInt(Version()))
	{
		archive.EndWrite3dmChunk();
		return false;
	}

	bool bRet = true;

	if (bRet)	{bRet = archive.WriteInt(m_iAnimationFrame);}

	if (!archive.EndWrite3dmChunk())
		return false;

	return bRet;
}
