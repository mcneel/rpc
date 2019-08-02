
#pragma once

class CRpcDragDropHandler : public CRhinoDropTarget
{
public:
	CRpcDragDropHandler(void);
	~CRpcDragDropHandler(void);

public:
	// CRhinoDropTarget overrides
	virtual bool SupportDataObject(COleDataObject* pDataObject);
	virtual bool OnDropOnRhinoView( CRhinoView* pRhinoView, COleDataObject* pDataObject, DROPEFFECT dropEffect, const ON_2iPoint& point) override;

private:
	void RpcPathFromOleData(COleDataObject* pDataObject, ON_ClassArray<CLBPString>& aRpcs);
};
