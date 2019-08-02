// GAWK-MARKER

#pragma once
#ifndef LBPRHLIB
#error RHLIB header included in non-Rhino compile
#endif

class CLBPRhObjectChangeTracker
{
public:
	CLBPRhObjectChangeTracker() { m_dwSignature = 0; }

public:
	DWORD m_dwSignature;
};

class CLBPRhObjectSelection
{
public:
	CLBPRhObjectSelection(const class IRhinoPropertiesPanelPage* pDialogPage, bool bMeshableOnly);
	CLBPRhObjectSelection(const class IRhinoPropertiesPanelPage* pDialogPage, ON::object_type types);
	CLBPRhObjectSelection(const CRhinoDoc& doc, bool bMeshableOnly); //Use the document iterator
	CLBPRhObjectSelection(const CLBPRhObjectSelection& sel);
	CLBPRhObjectSelection(const CRhinoDoc& doc, const ON_SimpleArray<UUID>& aObjects);
	CLBPRhObjectSelection(const CRhinoDoc& doc);	//Create an empty selection

	bool operator==(const CLBPRhObjectSelection& selection);
	bool operator!=(const CLBPRhObjectSelection& selection) { return !(*this==selection); }

	int		Count(void) const;
	const	CRhinoObject * First(void) const;
	const	CRhinoObject * Next(void) const;
	void	BoundingBox(ON_BoundingBox& boxOut) const;
	DWORD	CRC(void) const;
	unsigned int DocumentSerialNumber(void) const { return m_runtime_document_serial_number; }

private:
	mutable int	m_iIterationIndex;
	ON_SimpleArray<UUID> m_aObjects;
	const unsigned int m_runtime_document_serial_number;
};

#include "ON_SimpleMap.h"
class CLBPRhObjectList
{
public:
	CLBPRhObjectList(const CRhinoDoc& doc);

	int Count(void) const;
	const CRhinoObject* Object(int iIndex) const;
	void Add(const CRhinoObject* pObject);
	void AddObjects(const CLBPRhObjectSelection& sel);
	void Clear(void);
	unsigned int DocumentSerialNumber(void) const { return m_runtime_document_serial_number; }

private:
	ON_SimpleArray<UUID> m_aObjects;
	ON_SimpleUuidMap<int> m_mapObjects;
	const unsigned int m_runtime_document_serial_number;
};

int  LBPRhCountMeshableObjects(const IRhinoPropertiesPanelPage* pDialogPage);
bool LBPRhContainsMeshableObjects(const IRhinoPropertiesPanelPage* pDialogPage);
