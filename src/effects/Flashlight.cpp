#include "Flashlight.h"


void FlashlightEffect::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_FlashLightViewProjTransform", (D3DXVECTOR4*)&Constants.FlashlightViewProj);
	TheShaderManager->RegisterConstant("TESR_FlashLightPosition", (D3DXVECTOR4*)&Constants.FlashlightViewProj);
	TheShaderManager->RegisterConstant("TESR_FlashLightDirection", (D3DXVECTOR4*)&Constants.FlashlightViewProj);
	TheShaderManager->RegisterConstant("TESR_FlashLightColor", (D3DXVECTOR4*)&Constants.FlashlightViewProj);
};

void FlashlightEffect::UpdateSettings() {

	Settings.attachToWeapon = TheSettingManager->GetSettingI("Shaders.Flashlight.Main", "AttachToWeapon");
	Settings.renderShadows = TheSettingManager->GetSettingI("Shaders.Flashlight.Main", "RenderShadows");
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
	
	if (!SpotLight) return;

	// based on JIP https://github.com/jazzisparis/JIP-LN-NVSE/blob/5a30ac4356ea0e93b9ff357b5031b1e420240a4d/functions_jip/jip_fn_ui.h#L1469
	UInt32 lightIsOn = ThisStdCall<bool>(0x822B90, &Player->magicTarget, (void*)&(*(MagicItem**)(ThisStdCall<uint8_t*>(0x93CCD0, NULL) + 0x18)), 1);

	spotLightActive = Enabled && lightIsOn;
	if (!spotLightActive) {
		// disable light by setting it to 0 dimmer & radius
		SpotLight->Dimmer = 0;
		SpotLight->Spec = NiColor(0, 0, 0);
		SpotLight->CastShadows = false;
		return;
	}

	NiPoint3 WeaponPos;
	NiMatrix33 WeaponRot;
	bool melee = false;
	if (Player->process->IsWeaponOut()) {
		melee = !Player->ActorSkinInfo->WeaponForm ||
			Player->ActorSkinInfo->WeaponForm->weaponType == TESObjectWEAP::WeaponType::kWeapType_HandToHandMelee ||
			Player->ActorSkinInfo->WeaponForm->weaponType == TESObjectWEAP::WeaponType::kWeapType_OneHandMelee ||
			Player->ActorSkinInfo->WeaponForm->weaponType == TESObjectWEAP::WeaponType::kWeapType_TwoHandMelee ||
			Player->ActorSkinInfo->WeaponForm->weaponType == TESObjectWEAP::WeaponType::kWeapType_OneHandGrenade ||
			Player->ActorSkinInfo->WeaponForm->weaponType == TESObjectWEAP::WeaponType::kWeapType_OneHandMine ||
			Player->ActorSkinInfo->WeaponForm->weaponType == TESObjectWEAP::WeaponType::kWeapType_OneHandLunchboxMine ||
			Player->ActorSkinInfo->WeaponForm->weaponType == TESObjectWEAP::WeaponType::kWeapType_OneHandThrown;
	}

	if (Player->isThirdPerson) {
		if (Settings.attachToWeapon && !melee && Player->process->IsWeaponOut() && Player->IsReloading() && Player->IsAiming()) {
			WeaponPos = Player->ActorSkinInfo->WeaponNode->m_worldTransform.pos;
			WeaponRot = Player->ActorSkinInfo->WeaponNode->m_worldTransform.rot;
		}
		else {
			// matrix that will rotate 90 degrees on the Z then X axis
			NiMatrix33 rotation = NiMatrix33();
			rotation.data[0][0] = 0;
			rotation.data[0][1] = -1;
			rotation.data[0][2] = 0;
			rotation.data[1][0] = 0;
			rotation.data[1][1] = 0;
			rotation.data[1][2] = 1;
			rotation.data[2][0] = -1;
			rotation.data[2][1] = 0;
			rotation.data[2][2] = 0;

			WeaponPos = Player->ActorSkinInfo->HeadNode->m_worldTransform.pos;
			WeaponRot = Player->ActorSkinInfo->HeadNode->m_worldTransform.rot;
			rotation = rotation * WeaponRot; // we place the rotation matrix in the referential of the bone
			WeaponRot = WeaponRot * rotation; // we apply it
		}
	}
	else {
		// isReloading is inverted and returns 0 when player is reloading
		if (Settings.attachToWeapon && !melee && Player->process->IsWeaponOut() && Player->IsReloading()) {
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
	SpotLight->Dimmer = Settings.Dimmer * 10.0;
	SpotLight->m_worldTransform.pos = WeaponPos;
	SpotLight->m_worldTransform.rot = WeaponRot;
	SpotLight->m_worldTransform.scale = 1.0f;
	SpotLight->OuterSpotAngle = Settings.ConeAngle;
	SpotLight->Spec = NiColor(Settings.Distance, 0, 0); // radius in r channel

	GetFlashlightViewProj();
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