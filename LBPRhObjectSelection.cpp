// GAWK-MARKER

#include "stdafx.h"

#ifdef LBPRHLIB

#include "LBPRhObjectSelection.h"
#include "LBPLibUtilities.h"

static IRhinoPropertiesPanelPageEventArgs* Args(const IRhinoPropertiesPanelPage* page)
{
    return IRhinoPropertiesPanelPageEventArgs::FromPage(page);
}

int LBPRhCountMeshableObjects(const IRhinoPropertiesPanelPage* pDialogPage)
{
  IRhinoPropertiesPanelPageEventArgs* args = IRhinoPropertiesPanelPageEventArgs::FromPage(pDialogPage);
  if (args == nullptr)
    return 0;
	const int iCount = args->ObjectCount();
	int iMeshableCount = 0;

	for (int i = 0; i < iCount; i++)
	{
		const CRhinoObject* pObject = args->ObjectAt(i);
		if(pObject && pObject->IsMeshable(ON::render_mesh))
		{
			iMeshableCount++;
		}
	}

	return iMeshableCount;
}

bool LBPRhContainsMeshableObjects(const IRhinoPropertiesPanelPage* pDialogPage)
{
  IRhinoPropertiesPanelPageEventArgs* args = IRhinoPropertiesPanelPageEventArgs::FromPage(pDialogPage);
  if (args == nullptr)
    return 0;
  
  const int iCount = args->ObjectCount();

	for (int i = 0; i < iCount; i++)
	{
		const CRhinoObject* pObject = args->ObjectAt(i);
		if(pObject && pObject->IsMeshable(ON::render_mesh))
		{
			return true;
		}
	}

	return false;
}

CLBPRhObjectSelection::CLBPRhObjectSelection(const CRhinoDoc& doc)
    : m_runtime_document_serial_number(doc.RuntimeSerialNumber())
{
	m_iIterationIndex = -1;
}

CLBPRhObjectSelection::CLBPRhObjectSelection(const IRhinoPropertiesPanelPage* pDialogPage, bool bMeshableOnly)
  : m_aObjects(Args(pDialogPage) ? Args(pDialogPage)->ObjectCount() : 1),
    m_runtime_document_serial_number(Args(pDialogPage) ? Args(pDialogPage)->DocumentRuntimeSerialNumber() : CRhinoDoc::NullRuntimeSerialNumber)
{
	m_iIterationIndex = -1;

  IRhinoPropertiesPanelPageEventArgs* args = IRhinoPropertiesPanelPageEventArgs::FromPage(pDialogPage);
	const int iCount = args == nullptr ? 0 : args->ObjectCount();

	m_aObjects.SetCapacity(iCount);

	for (int i = 0; i < iCount; i++)
	{
		const CRhinoObject* pObject = args->ObjectAt(i);
		if(pObject)
		{
			if(!bMeshableOnly || pObject->IsMeshable(ON::render_mesh))
			{
				m_aObjects.Append(pObject->Attributes().m_uuid);
			}
		}
	}
}

CLBPRhObjectSelection::CLBPRhObjectSelection(const CRhinoDoc& doc, bool bMeshableOnly)
  : m_aObjects(CountSelectedObjects(doc)),
	m_runtime_document_serial_number(doc.RuntimeSerialNumber())
{
	CRhinoObjectIterator it(doc, CRhinoObjectIterator::normal_or_locked_objects, CRhinoObjectIterator::active_and_reference_objects);
	const CRhinoObject* pObject = it.First();

	while(pObject != NULL)
	{
		if(pObject->IsSelected() && (!bMeshableOnly || pObject->IsMeshable(ON::render_mesh)))
		{
			m_aObjects.Append(pObject->Attributes().m_uuid);
		}

		pObject = it.Next();
	}
}



CLBPRhObjectSelection::CLBPRhObjectSelection(const IRhinoPropertiesPanelPage* pDialogPage, ON::object_type ot)
  : m_aObjects(Args(pDialogPage) ? Args(pDialogPage)->ObjectCount() : 1),
    m_runtime_document_serial_number(Args(pDialogPage) ? Args(pDialogPage)->DocumentRuntimeSerialNumber() : CRhinoDoc::NullRuntimeSerialNumber)
{
	m_iIterationIndex = -1;

    auto* args = Args(pDialogPage);
	const int iCount = args == nullptr ? 0 : args->ObjectCount();

	m_aObjects.SetCapacity(iCount);

	for (int i = 0; i < iCount; i++)
	{
		const CRhinoObject* pObject = args->ObjectAt(i);
		if(pObject)
		{
			if(ot & pObject->ObjectType())
			{
				m_aObjects.Append(pObject->Attributes().m_uuid);
			}
		}
	}
}

CLBPRhObjectSelection::CLBPRhObjectSelection(const CLBPRhObjectSelection& sel)
: m_aObjects(sel.Count()),
  m_runtime_document_serial_number(sel.m_runtime_document_serial_number)
{
	m_iIterationIndex = -1;

	const int iCount = sel.m_aObjects.Count();

	m_aObjects.SetCapacity(iCount);

	for (int i = 0; i < iCount; i++)
	{
		m_aObjects.Append(sel.m_aObjects[i]);
	}
}

CLBPRhObjectSelection::CLBPRhObjectSelection(const CRhinoDoc& doc, const ON_SimpleArray<UUID>& aObjects)
  : m_aObjects(aObjects.Count()),
	m_runtime_document_serial_number(doc.RuntimeSerialNumber())

{
	m_iIterationIndex = -1;

	const int iCount = aObjects.Count();
	m_aObjects.SetCapacity(iCount);

	for (int i=0; i<iCount; i++)
	{
		m_aObjects.Append(aObjects[i]);
	}
}

int CLBPRhObjectSelection::Count(void) const
{
	return m_aObjects.Count();
}

const CRhinoObject * CLBPRhObjectSelection::First(void) const
{
	m_iIterationIndex = -1;

	return Next();
}

const CRhinoObject * CLBPRhObjectSelection::Next(void) const
{
	if (++m_iIterationIndex >= m_aObjects.Count())
		return NULL;

	const CRhinoDoc* pDoc = LBPRhDocFromRuntimeSerialNumber(m_runtime_document_serial_number);
	if (pDoc != NULL)
	{
		return pDoc->LookupObject(m_aObjects[m_iIterationIndex]);
	}
	return NULL;
}


DWORD CLBPRhObjectSelection::CRC(void) const
{
	//May not work if the objects are in a different order
	const int iCount = m_aObjects.Count();
	if(0==iCount) return 0;

	UUID* pBuffer = new UUID[iCount];
    
    int i;

	for (i = 0; i < iCount; i++)
	{
		const UUID uuid = m_aObjects[i];
		memcpy((void*)(&pBuffer[i]), &uuid, sizeof(m_aObjects[i]));
	}

	DWORD dw = 123456;

	dw = ON_CRC32(dw, sizeof(m_aObjects[i]) * iCount, (void*)pBuffer);

	delete [] pBuffer;
	return dw;
}

void CLBPRhObjectSelection::BoundingBox(ON_BoundingBox& boxOut) const
{
	for (int i = 0; i < m_aObjects.Count(); i++)
	{
        auto* pDoc = CRhinoDoc::FromRuntimeSerialNumber(DocumentSerialNumber());
        if (pDoc)
        {
            const CRhinoObject* pObject = pDoc->LookupObject(m_aObjects[i]);
            if (NULL != pObject)
            {
                if (0 == i)
                {
                    boxOut = pObject->BoundingBox();
                }
                else
                {
                    boxOut.Union(pObject->BoundingBox());
                }
            }
        }
	}
}

bool CLBPRhObjectSelection::operator==(const CLBPRhObjectSelection& selection)
{
	return CRC() == selection.CRC();
}







CLBPRhObjectList::CLBPRhObjectList(const CRhinoDoc& doc)
	: m_runtime_document_serial_number(doc.RuntimeSerialNumber())
{
}

int CLBPRhObjectList::Count(void) const
{
	ASSERT(m_aObjects.Count() == (int)m_mapObjects.Count());
	return m_aObjects.Count();
}

void CLBPRhObjectList::AddObjects(const CLBPRhObjectSelection& sel)
{
	const CRhinoObject* pObject = sel.First();
	while (NULL != pObject)
	{
		Add(pObject);

		pObject = sel.Next();
	}
}

void CLBPRhObjectList::Add(const CRhinoObject* pObject)
{
	const UUID& uuid = pObject->Attributes().m_uuid;

	int i;
	if (m_mapObjects.Lookup(uuid, i))
		return;

	m_aObjects.Append(uuid);
	m_mapObjects.SetAt(uuid, 0);
}

void CLBPRhObjectList::Clear(void)
{
	m_aObjects.Destroy();
	m_mapObjects.Destroy();
}



const CRhinoObject* CLBPRhObjectList::Object(int iIndex) const
{
	if ((iIndex < 0) || (iIndex >= m_aObjects.Count()))
		return NULL;

	const CRhinoDoc* pDoc = LBPRhDocFromRuntimeSerialNumber(m_runtime_document_serial_number);
	if (pDoc != NULL)
	{
		return pDoc->LookupObject(m_aObjects[iIndex]);
	}
	return NULL;
}


#endif
