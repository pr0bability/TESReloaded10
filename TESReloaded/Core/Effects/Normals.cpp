#include "Normals.h"

void NormalsEffect::RegisterConstants() {
	TheShaderManager->ConstantsTable["TESR_NormalsData"] = &Constants.Data;
}