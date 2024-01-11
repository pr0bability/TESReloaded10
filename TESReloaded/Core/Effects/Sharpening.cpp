#include "Sharpening.h"

void SharpeningEffect::UpdateConstants() {
}

void SharpeningEffect::UpdateSettings(){
	Constants.Data.x = TheSettingManager->GetSettingF("Shaders.Sharpening.Main", "Strength");
	Constants.Data.y = TheSettingManager->GetSettingF("Shaders.Sharpening.Main", "Clamp");
	Constants.Data.z = TheSettingManager->GetSettingF("Shaders.Sharpening.Main", "Offset");
}

void SharpeningEffect::RegisterConstants(){
	TheShaderManager->RegisterConstant("TESR_SharpeningData", &Constants.Data);
}