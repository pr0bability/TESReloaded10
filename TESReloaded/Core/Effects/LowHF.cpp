#include "LowHF.h"

void LowHFEffect::UpdateConstants() {
	float PlayerHealthPercent = (float)Player->GetActorValue(Actor::ActorVal::kActorVal_Health) / (float)Player->GetBaseActorValue(Actor::ActorVal::kActorVal_Health);
	float PlayerFatiguePercent = (float)Player->GetActorValue(Actor::ActorVal::kActorVal_Stamina) / (float)Player->GetBaseActorValue(Actor::ActorVal::kActorVal_Stamina);

	Constants.Data.x = 0.0f;
	Constants.Data.y = 0.0f;
	Constants.Data.z = 0.0f;
	Constants.Data.w = 0.0f;

	float healthLimit = TheSettingManager->GetSettingF("Shaders.LowHF.Main", "HealthLimit");
	if (Player->IsAlive()) {
		Constants.HealthCoeff = 1.0f - PlayerHealthPercent / healthLimit;
		Constants.FatigueCoeff = 1.0f - PlayerFatiguePercent / TheSettingManager->GetSettingF("Shaders.LowHF.Main", "FatigueLimit");
		if (PlayerHealthPercent < healthLimit) {
			Constants.Data.x = Constants.HealthCoeff * TheSettingManager->GetSettingF("Shaders.LowHF.Main", "LumaMultiplier");
			Constants.Data.y = Constants.HealthCoeff * 0.01f * TheSettingManager->GetSettingF("Shaders.LowHF.Main", "BlurMultiplier");
			Constants.Data.z = Constants.HealthCoeff * 20.0f * TheSettingManager->GetSettingF("Shaders.LowHF.Main", "VignetteMultiplier");
			Constants.Data.w = (1.0f - Constants.HealthCoeff) * TheSettingManager->GetSettingF("Shaders.LowHF.Main", "DarknessMultiplier");
		}
		if (!Constants.Data.x && PlayerFatiguePercent < TheSettingManager->GetSettingF("Shaders.LowHF.Main", "FatigueLimit"))
			Constants.Data.x = Constants.FatigueCoeff * TheSettingManager->GetSettingF("Shaders.LowHF.Main", "LumaMultiplier");
	}
}

void LowHFEffect::UpdateSettings() {}

void LowHFEffect::RegisterConstants() {
	TheShaderManager->ConstantsTable["TESR_LowHFData"] = &Constants.Data;
}