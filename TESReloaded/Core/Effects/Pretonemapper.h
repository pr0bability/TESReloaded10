#pragma once

class PretonemapperEffect : public EffectRecord
{
public:
	PretonemapperEffect() : EffectRecord("Pretonemapper") {};

	struct PretonemapperStruct {
	};

	void	UpdateConstants() {};

	PretonemapperStruct	Constants;
};