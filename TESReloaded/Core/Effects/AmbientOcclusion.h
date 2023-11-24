#pragma once

class AmbientOcclusionEffect : public EffectRecord
{
public:

	struct AmbientOcclusionStruct {
		bool			Enabled;
		D3DXVECTOR4		AOData;
		D3DXVECTOR4		Data;
	};

	void	UpdateConstants();

	AmbientOcclusionStruct	Constants;
};