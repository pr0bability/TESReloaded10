#pragma once

class TonemappingShaders : public ShaderCollection
{
public:
	TonemappingShaders() : ShaderCollection("Tonemapping") {};

	struct ValuesStruct {
		float HighlightSaturation;
		float WeatherContrast;
		float ToneMappingColor;
		float Linearization;
		float BloomStrength;
		float BloomExponent;
		float WeatherModifier;
		float TonemapWhitePoint;
		float TonemappingMode;
		float Exposure;
		float Saturation;
		float Gamma;

		float TonemapContrast;
		float TonemapBrightness;
		float TonemapMidpoint;
		float TonemapShoulder;
	};

	struct TonemappingSettingsStruct {
		ValuesStruct Main;
		ValuesStruct Night;
		ValuesStruct Interiors;
	};
	TonemappingSettingsStruct Settings;

	struct TonemappingStruct {
		D3DXVECTOR4		ToneMapping;
		D3DXVECTOR4		BloomData;
		D3DXVECTOR4		SkyData;
		D3DXVECTOR4		HDRData;
		D3DXVECTOR4		LotteData;
		float			PointLightMult;
	};
	TonemappingStruct Constants;
	
	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();
};