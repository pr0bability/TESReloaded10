#pragma once

class TerrainShaders : public ShaderCollection
{
public:
	TerrainShaders() : ShaderCollection("Terrain") {};

	std::map<std::string_view, ShaderTemplate> Templates() {
		return std::map<std::string_view, ShaderTemplate>{
			{ "SLS2100.vso", ShaderTemplate{ "TerrainTemplate", {{"VS", ""}} } },
			{ "SLS2092.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "1"}}} },
			{ "SLS2094.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "1"}, {"NUM_PT_LIGHTS", "6"}}} },
			{ "SLS2096.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "1"}, {"NUM_PT_LIGHTS", "12"}}} },
			{ "SLS2098.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "1"}, {"NUM_PT_LIGHTS", "24"}}} },
			{ "SLS2100.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "2"}}} },
			{ "SLS2102.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "2"}, {"NUM_PT_LIGHTS", "6"}}} },
			{ "SLS2104.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "2"}, {"NUM_PT_LIGHTS", "12"}}} },
			{ "SLS2106.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "2"}, {"NUM_PT_LIGHTS", "24"}}} },
			{ "SLS2108.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "3"}}} },
			{ "SLS2110.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "3"}, {"NUM_PT_LIGHTS", "6"}}} },
			{ "SLS2112.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "3"}, {"NUM_PT_LIGHTS", "12"}}} },
			{ "SLS2114.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "3"}, {"NUM_PT_LIGHTS", "24"}}} },
			{ "SLS2116.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "4"}}} },
			{ "SLS2118.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "4"}, {"NUM_PT_LIGHTS", "6"}}} },
			{ "SLS2120.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "4"}, {"NUM_PT_LIGHTS", "12"}}} },
			{ "SLS2122.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "4"}, {"NUM_PT_LIGHTS", "24"}}} },
			{ "SLS2124.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "5"}}} },
			{ "SLS2126.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "5"}, {"NUM_PT_LIGHTS", "6"}}} },
			{ "SLS2128.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "5"}, {"NUM_PT_LIGHTS", "12"}}} },
			{ "SLS2130.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "5"}, {"NUM_PT_LIGHTS", "24"}}} },
			{ "SLS2132.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "6"}}} },
			{ "SLS2134.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "6"}, {"NUM_PT_LIGHTS", "6"}}} },
			{ "SLS2136.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "6"}, {"NUM_PT_LIGHTS", "12"}}} },
			{ "SLS2138.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "6"}, {"NUM_PT_LIGHTS", "24"}}} },
			{ "SLS2140.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "7"}}} },
			{ "SLS2142.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "7"}, {"NUM_PT_LIGHTS", "6"}}} },
			{ "SLS2144.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "7"}, {"NUM_PT_LIGHTS", "12"}}} },
			{ "SLS2146.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "7"}, {"NUM_PT_LIGHTS", "24"}}} },
			{ "SLS2002.vso", ShaderTemplate{"TerrainLODTemplate", {{"VS", ""}}} },
			{ "SLS2003.pso", ShaderTemplate{"TerrainLODTemplate", {{"PS", ""}}} },
			{ "SLS2080.vso", ShaderTemplate{"TerrainFadeTemplate", {{"VS", ""}}} },
			{ "SLS2082.pso", ShaderTemplate{"TerrainFadeTemplate", {{"PS", ""}}} },
		};
	};

	struct TerrainSettings {
		float LightScale;
		float AmbientScale;
		float Roughness;
		float Metallicness;
		float Saturation;
	};
	struct ParallaxSettingsStruct {
		bool Enabled;
		bool HighQuality;
		bool Shadows;
		bool HeightBlend;
		float MaxDistance;
		float Height;
		float ShadowsIntensity;
	};
	struct LODSettingsStruct {
		float NoiseScale;
		float NoiseTile;
	};
	struct TerrainSettingsStruct {
		TerrainSettings Default;
		TerrainSettings Rain;
		TerrainSettings Night;
		TerrainSettings NightRain;
	};
	TerrainSettingsStruct Settings;
	ParallaxSettingsStruct ParallaxSettings;
	LODSettingsStruct LODSettings;

	struct TerrainStruct {
		D3DXVECTOR4		Data;
		D3DXVECTOR4		ExtraData;
	};
	TerrainStruct	Constants;

	struct ParallaxStruct {
		D3DXVECTOR4     Data;
		D3DXVECTOR4     ExtraData;
	};
	ParallaxStruct  ParallaxConstants;

	bool			usePBR;

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();
};