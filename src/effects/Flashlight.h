#pragma once

class FlashlightEffect : public EffectRecord
{
public:
	FlashlightEffect() : EffectRecord("Flashlight") {};

	struct FlashlightSettingsStruct {};
	FlashlightSettingsStruct	Settings;

	struct FlashlightStruct {
		D3DXVECTOR4		Data;
	};
	FlashlightStruct	Constants;

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();
	bool	ShouldRender();
};