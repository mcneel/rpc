#pragma once
#ifndef LBPRHLIB
#error RHLIB header included in non-Rhino compile
#endif

#ifdef _DEBUG
#include "LBPRhException.h"
#endif

#include "LBPLibUtilities.h"
#include "LBP_UUID.h"
#include "ILBPRhWrapper.h"
#include "ON_SimpleMap.h"

#if defined _MSC_VER && _MSC_VER < 1400
#define RHWRAP_VER 3
#error "This should no longer happen"
#else
#define RHWRAP_VER 4
#endif


#if RHWRAP_VER == 4
	enum lbprh_wrapper_eType			{	lwt_geometry,/*lwt_material,lwt_light,*/lwt_attributes,lwt_object,	};
#endif
#if RHWRAP_VER == 3
	enum lbprh_wrapper_eType			{	lwt_geometry,/*lwt_material,lwt_light,*/ };
#endif

template<class T> class CLBPRhObjectWrapper : public ILBPRhWrapper<T>
{
public:

	CLBPRhObjectWrapper(const CRhinoDoc& doc, const UUID& uuidObject, lbprh_wrapper_eType type = lwt_geometry);
	CLBPRhObjectWrapper(const CRhinoObject * pRhinoObject, lbprh_wrapper_eType type = lwt_geometry);

//BEGIN RHINO 4+ SPECIFIC CODE
#if RHWRAP_VER == 4
	CLBPRhObjectWrapper(const CRhinoDoc& doc, const CRhinoObjectAttributes* pAttributes);
#endif
//END RHINO 4+ SPECIFIC CODE

	virtual ~CLBPRhObjectWrapper();

	virtual eLBPRhWrapperType WrapperType(void) const { return lbprh_wt_object; }

	//type = attributes,	returns CRhinoObjectAttributes*
	//type = geometry,		returns CRhinoObject*
	//type = object,		returns CRhinoObject*
	inline  const ON_Object* OriginalObject() const	{return m_pObject;}
	inline  UUID			 OriginalUuid() const { return m_uuid; }

	//Cause the wrapper to put the object into a writable state
	inline	void Modify()							{ UserDataToModify(); }		


	//DEPRECATED - use "Suspend" instead
	bool TransformObject(const ON_Xform& xform);
		//Forces the object to be committed and replaced immediately - ie, bypasses the lazy update
		//feature.
		//Implemented using CRhinoDoc::TransformObject

	virtual CRhinoObject* ModifyObject();	
									//Force the wrapper to modify the actual object
									//This is useful when the type does not cause a CRhinoObject to be
									//cloned - eg. Attributes.  After this, the object will be
									//closed with a full "ReplaceObject" call instead of a ModifyObjectAttributes
									//Note: Multiple calls to this function do nothing, so you can use the 
									//function to get the modified object over and over

	CRhinoObject* ModifiedObject() const;
									//Call this one to get the ready-modified CRhinoObject clone
									//Will return NULL if this clone is not a CRhinoObject or there are no modifications

	//Helper functions to get around the const_casting problem
	//inherent in modifying duplicated objects
	//Note "ModifiedAttributes" does not require ModifyObject to be called first.
	//"ModifiedGeometry" does require a call to ModifyObject first.
	CRhinoObjectAttributes* ModifiedAttributes() const;							
	ON_Geometry*			ModifiedGeometry() const;

	//Nasty access to reference count for people using the back-door
	//modification capture method
	bool LastReference(void) const { return 1==m_dwRefCount; }
									
	inline	bool ModificationsPending() const			{ return NULL != m_pClone;}
									//Determines whether there is a current clone 
									//(ie - Modify() or UserDataToModify() has been called)

	//Note - the following 2 functions have no effect if the original object was not in the database.
	//In that case, modifications are made directly on the object.
	virtual void	UndoModifications();	//Revert the wrapper back to the read-state, and undo all modifications
	
	//Note -the function cannot be correctly virtualized because it is called from the destructor
	/*virtual*/bool	CommitChanges();		//Call to write changes to the document before destruction
											//leaves the wrapper in a re-usabled state
public:
	class Suspend
	{
	public:
		Suspend(CLBPRhObjectWrapper<T>& wrapper) 
			: m_wrapper(wrapper)
		{ m_wrapper.ReleaseObject(); }

		~Suspend() {m_wrapper.RecaptureObject();}

		const CRhinoObject* Object() const	{ return m_wrapper.OriginalRhinoObject(); }
		UUID Uuid() const					{ return m_wrapper.OriginalUuid(); }
	private:
		CLBPRhObjectWrapper<T>& m_wrapper;
	};
	

public:
	virtual bool	RemoveData();
	virtual bool	UserDataPresent() const;

public:
	//Copying data
	//Ensure that a wrapper is not constructed for the target
	//(debug builds will prohibit this)
	bool	CopyToObject(const CRhinoObject* pObject) const;
	bool	CopyToObject(const UUID& uuidObject) const;

	//Ensure that a wrapper is not constructed for the source
	//(debug builds will prohibit this)
	bool	CopyFromObject(const CRhinoObject* pObject);
	bool	CopyFromObject(const UUID& uuidObject);

	static const T* DefaultData() { return &g_ud; }

protected:

	//Implement your setters and getters based on these functions
	virtual const T*	UserData(eLBPRhWrapper_DefaultUse du = lbprh_dont_use_defaults) const;
	virtual T*			UserDataToModify(void);

	//Override this to get modification events
	virtual void OnModification() const {};

	const CRhinoObject*				OriginalRhinoObject() const;

	CRhinoDoc* Document() const { return LBPRhDocFromRuntimeSerialNumber(m_runtime_doc_serial_number); }

private:
	//IMPLEMENTATION DETAILS ONLY
	//Null attaches blank user data, otherwise the data is copied
	T*			AttachData(const T* pExistingData = NULL);	
	
	void		CommonCtor(const ON_Object* pObject, lbprh_wrapper_eType type);
	void		Clone() const;
	ON_Object*	GetTarget() const;
	bool		OriginalObjectIsCRhinoObject() const;

	//const CRhinoObject*				OriginalRhinoObject() const;
	const CRhinoObjectAttributes*	OriginalAttributes() const;
	
	void							RenewOriginalObject(const UUID& uuid);

	//Make copying illegal
	CLBPRhObjectWrapper(const CLBPRhObjectWrapper& src);
	const CLBPRhObjectWrapper& operator=(const CLBPRhObjectWrapper& src);

private:
	const	ON_Object*				m_pObject;
	mutable	ON_Object*				m_pClone;
	lbprh_wrapper_eType				m_type;
	bool							m_bObjectInDatabase;
	UUID							m_uuid;
	const unsigned int			m_runtime_doc_serial_number;

private:
	//For defaults implementation
	static T								g_ud;
	static bool								g_bDefaultsSet;

//Reference counted map implementation
public:
	DWORD	AddRef()			{ return ++m_dwRefCount; }
	DWORD	ReleaseRef()		{ return --m_dwRefCount; }

private:
	friend class Suspend;
	void ReleaseObject();
	void RecaptureObject();

private:
	DWORD	m_dwRefCount;

};



template<class T> T CLBPRhObjectWrapper<T>::g_ud;
template<class T> bool CLBPRhObjectWrapper<T>::g_bDefaultsSet = false;


//////////////////////////////////////////////////////////////////////////////////
//	Template implementation below












//////////////////////////////////////////////////////////////////////////////////
//
//	Template implementation
//
template <class T> 
CLBPRhObjectWrapper<T>::CLBPRhObjectWrapper(const CRhinoDoc& doc, const UUID& uuidObject, lbprh_wrapper_eType type)
	: m_runtime_doc_serial_number(LBPRhDocRuntimeSerialNumber(doc))
{
	const CRhinoObject* pObject = doc.LookupObject(uuidObject);
	ASSERT(NULL != pObject);

	m_uuid = uuidObject;

//BEGIN RHINO 4+ SPECIFIC CODE
#if RHWRAP_VER == 4
	if(lwt_attributes == type)
		CommonCtor(&pObject->Attributes(), type);
	else
#endif
//END RHINO 4+ SPECIFIC CODE
		CommonCtor(pObject, type);
}

template <class T> 
CLBPRhObjectWrapper<T>::CLBPRhObjectWrapper(const CRhinoObject* pObject, lbprh_wrapper_eType type)
	: m_runtime_doc_serial_number(LBPRhObjectDocSerialNumber(pObject))
{
//BEGIN RHINO 4+ SPECIFIC CODE
#if RHWRAP_VER == 4
	if(lwt_attributes == type)
		CommonCtor(&pObject->Attributes(), type);
	else
#endif
//END RHINO 4+ SPECIFIC CODE
		CommonCtor(pObject, type);

	//This object may not be in the database.
	//Test to see if it is:
	
	//Done here because it's more efficient - otherwise we have to do two lookups
	//in the UUID constructor unnecessarily
//	if(!IsObjectInDatabase(pObject))
	if (NULL == pObject->Document())
	{
		m_bObjectInDatabase = false;
	}

	m_uuid = pObject->Attributes().m_uuid;
}

//BEGIN RHINO 4+ SPECIFIC CODE
#if RHWRAP_VER == 4
template <class T> 
CLBPRhObjectWrapper<T>::CLBPRhObjectWrapper(const CRhinoDoc& doc, const CRhinoObjectAttributes* pAttributes)
: m_runtime_doc_serial_number(LBPRhDocRuntimeSerialNumber(doc))
{
	CommonCtor(pAttributes, lwt_attributes);
	m_uuid = pAttributes->m_uuid;
}
#endif
//END RHINO 4+ SPECIFIC CODE

template <class T> 
void CLBPRhObjectWrapper<T>::CommonCtor(const ON_Object* pObject, lbprh_wrapper_eType type)
{
	m_pClone = NULL;

	if(!g_bDefaultsSet)
	{
		g_ud.SetToDefaultsImpl(lbprh_wt_object);
		g_bDefaultsSet = true;
	}

	m_type = type;
	m_pObject = pObject;
	m_bObjectInDatabase = true;
	m_dwRefCount = 0;
}

template <class T> 
CLBPRhObjectWrapper<T>::~CLBPRhObjectWrapper()
{
	CommitChanges();
}


template <class T> 
void CLBPRhObjectWrapper<T>::UndoModifications()
{
	if(m_pClone)
	{
		if(m_bObjectInDatabase)
		{
			delete m_pClone;
		}
		m_pClone = NULL;
	}
}

template <class T>
void CLBPRhObjectWrapper<T>::Clone() const
{
	ASSERT(NULL == m_pClone);
	if(m_pClone) return;

	switch(m_type)
	{
	case lwt_geometry:
		if(m_bObjectInDatabase)
			m_pClone = m_pObject->Duplicate();
		else
			//Const cast is OK because we know this object can be modified
			m_pClone = const_cast<ON_Object*>(m_pObject);
		break;

//BEGIN RHINO 4+ SPECIFIC CODE
#if RHWRAP_VER == 4
	case lwt_object:
		if(m_bObjectInDatabase)
			m_pClone = m_pObject->Duplicate();
		else
			//Const cast is OK because we know this object can be modified
			m_pClone = const_cast<ON_Object*>(m_pObject);
		break;
	case lwt_attributes:
		if(m_bObjectInDatabase)
			m_pClone = new CRhinoObjectAttributes(*OriginalAttributes());
		else
			//Const cast is OK because we know this object can be modified
			m_pClone = const_cast<ON_Object*>(m_pObject);
		break;
#endif
//END RHINO 4+ SPECIFIC CODE

	default:
		//TODO: implement other types
		break;
	}

}

template <class T> 
ON_Object* CLBPRhObjectWrapper<T>::GetTarget() const
{
	switch(m_type)
	{
	case lwt_geometry:
		if(m_pClone)
			return const_cast<ON_Geometry*>(static_cast<CRhinoObject*>(m_pClone)->Geometry());
		else
			return const_cast<ON_Geometry*>(OriginalRhinoObject()->Geometry());

//BEGIN RHINO 4+ SPECIFIC CODE
#if RHWRAP_VER == 4
	case lwt_attributes:
		if(m_pClone)
			return CRhinoObjectAttributes::Cast(m_pClone) ? 
							m_pClone 
							: 
							const_cast<CRhinoObjectAttributes*>(&static_cast<CRhinoObject*>(m_pClone)->Attributes());
		else
			//The original object is "attributes" in all cases
			return const_cast<ON_Object*>(m_pObject);
	case lwt_object:
		if(m_pClone)
			return m_pClone;
		else
			return const_cast<ON_Object*>(m_pObject);
#endif
//END RHINO 4+ SPECIFIC CODE

	default:
		//TODO: Implement other types
		return NULL;
	}

}

template <class T> 
bool CLBPRhObjectWrapper<T>::RemoveData()
{
	if(!UserDataPresent()) return true;

	bool bWasCloned = false;
	if (!m_pClone)
	{
		Clone();
		bWasCloned = true;
	}

	if (!m_pClone) return NULL;
	//T* pUserData = NULL;

	ON_Object* pTarget = GetTarget();//m_pClone;
	if(pTarget)
	{
		BOOL bDetached = FALSE;

		ON_UserData* pUD = UserDataToModify();
		if (pUD != NULL)
		{
			bDetached = pTarget->DetachUserData(pUD);
			if (bDetached)
			{
				//Notify clients that a modification was made and that the UserData pointer has changed
				OnModification();
			}
			delete pUD;
		}
		
		return bDetached ? true : false;
	}

	if(bWasCloned)
	{
		if(m_bObjectInDatabase)
		{
			delete m_pClone;
		}
		m_pClone = NULL;
	}

	return false;
}

template <class T> 
bool CLBPRhObjectWrapper<T>::UserDataPresent() const
{
	return (NULL == UserData(lbprh_dont_use_defaults) ? false : true);
}

template <class T> 
bool CLBPRhObjectWrapper<T>::CopyToObject(const UUID& uuid) const
{
	CRhinoDoc* pDoc = Document();
	if (pDoc)
	{
		return CopyToObject(pDoc->LookupObject(uuid));
	}
	return false;
}

template <class T> 
bool CLBPRhObjectWrapper<T>::CopyToObject(const CRhinoObject* pObject) const
{
	if(!UserDataPresent()) return false;

	CLBPRhObjectWrapper<T> target(pObject, m_type);
	return NULL == target.AttachData(UserData()) ? false : true;
}

template <class T> 
bool CLBPRhObjectWrapper<T>::CopyFromObject(const UUID& uuid)
{
	const CRhinoDoc* pDoc = Document();
	if (pDoc)
	{
		return CopyFromObject(pDoc->LookupObject(uuid));
	}
	return false;
}

template <class T> 
bool CLBPRhObjectWrapper<T>::CopyFromObject(const CRhinoObject* pObject)
{
	CLBPRhObjectWrapper<T> source(pObject, m_type);
	if(!source.UserDataPresent()) return false;

	return NULL == AttachData(source.UserData()) ? false : true;
}

template <class T> 
const T* CLBPRhObjectWrapper<T>::UserData(eLBPRhWrapper_DefaultUse du) const
{
	ASSERT(NULL != m_pObject);

	const ON_Object* pTarget = GetTarget();
	if(!pTarget) return NULL;

	const T* pUD = T::Cast(pTarget->GetUserData(g_ud.m_userdata_uuid));
	if(pUD) return pUD;

	if(lbprh_use_defaults == du)
		return &g_ud;

	return NULL;
}

template <class T> 
CRhinoObject* CLBPRhObjectWrapper<T>::ModifiedObject() const
{
	if(CRhinoObject::Cast(m_pClone)) return static_cast<CRhinoObject*>(m_pClone);
	return NULL;
}

template <class T> 
CRhinoObjectAttributes* CLBPRhObjectWrapper<T>::ModifiedAttributes() const
{
	if(CRhinoObjectAttributes::Cast(m_pClone)) 
		return static_cast<CRhinoObjectAttributes*>(m_pClone);

	if(CRhinoObject::Cast(m_pClone)) 
		return const_cast<CRhinoObjectAttributes*>(&static_cast<CRhinoObject*>(m_pClone)->Attributes());

	return NULL;
}

template <class T> 
ON_Geometry* CLBPRhObjectWrapper<T>::ModifiedGeometry() const
{
	if(CRhinoObject::Cast(m_pClone)) 
		return const_cast<ON_Geometry*>(static_cast<CRhinoObject*>(m_pClone)->Geometry());
	return NULL;
}

template <class T> 
CRhinoObject* CLBPRhObjectWrapper<T>::ModifyObject(void)
{
	//If we're not already in a cloned state, get cloned.
	if(!ModificationsPending())
	{
		Modify();
	}

	//We may already have a cloned full object.  If so, return that.
	if(CRhinoObject::Cast(m_pClone)) return static_cast<CRhinoObject*>(m_pClone);


//BEGIN RHINO 4+ SPECIFIC CODE
#if RHWRAP_VER == 4
	//We currently have a modification which didn't result in a cloned object
	//Should never get here - but this is a placeholder for stuff which really can't happen
	//line for viewport, material and layer wrappers.
	if(m_type != lwt_attributes) return NULL;
#ifdef _DEBUG
	if(NULL == CRhinoObjectAttributes::Cast(m_pClone))
	{
		CLBPRhRunTimeException re(L"Wrong original Object type");
		throw re;
	}
#endif

	const CRhinoDoc* pDoc = Document();
	const CRhinoObject* pOriginalObject = pDoc ? pDoc->LookupObject(OriginalUuid()) : NULL;
	if(!pOriginalObject) return NULL;

	CRhinoObjectAttributes* pAttributesClone = static_cast<CRhinoObjectAttributes*>(m_pClone);
	CRhinoObject* pObject = pOriginalObject->Duplicate();
	if(!pObject) return NULL;

	//Copy the changes across to the newly duplicated object
	const_cast<CRhinoObjectAttributes&>(pObject->Attributes()) = *pAttributesClone;

	//Delete the old attributes
	delete m_pClone;

	//Set the new clone to an actual object.
	m_pClone = pObject;
	return pObject;
#else
	return NULL;
#endif
}


template <class T> 
T* CLBPRhObjectWrapper<T>::UserDataToModify(void)
{
	if (!m_pObject) return NULL;

	T* pExistingData = NULL;

	if(m_pClone)
	{
		//We know the data is coming from the clone, so we can
		//safely cast away the const-ness to enable us to reuse 
		//the UserData function
		pExistingData = const_cast<T*>(UserData());
		if(pExistingData)
		{
			return pExistingData;
		}
	}

	return AttachData();
}

template <class T> 
void CLBPRhObjectWrapper<T>::RenewOriginalObject(const UUID& uuid)
{
	const CRhinoDoc* pDoc = Document();
	if (NULL == pDoc)
	{
		m_pObject = NULL;
		return;
	}

	//THe underlying object has changed - renew the pointer
	if(OriginalObjectIsCRhinoObject())
	{
		m_pObject = pDoc->LookupObject(uuid);
	}
	else
	{
#if RHWRAP_VER == 4
		//This happens in the case of attributes
		m_pObject = &pDoc->LookupObject(uuid)->Attributes();
#else
		m_pObject = NULL;
#endif
	}
}

template <class T> 
void CLBPRhObjectWrapper<T>::ReleaseObject()
{
	CommitChanges();
}

template <class T> 
void CLBPRhObjectWrapper<T>::RecaptureObject()
{
	RenewOriginalObject(OriginalUuid());
}

template <class T> 
bool CLBPRhObjectWrapper<T>::TransformObject(const ON_Xform& xform)
{
	CRhinoDoc* pDoc = Document();
	LBPRhAssert(NULL != pDoc);
	if(NULL == pDoc) 
		return false;

	//Suspend ownership of the object
	Suspend o(*this);

	const UUID uuid = o.Uuid();
    
    CRhinoObjRef ref(uuid);

	CRhinoObject* pNewObject = pDoc->TransformObject(ref, xform);
#if RHWRAP_VER == 4
	LBPRhAssert(NULL != pNewObject);
#endif
	if(NULL == pNewObject) 
		return false;

	return true;
}



template <class T> 
bool CLBPRhObjectWrapper<T>::CommitChanges()
{
	if (NULL == m_pClone)
		return false;

	if (!m_bObjectInDatabase)
	{
		//In this case, the changes are already committed
		m_pClone = NULL;
		return true;
	}

	CRhinoDoc* pDoc = Document();
	if(NULL == pDoc) 
		return false;

	//Attributes does not change the object pointer, so this isn't needed
	//ON_UUID uuidOriginalId = OriginalUuid();

	bool bSucceeded = false;
	switch(m_type)
	{
	case lwt_geometry:
		bSucceeded = pDoc->ReplaceObject(CRhinoObjRef(OriginalRhinoObject()), 
							             static_cast<CRhinoObject*>(m_pClone)
										 );
		break;

//BEGIN RHINO 4+ SPECIFIC CODE
#if RHWRAP_VER == 4
	case lwt_object:
		bSucceeded = pDoc->ReplaceObject(CRhinoObjRef(OriginalRhinoObject()), 
							             static_cast<CRhinoObject*>(m_pClone)
										 );
		break;
	case lwt_attributes:
		{
			//Attribute user data requires that the copy count is incremented before
			//ModifyObjectAttributes is called
			T* pUD = UserDataPresent() ? UserDataToModify() : NULL;
			if(NULL != pUD)
			{
				pUD->m_userdata_copycount++;
			}

			if(CRhinoObjectAttributes::Cast(m_pClone))
			{
				//The clone is an attributes object
				ON_UUID uuidObject = OriginalUuid();

				const CRhinoObject* pObject = pDoc ? pDoc->LookupObject(uuidObject) : NULL;
				if(pObject)
				{
					bSucceeded = pDoc->ModifyObjectAttributes(	CRhinoObjRef(pObject), 
																*(static_cast<CRhinoObjectAttributes*>(m_pClone))
																);

					//ModifyObjectAttributes does not involve a change of ownership on the close, unlike 
					//the geometry or object attach sites.  So at this point, we have to delete the clone if
					//successful to avoid a memory leak.
					if (bSucceeded)
					{
						delete m_pClone;
						m_pClone = NULL;
					}
				}
			}
			else
			{
				//Copy the attributes
				CRhinoObjectAttributes attr = static_cast<CRhinoObject*>(m_pClone)->Attributes();
				//First replace the object
				bSucceeded = pDoc->ReplaceObject(CRhinoObjRef(pDoc->RuntimeSerialNumber(), OriginalUuid()),
												 static_cast<CRhinoObject*>(m_pClone)
												 );

				//Then modify the attributes
				if(bSucceeded)
				{
					bSucceeded = pDoc->ModifyObjectAttributes(CRhinoObjRef(pDoc->RuntimeSerialNumber(), OriginalUuid()), attr);
				}
			}
		}
		break;
#endif
//END RHINO 4+ SPECIFIC CODE

	default:
		//TODO: Implement other types
		break;
	}

	if(!bSucceeded)
	{
		//The process did not succeed - we basically have to get rid of the modifications
		if(m_bObjectInDatabase)
		{
			delete m_pClone;
		}
	} 
	else
	{
		//THe underlying object has changed - renew the pointer
		RenewOriginalObject(OriginalUuid());
	}

	m_pClone = NULL;
	return bSucceeded;
}

template <class T> 
bool CLBPRhObjectWrapper<T>::OriginalObjectIsCRhinoObject() const
{
//BEGIN RHINO 4+ SPECIFIC CODE
#if RHWRAP_VER == 4
	return m_type != lwt_attributes;
#else
	return true;
#endif
//END RHINO 4+ SPECIFIC CODE
}

template <class T> 
T* CLBPRhObjectWrapper<T>::AttachData(const T* pData)	
{
	bool bWasCloned = false;
	if (!m_pClone)
	{
		Clone();
		bWasCloned = true;
	}

	if (!m_pClone) return NULL;

	// We need non const user data
	ON_Object* pTarget = GetTarget();//
	if(NULL == pTarget) return NULL;

	T* pCloneUserData = T::Cast(pTarget->GetUserData(g_ud.m_userdata_uuid));
	if(pCloneUserData)
	{
		if(pData)
		{
			*pCloneUserData = *pData;
		}

		if(pData || bWasCloned)
		{
			OnModification();
		}

		return pCloneUserData;
	}

	//There's no data on the original source - we have to attach new data.
	//Either we're cloning for modications
	//or we're attaching new data from another source

	//We are now sure we can add the user data.
	pCloneUserData = new T;
	if(pData)
	{
		*pCloneUserData = *pData;
	}

	if(!pTarget->AttachUserData(pCloneUserData))
	{
		delete pCloneUserData;
		pCloneUserData = NULL;

		//There is a critical bug - we should never reach this point
		ASSERT(false);
	}

	if(NULL == pData)
	{
		pCloneUserData->SetToDefaultsImpl(lbprh_wt_object);
	}

	OnModification();
	return pCloneUserData;
}

template <class T> 
const CRhinoObject*	CLBPRhObjectWrapper<T>::OriginalRhinoObject() const
{
	if(OriginalObjectIsCRhinoObject())
	{
		return static_cast<const CRhinoObject*>(m_pObject);
	}
	else
	{
		const CRhinoDoc* pDoc = Document();

		return pDoc ? pDoc->LookupObject(OriginalUuid()) : NULL;
	}
}


template <class T> 
const CRhinoObjectAttributes* CLBPRhObjectWrapper<T>::OriginalAttributes() const
{
#if RHWRAP_VER == 4
	LBPRhAssert(!OriginalObjectIsCRhinoObject());
	return static_cast<const CRhinoObjectAttributes*>(m_pObject);
#else
	return NULL;
#endif
}






template<class TWrapper> class ILBPRhWrapperMap
{
public:
	virtual ~ILBPRhWrapperMap() {};

	//AddWrapperRef passes ownership to the map implementation
	 virtual bool		AddWrapper(TWrapper* wrapper) = 0;

	 //This should delete the wrapper from the map if the reference count is 0
	 virtual bool		RemoveWrapper(const UUID& uuid) = 0;

	 //Returns the one wrapper with the UUID in question
	 virtual TWrapper*	Wrapper(const UUID& uuid) const = 0;
};


template<class TWrapper> class CLBPRhWrapperMap : public ILBPRhWrapperMap<TWrapper>
{
public:

	virtual bool AddWrapper(TWrapper* wrapper)
	{
		m_map.SetAt(wrapper->OriginalUuid(), wrapper);
		return true;
	}

	virtual bool RemoveWrapper(const UUID& uuid)
	{
		delete Wrapper(uuid);
		return m_map.Remove(uuid) ? true : false;
	}

	virtual TWrapper* Wrapper(const UUID& uuid) const
	{
		TWrapper* p = NULL;
		if (!m_map.Lookup(uuid, p))
			return NULL;
		return p;
	}

private:
	ON_SimpleUuidMap<TWrapper*> m_map;

};


template<class TWrapper> class CLBPRhObjectWrapperRef
{
public:
	CLBPRhObjectWrapperRef(ILBPRhWrapperMap<TWrapper>& map, 
		                   const CRhinoDoc& doc,
						   const UUID& uuidObject, 
						   lbprh_wrapper_eType type = lwt_geometry);

	CLBPRhObjectWrapperRef(ILBPRhWrapperMap<TWrapper>& map, 
						   const CRhinoObject * pRhinoObject,
						   lbprh_wrapper_eType type = lwt_geometry);

	CLBPRhObjectWrapperRef(ILBPRhWrapperMap<TWrapper>& map, 
		                   const CRhinoDoc& doc,
						   const CRhinoObjectAttributes* pAttributes);

	~CLBPRhObjectWrapperRef();

public:

	TWrapper& Wrapper();
	TWrapper* operator->() { return &Wrapper(); }

private:
	void CommonCtor(const UUID& uuid, ILBPRhWrapperMap<TWrapper>* pMap);

	UUID m_uuid;
	ILBPRhWrapperMap<TWrapper>* m_pMap;
	TWrapper*	m_pWrapper;
	CCriticalSection m_cs;

};

template <class TWrapper> 
TWrapper& CLBPRhObjectWrapperRef<TWrapper>::Wrapper()
{
	LBPRhAssert(NULL != m_pMap);
	LBPRhAssert(NULL != m_pMap->Wrapper(m_uuid));

	return *m_pWrapper;
}

template <class TWrapper> 
void CLBPRhObjectWrapperRef<TWrapper>::CommonCtor(const UUID& uuid, ILBPRhWrapperMap<TWrapper>* pMap)
{
	m_pMap = pMap;

	m_uuid = uuid;
	LBPRhAssert(NULL != m_pWrapper);
	
	m_pWrapper->AddRef();
}


template <class TWrapper> 
CLBPRhObjectWrapperRef<TWrapper>::CLBPRhObjectWrapperRef(ILBPRhWrapperMap<TWrapper>& map,
	                                                     const CRhinoDoc& doc,
														 const UUID& uuidObject, 
														 lbprh_wrapper_eType type)
{
	m_pWrapper = map.Wrapper(uuidObject);

	if(NULL == m_pWrapper)
	{
		m_pWrapper = new TWrapper(uuidObject, type);
		bool bRet = map.AddWrapper(doc, m_pWrapper);
		LBPRhAssert(bRet);
	}

	CommonCtor(uuidObject, &map);

}

template <class TWrapper> 
CLBPRhObjectWrapperRef<TWrapper>::CLBPRhObjectWrapperRef(ILBPRhWrapperMap<TWrapper>& map, 
														 const CRhinoObject * pRhinoObject, 
														 lbprh_wrapper_eType type)
{
	LBPRhAssert(NULL != pRhinoObject);

	UUID uuid = pRhinoObject->Attributes().m_uuid;

	m_pWrapper = map.Wrapper(uuid);

	if(NULL == m_pWrapper)
	{
		m_pWrapper = new TWrapper(pRhinoObject, type);
		bool bRet = map.AddWrapper(m_pWrapper);
		LBPRhAssert(bRet);
	}

	CommonCtor(uuid, &map);
}


template <class TWrapper> 
CLBPRhObjectWrapperRef<TWrapper>::CLBPRhObjectWrapperRef(ILBPRhWrapperMap<TWrapper>& map,
	                                           const CRhinoDoc& doc,
											   const CRhinoObjectAttributes* pAttributes)
{
	m_cs.Lock();
	LBPRhAssert(NULL != pAttributes);

	UUID uuid = pAttributes->m_uuid;

	m_pWrapper = map.Wrapper(uuid);

	if(NULL == m_pWrapper)
	{
		m_pWrapper = new TWrapper(doc, pAttributes);
		bool bRet = map.AddWrapper(m_pWrapper);
		LBPRhAssert(bRet);
	}

	CommonCtor(uuid, &map);
	m_cs.Unlock();
}


template <class TWrapper> 
CLBPRhObjectWrapperRef<TWrapper>::~CLBPRhObjectWrapperRef()
{
	m_cs.Lock();
	LBPRhAssert(NULL != m_pMap);
	LBPRhAssert(NULL != m_pWrapper);

	if(m_pWrapper != NULL)
	{
		if(0 == m_pWrapper->ReleaseRef())
		{
			m_pMap->RemoveWrapper(m_uuid);
		}
	}
	m_cs.Unlock();
}
