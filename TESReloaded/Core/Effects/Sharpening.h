#pragma once

class SharpeningEffect : public EffectRecord
{
public:
	SharpeningEffect() : EffectRecord("Sharpening") {};

	struct SharpeningStruct {
		D3DXVECTOR4		Data;
	};
	SharpeningStruct	Constants;

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();

};