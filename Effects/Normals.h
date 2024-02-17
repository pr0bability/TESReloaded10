#pragma once

class NormalsEffect : public EffectRecord
{
public:
	NormalsEffect() : EffectRecord("Normals") {};

	struct NormalsStruct {
		D3DXVECTOR4		Data;
	};
	NormalsStruct	Constants;

	struct NormalTextures {
		IDirect3DTexture9* NormalsTexture;
		IDirect3DSurface9* NormalsSurface;
	};
	NormalTextures	Textures;

	void	UpdateConstants() {};
	void	UpdateSettings() {};
	void	RegisterConstants();
	void	RegisterTextures();
};
