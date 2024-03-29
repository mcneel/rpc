#include "stdafx.h"
#include "MaterialParams.h"
#include <map>


const std::map<MaterialParams, const RPCapi::TStringArg> PARAM_NAMES = {
	{MaterialParams::MATERIAL_MODE, RPCapi::TStringArg("material_mode")},
	{MaterialParams::BASE_WEIGHT, RPCapi::TStringArg("base_weight")},
	{MaterialParams::BASE_COLOR, RPCapi::TStringArg("base_color")},
	{MaterialParams::REFLECTIVITY, RPCapi::TStringArg("reflectivity")},
	{MaterialParams::ROUGHNESS, RPCapi::TStringArg("roughness")},
	{MaterialParams::ROUGHNESS_INVERSION, RPCapi::TStringArg("roughness_inv")},
	{MaterialParams::METALNESS, RPCapi::TStringArg("metalness")},
	{MaterialParams::REFLECTION_COLOR, RPCapi::TStringArg("refl_color")},
	{MaterialParams::DIFF_ROUGHNESS, RPCapi::TStringArg("diff_roughness")},
	{MaterialParams::BRDF_MODE, RPCapi::TStringArg("brdf_mode")},
	{MaterialParams::BRDF_LOW, RPCapi::TStringArg("brdf_low")},
	{MaterialParams::BRDF_HIGH, RPCapi::TStringArg("brdf_high")},
	{MaterialParams::BRDF_CURVE, RPCapi::TStringArg("brdf_curve")},
	{MaterialParams::ANISOTROPY, RPCapi::TStringArg("anisotropy")},
	{MaterialParams::ANISOANGLE, RPCapi::TStringArg("anisoangle")},
	{MaterialParams::ANISO_MODE, RPCapi::TStringArg("aniso_mode")},
	{MaterialParams::ANISO_CHANNEL, RPCapi::TStringArg("aniso_channel")},
	{MaterialParams::TRANSPARENCY, RPCapi::TStringArg("transparency")},
	{MaterialParams::TRANSPARENCY_COLOR, RPCapi::TStringArg("trans_color")},
	{MaterialParams::TRANSPARENCY_DEPTH, RPCapi::TStringArg("trans_depth")},
	{MaterialParams::TRANSPARENCY_IOR, RPCapi::TStringArg("trans_ior")},
	{MaterialParams::TRANSPARENCY_ROUGHNESS_LOCK, RPCapi::TStringArg("trans_roughness_lock")},
	{MaterialParams::TRANSPARENCY_ROUGHNESS, RPCapi::TStringArg("trans_roughness")},
	{MaterialParams::TRANSPARENCY_ROUGHNESS_INVERSION, RPCapi::TStringArg("trans_roughness_inv")},
	{MaterialParams::THIN_WALLED, RPCapi::TStringArg("thin_walled")},
	{MaterialParams::SCATTERING, RPCapi::TStringArg("scattering")},
	{MaterialParams::SSS_COLOR, RPCapi::TStringArg("sss_color")},
	{MaterialParams::SSS_DEPTH, RPCapi::TStringArg("sss_depth")},
	{MaterialParams::SSS_SCALE, RPCapi::TStringArg("sss_scale")},
	{MaterialParams::SSS_SCATTER_COLOR, RPCapi::TStringArg("sss_scatter_color")},
	{MaterialParams::EMISSION, RPCapi::TStringArg("emission")},
	{MaterialParams::EMISSION_COLOR, RPCapi::TStringArg("emit_color")},
	{MaterialParams::EMISSION_LUMINANCE, RPCapi::TStringArg("emit_luminance")},
	{MaterialParams::EMISSION_COLOR_PRESET, RPCapi::TStringArg("emit_color_preset")},
	{MaterialParams::EMISSION_KELVIN, RPCapi::TStringArg("emit_kelvin")},
	{MaterialParams::COATING, RPCapi::TStringArg("coating")},
	{MaterialParams::COAT_COLOR, RPCapi::TStringArg("coat_color")},
	{MaterialParams::COAT_ROUGHNESS, RPCapi::TStringArg("coat_roughness")},
	{MaterialParams::COAT_ROUGHNESS_INV, RPCapi::TStringArg("coat_roughness_inv")},
	{MaterialParams::COAT_AFFECT_COLOR, RPCapi::TStringArg("coat_affect_color")},
	{MaterialParams::COAT_AFFECT_ROUGHNESS, RPCapi::TStringArg("coat_affect_roughness")},
	{MaterialParams::COAT_IOR, RPCapi::TStringArg("coat_ior")}
};

const RPCapi::TStringArg getParamName(MaterialParams param)
{
	auto it = PARAM_NAMES.find(param);
	return it != PARAM_NAMES.end() ? it->second : "";
}

const std::map<MaterialMaps, const RPCapi::TStringArg> MAP_NAMES = {
	{MaterialMaps::BASE_WEIGHT_MAP, RPCapi::TStringArg("base_weight_map")},
	{MaterialMaps::BASE_COLOR_MAP, RPCapi::TStringArg("base_color_map")},
	{MaterialMaps::REFLECTIVITY_MAP, RPCapi::TStringArg("reflectivity_map")},
	{MaterialMaps::REFL_COLOR_MAP, RPCapi::TStringArg("refl_color_map")},
	{MaterialMaps::ROUGHNESS_MAP, RPCapi::TStringArg("roughness_map")},
	{MaterialMaps::METALNESS_MAP, RPCapi::TStringArg("metalness_map")},
	{MaterialMaps::DIFF_ROUGH_MAP, RPCapi::TStringArg("diff_rough_map")},
	{MaterialMaps::ANISOTROPY_MAP, RPCapi::TStringArg("anisotropy_map")},
	{MaterialMaps::ANISO_ANGLE_MAP, RPCapi::TStringArg("aniso_angle_map")},
	{MaterialMaps::TRANSPARENCY_MAP, RPCapi::TStringArg("transparency_map")},
	{MaterialMaps::TRANSPARENCY_COLOR_MAP, RPCapi::TStringArg("trans_color_map")},
	{MaterialMaps::TRANSPARENCY_ROUGH_MAP, RPCapi::TStringArg("trans_rough_map")},
	{MaterialMaps::TRANSPARENCY_IOR_MAP, RPCapi::TStringArg("trans_ior_map")},
	{MaterialMaps::SCATTERING_MAP, RPCapi::TStringArg("scattering_map")},
	{MaterialMaps::SSS_COLOR_MAP, RPCapi::TStringArg("sss_color_map")},
	{MaterialMaps::SSS_SCALE_MAP, RPCapi::TStringArg("sss_scale_map")},
	{MaterialMaps::EMISSION_MAP, RPCapi::TStringArg("emission_map")},
	{MaterialMaps::EMISSION_COLOR_MAP, RPCapi::TStringArg("emit_color_map")},
	{MaterialMaps::COAT_MAP, RPCapi::TStringArg("coat_map")},
	{MaterialMaps::COAT_COLOR_MAP, RPCapi::TStringArg("coat_color_map")},
	{MaterialMaps::COAT_ROUGH_MAP, RPCapi::TStringArg("coat_rough_map")},
	{MaterialMaps::BUMP_MAP, RPCapi::TStringArg("bump_map")},
	{MaterialMaps::COAT_BUMP_MAP, RPCapi::TStringArg("coat_bump_map")},
	{MaterialMaps::DISPLACEMENT_MAP, RPCapi::TStringArg("displacement_map")},
	{MaterialMaps::CUTOUT_MAP, RPCapi::TStringArg("cutout_map")}
};

const RPCapi::TStringArg getMapName(MaterialMaps param)
{
	auto it = MAP_NAMES.find(param);
	return it != MAP_NAMES.end() ? it->second : "";
}