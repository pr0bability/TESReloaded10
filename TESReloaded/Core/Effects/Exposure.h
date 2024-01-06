#pragma once

class ExposureEffect : public EffectRecord
{
public:
	ExposureEffect() : EffectRecord("Exposure") {};

	struct ExposureStruct {
		D3DXVECTOR4		Data;
	};
	ExposureStruct	Constants;

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();

};