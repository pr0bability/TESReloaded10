#include "CombineDepth.h"


void CombineDepthEffect::UpdateConstants() {
	NiCamera* Camera = WorldSceneGraph->camera;

	if (!johnnyguitar || !JG_GetClipDist) {
		Constants.viewNearZ = Camera->Frustum.Near;
	}
	else {
		float nearZ = JG_GetClipDist();

		Constants.viewNearZ = nearZ ? max(nearZ, 0.3)  : Camera->Frustum.Near;
	}
}

void CombineDepthEffect::RegisterTextures() {
	TheTextureManager->InitTexture("TESR_DepthBuffer", &Textures.CombinedDepthTexture, &Textures.CombinedDepthSurface, TheRenderManager->width, TheRenderManager->height, D3DFMT_G32R32F);
}