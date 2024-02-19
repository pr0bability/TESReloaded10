#include "Bloom.h"

void BloomEffect::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_BloomData", &Constants.Data);
};

void BloomEffect::RegisterTextures() {
	TheTextureManager->InitTexture("TESR_BloomBuffer", &Textures.BloomTexture, &Textures.BloomSurface, TheRenderManager->width / 2, TheRenderManager->height /2, D3DFMT_A16B16G16R16F);
};

void BloomEffect::UpdateSettings() {
	Constants.Data.x = TheSettingManager->GetSettingF("Shaders.Bloom.Main", "Treshold");
	Constants.Data.y = TheSettingManager->GetSettingF("Shaders.Bloom.Main", "Scale");
	Constants.Data.z = TheSettingManager->GetSettingF("Shaders.Bloom.Main", "Strength");
	Constants.Data.w = Enabled;
};

void BloomEffect::UpdateConstants() {};