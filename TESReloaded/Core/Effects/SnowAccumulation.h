#pragma once

class SnowAccumulationEffect : public EffectRecord
{
public:
	SnowAccumulationEffect() : EffectRecord("SnowAccumulation") {
		Constants.SnowAccumulationAnimator.Initialize(0);
		Constants.Data.w = 0.0f;
	};

	struct SnowAccumulationStruct {
		Animator		SnowAccumulationAnimator;
		D3DXVECTOR4		Data;
		D3DXVECTOR4		Color;
	};
	SnowAccumulationStruct	Constants;

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();
	bool	ShouldRender();
};