#include "Flashlight.h"


void FlashlightEffect::RegisterConstants() {};

void FlashlightEffect::UpdateSettings() {

	Settings.Color = NiColor(
		TheSettingManager->GetSettingF("Shaders.Flashlight.Main", "ColorR"),
		TheSettingManager->GetSettingF("Shaders.Flashlight.Main", "ColorG"),
		TheSettingManager->GetSettingF("Shaders.Flashlight.Main", "ColorB")
	);
	Settings.Dimmer = TheSettingManager->GetSettingF("Shaders.Flashlight.Main", "Dimmer");
	Settings.ConeAngle = TheSettingManager->GetSettingF("Shaders.Flashlight.Main", "Angle");
	Settings.Distance = TheSettingManager->GetSettingF("Shaders.Flashlight.Main", "Distance");

	if (!SpotLight)
		SpotLight = NiSpotLight::CreateObject();
};

void FlashlightEffect::UpdateConstants() {

	NiPoint3 WeaponPos;
	NiMatrix33 WeaponRot;
	if (Player->isThirdPerson) {
		WeaponPos = Player->ActorSkinInfo->WeaponNode->m_worldTransform.pos;
		WeaponRot = Player->ActorSkinInfo->WeaponNode->m_worldTransform.rot;
	}
	else {
		if (Player->process->IsWeaponOut()) {
			WeaponPos = Player->firstPersonSkinInfo->WeaponNode->m_worldTransform.pos;
			WeaponRot = Player->firstPersonSkinInfo->WeaponNode->m_worldTransform.rot;
		}
		else {
			WeaponPos = WorldSceneGraph->camera->m_worldTransform.pos;
			WeaponRot = WorldSceneGraph->camera->m_worldTransform.rot;
		}
	}

	UInt32* (__cdecl * GetPipboyManager)() = (UInt32 * (__cdecl*)())0x705990;
	bool(__thiscall * IsLightActive)(UInt32 * pPipBoyManager) = (bool(__thiscall*)(UInt32*))0x967700;

	bool pipLightActive = IsLightActive(GetPipboyManager());

	if (pipLightActive) {
		// find and disable pipboy light
		NiNode* PlayerNode = Player->GetNode();
		for (UINT32 i = 0; i < PlayerNode->m_children.capacity; i++) {
			NiAVObject* childNode = PlayerNode->m_children.data[i];
			if (childNode) {
				if (childNode->GetRTTI() == (void*)0x11F4A98) {
					childNode->m_flags |= childNode->APP_CULLED;
				}
			}
		}
	}

	if (SpotLight) {
		SpotLight->Diff = Settings.Color;
		SpotLight->Dimmer = Settings.Dimmer * pipLightActive;
		SpotLight->m_worldTransform.pos = WeaponPos;
		SpotLight->m_worldTransform.rot = WeaponRot;
		SpotLight->m_worldTransform.scale = 1.0f;
		SpotLight->OuterSpotAngle = Settings.ConeAngle;
		SpotLight->Spec = NiColor(Settings.Distance, 0, 0); // radius in r channel
	}

};

bool FlashlightEffect::ShouldRender() { return true; };

