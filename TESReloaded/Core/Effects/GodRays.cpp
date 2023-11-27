#include "GodRays.h"

void GodRaysEffect::UpdateConstants() {
	Logger::Log("Update godrays constants");

	if (TheSettingManager->SettingsChanged || TheShaderManager->isDayTimeChanged) {
		Constants.Ray.x = TheSettingManager->GetSettingF("Shaders.GodRays.Main", "RayIntensity");
		Constants.Ray.y = TheSettingManager->GetSettingF("Shaders.GodRays.Main", "RayLength");
		Constants.Ray.z = TheSettingManager->GetSettingF("Shaders.GodRays.Main", "RayDensity");
		Constants.Ray.w = TheSettingManager->GetSettingF("Shaders.GodRays.Main", "RayVisibility");
		Constants.RayColor.x = TheSettingManager->GetSettingF("Shaders.GodRays.Coloring", "RayR");
		Constants.RayColor.y = TheSettingManager->GetSettingF("Shaders.GodRays.Coloring", "RayG");
		Constants.RayColor.z = TheSettingManager->GetSettingF("Shaders.GodRays.Coloring", "RayB");
		Constants.RayColor.w = TheSettingManager->GetSettingF("Shaders.GodRays.Coloring", "Saturate");
		Constants.Data.x = TheSettingManager->GetSettingF("Shaders.GodRays.Main", "LightShaftPasses");
		Constants.Data.y = TheSettingManager->GetSettingF("Shaders.GodRays.Main", "Luminance");
		Constants.Data.w = TheSettingManager->GetSettingF("Shaders.GodRays.Main", "TimeEnabled");
	}

	float dayMult = TheSettingManager->GetSettingF("Shaders.GodRays.Main", "DayMultiplier");
	float nightMult = TheSettingManager->GetSettingF("Shaders.GodRays.Main", "NightMultiplier");
	Constants.Data.z = std::lerp(nightMult, dayMult, TheShaderManager->transitionCurve);

	if (TheSettingManager->GetSettingI("Shaders.GodRays.Main", "SunGlareEnabled")) {
		Constants.Ray.z *= TheShaderManager->ShaderConst.sunGlare;
		Constants.Ray.w *= TheShaderManager->ShaderConst.sunGlare;
	}

}