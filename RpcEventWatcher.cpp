#include "StdAfx.h"
#include "RpcEventWatcher.h"
#include "RpcMains.h"
#include "RpcDocument.h"
#include "RpcInstance.h"


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

void CRpcEventWatcher::OnEndCommand(const CRhinoCommand & command, const CRhinoCommandContext & context, CRhinoCommand::result rc)
{
	if (wcscmp(command.EnglishCommandName(), L"vrayRender") != 0
		& wcscmp(command.EnglishCommandName(), L"Render") != 0)
		return;

	CRhinoDoc* doc = RhinoApp().ActiveDoc();
	CRhinoObjectIterator it(*doc, CRhinoObjectIterator::normal_or_locked_objects, CRhinoObjectIterator::active_and_reference_objects);
	const CRhinoObject* pObject = it.First();

	while (pObject != NULL)
	{
		CRpcInstance rpc(*doc, *pObject);
		rpc.Replace(*doc);
		pObject = it.Next();
	}

	RhinoRedrawLayerManagerWindow();
	RhinoApp().ActiveView()->EnableDrawing(true);

}