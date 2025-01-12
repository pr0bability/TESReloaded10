#pragma once

class SunShadowsEffect : public EffectRecord
{
public:

	SunShadowsEffect() : EffectRecord("SunShadows") {};

	struct SunShadowStruct {
	};
	SunShadowStruct	Constants;

	void	SetCT();

	void	UpdateConstants() {};
	void	RegisterConstants() {};
	void	UpdateSettings() {};

};
