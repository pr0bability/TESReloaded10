#pragma once

class TerrainShaders : public ShaderCollection
{
public:
	TerrainShaders() : ShaderCollection("Terrain") {};

	struct TerrainSettings {
		float LightScale;
		float AmbientScale;
		float Roughness;
		float Metallicness;
		float Specular;
		float Fresnel;
		float Saturation;
	};
	struct TerrainSettingsStruct {
		TerrainSettings Default;
		TerrainSettings Rain;
		TerrainSettings Night;
		TerrainSettings NightRain;
	};
	TerrainSettingsStruct Settings;

	struct TerrainStruct {
		D3DXVECTOR4		Data;
		D3DXVECTOR4		ExtraData;
	};
	TerrainStruct	Constants;

	bool			usePBR;
	float			LodNoiseScale;

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();
};