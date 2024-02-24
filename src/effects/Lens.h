#pragma once

class LensEffect : public EffectRecord
{
public:
	LensEffect() : EffectRecord("Lens") {};

	struct LensSettingsStruct {
		float bloomExponent;
		float strength;
		float smudginess;
	};

	struct LensSettings {
		LensSettingsStruct Main;
		LensSettingsStruct Interiors;
		LensSettingsStruct Night;
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