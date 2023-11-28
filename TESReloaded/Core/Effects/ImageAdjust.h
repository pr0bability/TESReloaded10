#pragma once

class ImageAdjustEffect : public EffectRecord
{
public:
	ImageAdjustEffect() : EffectRecord("ImageAdjust") {};

	struct ImageAdjustStruct {
		D3DXVECTOR4		Data;
		D3DXVECTOR4		DarkColor;
		D3DXVECTOR4		LightColor;
	};
	ImageAdjustStruct	Constants;

	void	UpdateConstants();
};