#pragma once

enum eLBPRhWrapperType
{ 
	lbprh_wt_none, 
	lbprh_wt_object, 
	lbprh_wt_view, 
	lbprh_wt_material, 
	lbprh_wt_layer,
	lbprh_wt_document,
	lbprh_wt_light,
	lbprh_wt_rdk_content,
};


class IWrapperBase
{
public:
	virtual ON_Object*			GetTarget(void) const = 0;
	virtual UUID				OriginalUuid(void) const = 0;
	virtual const ON_Object*	OriginalObject(void) const = 0;
	virtual bool				RemoveData(void) = 0;
	virtual void				UndoModifications(void) = 0;
	virtual bool				CommitChanges(void) = 0;
	virtual void				RecaptureObject(void) = 0;
	virtual void				Modify(void) = 0;
	virtual bool				UserDataPresent(void) const = 0;
	virtual bool				ModificationsPending(void) const = 0;
	virtual bool				LastReference(void) const = 0;
	virtual eLBPRhWrapperType	WrapperType(void) const = 0;
	virtual void				OnModification(void) const = 0;
};

enum eLBPRhWrapper_DefaultUse { lbprh_use_defaults, lbprh_dont_use_defaults };

template<class T> class ILBPRhWrapper : public IWrapperBase
{
public:
	virtual const T*	UserData(eLBPRhWrapper_DefaultUse du = lbprh_dont_use_defaults) const = 0;
	virtual T*			UserDataToModify(void) = 0;
};
