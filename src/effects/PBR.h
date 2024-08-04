#pragma once

class PBRShaders : public ShaderCollection
{
public:
	PBRShaders() : ShaderCollection("PBR") {};

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