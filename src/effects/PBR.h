#pragma once

class PBRShaders : public ShaderCollection
{
public:
	PBRShaders() : ShaderCollection("PBR") {};

	std::map<std::string, ShaderTemplate> Templates() {
		return std::map<std::string, ShaderTemplate>{
			{ "SLS2000.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}} } },
			{ "SLS2001.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}} } },
			{ "SLS2003.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"SKIN", ""}} } },
			{ "SLS2004.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"PROJ_SHADOW", ""}} } },
			{ "SLS2006.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"PROJ_SHADOW", ""}, {"SKIN", ""}} } },
			{ "SLS2007.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}} } },
			{ "SLS2008.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"LIGHTS", "2"}} } },
			{ "SLS2009.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"LIGHTS", "2"}, {"SKIN", ""}} } },
			{ "SLS2010.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"LIGHTS", "2"}, {"PROJ_SHADOW", ""}} } },
			{ "SLS2011.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"LIGHTS", "2"}, {"PROJ_SHADOW", ""}, {"SKIN", ""}} } },
			{ "SLS2012.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"SPECULAR", ""}} } },
			{ "SLS2013.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"SPECULAR", ""}, {"SKIN", ""}} } },
			{ "SLS2014.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"SPECULAR", ""}, {"PROJ_SHADOW", ""}} } },
			{ "SLS2015.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"SPECULAR", ""}, {"PROJ_SHADOW", ""}, {"SKIN", ""}} } },
			{ "SLS2016.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"SPECULAR", ""}, {"LIGHTS", "2"}} } },
			{ "SLS2017.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"SPECULAR", ""}, {"LIGHTS", "2"}, {"SKIN", ""}} } },
			{ "SLS2018.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"SPECULAR", ""}, {"LIGHTS", "2"}, {"PROJ_SHADOW", ""}} } },
			{ "SLS2019.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"SPECULAR", ""}, {"LIGHTS", "2"}, {"PROJ_SHADOW", ""}, {"SKIN", ""}} } },
			{ "SLS2020.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"LIGHTS", "9"}} } },
			{ "SLS2021.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"LIGHTS", "9"}, {"SKIN", ""}} } },
			{ "SLS2022.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"LIGHTS", "4"}} } },
			{ "SLS2023.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"LIGHTS", "4"}, {"OPT", ""}} } },
			{ "SLS2024.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"LIGHTS", "4"}, {"SKIN", ""}} } },
			{ "SLS2025.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"LIGHTS", "4"}, {"SPECULAR", ""}} } },
			{ "SLS2026.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"LIGHTS", "4"}, {"SPECULAR", ""}, {"OPT", ""}} } },
			{ "SLS2027.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"LIGHTS", "4"}, {"SPECULAR", ""}, {"SKIN", ""}} } },
			{ "SLS2028.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"ONLY_LIGHT", ""}, {"LIGHTS", "2"}} } },
			{ "SLS2029.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"ONLY_LIGHT", ""}, {"LIGHTS", "2"}, {"SKIN", ""}} } },
			{ "SLS2030.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"ONLY_LIGHT", ""}, {"LIGHTS", "2"}, {"PROJ_SHADOW", ""}} } },
			{ "SLS2031.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"ONLY_LIGHT", ""}, {"LIGHTS", "2"}, {"PROJ_SHADOW", ""}, {"SKIN", ""}} } },
			{ "SLS2032.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"ONLY_LIGHT", ""}, {"LIGHTS", "3"}} } },
			{ "SLS2033.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"ONLY_LIGHT", ""}, {"LIGHTS", "3"}, {"SKIN", ""}} } },
			{ "SLS2034.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"ONLY_LIGHT", ""}, {"LIGHTS", "3"}, {"PROJ_SHADOW", ""}} } },
			{ "SLS2035.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"ONLY_LIGHT", ""}, {"LIGHTS", "3"}, {"PROJ_SHADOW", ""}, {"SKIN", ""}} } },
			{ "SLS2036.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"DIFFUSE", ""}, {"LIGHTS", "2"}} } },
			{ "SLS2037.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"DIFFUSE", ""}, {"LIGHTS", "2"}, {"SKIN", ""}} } },
			{ "SLS2038.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"DIFFUSE", ""}, {"LIGHTS", "3"}} } },
			{ "SLS2039.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"DIFFUSE", ""}, {"LIGHTS", "3"}, {"SKIN", ""}} } },
			{ "SLS2040.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"ONLY_SPECULAR", ""}} } },
			{ "SLS2041.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"ONLY_SPECULAR", ""}, {"SKIN", ""}} } },
			{ "SLS2042.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"ONLY_SPECULAR", ""}, {"PROJ_SHADOW", ""}} } },
			{ "SLS2043.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"ONLY_SPECULAR", ""}, {"PROJ_SHADOW", ""}, {"SKIN", ""}} } },
			{ "SLS2044.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"ONLY_SPECULAR", ""}, {"POINT", ""}} } },
			{ "SLS2045.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"ONLY_SPECULAR", ""}, {"POINT", ""}, {"SKIN", ""}} } },
			{ "SLS2046.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"ONLY_SPECULAR", ""}, {"POINT", ""}, {"NUM_PT_LIGHTS", "2"}} } },
			{ "SLS2047.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"ONLY_SPECULAR", ""}, {"POINT", ""}, {"NUM_PT_LIGHTS", "2"}, {"SKIN", ""}} } },
			{ "SLS2048.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"ONLY_SPECULAR", ""}, {"POINT", ""}, {"NUM_PT_LIGHTS", "3"}} } },
			{ "SLS2049.vso", ShaderTemplate{ "ObjectTemplate", {{"VS", ""}, {"ONLY_SPECULAR", ""}, {"POINT", ""}, {"NUM_PT_LIGHTS", "3"}, {"SKIN", ""}} } },
			{ "SLS2000.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}} } },
			{ "SLS2001.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"OPT", ""}} } },
			{ "SLS2002.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"OPT", ""}, {"LOD", ""}} } },
			{ "SLS2004.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"SI", ""}} } },
			{ "SLS2005.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"PROJ_SHADOW", ""}} } },
			{ "SLS2007.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"SI", ""}, {"PROJ_SHADOW", ""}} } },
			{ "SLS2008.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"STBB", ""}} } },
			{ "SLS2009.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"HAIR", ""}} } },
			{ "SLS2010.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"HAIR", ""}, {"PROJ_SHADOW", ""}} } },
			{ "SLS2011.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"LIGHTS", "2"}} } },
			{ "SLS2012.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"LIGHTS", "2"}, {"SI", ""}} } },
			{ "SLS2013.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"LIGHTS", "2"}, {"HAIR", ""}} } },
			{ "SLS2014.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"LIGHTS", "2"}, {"PROJ_SHADOW", ""}} } },
			{ "SLS2015.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"LIGHTS", "2"}, {"SI", ""}, {"PROJ_SHADOW", ""}} } },
			{ "SLS2016.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"LIGHTS", "2"}, {"HAIR", ""}, {"PROJ_SHADOW", ""}} } },
			{ "SLS2017.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"SPECULAR", ""}} } },
			{ "SLS2018.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"SPECULAR", ""}, {"SI", ""}} } },
			{ "SLS2019.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"SPECULAR", ""}, {"HAIR", ""}} } },
			{ "SLS2020.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"SPECULAR", ""}, {"PROJ_SHADOW", ""}} } },
			{ "SLS2021.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"SPECULAR", ""}, {"SI", ""}, {"PROJ_SHADOW", ""}} } },
			{ "SLS2022.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"SPECULAR", ""}, {"HAIR", ""}, {"PROJ_SHADOW", ""}} } },
			{ "SLS2023.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"SPECULAR", ""}, {"LIGHTS", "2"}} } },
			{ "SLS2024.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"SPECULAR", ""}, {"LIGHTS", "2"}, {"SI", ""}} } },
			{ "SLS2026.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"SPECULAR", ""}, {"LIGHTS", "2"}, {"PROJ_SHADOW", ""}} } },
			{ "SLS2027.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"SPECULAR", ""}, {"LIGHTS", "2"}, {"SI", ""}, {"PROJ_SHADOW", ""}} } },
			{ "SLS2029.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"LIGHTS", "9"}} } },
			{ "SLS2030.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"LIGHTS", "9"}, {"SI", ""}} } },
			{ "SLS2031.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"LIGHTS", "4"}} } },
			{ "SLS2032.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"LIGHTS", "4"}, {"OPT", ""}} } },
			{ "SLS2033.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"LIGHTS", "4"}, {"SI", ""}} } },
			{ "SLS2034.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"LIGHTS", "4"}, {"SPECULAR", ""}} } },
			{ "SLS2035.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"LIGHTS", "4"}, {"SPECULAR", ""}, {"OPT", ""}} } },
			{ "SLS2036.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"LIGHTS", "4"}, {"SPECULAR", ""}, {"SI", ""}} } },
			{ "SLS2037.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"ONLY_LIGHT", ""}, {"OPT", ""}, {"LIGHTS", "2"}} } },
			{ "SLS2038.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"ONLY_LIGHT", ""}, {"OPT", ""}, {"LIGHTS", "2"}, {"SI", ""}} } },
			{ "SLS2039.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"ONLY_LIGHT", ""}, {"OPT", ""}, {"LIGHTS", "2"}, {"PROJ_SHADOW", ""}} } },
			{ "SLS2040.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"ONLY_LIGHT", ""}, {"OPT", ""}, {"LIGHTS", "2"}, {"PROJ_SHADOW", ""}, {"SI", ""}} } },
			{ "SLS2041.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"ONLY_LIGHT", ""}, {"OPT", ""}, {"LIGHTS", "3"}} } },
			{ "SLS2042.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"ONLY_LIGHT", ""}, {"OPT", ""}, {"LIGHTS", "3"}, {"SI", ""}} } },
			{ "SLS2043.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"ONLY_LIGHT", ""}, {"OPT", ""}, {"LIGHTS", "3"}, {"PROJ_SHADOW", ""}} } },
			{ "SLS2044.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"ONLY_LIGHT", ""}, {"OPT", ""}, {"LIGHTS", "3"}, {"PROJ_SHADOW", ""}, {"SI", ""}} } },
			{ "SLS2045.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"DIFFUSE", ""}, {"LIGHTS", "2"}} } },
			{ "SLS2046.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"DIFFUSE", ""}, {"LIGHTS", "3"}} } },
			{ "SLS2047.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"ONLY_SPECULAR", ""}} } },
			{ "SLS2048.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"ONLY_SPECULAR", ""}, {"HAIR", ""}} } },
			{ "SLS2049.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"ONLY_SPECULAR", ""}, {"PROJ_SHADOW", ""}} } },
			{ "SLS2050.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"ONLY_SPECULAR", ""}, {"PROJ_SHADOW", ""}, {"HAIR", ""}} } },
			{ "SLS2051.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"ONLY_SPECULAR", ""}, {"POINT", ""}} } },
			{ "SLS2052.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"ONLY_SPECULAR", ""}, {"POINT", ""}, {"HAIR", ""}} } },
			{ "SLS2053.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"ONLY_SPECULAR", ""}, {"POINT", ""}, {"NUM_PT_LIGHTS", "2"}} } },
			{ "SLS2054.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"ONLY_SPECULAR", ""}, {"POINT", ""}, {"NUM_PT_LIGHTS", "2"}, {"HAIR", ""}} } },
			{ "SLS2055.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"ONLY_SPECULAR", ""}, {"POINT", ""}, {"NUM_PT_LIGHTS", "3"}} } },
			{ "SLS2056.pso", ShaderTemplate{ "ObjectTemplate", {{"PS", ""}, {"ONLY_SPECULAR", ""}, {"POINT", ""}, {"NUM_PT_LIGHTS", "3"}, {"HAIR", ""}} } }
		};
	};

	struct PBRSettings {
		float LightScale;
		float AmbientScale;
		float Roughness;
		float Metallicness;
		float Saturation;
	};
	struct PBRSettingsStruct {
		PBRSettings Default;
		PBRSettings Rain;
		PBRSettings Night;
		PBRSettings NightRain;
		PBRSettings Interiors;
	};
	PBRSettingsStruct Settings;

	struct PBRStruct {
		D3DXVECTOR4		Data;
		D3DXVECTOR4		ExtraData;
	};
	PBRStruct	Constants;

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();
};