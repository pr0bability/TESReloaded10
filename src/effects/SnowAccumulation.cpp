#include "SnowAccumulation.h"

void SnowAccumulationEffect::UpdateConstants() {

	// Snow Accumulation
	if (TheShaderManager->GameState.isSnow && !Constants.SnowAccumulationAnimator.switched) {
		// it just started snowing
		Constants.SnowAccumulationAnimator.switched = true;
		Constants.SnowAccumulationAnimator.Start(increaseRate, 1);
	}
	else if (!TheShaderManager->GameState.isSnow && Constants.SnowAccumulationAnimator.switched) {
		// it just stopped snowing
		Constants.SnowAccumulationAnimator.switched = false;
		Constants.SnowAccumulationAnimator.Start(decreaseRate, 0);
	}

	Constants.Data.w = Constants.SnowAccumulationAnimator.GetValue();
	if (Constants.Data.w) TheShaderManager->orthoRequired = true; // mark ortho map calculation as necessary

}

void SnowAccumulationEffect::UpdateSettings(){
	increaseRate = TheSettingManager->GetSettingF("Shaders.SnowAccumulation.Main", "Increase");
	decreaseRate = TheSettingManager->GetSettingF("Shaders.SnowAccumulation.Main", "Decrease");

	Constants.Data.x = TheSettingManager->GetSettingF("Shaders.SnowAccumulation.Main", "BlurNormDropThreshhold");
	Constants.Data.y = TheSettingManager->GetSettingF("Shaders.SnowAccumulation.Main", "BlurRadiusMultiplier");
	Constants.Data.z = TheSettingManager->GetSettingF("Shaders.SnowAccumulation.Main", "SunPower");
	Constants.Color.x = TheSettingManager->GetSettingF("Shaders.SnowAccumulation.Main", "SnowColorR");
	Constants.Color.y = TheSettingManager->GetSettingF("Shaders.SnowAccumulation.Main", "SnowColorG");
	Constants.Color.z = TheSettingManager->GetSettingF("Shaders.SnowAccumulation.Main", "SnowColorB");
}

void SnowAccumulationEffect::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_SnowAccumulationParams", &Constants.Data);
	TheShaderManager->RegisterConstant("TESR_SnowAccumulationColor", &Constants.Color);
}

bool SnowAccumulationEffect::ShouldRender() { 
	return Constants.Data.w > 0.0f && TheShaderManager->GameState.isExterior && !TheShaderManager->GameState.isUnderwater; 
}