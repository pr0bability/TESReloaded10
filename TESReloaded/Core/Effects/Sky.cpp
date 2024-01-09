#include "Sky.h"

void SkyShaders::RegisterConstants() {
	TheShaderManager->ConstantsTable["TESR_SkyData"] = &Constants.SkyData;
	TheShaderManager->ConstantsTable["TESR_CloudData"] = &Constants.CloudData;
	TheShaderManager->ConstantsTable["TESR_SunsetColor"] = &Constants.SunsetColor;
}

void SkyShaders::UpdateConstants() {}

void SkyShaders::UpdateSettings() {

	Constants.SkyData.x = TheSettingManager->GetSettingF("Shaders.Sky.Main", "AthmosphereThickness");
	Constants.SkyData.y = TheSettingManager->GetSettingF("Shaders.Sky.Main", "SunInfluence");
	Constants.SkyData.z = TheSettingManager->GetSettingF("Shaders.Sky.Main", "SunStrength");
	Constants.SkyData.w = TheSettingManager->GetSettingF("Shaders.Sky.Main", "StarStrength");

	Constants.CloudData.x = TheSettingManager->GetSettingF("Shaders.Sky.Clouds", "UseNormals");
	Constants.CloudData.y = TheSettingManager->GetSettingF("Shaders.Sky.Clouds", "SphericalNormals");
	Constants.CloudData.z = TheSettingManager->GetSettingF("Shaders.Sky.Clouds", "Transparency");
	Constants.CloudData.w = TheSettingManager->GetSettingF("Shaders.Sky.Clouds", "Brightness");

	// only add sunset color boost in exteriors
	if (TheShaderManager->isExterior) {
		Constants.SunsetColor.x = TheSettingManager->GetSettingF("Shaders.Sky.Main", "SunsetR");
		Constants.SunsetColor.y = TheSettingManager->GetSettingF("Shaders.Sky.Main", "SunsetG");
		Constants.SunsetColor.z = TheSettingManager->GetSettingF("Shaders.Sky.Main", "SunsetB");

		// TODO : fix sun culling for sun replacing?
		//if (TheSettingManager->GetMenuShaderEnabled("Sky")) {
		//	if (ShaderConst.SunAmount.z) WorldSky->sun->RootNode->m_flags |= ~NiAVObject::NiFlags::DISPLAY_OBJECT; // cull Sun node
		//	else WorldSky->sun->RootNode->m_flags &= NiAVObject::NiFlags::DISPLAY_OBJECT; // disable Sun node
		//}
	}
	else {
		Constants.SunsetColor.x = 0;
		Constants.SunsetColor.y = 0;
		Constants.SunsetColor.z = 0;
	}

}