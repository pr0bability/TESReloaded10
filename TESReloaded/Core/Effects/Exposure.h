#pragma once

class ExposureEffect : public EffectRecord
{
public:
	ExposureEffect() : EffectRecord("Exposure") {};

	struct ValuesStruct {
		float MinBrightness;
		float MaxBrightness;
		float DarkAdaptSpeed;
		float LightAdaptSpeed;
	};

	struct ExposureSettingsStruct {
		ValuesStruct Main;
		ValuesStruct Night;
		ValuesStruct Interiors;
	};
	ExposureSettingsStruct	Settings;

	struct ExposureStruct {
		D3DXVECTOR4		Data;
	};
	ExposureStruct	Constants;

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();

};