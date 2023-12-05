#pragma once

class SpecularEffect : public EffectRecord
{
public:
	SpecularEffect() : EffectRecord("Specular") {};

	struct SpecularStruct {
		D3DXVECTOR4		EffectStrength;
		D3DXVECTOR4		Data;
	};
	SpecularStruct	Constants;

	void	UpdateConstants();
};