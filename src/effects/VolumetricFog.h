#pragma once

class VolumetricFogEffect : public EffectRecord
{
public:
	VolumetricFogEffect() : EffectRecord("VolumetricFog") {};

	struct VolumetricFogStruct {
		D3DXVECTOR4		LowFog;
		D3DXVECTOR4		HighFog;
		D3DXVECTOR4		SimpleFog;
		D3DXVECTOR4		Blend;
		D3DXVECTOR4		Height;
		D3DXVECTOR4		Data;
	};
	VolumetricFogStruct	Constants;

	float	Amount;
	float	AmountInteriors;

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();
	bool	ShouldRender();
};