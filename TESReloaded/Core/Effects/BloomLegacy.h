#pragma once

class BloomLegacyEffect : public EffectRecord
{
public:
	BloomLegacyEffect() : EffectRecord("BloomLegacy") {};

	struct BloomLegacyStruct {
		D3DXVECTOR4		Data;
		D3DXVECTOR4		Values;
	};

	void	UpdateConstants();

	BloomLegacyStruct	Constants;
};