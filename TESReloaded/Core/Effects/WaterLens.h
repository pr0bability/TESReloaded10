#pragma once

class WaterLensEffect : public EffectRecord
{
public:
	WaterLensEffect() : EffectRecord("WaterLens") {};

	struct WaterLensStruct {
		Animator		WaterLensAnimator;
		D3DXVECTOR4		Data;
	};
	WaterLensStruct	Constants;

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();
};
