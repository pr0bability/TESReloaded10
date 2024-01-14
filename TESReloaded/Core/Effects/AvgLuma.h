#pragma once

class AvgLumaEffect : public EffectRecord
{
public:
	AvgLumaEffect() : EffectRecord("AvgLuma") {};

	struct AvgLumaStruct {
	};
	AvgLumaStruct	Constants;

	struct AvgLumaTextures {
		IDirect3DTexture9* AvgLumaTexture;
		IDirect3DSurface9* AvgLumaSurface;
	};
	AvgLumaTextures	Textures;

	void	UpdateConstants() {};
	void	RegisterTextures();
};