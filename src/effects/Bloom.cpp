#include <algorithm>

#include "Bloom.h"

void BloomEffect::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_BloomData", &Constants.Data);
	TheShaderManager->RegisterConstant("TESR_BloomExtraData", &Constants.ExtraData);
	TheShaderManager->RegisterConstant("TESR_BloomResolution", &Constants.Resolution);
};

void BloomEffect::RegisterTextures() {
	std::string bufferName = "TESR_BloomBuffer";
	int width = TheRenderManager->width;
	int height = TheRenderManager->height;
	int multiple = 1;
	for (int i = 0; i < MaxPasses; i++) {
		std::string name = (i>0?bufferName + std::to_string(multiple):bufferName);
		multiple *= 2;

		width /= 2;
		height /= 2;

		//Logger::Log("creating %s multiple %i %i:%i", name, multiple, TheRenderManager->width / multiple, TheRenderManager->height / multiple);
		Settings.Resolution[i] = D3DXVECTOR4(
			width, 
			height, 
			1.0f / (float)(width), 
			1.0f / (float)(height)
		);

		TheTextureManager->InitTexture(name.c_str(), &Textures.BloomTexture[i], &Textures.BloomSurface[i], Settings.Resolution[i].x, Settings.Resolution[i].y, D3DFMT_A16B16G16R16F);
		TheShaderManager->CreateFrameVertex(width, height, &Textures.BloomVertexBuffer[i]);
	}
};

void BloomEffect::UpdateSettings() {
	Settings.Main.Passes = TheSettingManager->GetSettingF("Shaders.Bloom.Main", "Passes");
	Settings.Main.PassBlending = TheSettingManager->GetSettingF("Shaders.Bloom.Main", "PassBlending");

	Settings.Night.Passes = TheSettingManager->GetSettingF("Shaders.Bloom.Night", "Passes");
	Settings.Night.PassBlending = TheSettingManager->GetSettingF("Shaders.Bloom.Night", "PassBlending");

	Settings.Interiors.Passes = TheSettingManager->GetSettingF("Shaders.Bloom.Interiors", "Passes");
	Settings.Interiors.PassBlending = TheSettingManager->GetSettingF("Shaders.Bloom.Interiors", "PassBlending");
};


void BloomEffect::UpdateConstants() {
	// .x and .y updated dynamically to reflect filterRadius.
	Constants.Data.x = 0.005f;
	Constants.Data.y = 0.005f * ((float)TheRenderManager->width / TheRenderManager->height);  // Scaled for non-square resolutions.
	Constants.Data.z = std::clamp(TheShaderManager->GetTransitionValue(Settings.Main.PassBlending, Settings.Night.PassBlending, Settings.Interiors.PassBlending), 0.0f, 1.0f);
	Constants.Data.w = 1.0f / std::clamp(TheShaderManager->GetTransitionValue(Settings.Main.Passes, Settings.Night.Passes, Settings.Interiors.Passes), 2.0f, 8.0f);

	// Flag for tonemapping.
	Constants.ExtraData.x = Enabled && (!TheShaderManager->GameState.OverlayIsOn);
};


bool BloomEffect::SwitchEffect() {
	EffectRecord::SwitchEffect();

	if (!Enabled) {
		// Clear the bloom buffer.
		IDirect3DDevice9* Device = TheRenderManager->device;
		Device->ColorFill(Textures.BloomSurface[0], NULL, D3DCOLOR_ARGB(255, 0, 0, 0));
	}

	Constants.ExtraData.x = Enabled;

	return Enabled;
}

/*
* Renders a single bloom pass to current render target, using the passed in technique. Clears target if specified.
*/
void BloomEffect::RenderPass(IDirect3DDevice9* Device, UINT techniqueIndex, bool ClearRenderTarget) {
	try {
		D3DXHANDLE technique = Effect->GetTechnique(techniqueIndex);
		Effect->SetTechnique(technique);
		SetCT(); // update the constant table
		UINT Passes;
		Effect->Begin(&Passes, NULL);
		for (UINT p = 0; p < Passes; p++) {
			if (ClearRenderTarget) Device->Clear(0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 0, 0, 0), 1.0f, 0L);
			Effect->BeginPass(p);
			Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
			Effect->EndPass();
		}
		Effect->End();
	}
	catch (const std::exception& e) {
		Logger::Log("Error during rendering of effect %s: %s", Name, e.what());
	}
}

/*
* Renders the final bloom buffer by iteratively downsampling and upsampling with intermediary blur.
*/
void BloomEffect::RenderBloomBuffer(IDirect3DSurface9* RenderTarget) {
	if (!Enabled) {
		renderTime = 0.0f;
		return; // skip rendering if the effect is disabled
	}

	auto timer = TimeLogger();

	IDirect3DDevice9* Device = TheRenderManager->device;
	NiDX9RenderState* RenderState = TheRenderManager->renderState;

	const int passes = std::clamp((int) TheShaderManager->GetTransitionValue(Settings.Main.Passes, Settings.Night.Passes, Settings.Interiors.Passes), 2, 8);

	int passNumber = 0;

	//progressively blur & downsample
	for (int i = 0; i < passes; i++) {
		Device->SetStreamSource(0, Textures.BloomVertexBuffer[i], 0, sizeof(FrameVS)); // Set correct vertex buffer for given resolution.
		Device->SetRenderTarget(0, Textures.BloomSurface[i]);  // Render to correct bloom buffer.

		Constants.Resolution = Settings.Resolution[i];
		RenderPass(Device, passNumber, true);
		passNumber++;
	}

	passNumber += 2 * (MaxPasses - passes);

	//progressively blur & upsample and combine
	for (int i = passes - 2; i >= 0; i--) {
		Device->SetStreamSource(0, Textures.BloomVertexBuffer[i], 0, sizeof(FrameVS)); // Set correct vertex buffer for given resolution.
		Device->SetRenderTarget(0, Textures.BloomSurface[i]);  // Render to correct bloom buffer.

		if (Constants.Data.z > 0.0f) {
			Constants.Data.x = Settings.Resolution[i+1].z; // Pixel size x axis of the upsampled texture.
			Constants.Data.y = Settings.Resolution[i+1].w; // Pixel size y axis of the upsampled texture.
		}
		Constants.Resolution = Settings.Resolution[i];
		RenderPass(Device, passNumber, false);
		passNumber++;
	}

	Device->SetStreamSource(0, TheShaderManager->FrameVertex, 0, sizeof(FrameVS)); // Reset vertex buffer for other effects.
	Device->SetRenderTarget(0, RenderTarget);  // Reset render target for other effects.

	renderTime = timer.LogTime("EffectRecord::Render Bloom");
}
