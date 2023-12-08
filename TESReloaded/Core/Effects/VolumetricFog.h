#pragma once

class VolumetricFogEffect : public EffectRecord
{
public:
	VolumetricFogEffect() : EffectRecord("VolumetricFog") {};

	struct VolumetricFogStruct {
		D3DXVECTOR4		Data;
	};
	VolumetricFogStruct	Constants;

	void	UpdateConstants();
};