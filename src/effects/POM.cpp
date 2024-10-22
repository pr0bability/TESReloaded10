#include "POM.h"

void POMShaders::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_ParallaxData", &Constants.Data);
}

void POMShaders::UpdateSettings() {
		Constants.Data.x = TheSettingManager->GetSettingF("Shaders.POM.Main", "HeightMapScale");
		Constants.Data.y = TheSettingManager->GetSettingF("Shaders.PBR.Status", "Enabled");
}

void POMShaders::UpdateConstants() {}
