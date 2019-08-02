
#pragma once

class CRhRdkVariant;

class RHRDK_SDK IRhRdkRegisteredProperty
{
public:
	enum units
	{
		no_units = 0,

		// SI units.
		meters=1, square_meters=2, cubic_meters=3, kilograms=4, seconds=5, radians=6,

		// Rhino units.
		rhino_units=7, square_rhino_units=8, cubic_rhino_units=9,

		// Others.
		percent=10, // The property will be of type vtDouble in the range 0..1 for 0..100%

		force32bit_units=0xFFFFFFFF
	};

	class ConduitData
	{
	public:
		ConduitData();

		CRhinoDisplayConduit* m_pConduit;
		CRhinoView* m_pView;
		CRhinoViewport* m_pVP;
		CChannelAttributes* m_pChannelAttrs;
		CDisplayPipelineAttributes* m_pDisplayAttrs;
		UINT m_iChannel;
		CRhinoDisplayPipeline* m_dp;
		void* m_pReserved;
	};

	// Unnamed enum - these can be combined for the iterator.
	enum
	{
		object = 1, document = 2, view = 4, application = 8, layer = 16, rdk_content = 32, light = 64,
		all = object | document | view | application | layer | rdk_content | light,
		force32bit_pt = 0xFFFFFFFF
	};
	typedef DWORD prop_type;

public:
	/** Display pipeline channels requested for dynamic display support - see subclasses SetDisplayToValue for more details */
	virtual CSupportChannels RequestedDisplayChannels(void) const = 0;

	/** You must implement this method as \code delete this; \endcode */
	virtual void DeleteThis(void) = 0;

	// Property Identification.

	virtual ON_wString LocalPropertyName(void) const = 0; // Was previously virtual ON_wString PropertyName(void) const = 0;
	virtual ON_wString EnglishPropertyName(void) const = 0;
	virtual ON_wString PropertyCategory(void) const = 0;
	virtual ON_wString LocalPropertyDescription(void) const = 0; // Was previously virtual ON_wString PropertyDescription(void) const = 0;

	virtual UUID PropertyUuid(void) const = 0;
	virtual prop_type PropertyType(void) const = 0;

	// Plugin identification.
	virtual ON_wString PlugInName(void) const = 0;
	virtual ON_wString PlugInDescription(void) const = 0;
	virtual UUID PlugInId(void) const = 0;

public:
	// For all types.
	virtual CRhRdkVariant DefaultValue(void) const = 0;
	virtual bool IsAnimatable(void) const = 0;
	virtual bool IsReadOnly(void) const = 0;
	virtual bool IsEnabled(void) const = 0;

	virtual CRhRdkVariant::VariantType ValueType(void) const = 0;

	// For integer and real types.
	virtual CRhRdkVariant MaxValue(void) const = 0;
	virtual CRhRdkVariant MinValue(void) const = 0;
	virtual units Units(void) const = 0;

	// For enumeration types.
	virtual int NumberOfValidValues(void) const = 0;
	virtual CRhRdkVariant GetValidValue(int iIndex) const = 0;

	/** Emergency virtual function for future expansion. */
	virtual void* EVF(const wchar_t*, void*) = 0;
	//Defined EVF calls: L"fixed-decimal-places", pvData: int*. Return NULL - use defaults.

protected:
	virtual ~IRhRdkRegisteredProperty() { }
};

// Specialized interface classes and sub-classes for the different property types.

// Object Properties

class RHRDK_SDK IRhRdkRegisteredObjectProperty : public IRhRdkRegisteredProperty
{
public:
	virtual bool SetValue(CRhinoDoc& doc, const UUID& objectId, ON_COMPONENT_INDEX ci, const CRhRdkVariant& value) = 0;

	virtual bool SetDisplayToValue(CRhinoDoc& doc, const UUID& objectId, ON_COMPONENT_INDEX ci, ConduitData& conduit, const CRhRdkVariant& value) const = 0;

	/** Should return CRhRdkVariant with type vtNull if the call fails.
	If IsEnabledForObject() returns \e true, this should return a value. */
	virtual CRhRdkVariant GetValue(const CRhinoDoc& doc, const UUID& objectID, ON_COMPONENT_INDEX ci) const = 0;

	virtual bool IsEnabledForObject(const CRhinoDoc& doc, const UUID& objectId, ON_COMPONENT_INDEX ci) const = 0;

	/** Should determine whether the input class is supported by this property.
	Return \e true if you want to decide on a per-object basis. Otherwise implement like this:
	\code return id.IsDerivedFrom(ON_Object::ClassId()); \endcode */
	virtual bool IsEnabledForObjectClass(const ON_ClassId& id) const = 0;
};

class RHRDK_SDK CRhRdkRegisteredObjectProperty : public IRhRdkRegisteredObjectProperty
{
public:
	virtual void DeleteThis(void) override { delete this; }
	virtual prop_type PropertyType(void) const override { return object; }
	virtual CRhRdkVariant::VariantType ValueType(void) const override { return DefaultValue().Type(); }
	virtual void* EVF(const wchar_t*, void*) override { return nullptr; }

	/** Call this function after changing a value on this property. */
	virtual void CallPropertyChangedEvent(const CRhinoDoc& doc, const UUID& objectId, ON_COMPONENT_INDEX ci, const CRhRdkVariant& valueOld) const;
};

// View Properties

class RHRDK_SDK IRhRdkRegisteredViewProperty : public IRhRdkRegisteredProperty
{
public:
	virtual bool SetValue(CRhinoDoc& doc, const UUID& viewId, const CRhRdkVariant& value) = 0;

	virtual bool SetDisplayToValue(CRhinoDoc& doc, const UUID& viewId, ConduitData& conduit, const CRhRdkVariant& value) const = 0;

	/** Should return CRhRdkVariant with type vtNull if the call fails.
	If IsEnabledForView() returns \e true, this should return a value. */
	virtual CRhRdkVariant GetValue(const CRhinoDoc& doc, const UUID& viewId) const = 0;

	virtual bool IsEnabledForView(const CRhinoDoc& doc, const UUID& viewId) const = 0;
};

class RHRDK_SDK CRhRdkRegisteredViewProperty : public IRhRdkRegisteredViewProperty
{
public:
	virtual void DeleteThis(void) { delete this; }
	virtual prop_type PropertyType(void) const { return view; }
	virtual CRhRdkVariant::VariantType ValueType(void) const { return DefaultValue().Type(); }

	/** Call this function after changing a value on this property. */
	virtual void CallPropertyChangedEvent(const CRhinoDoc& doc, const UUID& viewId, const CRhRdkVariant& valueOld) const;
};

// Application Properties

class RHRDK_SDK IRhRdkRegisteredAppProperty : public IRhRdkRegisteredProperty
{
public:
	virtual bool SetValue(const CRhRdkVariant& value) = 0;

	virtual bool SetDisplayToValue(ConduitData& conduit, const CRhRdkVariant& value) const = 0;

	/** Should return CRhRdkVariant with type vtNull if the call fails. */
	virtual CRhRdkVariant GetValue() const = 0;
};

class RHRDK_SDK CRhRdkRegisteredAppProperty : public IRhRdkRegisteredAppProperty
{
public:
	virtual void DeleteThis(void) { delete this; }
	virtual prop_type PropertyType(void) const { return application; }
	virtual CRhRdkVariant::VariantType ValueType(void) const { return DefaultValue().Type(); }

	/** Call this function after changing a value on this property. */
	virtual void CallPropertyChangedEvent(const CRhRdkVariant& valueOld) const;
};

// Document Properties

class RHRDK_SDK IRhRdkRegisteredDocProperty : public IRhRdkRegisteredProperty
{
public:
	virtual bool SetValue(CRhinoDoc& doc, const CRhRdkVariant& value) = 0;

	virtual bool SetDisplayToValue(CRhinoDoc& doc, ConduitData& conduit, const CRhRdkVariant& value) const = 0;

	/** Should return CRhRdkVariant with type vtNull if the call fails.
	If IsEnabledForDoc() returns \e true, this should return a value. */
	virtual CRhRdkVariant GetValue(const CRhinoDoc& doc) const = 0;

	virtual bool IsEnabledForDoc(const CRhinoDoc& doc) const = 0;
};

class RHRDK_SDK CRhRdkRegisteredDocProperty : public IRhRdkRegisteredDocProperty
{
public:
	virtual void DeleteThis(void) { delete this; }
	virtual prop_type PropertyType(void) const { return document; }
	virtual CRhRdkVariant::VariantType ValueType(void) const { return DefaultValue().Type(); }

	/** Call this function after changing a value on this property. */
	virtual void CallPropertyChangedEvent(const CRhinoDoc& doc, const CRhRdkVariant& valueOld) const;

	/** Implemented inline for Rhino without multiple document support. */
	virtual bool IsEnabledForDoc(const CRhinoDoc&) const { return IsEnabled(); }

	/** Emergency virtual function for future expansion. */
	virtual void* EVF(const wchar_t* wszFunc, void* pvData);
};

// Layer Properties

class RHRDK_SDK IRhRdkRegisteredLayerProperty : public IRhRdkRegisteredProperty
{
public:
	virtual bool SetValue(CRhinoDoc& doc, const UUID& layer_id, const CRhRdkVariant& value) = 0;

	virtual bool SetDisplayToValue(CRhinoDoc& doc, const UUID& layer_id, ConduitData& conduit, const CRhRdkVariant& value) const = 0;

	/** Should return CRhRdkVariant with type vtNull if the call fails.
	If IsEnabledForDoc() returns \e true, this should return a value. */
	virtual CRhRdkVariant GetValue(const CRhinoDoc& doc, const UUID& layer_id) const = 0;

	virtual bool IsEnabledForLayer(const CRhinoDoc& doc, const UUID& layer_id) const = 0;
};

class RHRDK_SDK CRhRdkRegisteredLayerProperty : public IRhRdkRegisteredLayerProperty
{
public:
	virtual void DeleteThis(void) { delete this; }
	virtual prop_type PropertyType(void) const { return layer; }
	virtual CRhRdkVariant::VariantType ValueType(void) const { return DefaultValue().Type(); }

	/** Call this function after changing a value on this property. */
	virtual void CallPropertyChangedEvent(const CRhinoDoc& doc, const UUID& layer_id, const CRhRdkVariant& valueOld) const;

	/** Implemented inline for Rhino without multiple document support. */
	virtual bool IsEnabledForLayer(const CRhinoDoc&, const UUID& /*layer_id*/) const { return IsEnabled(); }

	/** Emergency virtual function for future expansion. */
	virtual void* EVF(const wchar_t* wszFunc, void* pvData);
};

// RDK Content Properties

class RHRDK_SDK IRhRdkRegisteredRdkContentProperty : public IRhRdkRegisteredProperty
{
public:
	virtual bool SetValue(CRhinoDoc& doc, const UUID& instance_id, const CRhRdkVariant& value) = 0;

	virtual bool SetDisplayToValue(CRhinoDoc& doc, const UUID& instance_id, ConduitData& conduit, const CRhRdkVariant& value) const = 0;

	/** Should return CRhRdkVariant with type vtNull if the call fails.
	If IsEnabledForDoc() returns \e true, this should return a value. */
	virtual CRhRdkVariant GetValue(const CRhinoDoc& doc, const UUID& instance_id) const = 0;

	virtual bool IsEnabledForContent(const CRhinoDoc& doc, const UUID& instance_id) const = 0;
};

class RHRDK_SDK CRhRdkRegisteredRdkContentProperty : public IRhRdkRegisteredRdkContentProperty
{
public:
	virtual void DeleteThis(void) { delete this; }
	virtual prop_type PropertyType(void) const { return rdk_content; }
	virtual CRhRdkVariant::VariantType ValueType(void) const { return DefaultValue().Type(); }

	/** Call this function after changing a value on this property. */
	virtual void CallPropertyChangedEvent(const CRhinoDoc& doc, const UUID& instance_id, const CRhRdkVariant& valueOld) const;

	/** Implemented inline for Rhino without multiple document support. */
	virtual bool IsEnabledForContent(const CRhinoDoc&, const UUID& /*instance_id*/) const { return IsEnabled(); }

	/** Emergency virtual function for future expansion. */
	virtual void* EVF(const wchar_t* wszFunc, void* pvData);
};

// Light Properties

class RHRDK_SDK IRhRdkRegisteredLightProperty : public IRhRdkRegisteredProperty
{
public:
	virtual bool SetValue(CRhinoDoc& doc, const UUID& light_id, const CRhRdkVariant& value) = 0;

	virtual bool SetDisplayToValue(CRhinoDoc& doc, const UUID& light_id, ConduitData& conduit, const CRhRdkVariant& value) const = 0;

	/** Should return CRhRdkVariant with type vtNull if the call fails.
	If IsEnabledForLight() returns \e true, this should return a value. */
	virtual CRhRdkVariant GetValue(const CRhinoDoc& doc, const UUID& light_id) const = 0;

	virtual bool IsEnabledForLight(const CRhinoDoc& doc, const UUID& light_id) const = 0;
};

class RHRDK_SDK CRhRdkRegisteredLightProperty : public IRhRdkRegisteredLightProperty
{
public:
	virtual void DeleteThis(void) { delete this; }
	virtual prop_type PropertyType(void) const { return light; }
	virtual CRhRdkVariant::VariantType ValueType(void) const { return DefaultValue().Type(); }

	/** Call this function after changing a value on this property. */
	virtual void CallPropertyChangedEvent(const CRhinoDoc& doc, const UUID& light_id, const CRhRdkVariant& valueOld) const;

	/** Implemented inline for Rhino without multiple document support. */
	virtual bool IsEnabledForLight(const CRhinoDoc&, const UUID& /*light_id*/) const { return IsEnabled(); }

	/** Emergency virtual function for future expansion. */
	virtual void* EVF(const wchar_t* wszFunc, void* pvData);
};
