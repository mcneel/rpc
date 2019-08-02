#include "StdAfx.h"
#include "RpcDragDropHandler.h"
#include "RpcUtilities.h"
#include "RpcInstance.h"


template<typename T>
void SDSFromZDS(const T* p, ON_ClassArray<CLBPString>& string_array)
{
	CLBPString s(p);

	while (!s.IsEmpty())
	{
		if (CRpcInstance::IsValidRpc(s))
		{
			string_array.Append(s);
		}

		while(*p != 0)
		  p++;
		s = ++p;
	}
}


CRpcDragDropHandler::CRpcDragDropHandler(void)
{
}

CRpcDragDropHandler::~CRpcDragDropHandler(void)
{
}

bool CRpcDragDropHandler::SupportDataObject( COleDataObject* pDataObject)
{
	ON_ClassArray<CLBPString> aRpcs;
	RpcPathFromOleData(pDataObject, aRpcs);
	return (aRpcs.Count() > 0) ? true : false;
}

bool CRpcDragDropHandler::OnDropOnRhinoView(CRhinoView* pRhinoView, COleDataObject* pDataObject, DROPEFFECT dropEffect, const ON_2iPoint& point)
{
	if (NULL == pRhinoView)
		return false;

	CRhinoDoc* pDoc = pRhinoView->GetDocument();
	if (NULL == pDoc) return false;

	ON_ClassArray<CLBPString> aRpcs;
	RpcPathFromOleData(pDataObject, aRpcs);
	if (aRpcs.Count () < 1)
		return false;

	CPoint first_ptIn;
	GetCursorPos(&first_ptIn);

	ON_2iPoint ptIn = P2P(first_ptIn);
    pRhinoView->ScreenToClient(ptIn); 

	ON_3dPoint pt3D(ptIn.x, ptIn.y, 0);

	ON_Line line;
    pRhinoView->ActiveViewport().VP().GetFrustumLine(ptIn.x, ptIn.y, line);

	const ON_3dmConstructionPlane& plane = pRhinoView->ActiveViewport().ConstructionPlane();

	double dParam = 0.0;
	if (!ON_Intersect(line, plane.m_plane, &dParam))
		return false;

	pt3D = line.PointAt(dParam);

	for(int i=0; i<aRpcs.Count(); i++)
	{
		CRpcInstance rpc(*pDoc, aRpcs[i]);
		rpc.AddToDocument(*pDoc, pt3D);
	}

	pDoc->Redraw();

	return true;
}

void CRpcDragDropHandler::RpcPathFromOleData(COleDataObject* pDataObject, ON_ClassArray<CLBPString>& aRpcs)
{
	if (!pDataObject->IsDataAvailable(CF_HDROP))
		return;

	STGMEDIUM medium = { 0 };
	if (!pDataObject->GetData(CF_HDROP, &medium))
		return;

	if (TYMED_HGLOBAL != medium.tymed)
		return;	

	const DROPFILES* pDropFiles = reinterpret_cast<const DROPFILES *>(::GlobalLock(medium.hGlobal));
	if (NULL == pDropFiles) return;

	const BYTE* pText = reinterpret_cast<const BYTE*>(pDropFiles) + pDropFiles->pFiles;

	if (pDropFiles->fWide)
	{
		SDSFromZDS(reinterpret_cast<const wchar_t*>(pText), aRpcs);
	}
	else
	{
		SDSFromZDS(reinterpret_cast<const char*>(pText), aRpcs);
	}

	::GlobalUnlock(medium.hGlobal);
}
