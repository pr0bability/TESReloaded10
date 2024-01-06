#pragma once

class PretonemapperEffect : public EffectRecord
{
public:
	PretonemapperEffect() : EffectRecord("Pretonemapper") {};

	struct PretonemapperStruct {
	};
	PretonemapperStruct	Constants;

	void	UpdateConstants() {};
	void	RegisterConstants() {};
	void	UpdateSettings() {};

};