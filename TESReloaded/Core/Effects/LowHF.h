#pragma once

class LowHFEffect : public EffectRecord
{
public:
	LowHFEffect() : EffectRecord("LowHF") {};

	struct LowHFStruct {
		D3DXVECTOR4		Data;
		float			HealthCoeff;
		float			FatigueCoeff;
	};
	LowHFStruct	Constants;

	void	UpdateConstants();
};
