#include "POM.h"

void POMShaders::RegisterConstants() {
	TheShaderManager->ConstantsTable["TESR_ParallaxData"] = &Constants.Data;
}

void POMShaders::UpdateSettings() {
		Constants.Data.x = TheSettingManager->GetSettingF("Shaders.POM.Main", "HeightMapScale");
		Constants.Data.y = TheSettingManager->GetSettingF("Shaders.POM.Main", "MinSamples");
		Constants.Data.z = TheSettingManager->GetSettingF("Shaders.POM.Main", "MaxSamples");
}

void POMShaders::UpdateConstants() {}
