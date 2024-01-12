#include "VolumetricFog.h"

void VolumetricFogEffect::UpdateConstants() {

	Constants.Data.z = TheShaderManager->GameState.isExterior ? TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "Amount") : TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "AmountInterior");

	if (!TheShaderManager->GameState.isExterior) Constants.Height.w = 0.0f;
	else Constants.Height.w = 1.0f;

}

void VolumetricFogEffect::UpdateSettings(){
	Constants.Data.x = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "Exponent");
	Constants.Data.y = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "ColorCoeff");
	Constants.Data.w = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "MaxDistance");

	Constants.LowFog.w = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "SunPower");
	Constants.LowFog.x = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "LowFogDensity");
	Constants.LowFog.y = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "LowFogFalloff");
	Constants.LowFog.z = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "LowFogDist");
	Constants.Blend.x = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "LowFogBlend");
	Constants.Height.x = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "LowFogHeight");
	Constants.GeneralFog.x = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "HeightFogDensity");
	Constants.GeneralFog.y = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "HeightFogFalloff");
	Constants.GeneralFog.z = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "HeightFogDist");
	Constants.GeneralFog.w = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "HeightFogSkyColor");
	Constants.Height.y = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "HeightFogHeight");
	Constants.Blend.y = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "HeightFogBlend");
	Constants.Blend.z = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "HeightFogRolloff");
	Constants.SimpleFog.x = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "SimpleFogExtinction");
	Constants.SimpleFog.y = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "SimpleFogInscattering");
	Constants.SimpleFog.z = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "SimpleFogNight");
	Constants.Height.z = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "SimpleFogHeight");
	Constants.SimpleFog.w = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "SimpleFogSkyColor");
	Constants.Blend.w = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "SimpleFogBlend");

}

void VolumetricFogEffect::RegisterConstants(){
	TheShaderManager->RegisterConstant("TESR_VolumetricFogLow", &Constants.LowFog);
	TheShaderManager->RegisterConstant("TESR_VolumetricFogHigh", &Constants.GeneralFog);
	TheShaderManager->RegisterConstant("TESR_VolumetricFogSimple", &Constants.SimpleFog);
	TheShaderManager->RegisterConstant("TESR_VolumetricFogBlend", &Constants.Blend);
	TheShaderManager->RegisterConstant("TESR_VolumetricFogHeight", &Constants.Height);
	TheShaderManager->RegisterConstant("TESR_VolumetricFogData", &Constants.Data);
}


bool VolumetricFogEffect::ShouldRender() 
{
	return !TheShaderManager->GameState.PipBoyIsOn && !TheShaderManager->GameState.isUnderwater;
};