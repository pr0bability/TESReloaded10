#include "Flashlight.h"


void FlashlightEffect::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_FlashLightViewProjTransform", (D3DXVECTOR4*)&Constants.FlashlightViewProj);
	TheShaderManager->RegisterConstant("TESR_FlashLightPosition", (D3DXVECTOR4*)&Constants.FlashlightViewProj);
	TheShaderManager->RegisterConstant("TESR_FlashLightDirection", (D3DXVECTOR4*)&Constants.FlashlightViewProj);
	TheShaderManager->RegisterConstant("TESR_FlashLightColor", (D3DXVECTOR4*)&Constants.FlashlightViewProj);
};

void FlashlightEffect::UpdateSettings() {

	Settings.attachToWeapon = TheSettingManager->GetSettingI("Shaders.Flashlight.Main", "AttachToWeapon");
	Settings.renderShadows = 0; // disabled until fixed
	//Settings.renderShadows = TheSettingManager->GetSettingI("Shaders.Flashlight.Main", "RenderShadows");
	selectedPass = Settings.renderShadows;

	Settings.Offset = NiPoint3(
		TheSettingManager->GetSettingF("Shaders.Flashlight.Main", "OffsetX"),
		TheSettingManager->GetSettingF("Shaders.Flashlight.Main", "OffsetY"),
		TheSettingManager->GetSettingF("Shaders.Flashlight.Main", "OffsetZ")
	);

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

	//TODO: find a better place for this
	UInt32* (__cdecl * GetPipboyManager)() = (UInt32 * (__cdecl*)())0x705990;
	bool(__thiscall * IsLightActive)(UInt32 * pPipBoyManager) = (bool(__thiscall*)(UInt32*))0x967700;

	spotLightActive = Enabled && IsLightActive(GetPipboyManager());

	NiPoint3 WeaponPos;
	NiMatrix33 WeaponRot;
	if (Player->isThirdPerson) {
		WeaponPos = Player->ActorSkinInfo->WeaponNode->m_worldTransform.pos;
		WeaponRot = Player->ActorSkinInfo->WeaponNode->m_worldTransform.rot;
	}
	else {
		if (Player->process->IsWeaponOut() && Settings.attachToWeapon) {
			WeaponPos = Player->firstPersonSkinInfo->WeaponNode->m_worldTransform.pos;
			WeaponRot = Player->firstPersonSkinInfo->WeaponNode->m_worldTransform.rot;
		}
		else {
			WeaponPos = WorldSceneGraph->camera->m_worldTransform.pos;
			WeaponRot = WorldSceneGraph->camera->m_worldTransform.rot;
		}
	}

	// rotate offset in the direction of the cone
	NiPoint3 offset = WeaponRot * Settings.Offset;
	WeaponPos.x += offset.x;
	WeaponPos.y += offset.y;
	WeaponPos.z += offset.z;

	if (SpotLight) {
		if (spotLightActive) {
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

		SpotLight->CastShadows = Settings.renderShadows;
		SpotLight->Diff = Settings.Color;
		SpotLight->Dimmer = Settings.Dimmer * 10.0 * spotLightActive;
		SpotLight->m_worldTransform.pos = WeaponPos;
		SpotLight->m_worldTransform.rot = WeaponRot;
		SpotLight->m_worldTransform.scale = 1.0f;
		SpotLight->OuterSpotAngle = Settings.ConeAngle;
		SpotLight->Spec = NiColor(Settings.Distance * spotLightActive, 0, 0); // radius in r channel

		GetFlashlightViewProj();

	}

};

bool FlashlightEffect::ShouldRender() { return SpotLight && spotLightActive; };


void FlashlightEffect::GetFlashlightViewProj() {
	if (!SpotLight) return;
	
	D3DXVECTOR3 Up = D3DXVECTOR3(0, 0, 1);
	D3DXVECTOR3 Eye = SpotLight->m_worldTransform.pos.toD3DXVEC3();
	D3DXVECTOR3 Direction = D3DXVECTOR3(SpotLight->m_worldTransform.rot.data[0][0], SpotLight->m_worldTransform.rot.data[1][0], SpotLight->m_worldTransform.rot.data[2][0]);
	D3DXVECTOR3 At = Eye + Direction;

	float Radius = SpotLight->Spec.r;
	D3DXMATRIX View, Proj;
	D3DXMatrixPerspectiveFovRH(&Proj, D3DXToRadian(SpotLight->OuterSpotAngle * 2), 1.0f, 0.1f, Radius);
	D3DXMatrixLookAtRH(&View, &Eye, &At, &Up);

	Constants.FlashlightViewProj = View * Proj;
}