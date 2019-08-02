#pragma once

class CRpcEventWatcher : public CRhinoEventWatcher
{
public:
	CRpcEventWatcher(void);
	~CRpcEventWatcher(void);

public:
	bool IsMergingDocument(void) const;
	bool IsReferenceDocument(void) const;
	bool IsOpeningDocument(void) const;

	void SetReadingRpcData(void);

public:
	virtual void OnNewDocument(CRhinoDoc& doc);
	virtual void OnBeginOpenDocument(CRhinoDoc& doc, const wchar_t* filename, BOOL bMerge, BOOL bReference);
	virtual void OnEndOpenDocument(CRhinoDoc& doc, const wchar_t* filename, BOOL bMerge, BOOL bReference);

private:
	bool m_bMergeDocument;
	bool m_bReferenceDocument;
	bool m_bReadRpcData;
	bool m_bOpeningDocument;
};
