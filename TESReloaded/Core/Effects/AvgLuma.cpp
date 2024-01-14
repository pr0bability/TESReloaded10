#include "AvgLuma.h"

void AvgLumaEffect::RegisterTextures() {
	TheTextureManager->InitTexture("TESR_AvgLumaBuffer", &Textures.AvgLumaTexture, &Textures.AvgLumaSurface, 1, 1, D3DFMT_A16B16G16R16F);
}