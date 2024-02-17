#pragma once

class BloomLegacyEffect : public EffectRecord
{
public:
	BloomLegacyEffect() : EffectRecord("BloomLegacy") {};

	struct ValuesStruct {
		float Luminance;
		float MiddleGray;
		float WhiteCutOff;
		float BloomIntensity;
		float OriginalIntensity;
		float BloomSaturation;
		float OriginalSaturation;
	};
	struct BloomLegacySettingsStruct {
		ValuesStruct Main;
		ValuesStruct Interiors;
	};
	BloomLegacySettingsStruct Settings;

	struct BloomLegacyStruct {
		D3DXVECTOR4		Data;
		D3DXVECTOR4		Values;
	};

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();


	BloomLegacyStruct	Constants;
};