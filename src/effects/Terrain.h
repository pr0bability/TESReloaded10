#pragma once

class TerrainShaders : public ShaderCollection
{
public:
	TerrainShaders() : ShaderCollection("Terrain") {};

	std::map<std::string, ShaderTemplate> Templates() {
		return std::map<std::string, ShaderTemplate>{
			{ "SLS2100.vso", ShaderTemplate{ "TerrainTemplate", {{"VS", ""}} } },
			{ "SLS2092.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "1"}}} },
			{ "SLS2096.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "1"}, {"POINTLIGHT", ""}}} },
			{ "SLS2100.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "2"}}} },
			{ "SLS2104.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "2"}, {"POINTLIGHT", ""}}} },
			{ "SLS2108.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "3"}}} },
			{ "SLS2112.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "3"}, {"POINTLIGHT", ""}}} },
			{ "SLS2116.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "4"}}} },
			{ "SLS2120.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "4"}, {"POINTLIGHT", ""}}} },
			{ "SLS2124.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "5"}}} },
			{ "SLS2128.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "5"}, {"POINTLIGHT", ""}}} },
			{ "SLS2132.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "6"}}} },
			{ "SLS2136.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "6"}, {"POINTLIGHT", ""}}} },
			{ "SLS2140.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "7"}}} },
			{ "SLS2144.pso", ShaderTemplate{"TerrainTemplate", {{"PS", ""}, {"TEX_COUNT", "7"}, {"POINTLIGHT", ""}}} }
		};
	};

	struct TerrainSettings {
		float LightScale;
		float AmbientScale;
		float Roughness;
		float Metallicness;
		float Specular;
		float Fresnel;
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
	struct TerrainSettingsStruct {
		TerrainSettings Default;
		TerrainSettings Rain;
		TerrainSettings Night;
		TerrainSettings NightRain;
	};
	TerrainSettingsStruct Settings;
	ParallaxSettingsStruct ParallaxSettings;

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
	float			LodNoiseScale;

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();
};