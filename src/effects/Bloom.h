#pragma once

class BloomEffect : public EffectRecord
{
public:
	BloomEffect() : EffectRecord("Bloom") {};

	struct BloomSettingsStruct {
		D3DXVECTOR4		Resolution[8];
	};
	BloomSettingsStruct Settings;

	struct BloomStruct {
		D3DXVECTOR4		Data;
		D3DXVECTOR4		Resolution;
	};
	BloomStruct		Constants;

	struct BloomTexturesStruct {
		IDirect3DTexture9* BloomTexture[8];
		IDirect3DSurface9* BloomSurface[8];
	};
	BloomTexturesStruct		Textures;

	void	UpdateConstants();
	void	RegisterConstants();
	void	RegisterTextures();
	void	UpdateSettings();

	void	RenderBloomBuffer(IDirect3DSurface9* RenderTarget);
};