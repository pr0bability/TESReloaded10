#pragma once

class WetWorldEffect : public EffectRecord
{
public:
	WetWorldEffect() : EffectRecord("WetWorld") {
		Constants.Data.x = 0.0f;
		Constants.Data.y = 0.0f;
		Constants.Data.z = 0.0f;
		Constants.PuddlesAnimator.Initialize(0);
	};

	struct WetWorldStruct {
		Animator		PuddlesAnimator;
		D3DXVECTOR4		Coeffs;
		D3DXVECTOR4		Data;
	};
	WetWorldStruct	Constants;

	void	UpdateConstants();
};
