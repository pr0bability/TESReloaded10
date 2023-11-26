#pragma once

class AvgLumaEffect : public EffectRecord
{
public:
	AvgLumaEffect() : EffectRecord("AvgLuma") {};

	struct AvgLumaStruct {
	};

	void	UpdateConstants() {};

	AvgLumaStruct	Constants;
};