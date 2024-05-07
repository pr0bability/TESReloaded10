#include "VolumetricFog.h"

void VolumetricFogEffect::UpdateConstants() {



}

void VolumetricFogEffect::UpdateSettings(){

	char SettingCategory[50] = "Shaders.VolumetricFog.";
	
	if (TheShaderManager->GameState.isExterior) 
		strcat(SettingCategory, "Main");
	else
		strcat(SettingCategory, "Interiors");

	Logger::Log(SettingCategory);

	Constants.Data.x = TheSettingManager->GetSettingF(SettingCategory, "MinimumBaseFog");
	//Constants.Data.y = TheSettingManager->GetSettingF(SettingCategory, "ColorCoeff");
	//Constants.Data.w = TheSettingManager->GetSettingF(SettingCategory, "MaxDistance");
	Constants.Data.z = TheSettingManager->GetSettingF(SettingCategory, "Amount");

	Constants.LowFog.x = TheSettingManager->GetSettingF(SettingCategory, "FogSaturation");
	Constants.LowFog.y = TheSettingManager->GetSettingF(SettingCategory, "WeatherImpact");
	Constants.LowFog.z = TheSettingManager->GetSettingF(SettingCategory, "DistantFogRange");
	Constants.LowFog.w = TheSettingManager->GetSettingF(SettingCategory, "SunPower");

	Constants.HighFog.x = TheSettingManager->GetSettingF(SettingCategory, "HeightFogDensity");
	Constants.HighFog.y = TheSettingManager->GetSettingF(SettingCategory, "HeightFogFalloff");
	Constants.HighFog.z = TheSettingManager->GetSettingF(SettingCategory, "HeightFogDist");
	Constants.HighFog.w = TheSettingManager->GetSettingF(SettingCategory, "HeightFogSkyColor");

	Constants.Height.x = TheSettingManager->GetSettingF(SettingCategory, "DistantFogHeight");
	Constants.Height.y = TheSettingManager->GetSettingF(SettingCategory, "HeightFogHeight");
	Constants.Height.z = TheSettingManager->GetSettingF(SettingCategory, "SimpleFogHeight");
	Constants.Height.w = TheShaderManager->GameState.isExterior ? 1.0f : 0.0f;

	Constants.SimpleFog.x = TheSettingManager->GetSettingF(SettingCategory, "Extinction");
	Constants.SimpleFog.y = TheSettingManager->GetSettingF(SettingCategory, "Inscattering");
	Constants.SimpleFog.z = TheSettingManager->GetSettingF(SettingCategory, "FogNight");
	Constants.SimpleFog.w = TheSettingManager->GetSettingF(SettingCategory, "SimpleFogSkyColor");

	Constants.Blend.x = TheSettingManager->GetSettingF(SettingCategory, "DistantFogBlend");
	Constants.Blend.y = TheSettingManager->GetSettingF(SettingCategory, "HeightFogBlend");
	Constants.Blend.z = TheSettingManager->GetSettingF(SettingCategory, "HeightFogRolloff");
	Constants.Blend.w = TheSettingManager->GetSettingF(SettingCategory, "SimpleFogBlend");

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