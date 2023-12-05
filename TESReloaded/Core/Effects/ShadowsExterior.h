#pragma once

class ShadowsExteriorEffect : public EffectRecord
{
public:
	ShadowsExteriorEffect() : EffectRecord("ShadowsExteriors") { Logger::Log("new shadows exterior effect"); };

	struct ShadowStruct {
		D3DXVECTOR4		Data;
		D3DXVECTOR4		ScreenSpaceData;
		D3DXVECTOR4		OrthoData;
	};
	ShadowStruct	Constants;

	void	UpdateConstants();
};