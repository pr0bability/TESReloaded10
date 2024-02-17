#pragma once

class ColoringEffect : public EffectRecord
{
public:
	ColoringEffect() : EffectRecord("Coloring") {};

	struct ColoringStruct {
		D3DXVECTOR4		ColorCurve;
		D3DXVECTOR4		EffectGamma;
		D3DXVECTOR4		Data;
		D3DXVECTOR4		Values;
	};

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();

	ColoringStruct	Constants;
};