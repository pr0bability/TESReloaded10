#pragma once

class LensEffect : public EffectRecord
{
public:
	LensEffect() : EffectRecord("Lens") {};

	struct LensSettings {
		float MainThreshold;
		float NightThreshold;
		float InteriorThreshold;
	};
	LensSettings	Settings;

	struct LensStruct {
		D3DXVECTOR4		Data;
	};
	LensStruct	Constants;

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();
	bool	ShouldRender();
};