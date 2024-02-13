#include "VolumetricFog.h"

void VolumetricFogEffect::UpdateConstants() {

	if (TheShaderManager->GameState.isExterior) {
		Constants.Height.w = 1.0f;
		Constants.Data.z = Amount;
	}
	else {
		Constants.Height.w = 0.0f;
		Constants.Data.z = AmountInteriors;
	}

}

void VolumetricFogEffect::UpdateSettings(){
	Amount = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "Amount");
	AmountInteriors = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "AmountInterior");

	Constants.Data.x = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "Exponent");
	Constants.Data.y = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "ColorCoeff");
	Constants.Data.w = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "MaxDistance");

	Constants.LowFog.x = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "LowFogDensity");
	Constants.LowFog.y = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "LowFogFalloff");
	Constants.LowFog.z = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "LowFogDist");
	Constants.LowFog.w = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "SunPower");

	Constants.HighFog.x = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "HeightFogDensity");
	Constants.HighFog.y = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "HeightFogFalloff");
	Constants.HighFog.z = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "HeightFogDist");
	Constants.HighFog.w = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "HeightFogSkyColor");

	Constants.Height.x = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "LowFogHeight");
	Constants.Height.y = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "HeightFogHeight");
	Constants.Height.z = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "SimpleFogHeight");

	Constants.SimpleFog.x = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "SimpleFogExtinction");
	Constants.SimpleFog.y = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "SimpleFogInscattering");
	Constants.SimpleFog.z = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "SimpleFogNight");
	Constants.SimpleFog.w = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "SimpleFogSkyColor");

	Constants.Blend.x = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "LowFogBlend");
	Constants.Blend.y = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "HeightFogBlend");
	Constants.Blend.z = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "HeightFogRolloff");
	Constants.Blend.w = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "SimpleFogBlend");

}

void VolumetricFogEffect::RegisterConstants(){
	TheShaderManager->RegisterConstant("TESR_VolumetricFogLow", &Constants.LowFog);
	TheShaderManager->RegisterConstant("TESR_VolumetricFogHigh", &Constants.HighFog);
	TheShaderManager->RegisterConstant("TESR_VolumetricFogSimple", &Constants.SimpleFog);
	TheShaderManager->RegisterConstant("TESR_VolumetricFogBlend", &Constants.Blend);
	TheShaderManager->RegisterConstant("TESR_VolumetricFogHeight", &Constants.Height);
	TheShaderManager->RegisterConstant("TESR_VolumetricFogData", &Constants.Data);
}


bool VolumetricFogEffect::ShouldRender() 
{
	return !TheShaderManager->GameState.isUnderwater;
};