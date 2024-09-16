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
	struct ParallaxSettingsStruct {
		bool Enabled;
		bool HighQuality;
		bool Shadows;
		float Height;
		float MaxDistance;
		float Range;
		float BlendRange;
		float ShadowsFade;
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