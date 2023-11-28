#pragma once

class MotionBlurEffect : public EffectRecord
{
public:
	MotionBlurEffect() : EffectRecord("MotionBlur") {};

	struct MotionBlurStruct {
		D3DXVECTOR4		BlurParams;
		D3DXVECTOR4		Data;
		float			oldAngleX;
		float			oldAngleZ;
		float			oldAmountX;
		float			oldAmountY;
		float			oldoldAmountX;
		float			oldoldAmountY;
	};
	MotionBlurStruct	Constants;

	void	UpdateConstants();
};