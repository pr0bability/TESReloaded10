#include "Normals.h"

void NormalsEffect::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_NormalsData", &Constants.Data);
}
	
void NormalsEffect::RegisterTextures() {
	TheTextureManager->InitTexture("TESR_NormalsBuffer", &Textures.NormalsTexture, &Textures.NormalsSurface, TheRenderManager->width, TheRenderManager->height, D3DFMT_A16B16G16R16F);
}