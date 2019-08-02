
#pragma once

//#ifdef RHRDK_REGPROPS

#include "IRhRdkRegisteredProperty.h"

class RHRDK_SDK IRhRdkRegisteredPropertyManager
{
public:
	virtual ~IRhRdkRegisteredPropertyManager() { }

public:
	/** Add a registered property. When you add a property object, you are transferring
	    ownership to the property manager, so simply call with \code new CMyProperty \endcode
		as a parameter. */
	virtual bool Add(IRhRdkRegisteredProperty*) = 0;

public:
	// Property Iteration and location.
	class Iterator
	{
	public:
		virtual ~Iterator() { }

	public:
		virtual void Reset(void) = 0;
		virtual IRhRdkRegisteredProperty* GetNextProperty(void) = 0;

		/** Emergency virtual function for future expansion. */
		virtual void* EVF(const wchar_t*, void*) = 0;
	};

	/** Get an iterator for iterating over properties. The caller shall delete the iterator. */
	virtual Iterator* NewRegisteredPropertyIterator(IRhRdkRegisteredProperty::prop_type t) const = 0;

	/** Find a registered property by its UUID. */
	virtual IRhRdkRegisteredProperty* FindProperty(UUID uuid) const = 0;

	/** Emergency virtual function for future expansion. */
	virtual void* EVF(const wchar_t*, void*) = 0;
};

namespace RhRcmInternalPropertyIds
{
	RHRDK_SDK const UUID& ObjectColor(void);
	RHRDK_SDK const UUID& ObjectName(void);
	RHRDK_SDK const UUID& ObjectVisibility(void);
	RHRDK_SDK const UUID& ObjectMaterialDiffuseColor(void);
	RHRDK_SDK const UUID& ObjectMaterialGlossyColor(void);
	RHRDK_SDK const UUID& ObjectMaterialTransparency(void);
	RHRDK_SDK const UUID& ObjectMaterialGlossFinish(void);


	//RDK sun properties
	RHRDK_SDK const UUID& RdkSunEnableOn(void);
	RHRDK_SDK const UUID& RdkSunLongitude(void);
	RHRDK_SDK const UUID& RdkSunLatitude(void);
	RHRDK_SDK const UUID& RdkSunTimezone(void);
	RHRDK_SDK const UUID& RdkSunDaylightSavingOn(void);
	RHRDK_SDK const UUID& RdkSunDaylightSavingMinutes(void);
	RHRDK_SDK const UUID& RdkSunLocalDateTime(void);

	//Layer properties
	RHRDK_SDK const UUID& LayerVisible(void);

	//Light properties
	RHRDK_SDK const UUID& LightOn(void);
	RHRDK_SDK const UUID& LightColor(void);
	RHRDK_SDK const UUID& LightShadowIntensity(void);
	RHRDK_SDK const UUID& LightIntensity(void);
	RHRDK_SDK const UUID& LightSpotlightHardness(void);

	//Content properties
	RHRDK_SDK const UUID& RdkTextureRepeat(void);
	RHRDK_SDK const UUID& RdkTextureOffset(void);
	RHRDK_SDK const UUID& RdkTextureRotation(void);

}



//#endif
