#include "CombineDepth.h"

void CombineDepthEffect::RegisterTextures() {
	TheTextureManager->InitTexture("TESR_DepthBuffer", &Textures.CombinedDepthTexture, &Textures.CombinedDepthSurface, TheRenderManager->width, TheRenderManager->height, D3DFMT_G32R32F);
}