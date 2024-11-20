#include "RenderPass.h"


void RenderPass::RenderAccum() {
	if (GeometryList.empty() && GeometryInstances.empty()) return;

	// Could add setup of device/renderstate/current shaders here
	NiDX9RenderState* RenderState = TheRenderManager->renderState;
	RenderState->SetPixelShader(PixelShader->ShaderHandle, false);
	RenderState->SetVertexShader(InstancedVertexShader->ShaderHandle, false);

	// Render instanced geometry
	while (!GeometryInstances.empty()) {
		auto head = GeometryInstances.begin();

		NiGeometryData* geoData = head->first;
		std::vector<NiGeometry*> &geometries = head->second;

		if (!geometries.empty()) {
			UpdateConstants(geometries.front());
			PixelShader->SetCT();
			InstancedVertexShader->SetCT();

			RenderInstancedGeometry(geometries);
		}

		GeometryInstances.erase(geoData);
		geoData->DecRefCount();
	}

	RenderState->SetVertexShader(VertexShader->ShaderHandle, false);

	// Render normal geometry
	while (!GeometryList.empty()) {
		NiGeometry* Geo = GeometryList.top();
		if (!Geo) continue;

		UpdateConstants(Geo);
		PixelShader->SetCT();
		VertexShader->SetCT();

		RenderGeometry(Geo);
		GeometryList.pop();
	}
}


void RenderPass::RenderGeometry(NiGeometry* Geo) {
	NiGeometryData* ModelData = Geo->geomData;
	NiGeometryBufferData* GeoData = ModelData->BuffData;
	NiD3DShaderDeclaration* ShaderDeclaration = Geo->shader->ShaderDeclaration;
	
	TheRenderManager->PackGeometryBuffer(GeoData, ModelData, NULL, ShaderDeclaration);
	if (GeoData && GeoData->VertCount) DrawGeometryBuffer(Geo, GeoData);
}

// Render instanced geometries.
void RenderPass::RenderInstancedGeometry(std::vector<NiGeometry*> &geometries) {
	// Since all geometries are the same form, take the first one and set up render state based on it.
	NiGeometry* geo = geometries.front();
	NiGeometryData* geoData = geo->geomData;
	NiGeometryBufferData* bufferData = geo->geomData->BuffData;
	NiD3DShaderDeclaration* shaderDeclaration = geo->shader->ShaderDeclaration;

	shaderDeclaration->SetEntry(6, 0, NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD4, NiShaderDeclaration::SPTYPE_FLOAT4, 1);
	shaderDeclaration->SetEntry(7, 0, NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD5, NiShaderDeclaration::SPTYPE_FLOAT4, 1);
	shaderDeclaration->SetEntry(8, 0, NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD6, NiShaderDeclaration::SPTYPE_FLOAT4, 1);
	shaderDeclaration->SetEntry(9, 0, NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD7, NiShaderDeclaration::SPTYPE_FLOAT4, 1);

	TheRenderManager->PackGeometryBuffer(bufferData, geoData, NULL, shaderDeclaration);

	for (UInt32 i = 0; i < bufferData->StreamCount; i++) {
		TheRenderManager->device->SetStreamSource(i, bufferData->VBChip[i]->VB, 0, bufferData->VertexStride[i]);
	}

	if (!bufferData->IB) return; // breaks on hand models for some reason? TODO: figure out what breaks for hands
	TheRenderManager->device->SetIndices(bufferData->IB);
	if (bufferData->FVF)
		TheRenderManager->renderState->SetFVF(bufferData->FVF, false);
	else
		TheRenderManager->renderState->SetVertexDeclaration(shaderDeclaration->GetD3DDeclaration(), false);

	UInt16 count = geometries.size();

	IDirect3DDevice9* device = NiDX9Renderer::GetSingleton()->device;

	IDirect3DVertexBuffer9* vertexBuffer = nullptr;
	
	device->CreateVertexBuffer(
		sizeof(D3DXMATRIX) * count,
		D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_DEFAULT,
		&vertexBuffer,
		nullptr
	);

	D3DXMATRIX* instanceData = nullptr;

	vertexBuffer->Lock(0, sizeof(D3DXMATRIX) * count, reinterpret_cast<void**>(&instanceData), D3DLOCK_NOSYSLOCK);

	for (UInt16 i = 0; i < count; i++) {
		TheRenderManager->CreateD3DMatrix(&instanceData[i], &geometries[i]->m_worldTransform);
	}

	vertexBuffer->Unlock();

	device->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | count);

	device->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1ul);
	device->SetStreamSource(1, vertexBuffer, 0, sizeof(D3DXMATRIX));

	NiTriStrips* pStrips = geo->IsTriStrips();
	if (pStrips) {
		ThisStdCall(0xE74840, NiDX9Renderer::GetSingleton(), geo);  // RenderTriStripsAlt
	}
	else {
		ThisStdCall(0xE745A0, NiDX9Renderer::GetSingleton(), geo);  // RenderTriShapeAlt
	}

	vertexBuffer->Release();

	device->SetStreamSourceFreq(0, 1);
	device->SetStreamSourceFreq(1, 1);
}

// draws the geo data from the Geometry buffer
void RenderPass::DrawGeometryBuffer(NiGeometry* Geo, NiGeometryBufferData* GeoData) {
	for (UInt32 i = 0; i < GeoData->StreamCount; i++) {
		TheRenderManager->device->SetStreamSource(i, GeoData->VBChip[i]->VB, 0, GeoData->VertexStride[i]);
	}

	if (!GeoData->IB) return; // breaks on hand models for some reason? TODO: figure out what breaks for hands
	TheRenderManager->device->SetIndices(GeoData->IB);
	if (GeoData->FVF)
		TheRenderManager->renderState->SetFVF(GeoData->FVF, false);
	else
		TheRenderManager->renderState->SetVertexDeclaration(GeoData->VertexDeclaration, false);

	NiTriStrips* pStrips = Geo->IsTriStrips();
	if (pStrips) {
		ThisStdCall(0xE74840, NiDX9Renderer::GetSingleton(), Geo);  // RenderTriStripsAlt
	}
	else {
		ThisStdCall(0xE745A0, NiDX9Renderer::GetSingleton(), Geo);  // RenderTriShapeAlt
	}
}


ShadowRenderPass::ShadowRenderPass() {
	PixelShader = TheShadowManager->ShadowMapPixel;
	VertexShader = TheShadowManager->ShadowMapVertex;
	RegisterConstants();
}


bool ShadowRenderPass::AccumObject(NiGeometry* Geo) {
	if (!Geo->geomData || !Geo->geomData->BuffData) return false; // discard objects without buffer data

	BSShaderProperty* ShaderProperty = (BSShaderProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Shade);
	if (!ShaderProperty || !ShaderProperty->IsLightingProperty()) return false;

	if (EnableInstancing) {
		if (Geo->geomData->BuffData->StreamCount > 1)
			GeometryList.push(Geo);
		else if (Geo->geomData->GetConsistency() == NiGeometryData::MUTABLE)
			GeometryList.push(Geo);
		else if (Geo->m_controller || Geo->GetHasPropertyController())
			GeometryList.push(Geo);
		else {
			std::vector<NiGeometry*>& instances = GeometryInstances[Geo->geomData];

			if (instances.empty())
				Geo->geomData->IncRefCount();

			if (instances.size() < MaxInstances)
				instances.push_back(Geo);
			else
				GeometryList.push(Geo);
		}
	}
	else {
		GeometryList.push(Geo);
	}

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


bool AlphaShadowRenderPass::AccumObject(NiGeometry* Geo) {
	if (!Geo->geomData || !Geo->geomData->BuffData) return false; // discard objects without buffer data

	BSShaderProperty* ShaderProperty = (BSShaderProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Shade);
	NiAlphaProperty* AProp = (NiAlphaProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Alpha);

	if (!ShaderProperty || !ShaderProperty->IsLightingProperty()) return false;
	if (!AProp) return false;
	if (!(AProp->flags & NiAlphaProperty::AlphaFlags::ALPHA_BLEND_MASK) && !(AProp->flags & NiAlphaProperty::AlphaFlags::TEST_ENABLE_MASK)) return false;

	if (EnableInstancing) {
		if (Geo->geomData->BuffData->StreamCount > 1)
			GeometryList.push(Geo);
		else if (Geo->geomData->GetConsistency() == NiGeometryData::MUTABLE)
			GeometryList.push(Geo);
		else if (Geo->m_controller || Geo->GetHasPropertyController())
			GeometryList.push(Geo);
		else {
			std::vector<NiGeometry*>& instances = GeometryInstances[Geo->geomData];

			if (instances.empty())
				Geo->geomData->IncRefCount();
			
			if (instances.size() < MaxInstances)
				instances.push_back(Geo);
			else
				GeometryList.push(Geo);
		}
	}
	else {
		GeometryList.push(Geo);
	}

	return true;
}


void AlphaShadowRenderPass::RegisterConstants() {
}


void AlphaShadowRenderPass::UpdateConstants(NiGeometry* Geo) {
	ShadowsExteriorEffect::ShadowStruct* ShadowConstants = &TheShaderManager->Effects.ShadowsExteriors->Constants;
	ShadowConstants->Data.x = 0.0f; // Type of geo (0 normal, 1 actors (skinned), 2 speedtree leaves)
	ShadowConstants->Data.y = 0.0f; // Alpha Control
	TheRenderManager->CreateD3DMatrix(&TheShaderManager->ShaderConst.ShadowWorld, &Geo->m_worldTransform);

	BSShaderProperty* ShaderProperty = (BSShaderProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Shade);
	NiTexture* Texture = *((BSShaderPPLightingProperty*)ShaderProperty)->ppTextures[0];

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


SkinnedGeoShadowRenderPass::SkinnedGeoShadowRenderPass() {
	PixelShader = TheShadowManager->ShadowMapPixel;
	VertexShader = TheShadowManager->ShadowMapVertex;
	RegisterConstants();
}


bool SkinnedGeoShadowRenderPass::AccumObject(NiGeometry* Geo) {
	// check data for rigged geometry
	if (Geo->skinInstance &&
		Geo->skinInstance->SkinPartition &&
		Geo->skinInstance->SkinPartition->Partitions) {

		// only accum if valid data preset
		if (Geo->skinInstance->SkinPartition->Partitions[0].BuffData) {
			GeometryList.push(Geo);
		}
	
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
	NiGeometryBufferData* GeoData = ModelData->BuffData;
	NiSkinInstance* SkinInstance = Geo->skinInstance;
	NiD3DShaderDeclaration* ShaderDeclaration = Geo->shader->ShaderDeclaration;

	NiSkinPartition* SkinPartition = SkinInstance->SkinPartition;
	D3DPRIMITIVETYPE PrimitiveType = (SkinPartition->Partitions[0].Strips == 0) ? D3DPT_TRIANGLELIST : D3DPT_TRIANGLESTRIP;
	TheRenderManager->CalculateBoneMatrixes(SkinInstance, &Geo->m_worldTransform);
	
	if (SkinInstance->SkinToWorldWorldToSkin) 
		memcpy(&TheShaderManager->Effects.ShadowsExteriors->Constants.ShadowWorld, SkinInstance->SkinToWorldWorldToSkin, 0x40);

	for (UInt32 p = 0; p < SkinPartition->PartitionsCount; p++) {
		if (!SkinInstance->IsPartitionEnabled(p)) continue;

		NiSkinPartition::Partition* Partition = &SkinPartition->Partitions[p];
		GeoData = Partition->BuffData;

		//Constants.BoneMatrices = (D3DXVECTOR4*)SkinInstance->BoneMatrixes;
		int StartRegister = 9;
		for (int i = 0; i < Partition->Bones; i++) {
			UInt16 NewIndex = (Partition->pBones == NULL) ? i : Partition->pBones[i];
			TheRenderManager->device->SetVertexShaderConstantF(StartRegister, ((float*)SkinInstance->BoneMatrixes) + (NewIndex * 3 * 4), 3);
			StartRegister += 3;
		}

		TheRenderManager->PackSkinnedGeometryBuffer(GeoData, ModelData, SkinInstance, Partition, ShaderDeclaration);
		DrawGeometryBuffer(Geo, GeoData);
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


bool SpeedTreeShadowRenderPass::AccumObject(NiGeometry* Geo) {

	NiShadeProperty* shaderProp = static_cast<NiShadeProperty*>(Geo->GetProperty(NiProperty::kType_Shade));
	if (shaderProp->type != NiShadeProperty::kProp_SpeedTreeLeaf) return false;

	GeometryList.push(Geo);
	return true;
}


void SpeedTreeShadowRenderPass::UpdateConstants(NiGeometry* Geo) {

	ShadowsExteriorEffect::ShadowStruct* ShadowConstants = &TheShaderManager->Effects.ShadowsExteriors->Constants;
	IDirect3DDevice9* Device = TheRenderManager->device;
	NiDX9RenderState* RenderState = TheRenderManager->renderState;

	ShadowConstants->Data.x = 2.0f; // Type of geo (0 normal, 1 actors (skinned), 2 speedtree leaves)
	ShadowConstants->Data.y = 0.0f; // Alpha control
	TheRenderManager->CreateD3DMatrix(&TheShaderManager->ShaderConst.ShadowWorld, &Geo->m_worldTransform);

	//Constants.BillboardRight = (D3DXVECTOR4*)&TheShadowManager->BillboardRight;
	//Constants.BillboardUp = (D3DXVECTOR4*)&TheShadowManager->BillboardUp;

	//// Bind constant values for leaf transformation
	Device->SetVertexShaderConstantF(63, (float*)&TheShadowManager->BillboardRight, 1);
	Device->SetVertexShaderConstantF(64, (float*)&TheShadowManager->BillboardUp, 1);
	Device->SetVertexShaderConstantF(65, Pointers::ShaderParams::RockParams, 1);
	Device->SetVertexShaderConstantF(66, Pointers::ShaderParams::RustleParams, 1);
	Device->SetVertexShaderConstantF(67, Pointers::ShaderParams::WindMatrixes, 16);

	SpeedTreeLeafShaderProperty* STProp = (SpeedTreeLeafShaderProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Shade);
	BSTreeNode* Node = (BSTreeNode*)Geo->m_parent->m_parent;
	NiDX9SourceTextureData* Texture = (NiDX9SourceTextureData*)Node->TreeModel->LeavesTexture->rendererData;

	//Constants.LeafBase = (D3DXVECTOR4*)STProp->leafData->leafBase;
	if (Texture) ShadowConstants->Data.y = 1.0f;
	//Constants.DiffuseMap = Texture->dTexture;

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