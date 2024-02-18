#pragma once

class PointShadowsEffect : public EffectRecord
{
public:
	PointShadowsEffect() : EffectRecord("PointShadows") {};

	struct PointShadowsStruct {
	};
	PointShadowsStruct	Constants;

	void	UpdateConstants() {};
	void	RegisterConstants() {};
	void	UpdateSettings() {};

};