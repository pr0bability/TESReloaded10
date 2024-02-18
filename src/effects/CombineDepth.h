#pragma once

class CombineDepthEffect : public EffectRecord
{
public:
	CombineDepthEffect() : EffectRecord("CombineDepth") {};

	HMODULE johnnyguitar = nullptr;
	bool(__cdecl* JG_SetClipDist)(float) = nullptr;
	float(__cdecl* JG_GetClipDist)() = nullptr;

	struct CombineDepthStruct {
		float viewNearZ;
	};
	CombineDepthStruct	Constants;

	struct CombineDepthTextures {
		IDirect3DTexture9* CombinedDepthTexture;
		IDirect3DSurface9* CombinedDepthSurface;
	};
	CombineDepthTextures	Textures;

	void	UpdateConstants();
	void	RegisterTextures();
};