#include "Bloom.h"

void BloomEffect::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_BloomData", &Constants.Data);
	TheShaderManager->RegisterConstant("TESR_BloomResolution", &Constants.Resolution);
};

void BloomEffect::RegisterTextures() {
	std::string bufferName = "TESR_BloomBuffer";
	int multiple = 1;
	for (int i = 0; i < 6; i++) {
		std::string name = (i>0?bufferName + std::to_string(multiple):bufferName);
		//Logger::Log("creating %s multiple %i %i:%i", name, multiple, TheRenderManager->width / multiple, TheRenderManager->height / multiple);
		multiple *= 2;

		Settings.Resolution[i] = D3DXVECTOR4(
			TheRenderManager->width / multiple, 
			TheRenderManager->height / multiple, 
			1.0f / (float)(TheRenderManager->width / multiple), 
			1.0f / (float)(TheRenderManager->height / multiple)
		);
		TheTextureManager->InitTexture(name.c_str(), &Textures.BloomTexture[i], &Textures.BloomSurface[i], Settings.Resolution[i].x, Settings.Resolution[i].y, D3DFMT_A16B16G16R16F);
	}
};

void BloomEffect::UpdateSettings() {
	Settings.Main.Treshold = TheSettingManager->GetSettingF("Shaders.Bloom.Main", "Treshold");
	Settings.Main.Scale = TheSettingManager->GetSettingF("Shaders.Bloom.Main", "Scale");
	Settings.Main.Strength = TheSettingManager->GetSettingF("Shaders.Bloom.Main", "Strength");
	Settings.Night.Treshold = TheSettingManager->GetSettingF("Shaders.Bloom.Night", "Treshold");
	Settings.Night.Scale = TheSettingManager->GetSettingF("Shaders.Bloom.Night", "Scale");
	Settings.Night.Strength = TheSettingManager->GetSettingF("Shaders.Bloom.Night", "Strength");
	Settings.Interiors.Treshold = TheSettingManager->GetSettingF("Shaders.Bloom.Interiors", "Treshold");
	Settings.Interiors.Scale = TheSettingManager->GetSettingF("Shaders.Bloom.Interiors", "Scale");
	Settings.Interiors.Strength = TheSettingManager->GetSettingF("Shaders.Bloom.Interiors", "Strength");
	Constants.Data.w = Enabled && ShouldRender();
};


void BloomEffect::UpdateConstants() {
	Constants.Data.x = TheShaderManager->GetTransitionValue(Settings.Main.Treshold, Settings.Night.Treshold, Settings.Interiors.Treshold);
	Constants.Data.y = TheShaderManager->GetTransitionValue(Settings.Main.Scale, Settings.Night.Scale, Settings.Interiors.Scale);
	Constants.Data.z = TheShaderManager->GetTransitionValue(Settings.Main.Strength, Settings.Night.Strength, Settings.Interiors.Strength);
};


bool BloomEffect::ShouldRender() {
	return !TheShaderManager->GameState.OverlayIsOn;
};


void BloomEffect::RenderBloomBuffer(IDirect3DSurface9* RenderTarget) {
	if (!Enabled) return; // skip rendering if the effect is disabled

	IDirect3DDevice9* Device = TheRenderManager->device;
	std::string bufferName = "TESR_BloomBuffer";

	Device->SetRenderTarget(0, Textures.BloomSurface[0]);
	Constants.Resolution = Settings.Resolution[0];
	Render(Device, Textures.BloomSurface[0], NULL, 0, false, NULL);

	int multiple = 1;
	int passNumber = 1;

	//progressively blur & downsample
	for (int i = 1; i <= 5; i++) {
		multiple *= 2;
		Constants.Resolution = Settings.Resolution[i];
		Device->SetRenderTarget(0, Textures.BloomSurface[i]);
		Render(Device, Textures.BloomSurface[i], NULL, passNumber, false, NULL);
		passNumber++;
	}

	//progressively blur & upsample and combine
	for (int i = 4; i >= 0; i--) {
		multiple /= 2;
		Constants.Resolution = Settings.Resolution[i];
		Device->SetRenderTarget(0, Textures.BloomSurface[i]);
		Render(Device, Textures.BloomSurface[i], NULL, passNumber, false, NULL);
		passNumber++;
	}

	Device->SetRenderTarget(0, RenderTarget);
	Device->StretchRect(TheTextureManager->RenderedSurface, NULL, RenderTarget, NULL, D3DTEXF_LINEAR);
}