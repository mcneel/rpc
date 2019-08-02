
#pragma once

// {B5F7D9C7-058B-411e-BE31-417DAA219B41}
static const GUID idAF = { 0xb5f7d9c7, 0x58b, 0x411e, { 0xbe, 0x31, 0x41, 0x7d, 0xaa, 0x21, 0x9b, 0x41 } };

class CRpcAnimationFrameRrp : public CRhRdkRegisteredDocProperty
{
protected:
	virtual CSupportChannels RequestedDisplayChannels(void) const { return 0; }
	virtual void DeleteThis(void) { delete this; }

	// Property Identification.
	virtual ON_wString LocalPropertyName(void) const		{ return L"Frame Number"; }
	virtual ON_wString EnglishPropertyName(void) const      { return LocalPropertyName(); }
	virtual ON_wString PropertyCategory(void) const         { return L"RPC"; }
	virtual ON_wString LocalPropertyDescription(void) const { return L"Controls the frame number of the RPC(s)"; }
	virtual UUID       PropertyUuid(void) const             { return idAF; }

	// Plugin identification.
	virtual ON_wString		PlugInName(void) const			{ return L"RPC"; }
	virtual ON_wString		PlugInDescription(void) const	{ return L"RPC Animation"; }
	virtual UUID			PlugInId(void) const;

	// For all types.
	virtual CRhRdkVariant	DefaultValue(void) const		{ return 0; }
	virtual bool			IsAnimatable(void) const		{ return true; }
	virtual bool			IsReadOnly(void) const			{ return false; }
	virtual bool			IsEnabled(void) const			{ return true; }

	virtual CRhRdkVariant::VariantType ValueType(void) const { return CRhRdkVariant::vtInteger; }

	// For integer and real types.
	virtual CRhRdkVariant	MaxValue(void) const			{ return CRhRdkVariant::Null(); }
	virtual CRhRdkVariant	MinValue(void) const			{ return CRhRdkVariant::Null(); }
	virtual units			Units(void) const				{ return no_units; }

	// For enumeration types.
	virtual int				NumberOfValidValues(void) const { return 0; }
	virtual CRhRdkVariant	GetValidValue(int iIndex) const { return CRhRdkVariant::Null(); }

	virtual bool SetValue(CRhinoDoc& doc, const CRhRdkVariant& value);
	virtual CRhRdkVariant GetValue(const CRhinoDoc& doc) const;
	virtual bool IsEnabledForDoc(const CRhinoDoc& doc) const;

	virtual bool SetDisplayToValue(CRhinoDoc& doc, 
								   ConduitData& conduit,								    
								   const CRhRdkVariant& value) const { return false; }
};
