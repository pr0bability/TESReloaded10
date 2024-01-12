#include "Underwater.h"

bool UnderwaterEffect::ShouldRender() {
	return TheShaderManager->GameState.isUnderwater; 
};