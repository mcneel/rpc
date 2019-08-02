#include "StdAfx.h"
#include "RpcEventWatcher.h"
#include "RpcMains.h"
#include "RpcDocument.h"


CRpcEventWatcher::CRpcEventWatcher(void)
{
	m_bMergeDocument = false;
	m_bReferenceDocument = false;
	m_bOpeningDocument = false;
	m_bReadRpcData = false;
}

CRpcEventWatcher::~CRpcEventWatcher(void)
{
}

bool CRpcEventWatcher::IsMergingDocument(void) const
{
	return m_bMergeDocument;
}

bool CRpcEventWatcher::IsReferenceDocument(void) const
{
	return m_bReferenceDocument;
}

bool CRpcEventWatcher::IsOpeningDocument(void) const
{
	return m_bOpeningDocument;
}

void CRpcEventWatcher::SetReadingRpcData(void)
{
	m_bReadRpcData = true;
}

void CRpcEventWatcher::OnNewDocument(CRhinoDoc& doc)
{
	Mains().RpcDocument().Defaults();
}

void CRpcEventWatcher::OnBeginOpenDocument(CRhinoDoc& doc, const wchar_t* filename, BOOL bMerge, BOOL bReference)
{
	m_bMergeDocument = bMerge ? true : false;
	m_bReferenceDocument = bReference ? true : false;
	m_bOpeningDocument = true;
	m_bReadRpcData = false;
}

void CRpcEventWatcher::OnEndOpenDocument(CRhinoDoc& doc, const wchar_t* filename, BOOL bMerge, BOOL bReference)
{
	m_bOpeningDocument = false;

	if (!m_bReadRpcData)
	{
		Mains().RpcDocument().Defaults();
	}
}
