#include "WaterLens.h"

void WaterLensEffect::UpdateConstants() {

	if (TheShaderManager->GameState.isUnderwater && Constants.WaterLensAnimator.switched == false) {
		Constants.WaterLensAnimator.switched = true;
		Constants.WaterLensAnimator.Start(0.0, 0);
	}else if (!TheShaderManager->GameState.isUnderwater && Constants.WaterLensAnimator.switched == true) {
		Constants.WaterLensAnimator.switched = false;
		// start the waterlens effect and animate it fading
		 Constants.WaterLensAnimator.Initialize(1);
		 Constants.WaterLensAnimator.Start(0.01, 0);
	}

	Constants.Data.w = TheSettingManager->GetSettingF("Shaders.WaterLens.Main", "Amount") * Constants.WaterLensAnimator.GetValue();
}

void WaterLensEffect::UpdateSettings(){
	Constants.Data.x = TheSettingManager->GetSettingF("Shaders.WaterLens.Main", "TimeMultA");
	Constants.Data.y = TheSettingManager->GetSettingF("Shaders.WaterLens.Main", "TimeMultB");
	Constants.Data.z = TheSettingManager->GetSettingF("Shaders.WaterLens.Main", "Viscosity");
}

void WaterLensEffect::RegisterConstants(){
	TheShaderManager->RegisterConstant("TESR_WaterLensData", &Constants.Data);
}

bool WaterLensEffect::ShouldRender() {
	return Constants.Data.w > 0.0f; 
};