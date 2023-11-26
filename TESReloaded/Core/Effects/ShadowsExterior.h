#pragma once

class ShadowsExteriorEffect : public EffectRecord
{
public:

	ShadowsExteriorEffect() : EffectRecord("ShadowsExterior") {};

	struct ShadowStruct {
		D3DXVECTOR4		Data;
		D3DXVECTOR4		ScreenSpaceData;
		D3DXVECTOR4		OrthoData;
	};

	void	UpdateConstants();

	ShadowStruct	Constants;
};