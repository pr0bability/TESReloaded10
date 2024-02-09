#pragma once

class AmbientOcclusionEffect : public EffectRecord
{
public:
	AmbientOcclusionEffect() : EffectRecord("AmbientOcclusion") {};

	struct AmbientOcclusionStruct {
		bool			Enabled;
		D3DXVECTOR4		AOData;
		D3DXVECTOR4		Data;
	};
	AmbientOcclusionStruct	Constants;

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();
	bool	ShouldRender();
};