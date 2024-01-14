#pragma once

class ShadowsExteriorEffect : public EffectRecord
{
public:
	ShadowsExteriorEffect() : EffectRecord("ShadowsExteriors") {};

	struct ShadowStruct {
		D3DXVECTOR4		Data;
		D3DXVECTOR4		ScreenSpaceData;
		D3DXVECTOR4		OrthoData;
		D3DXVECTOR4		ShadowFade;
	};
	ShadowStruct	Constants;

	struct ShadowTextures {
		IDirect3DTexture9* ShadowPassTexture;
		IDirect3DSurface9* ShadowPassSurface;
	};
	ShadowTextures	Textures;

	void	UpdateConstants();
	void	UpdateSettings();
	void	RegisterConstants();
	void	RegisterTextures();

};