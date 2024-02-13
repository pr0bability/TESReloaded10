#include "CombineDepth.h"


void CombineDepthEffect::UpdateConstants() {
	if (!johnnyguitar) {
		Logger::Log("No JG present");
		NiCamera* Camera = WorldSceneGraph->camera;
		Constants.viewNearZ = Camera->Frustum.Near;
	}
	else {
		Constants.viewNearZ = JG_GetClipDist();
	}
}

void CombineDepthEffect::RegisterTextures() {
	TheTextureManager->InitTexture("TESR_DepthBuffer", &Textures.CombinedDepthTexture, &Textures.CombinedDepthSurface, TheRenderManager->width, TheRenderManager->height, D3DFMT_G32R32F);
}