#pragma once

class ShadowsInteriorsEffect : public EffectRecord
{
public:
	ShadowsInteriorsEffect() : EffectRecord("ShadowsInteriors") {};

	struct ShadowsInteriorsStruct {
	};
	ShadowsInteriorsStruct	Constants;

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();

};