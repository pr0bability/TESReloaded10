#pragma once

// 0xB89D80
VirtFuncDetour kSkyShaderConstantsDetour;
void __fastcall SkyShader__UpdateConstants(SkyShader* apThis, void*, const NiPropertyState* apProperties) {
    const SkyShaderProperty* pShaderProp = apProperties->GetShadeProperty<SkyShaderProperty const>();
    uint32_t eSkyObjectType = pShaderProp->eSkyObjectType;
    TheShaderManager->ShaderConst.skyObjectID = eSkyObjectType;

    ThisCall(kSkyShaderConstantsDetour.GetOverwrittenAddr(), apThis, apProperties);

    TheRenderManager->device->SetPixelShaderConstantF(20, (const float*)&TheShaderManager->ShaderConst.skyObjectID, 1);
}
