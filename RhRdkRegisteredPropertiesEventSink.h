
#pragma once // SDK header

class CRhRdkContent;
class IRhRdkContentEditor;
class CRhRdkContentFactory;
class IRhRdkContentList;
class ON_TextureMapping;
class CRhRdkUuidCollection;
class CRhRdkVariant;

#include "IRhRdkRegisteredProperty.h"

/** \class CRhRdkRegisteredPropertiesEventSinkBase

	Base class for 'properties event sinks'. Used internally by RDK for event sinks that are not automatically
	registered with the event machine system. Generally you should not derive from this class directly.
	\see CRhRdkEventSink.
*/
class RHRDK_SDK CRhRdkRegisteredPropertiesEventSinkBase
{
public:
	CRhRdkRegisteredPropertiesEventSinkBase();
	virtual ~CRhRdkRegisteredPropertiesEventSinkBase();

	// Registered property events.
	virtual void OnPropertyRegistered(const IRhRdkRegisteredProperty&) { }
	virtual void OnPropertyUnregistering(const IRhRdkRegisteredProperty&) { }
	virtual void OnPropertyChanged(const IRhRdkRegisteredObjectProperty&, const CRhinoDoc&, const UUID& /*objectId*/, ON_COMPONENT_INDEX, const CRhRdkVariant& /*valueOld*/) { }
	virtual void OnPropertyChanged(const IRhRdkRegisteredDocProperty&, const CRhinoDoc&, const CRhRdkVariant& /*valueOld*/) { }
	virtual void OnPropertyChanged(const IRhRdkRegisteredAppProperty&, const CRhRdkVariant& /*valueOld*/) { }
	virtual void OnPropertyChanged(const IRhRdkRegisteredViewProperty&, const CRhinoDoc&, const UUID& /*viewId*/, const CRhRdkVariant& /*valueOld*/) { }
	virtual void OnPropertyChanged(const IRhRdkRegisteredLayerProperty&, const CRhinoDoc&, const UUID& /*layer_id*/, const CRhRdkVariant& /*valueOld*/) { }
	virtual void OnPropertyChanged(const IRhRdkRegisteredRdkContentProperty&, const CRhinoDoc&, const UUID& /*instance_id*/, const CRhRdkVariant& /*valueOld*/) { }
	virtual void OnPropertyChanged(const IRhRdkRegisteredLightProperty&, const CRhinoDoc&, const UUID& /*light_id*/, const CRhRdkVariant& /*valueOld*/) { }

	// Future expansion.

	/** Custom events. */
	static const ON_UUID& BeginChanges(void);	//Signal to client that a block of changes is about to take place.  Do not react until end. pvContext = nullptr
	static const ON_UUID& EndChanges(void);		//Ends block of changes started by BeginChanges event. pvContext = nullptr

	virtual void OnCustomEvent(const UUID& /*uuidCode*/, void* /*pvContext*/) { }

	/** Emergency virtual function for future expansion. */
	virtual void* EVF(const wchar_t* wszFunc, void* pvData);
};

/** \class CRhRdkRegisteredPropertiesEventSink

	Base class for auto-registering 'event sinks'. To allow a class to receive events, derive it from
	CRhRdkRegisteredPropertiesEventSink and override the methods of CRhRdkRegisteredPropertiesEventSinkBase 
	which you want to be called on.
	There is no need to call the base class. Some of these methods, particularly those dealing with contents
	will be called with NULL pointers where NULL means "no content". Event handlers must check
	all incoming pointers for NULL before dereferencing them unless otherwise stated.
*/
class RHRDK_SDK CRhRdkRegisteredPropertiesEventSink : protected CRhRdkRegisteredPropertiesEventSinkBase
{
public:
	CRhRdkRegisteredPropertiesEventSink();
	virtual ~CRhRdkRegisteredPropertiesEventSink();

	static void RaiseCustomEvent(const UUID&, void*);
};
