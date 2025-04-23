#pragma once

static bool EquippingBow = 0;

// This function is the bool 0x005E3350.
// It cannot be detoured and modified at global level. We need to modify it only when it is called from the specific hook.
__declspec(naked) void ReadyWeaponHook() {

	__asm {
		push    esi
		mov     esi, ecx
		mov     eax, dword ptr [esi]
		mov     edx, dword ptr [eax+0x1A0]
		call    edx
		test    al, al
		jnz     short loc_5E33A6
		mov     eax, dword ptr [esi]
		mov     edx, dword ptr [eax+0x198]
		push    0
		mov     ecx, esi
		call    edx
		test    al, al
		jnz     short loc_5E33A6
		mov     eax, dword ptr [esi]
		mov     edx, dword ptr [eax+0x19C]
		mov     ecx, esi
		call    edx
		test    al, al
		jnz     short loc_5E33A6
		mov     eax, dword ptr [esi+0x0B0]
		cmp     eax, 5
		jz      short loc_5E33A6
		cmp     eax, 3
		jz      short loc_5E33A6
		mov     eax, dword ptr [esi]
		mov     edx, dword ptr [eax+0x18C]
		mov     ecx, esi
		call    edx
		
		cmp		eax, 4
		jnz		short loc_jumphack
		xor		eax, eax

	loc_jumphack:
		test    eax, eax
		setz    al
		pop     esi
		retn
	loc_5E33A6:
		xor     al, al
		pop     esi
		retn
	}

}

__declspec(naked) void ActorReadyWeaponSittingHook() {
	
	__asm {
		mov     edx, dword ptr [esi]
		mov     eax, dword ptr [edx+0x18C]
		mov     ecx, esi
		call	eax

		cmp		eax, 4
		jnz		short loc_jumphack
		xor		eax, eax

	loc_jumphack:
		jmp		Jumpers::Mounted::ActorReadyWeaponSittingReturn
	}
	
}

__declspec(naked) void PlayerAttackHook() {

	__asm {
		mov		ecx, dword ptr [ebx+0x058]
		mov		edx, dword ptr [ecx]
		mov		eax, dword ptr [edx+0x36C]
		call	eax

		cmp		eax, 4
		jnz		short loc_jumphack
		mov		ecx, dword ptr [ebx+0x0D4]
		test	ecx, ecx
		jz		short loc_jumphack
		xor		eax, eax

	loc_jumphack:
		jmp		Jumpers::Mounted::PlayerAttackReturn
	}

}

static int HittingMountedCreature(Actor* Attacker, TESObjectREFR* Target) {

	int result = 0;

	if (Target) {
		Actor* HorseOrRider = Attacker->horseOrRider;
		if (HorseOrRider && HorseOrRider == Target) result = 1;
	}
	return result;

}

__declspec(naked) void HittingMountedCreatureHook() {

	__asm {
		add		esp, 8
		mov		esi, eax
		pushad
		push	esi
		push	edi
		call	HittingMountedCreature
		pop		edi
		pop		esi
		test	eax, eax
		jz		short loc_jumphack

		popad
		xor		esi, esi
		jmp		Jumpers::Mounted::HittingMountedCreatureReturn

loc_jumphack:
		popad
		jmp		Jumpers::Mounted::HittingMountedCreatureReturn
	}

}

static void HideWeapon(Character* Act, UInt8 SitSleepState) {

	EquipmentManager::PositionRotationType Type = EquipmentManager::PositionRotationType::None;

	if (Act->formType == TESForm::FormType::kFormType_ACHR && Act->process->GetProcessLevel() == 0) {
		HighProcessEx* Process = (HighProcessEx*)Act->process;
		InventoryChanges::EntryData* WeaponData = Process->equippedWeaponData;
		InventoryChanges::EntryData* LeftWeaponData = Process->EquippedLeftWeaponData;
		if (SitSleepState < 3 || SitSleepState > 5) {
			if (LeftWeaponData)
				TheEquipmentManager->SetAnimGroup(Process, 21, 2, "Characters\\_Male\\OnehandAttackRight_OR_dual.kf");
			else
				TheEquipmentManager->ResetAnimGroup(Process, 21, 2);
		}
		else
			TheEquipmentManager->SetAnimGroup(Process, 21, 2, "Characters\\_Male\\OnehandAttackRight_OR_mounted.kf");
		if (!Process->WeaponState) {
			if (WeaponData) {
				TESObjectWEAP* Weapon = (TESObjectWEAP*)WeaponData->type;
				EquipmentManager::PositionRotationType Type = EquipmentManager::PositionRotationType::None;
				if (Weapon->weaponType == TESObjectWEAP::kWeapType_BladeTwoHand || Weapon->weaponType == TESObjectWEAP::kWeapType_BluntTwoHand)
					Type = EquipmentManager::PositionRotationType::TwoHandWeapon;
				else if (Weapon->weaponType == TESObjectWEAP::kWeapType_Staff)
					Type = EquipmentManager::PositionRotationType::Staff;
				else if (Weapon->weaponType == TESObjectWEAP::kWeapType_Bow)
					Type = EquipmentManager::PositionRotationType::Bow;
				NiAVObject* Object = Act->ActorSkinInfo->WeaponObject;
				if (Object) {
					TheEquipmentManager->SetRotationPosition(Object, Type, SitSleepState);
					if (Act == Player) {
						Object = Player->firstPersonSkinInfo->WeaponObject;
						TheEquipmentManager->SetRotationPosition(Object, Type, SitSleepState);
					}
				}
			}
			if (LeftWeaponData) {
				NiNode* Object = Process->LeftWeaponObject;
				TheEquipmentManager->SetRotationPosition(Object, EquipmentManager::PositionRotationType::Weapon, SitSleepState);
				if (Act == Player) {
					Object = Process->LeftWeaponObjectFirst;
					TheEquipmentManager->SetRotationPosition(Object, EquipmentManager::PositionRotationType::Weapon, SitSleepState);
				}
			}
		}
	}
}

__declspec(naked) void HideWeaponHook() {

	__asm {
		pushad
		push	ebx
		push	ebp
		call	HideWeapon
		add		esp, 8
		popad
		jmp		Jumpers::Mounted::HideWeaponReturn
	}

}

__declspec(naked) void BowEquipHook() {

	__asm {
		mov		EquippingBow, 1
		mov		eax, 0x005E13D0
		call	eax
		mov		EquippingBow, 0
		jmp		Jumpers::Mounted::BowEquipReturn
	}

}

__declspec(naked) void BowUnequipHook() {

	__asm {
		mov		eax, 0x005E13D0
		call	eax
		mov		eax, [ebp+0x0D4]
		test	eax, eax
		jz		short skip_blend
		push	1
		mov		ecx, eax
		mov		eax, 0x005E13D0
		call	eax
	skip_blend:
		jmp		Jumpers::Mounted::BowUnequipReturn
	}

}

__declspec(naked) void AnimControllerHook() {

	__asm {
		mov		edi, eax
		cmp		edi, ebp
		jz		short skip_controller
		cmp		EquippingBow, 1
		jz		short skip_controller
		jmp		Jumpers::Mounted::AnimControllerReturn1

	skip_controller:
		jmp		Jumpers::Mounted::AnimControllerReturn2
	}

}


static int HorsePalette(NiNode* Node, NiDefaultAVObjectPalette* Palette) {

	int result = 0;
	Actor* HorseOrRider = Player->horseOrRider;

	if (HorseOrRider && HorseOrRider->niNode == Palette->niNode && Node->m_pcName && !memcmp(Node->m_pcName, "Player1stPerson", 15)) result = 1;
	return result;

}

__declspec(naked) void HorsePaletteHook() {

	__asm {
		push	ebx
		mov		ebx, dword ptr ss:[esp+0x10]
		test	ebx, ebx
		jz		short skip_node
		pushad
		push	ebx
		push	esi
		call	HorsePalette
		add		esp, 8
		test	al, al
		popad
		jne		short skip_node
		jmp		Jumpers::Mounted::HorsePaletteReturn1

	skip_node:
		jmp		Jumpers::Mounted::HorsePaletteReturn2
	}

}