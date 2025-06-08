#include "RenderPass.h"

#include <tracy/Tracy.hpp>

bool CheckShaderFlags(NiGeometry* Geometry) {
	BSShaderProperty* shaderProp = static_cast<BSShaderProperty*>(Geometry->GetProperty(NiProperty::kType_Shade));

	if (!shaderProp)
		return false;

	return !(shaderProp->GetFlag(BSSP_REFRACTION) ||
		shaderProp->GetFlag(BSSP_FIRE_REFRACTION) ||
		shaderProp->GetFlag(BSSP_DECAL) ||
		shaderProp->GetFlag(BSSP_DYNAMIC_DECAL));
}

void RenderPass::RenderAccum(const std::vector<NiGeometry*>& geometry) {
	ZoneScoped;

	if (geometry.empty()) {
		return;
	}

	// Could add setup of device/renderstate/current shaders here
	NiDX9RenderState* RenderState = TheRenderManager->renderState;
	RenderState->SetPixelShader(PixelShader->ShaderHandle, false);
	RenderState->SetVertexShader(VertexShader->ShaderHandle, false);

	// Render normal geometry
	for (const auto& Geo : geometry) {
		if (!Geo) {
			continue;
		}

		UpdateConstants(Geo);
		PixelShader->SetCT();
		VertexShader->SetCT();

		RenderGeometry(Geo);
	}
}


void RenderPass::RenderGeometry(NiGeometry* Geo) {
	ZoneScoped;

	NiGeometryData* ModelData = Geo->geomData;
	NiGeometryBufferData* GeoData = ModelData->m_pkBuffData;
	NiD3DShaderDeclaration* ShaderDeclaration = Geo->shader->ShaderDeclaration;

	// Set proper cull based on stencil property.
	NiProperty* pProp = Geo->GetProperty(NiProperty::PropertyType::kType_Stencil);

	NiDX9RenderState* RenderState = TheRenderManager->renderState;
	if (pProp) {
		NiStencilProperty* pStencilProp = static_cast<NiStencilProperty*>(pProp);
		RenderState->SetCullMode(pStencilProp->GetDrawMode());
	}
	else {
		RenderState->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW, RenderStateArgs);
	}

	TheRenderManager->PackGeometryBuffer(GeoData, ModelData, NULL, ShaderDeclaration);
	if (GeoData && GeoData->VertCount) DrawGeometryBuffer(Geo, GeoData);
}


// draws the geo data from the Geometry buffer
void RenderPass::DrawGeometryBuffer(NiGeometry* Geo, NiGeometryBufferData* GeoData) {
	int StartIndex = 0;
	int PrimitiveCount = 0;

	for (UInt32 i = 0; i < GeoData->StreamCount; i++) {
		TheRenderManager->device->SetStreamSource(i, GeoData->VBChip[i]->VB, 0, GeoData->VertexStride[i]);
	}

	if (GeoData->IB)
		TheRenderManager->device->SetIndices(GeoData->IB);
	if (GeoData->FVF)
		TheRenderManager->renderState->SetFVF(GeoData->FVF, false);
	else
		TheRenderManager->renderState->SetVertexDeclaration(GeoData->VertexDeclaration, false);

	uint16_t dirtyFlags = Geo->geomData->m_usDirtyFlags;
	NiTriStrips* pStrips = Geo->IsTriStrips();
	if (pStrips) {
		ThisStdCall(0xE74840, NiDX9Renderer::GetSingleton(), Geo);  // RenderTriStripsAlt
	}
	else {
		ThisStdCall(0xE745A0, NiDX9Renderer::GetSingleton(), Geo);  // RenderTriShapeAlt
	}
	// Add back the dirty flag that are reset with vanilla render functions.
	Geo->geomData->m_usDirtyFlags = dirtyFlags;
}

void RenderPass::DrawSkinnedGeometryBuffer(NiGeometry* Geo, NiGeometryBufferData* GeoData, NiSkinPartition::Partition* Partition) {
	int StartIndex = 0;
	int PrimitiveCount = 0;

	for (UInt32 i = 0; i < GeoData->StreamCount; i++) {
		TheRenderManager->device->SetStreamSource(i, GeoData->VBChip[i]->VB, 0, GeoData->VertexStride[i]);
	}

	if (GeoData->IB)
		TheRenderManager->device->SetIndices(GeoData->IB);

	if (GeoData->FVF)
		TheRenderManager->renderState->SetFVF(GeoData->FVF, false);
	else
		TheRenderManager->renderState->SetVertexDeclaration(GeoData->VertexDeclaration, false);

	ThisStdCall(0xE6D310, NiDX9Renderer::GetSingleton(), GeoData, Partition, nullptr);  // DrawSkinnedGeometry
}


ShadowRenderPass::ShadowRenderPass() {
	PixelShader = TheShadowManager->ShadowMapPixel;
	VertexShader = TheShadowManager->ShadowMapVertex;
	RegisterConstants();
}


bool ShadowRenderPass::AccumObject(NiGeometry* Geo, bool* append) {
	if (!Geo->geomData || !Geo->geomData->m_pkBuffData) return false; // discard objects without buffer data

	BSShaderProperty* ShaderProperty = (BSShaderProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Shade);
	if (!ShaderProperty || !ShaderProperty->IsLightingProperty()) return false;

	*append = true;
	return true;
}


void ShadowRenderPass::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_ShadowWorldTransform", (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowWorld);
	//TheTextureManager->RegisterTexture("TESR_DiffuseMap", &Constants.DiffuseMap);
}


void ShadowRenderPass::UpdateConstants(NiGeometry* Geo) {
	ShadowsExteriorEffect::ShadowStruct* ShadowConstants = &TheShaderManager->Effects.ShadowsExteriors->Constants;
	ShadowConstants->Data.x = 0.0f; // Type of geo (0 normal, 1 actors (skinned), 2 speedtree leaves)
	ShadowConstants->Data.y = 0.0f; // Alpha Control
	TheRenderManager->CreateD3DMatrix(&TheShaderManager->ShaderConst.ShadowWorld, &Geo->m_worldTransform);
}



AlphaShadowRenderPass::AlphaShadowRenderPass() {
	PixelShader = TheShadowManager->ShadowMapPixel;
	VertexShader = TheShadowManager->ShadowMapVertex;
	RegisterConstants();
}


bool AlphaShadowRenderPass::AccumObject(NiGeometry* Geo, bool* append) {
	if (!Geo->geomData || !Geo->geomData->m_pkBuffData) return false; // discard objects without buffer data

	BSShaderProperty* ShaderProperty = (BSShaderProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Shade);
	NiAlphaProperty* AProp = (NiAlphaProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Alpha);

	if (!ShaderProperty || !ShaderProperty->IsLightingProperty()) return false;
	if (!AProp) return false;
	if (!(AProp->flags & NiAlphaProperty::AlphaFlags::ALPHA_BLEND_MASK) && !(AProp->flags & NiAlphaProperty::AlphaFlags::TEST_ENABLE_MASK)) return false;

	*append = true;
	return true;
}


void AlphaShadowRenderPass::RegisterConstants() {
}


void AlphaShadowRenderPass::UpdateConstants(NiGeometry* Geo) {
	ShadowsExteriorEffect::ShadowStruct* ShadowConstants = &TheShaderManager->Effects.ShadowsExteriors->Constants;
	ShadowConstants->Data.x = 0.0f; // Type of geo (0 normal, 1 actors (skinned), 2 speedtree leaves)
	ShadowConstants->Data.y = 0.0f; // Alpha Control
	TheRenderManager->CreateD3DMatrix(&TheShaderManager->ShaderConst.ShadowWorld, &Geo->m_worldTransform);

	if (const auto ShaderProperty = static_cast<BSShaderProperty*>(Geo->GetProperty(NiProperty::PropertyType::kType_Shade))) {
		const auto ppl = static_cast<BSShaderPPLightingProperty*>(ShaderProperty);
		if (!ppl->ppTextures || !ppl->ppTextures[0]) {
			return;
		}

		const auto Texture = *ppl->ppTextures[0];
		if (Texture && Texture->rendererData->dTexture) {

			ShadowConstants->Data.y = 1.0f; // Alpha Control
			//			Constants.DiffuseMap = Texture->rendererData->dTexture;

					//// Set diffuse texture at register 0
			NiDX9RenderState* RenderState = TheRenderManager->renderState;
			RenderState->SetTexture(0, Texture->rendererData->dTexture);
			RenderState->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP, false);
			RenderState->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP, false);
			RenderState->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT, false);
			RenderState->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT, false);
			RenderState->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT, false);
		}
	}
}


SkinnedGeoShadowRenderPass::SkinnedGeoShadowRenderPass() {
	PixelShader = TheShadowManager->ShadowMapPixel;
	VertexShader = TheShadowManager->ShadowMapVertex;
	RegisterConstants();
}


bool SkinnedGeoShadowRenderPass::AccumObject(NiGeometry* Geo, bool* append) {
	// check data for rigged geometry
	if (!Geo->skinInstance)
		return false;

	NiSkinInstance* skinInstance = Geo->skinInstance;
	if (skinInstance->IsKindOf<BSDismemberSkinInstance>() && !((BSDismemberSkinInstance*)skinInstance)->IsRenderable)
		return true;

	if (Geo->skinInstance->SkinPartition && Geo->skinInstance->SkinPartition->Partitions) {

		// only accum if valid data preset
		//if (Geo->skinInstance->SkinPartition->Partitions[0].BuffData)
			*append = true;
	
		// we return true in any case because we still found skinned geo either way
		return true;
	}

	return false;
}


void SkinnedGeoShadowRenderPass::RegisterConstants() {
	//TheShaderManager->RegisterConstant("TESR_Bones", NULL);
}


void SkinnedGeoShadowRenderPass::UpdateConstants(NiGeometry* Geo) {
	// Render skinned geometry
	ShadowsExteriorEffect::ShadowStruct* Constants = &TheShaderManager->Effects.ShadowsExteriors->Constants;
	Constants->Data.x = 1.0f; // Type of geo (0 normal, 1 actors (skinned), 2 speedtree leaves)
	Constants->Data.y = 0.0f; // Alpha control
	TheRenderManager->CreateD3DMatrix(&TheShaderManager->ShaderConst.ShadowWorld, &Geo->m_worldTransform);
}


void SkinnedGeoShadowRenderPass::RenderGeometry(NiGeometry* Geo) {
	NiGeometryData* ModelData = Geo->geomData;
	NiGeometryBufferData* GeoData = ModelData->m_pkBuffData;
	NiSkinInstance* SkinInstance = Geo->skinInstance;
	NiD3DShaderDeclaration* ShaderDeclaration = Geo->shader->ShaderDeclaration;

	// Set proper cull based on stencil property.
	NiProperty* pProp = Geo->GetProperty(NiProperty::PropertyType::kType_Stencil);

	NiDX9RenderState* RenderState = TheRenderManager->renderState;
	if (pProp) {
		NiStencilProperty* pStencilProp = static_cast<NiStencilProperty*>(pProp);
		RenderState->SetCullMode(pStencilProp->GetDrawMode());
	}
	else {
		RenderState->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW, RenderStateArgs);
	}

	NiSkinPartition* SkinPartition = SkinInstance->SkinPartition;
	TheRenderManager->CalculateBoneMatrixes(SkinInstance, &Geo->m_worldTransform);
	
	if (SkinInstance->SkinToWorldWorldToSkin) 
		memcpy(&TheShaderManager->Effects.ShadowsExteriors->Constants.ShadowWorld, SkinInstance->SkinToWorldWorldToSkin, 0x40);

	DismemberPartition* pDismemberPartition = NULL;
	if (SkinInstance->IsKindOf<BSDismemberSkinInstance>())
		pDismemberPartition = ((BSDismemberSkinInstance*)SkinInstance)->partitions;

	for (UInt32 p = 0; p < SkinPartition->PartitionsCount; p++) {
		if (pDismemberPartition && !pDismemberPartition[p].Enabled)
			continue;

		NiSkinPartition::Partition* Partition = &SkinPartition->Partitions[p];
		if (!Partition)
			continue;

		GeoData = Partition->BuffData;
		if (GeoData) {
			//Constants.BoneMatrices = (D3DXVECTOR4*)SkinInstance->BoneMatrixes;
			int StartRegister = 9;
			for (int i = 0; i < Partition->Bones; i++) {
				UInt16 NewIndex = (Partition->pBones == NULL) ? i : Partition->pBones[i];
				TheRenderManager->device->SetVertexShaderConstantF(StartRegister, ((float*)SkinInstance->BoneMatrixes) + (NewIndex * 3 * 4), 3);
				StartRegister += 3;
			}

			//TheRenderManager->PackSkinnedGeometryBuffer(GeoData, ModelData, SkinInstance, Partition, ShaderDeclaration);
			DrawSkinnedGeometryBuffer(Geo, GeoData, Partition);
		}

		if (!pDismemberPartition)
			continue;

		for (UInt32 d = p + 1; d < SkinPartition->PartitionsCount; d++) {
			if (pDismemberPartition[p + 1].StartCap)
				break;

			++p;
			++Partition;
			if (pDismemberPartition[p].Enabled) {
				int StartRegister = 9;
				GeoData = Partition->BuffData;
				if (GeoData) {
					for (int i = 0; i < Partition->Bones; i++) {
						UInt16 NewIndex = (Partition->pBones == NULL) ? i : Partition->pBones[i];
						TheRenderManager->device->SetVertexShaderConstantF(StartRegister, ((float*)SkinInstance->BoneMatrixes) + (NewIndex * 3 * 4), 3);
						StartRegister += 3;
					}


					DrawSkinnedGeometryBuffer(Geo, GeoData, Partition);
				}
			}

		}
	}
}


SpeedTreeShadowRenderPass::SpeedTreeShadowRenderPass() {
	PixelShader = TheShadowManager->ShadowMapPixel;
	VertexShader = TheShadowManager->ShadowMapVertex;
	RegisterConstants();
}


void SpeedTreeShadowRenderPass::RegisterConstants() {
	//TheShaderManager->RegisterConstant("TESR_BillboardRight", Constants.BillboardRight);
	//TheShaderManager->RegisterConstant("TESR_BillboardUp", Constants.BillboardUp);
	//TheShaderManager->RegisterConstant("TESR_RockParams", (D3DXVECTOR4*)Pointers::ShaderParams::RockParams);
	//TheShaderManager->RegisterConstant("TESR_RustleParams", (D3DXVECTOR4*)Pointers::ShaderParams::RustleParams);
	//TheShaderManager->RegisterConstant("TESR_WindMatrices", (D3DXVECTOR4*)Pointers::ShaderParams::WindMatrixes);
	//TheShaderManager->RegisterConstant("TESR_LeafBase", Constants.LeafBase);
	//TheTextureManager->RegisterTexture("TESR_LeafDiffuseMap", &Constants.DiffuseMap);
}


bool SpeedTreeShadowRenderPass::AccumObject(NiGeometry* Geo, bool* append) {
	NiShadeProperty* shaderProp = static_cast<NiShadeProperty*>(Geo->GetProperty(NiProperty::kType_Shade));
	if (shaderProp->m_eShaderType != NiShadeProperty::kProp_SpeedTreeLeaf) return false;

	*append = true;
	return true;
}


void SpeedTreeShadowRenderPass::UpdateConstants(NiGeometry* Geo) {
	ShadowsExteriorEffect::ShadowStruct* ShadowConstants = &TheShaderManager->Effects.ShadowsExteriors->Constants;
	IDirect3DDevice9* Device = TheRenderManager->device;
	NiDX9RenderState* RenderState = TheRenderManager->renderState;

	ShadowConstants->Data.x = 2.0f; // Type of geo (0 normal, 1 actors (skinned), 2 speedtree leaves)
	ShadowConstants->Data.y = 0.0f; // Alpha control
	TheRenderManager->CreateD3DMatrix(&TheShaderManager->ShaderConst.ShadowWorld, &Geo->m_worldTransform);

	// Bind constant values for leaf transformation
	Device->SetVertexShaderConstantF(63, (float*)&TheShadowManager->BillboardRight, 1);
	Device->SetVertexShaderConstantF(64, (float*)&TheShadowManager->BillboardUp, 1);
	Device->SetVertexShaderConstantF(65, Pointers::ShaderParams::RockParams, 1);
	Device->SetVertexShaderConstantF(66, Pointers::ShaderParams::RustleParams, 1);
	Device->SetVertexShaderConstantF(67, Pointers::ShaderParams::WindMatrixes, 16);

	SpeedTreeLeafShaderProperty* STProp = (SpeedTreeLeafShaderProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Shade);
	BSTreeNode* Node = (BSTreeNode*)Geo->m_parent->m_parent;
	NiDX9SourceTextureData* Texture = (NiDX9SourceTextureData*)Node->TreeModel->LeavesTexture->rendererData;

	if (Texture) ShadowConstants->Data.y = 1.0f;

	// Bind constant values for leaf transformation
	Device->SetVertexShaderConstantF(83, STProp->leafData->leafBase, 48);
	
	// Set diffuse texture at register 0
	RenderState->SetTexture(0, Texture->dTexture);
	RenderState->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP, false);
	RenderState->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP, false);
	RenderState->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT, false);
	RenderState->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT, false);
	RenderState->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT, false);
}


TerrainLODPass::TerrainLODPass() {
	PixelShader = TheShadowManager->ShadowMapPixel;
	VertexShader = TheShadowManager->ShadowMapVertex;
	RegisterConstants();
}


bool TerrainLODPass::AccumObject(NiGeometry* Geo, bool* append) {
	if (!Geo->geomData || !Geo->geomData->m_pkBuffData) return false; // discard objects without buffer data

	BSShaderProperty* ShaderProperty = (BSShaderProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Shade);
	if (!ShaderProperty || !ShaderProperty->IsLightingProperty()) return false;

	*append = true;
	return true;
}


void TerrainLODPass::UpdateConstants(NiGeometry* Geo) {
	ShadowsExteriorEffect::ShadowStruct* ShadowConstants = &TheShaderManager->Effects.ShadowsExteriors->Constants;
	ShadowConstants->Data.x = 3.0f; // Type of geo (0 normal, 1 actors (skinned), 2 speedtree leaves, 3 terrain LOD)
	ShadowConstants->Data.y = 0.0f; // Alpha Control
	TheRenderManager->CreateD3DMatrix(&TheShaderManager->ShaderConst.ShadowWorld, &Geo->m_worldTransform);
	D3DXMatrixTranspose(&Constants.WorldTranspose, &TheShaderManager->ShaderConst.ShadowWorld);

	BSShaderPPLightingProperty* prop = (BSShaderPPLightingProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Shade);

	Constants.LODLandParams.x = prop->fMorphDistance;
	Constants.LODLandParams.y = *BSShaderManager::fLODLandDrop;

	Constants.HighDetailRange.x = BSShaderManager::kLoadedRange->x - BSShaderManager::kCameraPos->x;
	Constants.HighDetailRange.y = BSShaderManager::kLoadedRange->y - BSShaderManager::kCameraPos->y;
	Constants.HighDetailRange.z = BSShaderManager::kLoadedRange->z - 15;
	Constants.HighDetailRange.w = BSShaderManager::kLoadedRange->w - 15;

	IDirect3DDevice9* Device = TheRenderManager->device;
	Device->SetVertexShaderConstantF(140, (float*)&Constants.WorldTranspose, 4);
	Device->SetVertexShaderConstantF(144, (float*)&Constants.HighDetailRange, 1);
	Device->SetVertexShaderConstantF(145, (float*)&Constants.LODLandParams, 1);
}
