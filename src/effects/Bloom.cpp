#include "Bloom.h"

void BloomEffect::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_BloomData", &Constants.Data);
	TheShaderManager->RegisterConstant("TESR_BloomResolution", &Constants.Resolution);
};

void BloomEffect::RegisterTextures() {
	std::string bufferName = "TESR_BloomBuffer";
	int multiple = 1;
	for (int i = 0; i < 7; i++) {
		const char* name = i>0?(bufferName + std::to_string(multiple)).c_str():bufferName.c_str();
		Logger::Log("creating %s multiple %i %i:%i", name, multiple, TheRenderManager->width / multiple, TheRenderManager->height / multiple);
		multiple *= 2;

		Settings.Resolution[i] = D3DXVECTOR4(
			TheRenderManager->width / multiple, 
			TheRenderManager->height / multiple, 
			1.0f / (float)(TheRenderManager->width / multiple), 
			1.0f / (float)(TheRenderManager->height / multiple)
		);
		TheTextureManager->InitTexture(name, &Textures.BloomTexture[i], &Textures.BloomSurface[i], Settings.Resolution[i].x, Settings.Resolution[i].y, D3DFMT_A16B16G16R16F);
	}
};

void BloomEffect::UpdateSettings() {
	Constants.Data.x = TheSettingManager->GetSettingF("Shaders.Bloom.Main", "Treshold");
	Constants.Data.y = TheSettingManager->GetSettingF("Shaders.Bloom.Main", "Scale");
	Constants.Data.z = TheSettingManager->GetSettingF("Shaders.Bloom.Main", "Strength");
	Constants.Data.w = Enabled;
};

void BloomEffect::UpdateConstants() {};


void BloomEffect::RenderBloomBuffer(IDirect3DSurface9* RenderTarget) {
	IDirect3DDevice9* Device = TheRenderManager->device;
	std::string bufferName = "TESR_BloomBuffer";

	Device->SetRenderTarget(0, Textures.BloomSurface[0]);
	Constants.Resolution = Settings.Resolution[0];
	Render(Device, Textures.BloomSurface[0], NULL, 0, false, NULL);
	//TheTextureManager->DumpToFile(Textures.BloomTexture[0], "bloom");
	int multiple = 1;
	int passNumber = 1;
	for (int i = 1; i <= 5; i++) {
		multiple *= 2;
		//std::string name = bufferName + std::to_string(multiple);
		//Logger::Log("rendering bloom pass %i to buffer %i : %s", passNumber, i, name);
		Device->SetRenderTarget(0, Textures.BloomSurface[i]);
		Render(Device, Textures.BloomSurface[i], NULL, passNumber, false, NULL);
		//TheTextureManager->DumpToFile(Textures.BloomTexture[i], name.c_str());
	}

	for (int i = 4; i >= 0; i--) {
		multiple /= 2;
		//std::string name = (i> 0 ? bufferName + std::to_string(multiple) : bufferName) + "up";
		Device->SetRenderTarget(0, Textures.BloomSurface[i]);
		Render(Device, Textures.BloomSurface[i], NULL, passNumber, false, NULL);
		//TheTextureManager->DumpToFile(Textures.BloomTexture[i], name.c_str());
	}

	Device->SetRenderTarget(0, RenderTarget);
	Device->StretchRect(TheTextureManager->RenderedSurface, NULL, RenderTarget, NULL, D3DTEXF_LINEAR);
}