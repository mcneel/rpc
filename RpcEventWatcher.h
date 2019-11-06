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
	void OnNewDocument(CRhinoDoc& doc) override;
	void OnBeginOpenDocument(CRhinoDoc& doc, const wchar_t* filename, BOOL bMerge, BOOL bReference) override;
	void OnEndOpenDocument(CRhinoDoc& doc, const wchar_t* filename, BOOL bMerge, BOOL bReference) override;
	void OnReplaceObject(CRhinoDoc& doc, CRhinoObject& old_object, CRhinoObject& new_object) override;

private:
	bool m_bMergeDocument;
	bool m_bReferenceDocument;
	bool m_bReadRpcData;
	bool m_bOpeningDocument;
};