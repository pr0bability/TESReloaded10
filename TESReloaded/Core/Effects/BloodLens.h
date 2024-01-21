#pragma once

class BloodLensEffect : public EffectRecord
{
public:
	BloodLensEffect() : EffectRecord("BloodLens") {
		Constants.Percent = 0.0f;
	};

	struct BloodLensStruct {
		D3DXVECTOR4		Params;
		D3DXVECTOR4		BloodColor;
		D3DXVECTOR4		Time;
		float			Percent;
	};
	BloodLensStruct	Constants;

	float effectTime;

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();
	bool	ShouldRender();
};