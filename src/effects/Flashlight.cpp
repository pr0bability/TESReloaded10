#include "Flashlight.h"

void FlashlightEffect::RegisterConstants() {};

void FlashlightEffect::UpdateSettings() {
	Logger::Log("Creating spotlight");
	return;
	if (!SpotLight)
		SpotLight = NiSpotLight::CreateObject();
};

void FlashlightEffect::UpdateConstants() {

	if (SpotLight) {
		Logger::Log("Setting up spotlight");

		SpotLight->Diff = NiColor(1, 1, 0.5);
		SpotLight->Dimmer = 1.0f;
		SpotLight->m_worldTransform.pos = NiPoint3(TheRenderManager->CameraPosition.x, TheRenderManager->CameraPosition.y, TheRenderManager->CameraPosition.z + 100);
		SpotLight->m_worldTransform.rot = WorldSceneGraph->camera->m_worldTransform.rot;
		SpotLight->m_worldTransform.scale = 1.0f;
		SpotLight->OuterSpotAngle = 20.0f;
		SpotLight->Spec = NiColor(1000, 0, 0); // radius in r channel
	}

};

bool FlashlightEffect::ShouldRender() { return true; };

