#include "FakeImageSpaceEffect.h"

void* ImageSpaceEffectAfterTonemapping::vtable[8] = {};

ImageSpaceEffectAfterTonemapping* ImageSpaceEffectAfterTonemapping::CreateObject() {
	ImageSpaceEffectAfterTonemapping* pShader = NiNew<ImageSpaceEffectAfterTonemapping>();

	// Call constructor for ImageSpaceShader and set vtable pointers.
	ThisCall(0xBA4080, pShader);

	if (!vtable[0]) [[unlikely]] {
		for (uint32_t i = 0; i < _countof(vtable); i++) {
			vtable[i] = ((void***)pShader)[0][i];
		}

		ReplaceVTableEntry(vtable, 1, &ImageSpaceEffectAfterTonemapping::RenderEx);
		ReplaceVTableEntry(vtable, 6, &ImageSpaceEffectAfterTonemapping::IsActiveEx);

	}
	((DWORD*)pShader)[0] = (DWORD)vtable;

	pShader->Textures.SetSize(2);

	return pShader;
}

void ImageSpaceEffectAfterTonemapping::RenderEx(NiTriShape* apScreenShape, NiDX9Renderer* apRenderer, ImageSpaceEffectParam* apParam, bool bEndFrame) {
	// Call vanilla COPY shader just to do the RT setup as expected.
	ImageSpaceManager::GetSingleton()->RenderEffect(ImageSpaceManager::IS_SHADER_COPY, apRenderer, Textures.data[1]->GetRenderedTexture(), Textures.data[0]->GetRenderedTexture(), nullptr, true);

	BSRenderedTexture* pOutput = Textures.data[0]->GetRenderedTexture();

	IDirect3DSurface9* pOutputSurface = nullptr;
	if (pOutput)
		pOutputSurface = pOutput->RenderTargetGroups[0]->RenderTargets[0]->data->Surface;
	else if (TheRenderManager->currentRTGroup)
		pOutputSurface = TheRenderManager->currentRTGroup->RenderTargets[0]->data->Surface;

	if (!pOutputSurface)
		return;

	if (!TheSettingManager->SettingsMain.Main.RenderPreTonemapping) TheShaderManager->RenderEffectsPreTonemapping(pOutputSurface);
	TheShaderManager->RenderEffects(pOutputSurface);
}

bool ImageSpaceEffectAfterTonemapping::IsActiveEx() {
	return TheShaderManager->RenderAfterTonemapping;
}
