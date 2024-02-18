#include "Bloom.h"

void BloomEffect::RegisterConstants() {};

void BloomEffect::RegisterTextures() {
	TheTextureManager->InitTexture("TESR_BloomBuffer", &Textures.BloomTexture, &Textures.BloomSurface, TheRenderManager->width / 2, TheRenderManager->height /2, D3DFMT_A16B16G16R16F);
};

void BloomEffect::UpdateSettings() {};
void BloomEffect::UpdateConstants() {};