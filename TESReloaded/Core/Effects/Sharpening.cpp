#include "Sharpening.h"

void SharpeningEffect::UpdateConstants() {
	Constants.Data.x = TheSettingManager->GetSettingF("Shaders.Sharpening.Main", "Strength");
	Constants.Data.y = TheSettingManager->GetSettingF("Shaders.Sharpening.Main", "Clamp");
	Constants.Data.z = TheSettingManager->GetSettingF("Shaders.Sharpening.Main", "Offset");
}