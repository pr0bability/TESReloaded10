#pragma once

class DepthOfFieldEffect : public EffectRecord
{
public:
	DepthOfFieldEffect() : EffectRecord("DepthOfField") {};

	struct DepthOfFieldStruct {
		bool			Enabled;
		D3DXVECTOR4		Blur;
		D3DXVECTOR4		Data;
	};
	DepthOfFieldStruct	Constants;

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();

};