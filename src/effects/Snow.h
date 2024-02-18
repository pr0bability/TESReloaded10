#pragma once

class SnowEffect : public EffectRecord
{
public:
	SnowEffect() : EffectRecord("Snow") {
		Constants.SnowAnimator.Initialize(0);
	};

	struct SnowStruct {
		Animator		SnowAnimator;
		D3DXVECTOR4		Data;
	};
	SnowStruct	Constants;

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();
	bool	ShouldRender();

};