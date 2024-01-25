#define ShadowMapFarPlane 32768;

/*
* Initializes the Shadow Manager by grabbing the relevant settings and shaders, and setting up map sizes.
*/
void ShadowManager::Initialize() {
	
	Logger::Log("Starting the shadows manager...");
	TheShadowManager = new ShadowManager();

	// load the shaders
	TheShadowManager->ShadowMapVertex = (ShaderRecordVertex*)ShaderRecord::LoadShader("ShadowMap.vso", "Shadows\\");
	TheShadowManager->ShadowMapPixel = (ShaderRecordPixel*)ShaderRecord::LoadShader("ShadowMap.pso", "Shadows\\");
	TheShadowManager->ShadowCubeMapVertex = (ShaderRecordVertex*)ShaderRecord::LoadShader("ShadowCubeMap.vso", "Shadows\\");
	TheShadowManager->ShadowCubeMapPixel = (ShaderRecordPixel*)ShaderRecord::LoadShader("ShadowCubeMap.pso", "Shadows\\");

    TheShadowManager->ShadowMapBlurVertex = (ShaderRecordVertex*) ShaderRecord::LoadShader("ShadowMapBlur.vso", "Shadows\\");
    TheShadowManager->ShadowMapBlurPixel = (ShaderRecordPixel*) ShaderRecord::LoadShader("ShadowMapBlur.pso", "Shadows\\");

    if (TheShadowManager->ShadowMapVertex == nullptr || TheShadowManager->ShadowMapPixel == nullptr  || TheShadowManager->ShadowMapBlurVertex  == nullptr
        || TheShadowManager->ShadowCubeMapVertex == nullptr || TheShadowManager->ShadowCubeMapPixel == nullptr || TheShadowManager->ShadowMapBlurPixel  == nullptr ){
        Logger::Log("[ERROR]: Could not load one or more of the ShadowMap generation shaders. Reinstall the mod.");
    }

	UINT ShadowCubeMapSize = TheShaderManager->Effects.ShadowsExteriors->Settings.Interiors.ShadowCubeMapSize;
	TheShadowManager->ShadowCubeMapViewPort = { 0, 0, ShadowCubeMapSize, ShadowCubeMapSize, 0.0f, 1.0f };
	memset(TheShadowManager->ShadowCubeMapLights, NULL, sizeof(ShadowCubeMapLights));

	TheShadowManager->shadowMapsRenderTime = 0;
}


/*
* Returns the given object ref if it passes the test for excluded form types, otherwise returns NULL.
*/
TESObjectREFR* ShadowManager::GetRef(TESObjectREFR* Ref, ShadowsExteriorEffect::FormsStruct* Forms) {
	
	if (!Ref || !Ref->GetNode()) return NULL;
	if (Ref->flags & TESForm::FormFlags::kFormFlags_NotCastShadows) return NULL;

	TESForm* Form = Ref->baseForm;
	UInt8 TypeID = Form->formType;
	switch (TypeID) {
	case TESForm::FormType::kFormType_Activator:
		if (!Forms->Activators) return NULL; 
		break;
	case TESForm::FormType::kFormType_Apparatus:
		if (!Forms->Apparatus) return NULL;
		break;
	case TESForm::FormType::kFormType_Book:
		if (!Forms->Books) return NULL;
		break;
	case TESForm::FormType::kFormType_Container:
		if (!Forms->Containers) return NULL;
		break;
	case TESForm::FormType::kFormType_Door:
		if (!Forms->Doors) return NULL;
		break;
	case TESForm::FormType::kFormType_Misc:
		if (!Forms->Misc) return NULL;
		break;
	case TESForm::FormType::kFormType_Tree:
		if (!Forms->Trees) return NULL;
		break;
	case TESForm::FormType::kFormType_Furniture:
		if (!Forms->Furniture) return NULL;
		break;
	case TESForm::FormType::kFormType_Land:
		if (!Forms->Terrain) return NULL;
		break;
	case TESForm::FormType::kFormType_NPC:
		if (TypeID <= TESForm::FormType::kFormType_LeveledCreature && !Forms->Actors) return NULL;
		break;
	case TESForm::FormType::kFormType_Stat:
		if (TypeID <= TESForm::FormType::kFormType_MoveableStatic && !Forms->Statics) return NULL;
		break;
	default:
		break;
	}
	// disabled for now since it's an obscure functionality
	//if (ExcludedForms->size() > 0 && std::binary_search(ExcludedForms->begin(), ExcludedForms->end(), Form->refID)) return NULL;

	ExtraRefractionProperty* RefractionExtraProperty = (ExtraRefractionProperty*)Ref->extraDataList.GetExtraData(BSExtraData::ExtraDataType::kExtraData_RefractionProperty);
	float Refraction = RefractionExtraProperty ? (1 - RefractionExtraProperty->refractionAmount) : 0.0f;
	if (Refraction >= 0.5) return NULL;

	return Ref;
}

// Detect which pass the object must be added to
void ShadowManager::AccumObject(std::stack<NiAVObject*>* containersAccum, NiAVObject* NiObject) {
	NiGeometry* geo;
	if (NiObject->IsGeometry()) {
		geo = static_cast<NiGeometry*>(NiObject);
		if (!geo->shader) return; // skip Geometry without a shader

		NiShadeProperty* shaderProp = static_cast<NiShadeProperty*>(geo->GetProperty(NiProperty::kType_Shade));

#if defined(OBLIVION)
		if (geo->m_pcName && !memcmp(geo->m_pcName, "Torch", 5)) return; // No torch geo, it is too near the light and a bad square is rendered.
#endif
		// check data for rigged geometry
		if (geo->skinInstance && geo->skinInstance->SkinPartition && geo->skinInstance->SkinPartition->Partitions) {
			if (!geo->skinInstance->SkinPartition->Partitions[0].BuffData) return; // discard objects without buffer data
			skinnedGeoAccum.push(geo);
		}
		else if (shaderProp->type == NiShadeProperty::kProp_SpeedTreeLeaf) {
			speedTreeAccum.push(geo);
		}
		else {
			if (!geo->geomData->BuffData) return; // discard objects without buffer data
			geometryAccum.push(geo);
		}
	}
	else {
		containersAccum->push(NiObject);
	}
}

// go through the Object children and sort the ones that will be rendered based on their properties
void ShadowManager::AccumChildren(NiAVObject* NiObject, float MinRadius) {
	std::stack<NiAVObject*> containers;
	NiAVObject* child;
	NiAVObject* object;
	NiNode* Node;

	AccumObject(&containers, NiObject); //list all objects contained, or sort the object if not a container

	// Gather geometry
	while (!containers.empty()) {
    		object = containers.top();
    		containers.pop();

    		Node = object ? object->IsNiNode() : nullptr;
    		if (!Node || Node->m_flags & NiAVObject::NiFlags::APP_CULLED || Node->GetWorldBoundRadius() < MinRadius) continue; // culling containers

		for (int i = 0; i < Node->m_children.end; i++) {
			child = Node->m_children.data[i];
			if (!child || child->m_flags & NiAVObject::NiFlags::APP_CULLED || child->GetWorldBoundRadius() < MinRadius) continue; // culling children
			if (child->IsFadeNode() && static_cast<BSFadeNode*>(child)->FadeAlpha < 0.75f) continue; // stop rendering fadenodes below a certain opacity
			AccumObject(&containers, child);
		}
	}
}

// Go through accumulations and render found objects
void ShadowManager::RenderAccums() {
	NiDX9RenderState* RenderState = TheRenderManager->renderState;
	IDirect3DDevice9* Device = TheRenderManager->device;
	ShadowsExteriorEffect::ShadowStruct* Constants = &TheShaderManager->Effects.ShadowsExteriors->Constants;

	// Render normal geometry
	if (!geometryAccum.empty()) {
		Constants->Data.x = 0.0f; // Type of geo (0 normal, 1 actors (skinned), 2 speedtree leaves)
		Constants->Data.y = 0.0f; // Alpha control
		CurrentPixel->SetCT();
		CurrentVertex->SetCT();

		// Set diffuse texture sampler state
		RenderState->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP, false);
		RenderState->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP, false);
		RenderState->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT, false);
		RenderState->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT, false);
		RenderState->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT, false);
		while (!geometryAccum.empty()) {
			RenderGeometry(geometryAccum.top());
			geometryAccum.pop();
		}
	}

	// Render skinned geometry
	Constants->Data.x = 1.0f; // Type of geo (0 normal, 1 actors (skinned), 2 speedtree leaves)
	Constants->Data.y = 0.0f; // Alpha control

	CurrentPixel->SetCT();
	CurrentVertex->SetCT();
	while (!skinnedGeoAccum.empty()) {
		RenderSkinnedGeometry(skinnedGeoAccum.top());
		skinnedGeoAccum.pop();
	}

	// Render speedtree
	if (!speedTreeAccum.empty()) {
		Constants->Data.x = 2.0f; // Type of geo (0 normal, 1 actors (skinned), 2 speedtree leaves)
		Constants->Data.y = 0.0f; // Alpha control
		CurrentPixel->SetCT();
		CurrentVertex->SetCT();

		// Bind constant values for leaf transformation
		Device->SetVertexShaderConstantF(63, (float*)&BillboardRight, 1);
		Device->SetVertexShaderConstantF(64, (float*)&BillboardUp, 1);
		Device->SetVertexShaderConstantF(65, Pointers::ShaderParams::RockParams, 1);
		Device->SetVertexShaderConstantF(66, Pointers::ShaderParams::RustleParams, 1);
		Device->SetVertexShaderConstantF(67, Pointers::ShaderParams::WindMatrixes, 16);

		// Set diffuse texture sampler state
		RenderState->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP, false);
		RenderState->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP, false);
		RenderState->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT, false);
		RenderState->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT, false);
		RenderState->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT, false);
		while (!speedTreeAccum.empty()) {
			RenderSpeedTreeGeometry(speedTreeAccum.top());
			speedTreeAccum.pop();
		}
	}

}

// Renders basic geometry
void ShadowManager::RenderGeometry(NiGeometry* Geo) {	
	NiDX9RenderState* RenderState = TheRenderManager->renderState;
	NiGeometryData* ModelData = Geo->geomData;
	NiGeometryBufferData* GeoData = ModelData->BuffData;
	NiD3DShaderDeclaration* ShaderDeclaration = Geo->shader->ShaderDeclaration;
	ShadowsExteriorEffect::ShadowStruct* Constants = &TheShaderManager->Effects.ShadowsExteriors->Constants;

	TheRenderManager->CreateD3DMatrix(&Constants->ShadowWorld, &Geo->m_worldTransform);
	BSShaderProperty* ShaderProperty = (BSShaderProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Shade);
	if (!ShaderProperty || !ShaderProperty->IsLightingProperty()) return;

	Constants->Data.y = 0.0f; // Alpha Control
	if (AlphaEnabled) {
		NiAlphaProperty* AProp = (NiAlphaProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Alpha);
		if (AProp->flags & NiAlphaProperty::AlphaFlags::ALPHA_BLEND_MASK || AProp->flags & NiAlphaProperty::AlphaFlags::TEST_ENABLE_MASK) {
			if (NiTexture* Texture = *((BSShaderPPLightingProperty*)ShaderProperty)->ppTextures[0]) {
				
				Constants->Data.y = 1.0f; // Alpha Control

				// Set diffuse texture at register 0
				RenderState->SetTexture(0, Texture->rendererData->dTexture);
			}
		}
	}

	// update alpha control constant
	CurrentVertex->SetCT();
	CurrentPixel->SetCT();

	TheRenderManager->PackGeometryBuffer(GeoData, ModelData, NULL, ShaderDeclaration);
	DrawGeometryBuffer(GeoData, GeoData->VertCount);
}

// Render Speedtree
void ShadowManager::RenderSpeedTreeGeometry(NiGeometry* Geo) {

	IDirect3DDevice9* Device = TheRenderManager->device;
	NiDX9RenderState* RenderState = TheRenderManager->renderState;
	NiGeometryData* ModelData = Geo->geomData;
	NiGeometryBufferData* GeoData = ModelData->BuffData;
	NiD3DShaderDeclaration* ShaderDeclaration = Geo->shader->ShaderDeclaration;

	TheRenderManager->CreateD3DMatrix(&TheShaderManager->Effects.ShadowsExteriors->Constants.ShadowWorld, &Geo->m_worldTransform);
	SpeedTreeLeafShaderProperty* STProp = (SpeedTreeLeafShaderProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Shade);
	BSTreeNode* Node = (BSTreeNode*)Geo->m_parent->m_parent;
	NiDX9SourceTextureData* Texture = (NiDX9SourceTextureData*)Node->TreeModel->LeavesTexture->rendererData;

	// Bind constant values for leaf transformation
	Device->SetVertexShaderConstantF(83, STProp->leafData->leafBase, 48);

	// Set diffuse texture at register 0
	RenderState->SetTexture(0, Texture->dTexture);
	TheRenderManager->PackGeometryBuffer(GeoData, ModelData, NULL, ShaderDeclaration);
	DrawGeometryBuffer(GeoData, GeoData->VertCount);

}

// Prepares and renders geometry that is skinned (uses bones for deformation)
void ShadowManager::RenderSkinnedGeometry(NiGeometry* Geo) {
	NiGeometryData* ModelData = Geo->geomData;
	NiGeometryBufferData* GeoData = ModelData->BuffData;
	NiSkinInstance* SkinInstance = Geo->skinInstance;
	NiD3DShaderDeclaration* ShaderDeclaration = Geo->shader->ShaderDeclaration;

	NiSkinPartition* SkinPartition = SkinInstance->SkinPartition;
	D3DPRIMITIVETYPE PrimitiveType = (SkinPartition->Partitions[0].Strips == 0) ? D3DPT_TRIANGLELIST : D3DPT_TRIANGLESTRIP;
	TheRenderManager->CalculateBoneMatrixes(SkinInstance, &Geo->m_worldTransform);
	if (SkinInstance->SkinToWorldWorldToSkin) memcpy(&TheShaderManager->Effects.ShadowsExteriors->Constants.ShadowWorld, SkinInstance->SkinToWorldWorldToSkin, 0x40);
	for (UInt32 p = 0; p < SkinPartition->PartitionsCount; p++) {
		if (!SkinInstance->IsPartitionEnabled(p)) continue;

		int StartRegister = 9;
		NiSkinPartition::Partition* Partition = &SkinPartition->Partitions[p];
		for (int i = 0; i < Partition->Bones; i++) {
			UInt16 NewIndex = (Partition->pBones == NULL) ? i : Partition->pBones[i];
			TheRenderManager->device->SetVertexShaderConstantF(StartRegister, ((float*)SkinInstance->BoneMatrixes) + (NewIndex * 3 * 4), 3);
			StartRegister += 3;
		}

		GeoData = Partition->BuffData;
		TheRenderManager->PackSkinnedGeometryBuffer(GeoData, ModelData, SkinInstance, Partition, ShaderDeclaration);
		DrawGeometryBuffer(GeoData, Partition->Vertices);
	}
}

// draws the geo data from the Geometry buffer
void ShadowManager::DrawGeometryBuffer(NiGeometryBufferData* GeoData, UINT verticesCount) {
	IDirect3DDevice9* Device = TheRenderManager->device;
	NiDX9RenderState* RenderState = TheRenderManager->renderState;
	int StartIndex = 0;
	int PrimitiveCount = 0;

	for (UInt32 i = 0; i < GeoData->StreamCount; i++) {
		Device->SetStreamSource(i, GeoData->VBChip[i]->VB, 0, GeoData->VertexStride[i]);
	}
	Device->SetIndices(GeoData->IB);
	if (GeoData->FVF)
		RenderState->SetFVF(GeoData->FVF, false);
	else
		RenderState->SetVertexDeclaration(GeoData->VertexDeclaration, false);

	for (UInt32 i = 0; i < GeoData->NumArrays; i++) {
		if (GeoData->ArrayLengths)
			PrimitiveCount = GeoData->ArrayLengths[i] - 2;
		else
			PrimitiveCount = GeoData->TriCount;
		Device->DrawIndexedPrimitive(GeoData->PrimitiveType, GeoData->BaseVertexIndex, 0, verticesCount, StartIndex, PrimitiveCount);
		StartIndex += PrimitiveCount + 2;
	}
}


void ShadowManager::RenderShadowMap(ShadowsExteriorEffect::ShadowMapSettings* ShadowMap, D3DMATRIX* ViewProj) {
	auto timer = TimeLogger();
	ShadowsExteriorEffect* Shadows = TheShaderManager->Effects.ShadowsExteriors;

	if (!ShadowMap->ShadowMapDepthSurface) {
		//Logger::Log("ShadowDepthSurface missing for cascade %i", ShadowMapType);
		return;
	}
	if (!ShadowMap->ShadowMapSurface) {
		//Logger::Log("ShadowSurface missing for cascade %i", ShadowMapType);
		return;
	}

	IDirect3DDevice9* Device = TheRenderManager->device;
	NiDX9RenderState* RenderState = TheRenderManager->renderState;

	ShadowMap->ShadowCameraToLight = TheRenderManager->InvViewProjMatrix * (*ViewProj);
	TheCameraManager->SetFrustum(&ShadowMap->ShadowMapFrustum, ViewProj);
    /*for (int i = 1; i < 4; i++){
        IDirect3DSurface9* targ= nullptr;
        Device->GetRenderTarget(i , &targ);
        Logger::Log("%u  : %08X", i, targ );
    }*/
    Device->SetRenderTarget(0, ShadowMap->ShadowMapSurface);
	Device->SetDepthStencilSurface(ShadowMap->ShadowMapDepthSurface);
	Device->SetViewport(&ShadowMap->ShadowMapViewPort);

	Device->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(1.0f, 0.25f, 0.25f, 0.55f), 1.0f, 0L);

	RenderState->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_ZWRITEENABLE, D3DZB_TRUE, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_ALPHABLENDENABLE, 0, RenderStateArgs);
	RenderState->SetVertexShader(ShadowMapVertex->ShaderHandle, false);
	RenderState->SetPixelShader(ShadowMapPixel->ShaderHandle, false);
	Device->BeginScene();

	GridCellArray* CellArray = Tes->gridCellArray;
	UInt32 CellArraySize = CellArray->size * CellArray->size;
	if (Player->GetWorldSpace()) {
		for (UInt32 i = 0; i < CellArraySize; i++) {
			if (TESObjectCELL* Cell = CellArray->GetCell(i)) {
				RenderExteriorCell(Cell, ShadowMap);
			}
		}
	}
	else {
		RenderExteriorCell(Player->parentCell, ShadowMap);
	}

	Device->EndScene();

	shadowMapsRenderTime = timer.LogTime("ShadowManager::RenderShadowMap ");
}


void ShadowManager::RenderExteriorCell(TESObjectCELL* Cell, ShadowsExteriorEffect::ShadowMapSettings* ShadowMap) {
	if (!Cell || Cell->IsInterior())
		return;
	
	if (ShadowMap->Forms.Terrain) {
		NiNode* LandNode = Cell->GetChildNode(TESObjectCELL::kCellNode_Land);
		// if (ShadowsExteriors->Forms[ShadowMapType].Lod) RenderLod(Tes->landLOD, ShadowMapType); //Render terrain LOD
		if (LandNode) AccumChildren((NiAVObject*)LandNode, 0);
	}

	TList<TESObjectREFR>::Entry* Entry = &Cell->objectList.First;
	while (Entry) {
		if (TESObjectREFR* Ref = GetRef(Entry->item, &ShadowMap->Forms)) {
			NiNode* RefNode = Ref->GetNode();
			if (TheCameraManager->InFrustum(&ShadowMap->ShadowMapFrustum, RefNode)) AccumChildren(RefNode, ShadowMap->Forms.MinRadius);
		}
		Entry = Entry->next;
	}

	RenderAccums();
}

void ShadowManager::RenderShadowCubeMap(ShadowSceneLight** Lights, UInt32 LightIndex) {
	if (Lights[LightIndex] == NULL) return; // No light at current index
	
	ShadowsExteriorEffect* Shadows = TheShaderManager->Effects.ShadowsExteriors;
	ShadowsExteriorEffect::InteriorsStruct* Settings = &Shadows->Settings.Interiors;

	if (!Shadows->Textures.ShadowCubeMapDepthSurface) {
		Logger::Log("ShadowCubemapDepth missing for light %i", LightIndex);
		return;
	}

	auto timer = TimeLogger();

	//ShaderConstants::ShadowMapStruct* ShadowMap = &TheShaderManager->ShaderConst.ShadowMap;
	IDirect3DDevice9* Device = TheRenderManager->device;
	NiDX9RenderState* RenderState = TheRenderManager->renderState;
	float Radius = 0.0f;
	float MinRadius = Settings->Forms.MinRadius;
	NiPoint3* LightPos = NULL;
	D3DXMATRIX View, Proj;
	D3DXVECTOR3 Eye, At, Up, CameraDirection;

	Device->SetDepthStencilSurface(Shadows->Textures.ShadowCubeMapDepthSurface);

	NiPointLight* pNiLight = Lights[LightIndex]->sourceLight;

	LightPos = &pNiLight->m_worldTransform.pos;
	Radius = pNiLight->Spec.r * Shadows->Settings.Interiors.LightRadiusMult;
	if (pNiLight->CanCarry)
		Radius = 256.0f;
	Eye.x = LightPos->x - TheRenderManager->CameraPosition.x;
	Eye.y = LightPos->y - TheRenderManager->CameraPosition.y;
	Eye.z = LightPos->z - TheRenderManager->CameraPosition.z;
	Shadows->Constants.ShadowCubeMapLightPosition.x = Eye.x;
	Shadows->Constants.ShadowCubeMapLightPosition.y = Eye.y;
	Shadows->Constants.ShadowCubeMapLightPosition.z = Eye.z;
	Shadows->Constants.ShadowCubeMapLightPosition.w = Radius;
	Shadows->Constants.Data.z = Radius;
	D3DXMatrixPerspectiveFovRH(&Proj, D3DXToRadian(90.0f), 1.0f, 0.1f, Radius);

	RenderState->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_ZWRITEENABLE, D3DZB_TRUE, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_ALPHABLENDENABLE, 0, RenderStateArgs);
	RenderState->SetVertexShader(ShadowCubeMapVertex->ShaderHandle, false);
	RenderState->SetPixelShader(ShadowCubeMapPixel->ShaderHandle, false);

	for (int Face = 0; Face < 6; Face++) {
		At = Eye;
		switch (Face) {
		case D3DCUBEMAP_FACE_POSITIVE_X:
			CameraDirection = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
			Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			break;
		case D3DCUBEMAP_FACE_NEGATIVE_X:
			CameraDirection = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
			Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			break;
		case D3DCUBEMAP_FACE_POSITIVE_Y:
			CameraDirection = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			Up = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
			break;
		case D3DCUBEMAP_FACE_NEGATIVE_Y:
			CameraDirection = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
			Up = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			break;
		case D3DCUBEMAP_FACE_POSITIVE_Z:
			CameraDirection = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			break;
		case D3DCUBEMAP_FACE_NEGATIVE_Z:
			CameraDirection = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
			Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			break;
		}
		At += CameraDirection;

		// Since this is pure geometry, getting reference data will be difficult (read: slow)
		auto iter = Lights[LightIndex]->kGeometryList.start;
		while (iter) {
			NiGeometry* geo = iter->data;
			iter = iter->next;
			if (!geo || geo->m_flags & NiAVObject::APP_CULLED)
				continue;

			NiShadeProperty* shaderProp = static_cast<NiShadeProperty*>(geo->GetProperty(NiProperty::kType_Shade));
			NiMaterialProperty* matProp = static_cast<NiMaterialProperty*>(geo->GetProperty(NiProperty::kType_Material));

			if (!shaderProp)
				continue;

			bool isFirstPerson = (shaderProp->flags & NiShadeProperty::kFirstPerson) != 0;
			bool isThirdPerson = (shaderProp->flags & NiShadeProperty::kThirdPerson) != 0;

			// Skip objects if they are barely visible. 
			if ((matProp && matProp->fAlpha < 0.05f))
				continue;

			// Also skip viewmodel due to issues, and render player's model only in 3rd person
			if (isFirstPerson && !Player->isThirdPerson && !Settings->PlayerShadowFirstPerson)
				continue;
				
			if (isThirdPerson && Player->isThirdPerson && !Settings->PlayerShadowThirdPerson)
				continue;

			// check data for rigged geometry
			if (geo->skinInstance && geo->skinInstance->SkinPartition && geo->skinInstance->SkinPartition->Partitions) {
				if (!geo->skinInstance->SkinPartition->Partitions[0].BuffData) 
					continue; // discard objects without buffer data
				skinnedGeoAccum.push(geo);
			}
			else if (shaderProp->type == NiShadeProperty::kProp_SpeedTreeLeaf) {
				speedTreeAccum.push(geo);
			}
			else {
				if (!geo->geomData->BuffData) 
					continue; // discard objects without buffer data
				geometryAccum.push(geo);
			}
		}
		
		D3DXMatrixLookAtRH(&View, &Eye, &At, &Up);
		Shadows->Constants.ShadowViewProj = View * Proj;

		if (!Shadows->Textures.ShadowCubeMapSurface[LightIndex][Face]) {
			Logger::Log("ShadowCubemapSurface missing for light %i, face %i", LightIndex, Face);
			continue;
		}

		Device->SetRenderTarget(0, Shadows->Textures.ShadowCubeMapSurface[LightIndex][Face]);
		Device->SetViewport(&ShadowCubeMapViewPort);
		Device->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(1.0f, 0.25f, 0.25f, 0.55f), 1.0f, 0L);
		Device->BeginScene();

		RenderAccums();

		Device->EndScene();
	}

	timer.LogTime("RenderShadowCubeMap");	
}


static void FlagShaderPropertyRecurse(NiAVObject* apObject, UInt32 auiFlags, bool abSet) {
	if (!apObject)
		return;

	if (apObject->IsGeometry()) {
		NiGeometry* pGeometry = static_cast<NiGeometry*>(apObject);
		NiShadeProperty* shaderProperty = static_cast<NiShadeProperty*>(pGeometry->GetProperty(NiProperty::kType_Shade));
		if (shaderProperty) {
			if (abSet)
				shaderProperty->flags |= auiFlags;
			else
				shaderProperty->flags &= ~auiFlags;
		}

	}
	else if (apObject->IsNiNode()) {
		NiNode* pNiNode = static_cast<NiNode*>(apObject);
		for (UInt32 i = 0; i < pNiNode->m_children.end; i++) {
			FlagShaderPropertyRecurse(pNiNode->m_children.data[i], auiFlags, abSet);
		}
	}
}

static SInt32 frames = -1;
static void FlagPlayerGeometry() {
	frames++;

	// Run this function every 50 frames, or on launch
	if (frames > 50 || frames == -1) {
		if (Player->firstPersonNiNode)
			FlagShaderPropertyRecurse(Player->firstPersonNiNode, NiShadeProperty::kFirstPerson, true);

		NiNode* node = Player->GetNode();
		if (node)
			FlagShaderPropertyRecurse(node, NiShadeProperty::kThirdPerson, true);

		frames = 0;
	}
}


D3DXMATRIX ShadowManager::GetViewMatrix(D3DXVECTOR3* At, D3DXVECTOR4* Dir) {
	D3DXVECTOR3 Up = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	float FarPlane = ShadowMapFarPlane;

	// calculating the projection matrix for point of view of the light
	D3DXVECTOR3 Eye;
	Eye.x = At->x - FarPlane * Dir->x * -1;
	Eye.y = At->y - FarPlane * Dir->y * -1;
	Eye.z = At->z - FarPlane * Dir->z * -1;

	D3DXMATRIX View;
	D3DXMatrixLookAtRH(&View, &Eye, At, &Up);

	// save Billboard values for speedtree leafs rendering
	BillboardRight = { View._11, View._21, View._31, 0.0f };
	BillboardUp = { View._12, View._22, View._32, 0.0f };

	return View;
}

/*
* Renders the different shadow maps: Near, Far, Ortho.
*/
void ShadowManager::RenderShadowMaps() {
	if (!TheSettingManager->SettingsMain.Main.RenderEffects) return; // cancel out if rendering effects is disabled

	ShadowsExteriorEffect* Shadows = TheShaderManager->Effects.ShadowsExteriors;
	ShadowsExteriorEffect::ExteriorsStruct* ShadowsExteriors = &Shadows->Settings.Exteriors;
	ShadowsExteriorEffect::InteriorsStruct* ShadowsInteriors = &Shadows->Settings.Interiors;

	bool isExterior = TheShaderManager->GameState.isExterior;// || currentCell->flags0 & TESObjectCELL::kFlags0_BehaveLikeExterior; // exterior flag currently broken

	bool ExteriorEnabled = isExterior && TheShaderManager->Effects.ShadowsExteriors->Enabled && ShadowsExteriors->Enabled;
	bool InteriorEnabled = !isExterior && TheShaderManager->Effects.ShadowsInteriors->Enabled;

	// early out in case shadow rendering is not required
	if (!ExteriorEnabled && !InteriorEnabled && !TheShaderManager->orthoRequired) return;

	auto timer = TimeLogger();

	// prepare some pointers to the device and surfaces
	IDirect3DDevice9* Device = TheRenderManager->device;
	NiDX9RenderState* RenderState = TheRenderManager->renderState;
	IDirect3DSurface9* DepthSurface = NULL;
	IDirect3DSurface9* RenderSurface = NULL;
	D3DVIEWPORT9 viewport;

	D3DXVECTOR4* ShadowData = &TheShaderManager->Effects.ShadowsExteriors->Constants.Data;
	D3DXVECTOR4* OrthoData = &TheShaderManager->Effects.ShadowsExteriors->Constants.OrthoData;
	Device->GetDepthStencilSurface(&DepthSurface);
	Device->GetRenderTarget(0, &RenderSurface);
	Device->GetViewport(&viewport);	

	RenderState->SetRenderState(D3DRS_STENCILENABLE , 1 ,RenderStateArgs);
	RenderState->SetRenderState(D3DRS_STENCILREF , 0 ,RenderStateArgs);
 	RenderState->SetRenderState(D3DRS_STENCILFUNC , 8 ,RenderStateArgs);

	TheRenderManager->UpdateSceneCameraData();
	TheRenderManager->SetupSceneCamera();
	
	D3DXVECTOR4 PlayerPosition = Player->pos.toD3DXVEC4();
	TESObjectCELL* currentCell = Player->parentCell;

	// Flag player geometry so we can control if it should be rendered in shadow cubemaps
	FlagPlayerGeometry();

	// Render directional shadows for Sun/Moon
	ShadowData->w = ShadowsExteriors->ShadowMode;	// Mode (0:off, 1:VSM, 2:ESM, 3: ESSM);
	NiNode* PlayerNode = Player->GetNode();
	D3DXVECTOR3 At;
	At.x = PlayerNode->m_worldTransform.pos.x - TheRenderManager->CameraPosition.x;
	At.y = PlayerNode->m_worldTransform.pos.y - TheRenderManager->CameraPosition.y;
	At.z = PlayerNode->m_worldTransform.pos.z - TheRenderManager->CameraPosition.z;

	CurrentVertex = ShadowMapVertex;
	CurrentPixel = ShadowMapPixel;

	// Render all shadow maps
	D3DXVECTOR4* SunDir = &TheShaderManager->ShaderConst.SunDir;
	if (isExterior && ExteriorEnabled && SunDir->z > 0.0f) {
		ShadowData->z = 0; // set shader constant to identify other shadow maps
		D3DXMATRIX View = GetViewMatrix(&At, SunDir);
		for (int i = MapNear; i < MapOrtho; i++) {
			ShadowsExteriorEffect::ShadowMapSettings* ShadowMap = &Shadows->ShadowMaps[i];
			Shadows->Constants.ShadowViewProj = Shadows->GetCascadeViewProj(ShadowMap, View);

			RenderShadowMap(ShadowMap, &Shadows->Constants.ShadowViewProj);
			if(ShadowsExteriors->BlurShadowMaps) BlurShadowMap(ShadowMap);
		}
	}

	// render ortho map if one of the effects using ortho is active
	if (isExterior && TheShaderManager->orthoRequired) {
		ShadowData->z = 1; // identify ortho map in shader constant
		D3DXVECTOR4 OrthoDir = D3DXVECTOR4(0.05f, 0.05f, 1.0f, 1.0f);
		D3DMATRIX View = GetViewMatrix(&At, &OrthoDir);
		Shadows->Constants.ShadowViewProj = Shadows->GetOrthoViewProj(View);

		RenderShadowMap(&Shadows->ShadowMaps[MapOrtho], &Shadows->Constants.ShadowViewProj);
		OrthoData->x = Shadows->ShadowMaps[MapOrtho].ShadowMapRadius * 2;
	}

	// Render shadow maps for point lights
	bool usePointLights = (TheShaderManager->GameState.isDayTime > 0.5) ? ShadowsExteriors->UsePointShadowsDay : ShadowsExteriors->UsePointShadowsNight;

	// track point lights for interiors and exteriors
	ShadowSceneLight* ShadowLights[ShadowCubeMapsMax] = { NULL };
	NiPointLight* Lights[TrackedLightsMax] = { NULL };
	TheShaderManager->GetNearbyLights(ShadowLights, Lights);

	if ((isExterior && usePointLights) || (!isExterior && InteriorEnabled)) {
		CurrentVertex = ShadowCubeMapVertex;
		CurrentPixel = ShadowCubeMapPixel;
		AlphaEnabled = ShadowsInteriors->AlphaEnabled;

		// render the cubemaps for each light
		for (int i = 0; i < ShadowsInteriors->LightPoints; i++) {
			RenderShadowCubeMap(ShadowLights, i);
		}
	}

	if (isExterior) {
		// Update constants used by shadow shaders: x=quality, y=darkness
		ShadowData->x = ShadowsExteriors->Quality;
		if (TheShaderManager->Effects.ShadowsExteriors->Enabled) ShadowData->x = -1; // Disable the forward shadowing
		ShadowData->y = ShadowsExteriors->Darkness;
	}
	else {
		ShadowData->x = ShadowsInteriors->Quality;
		if (TheShaderManager->Effects.ShadowsInteriors->Enabled) ShadowData->x = -1; // Disable the forward shadowing
		ShadowData->y = ShadowsInteriors->Darkness;
		ShadowData->z = 1.0f / (float)ShadowsInteriors->ShadowCubeMapSize;
	}

	// reset renderer to previous state
	Device->SetDepthStencilSurface(DepthSurface);
	Device->SetRenderTarget(0, RenderSurface);
	Device->SetViewport(&viewport);

	//release smart pointers to prevent memory leak
	if (DepthSurface) DepthSurface->Release();
	if (RenderSurface) RenderSurface->Release();

	if (TheSettingManager->SettingsMain.Develop.DebugMode && !InterfaceManager->IsActive(Menu::MenuType::kMenuType_Console)) {
		if (Global->OnKeyDown(0x17)) { // TODO: setting for debug key ?
			char Filename[MAX_PATH];

			time_t CurrentTime = time(NULL);
			GetCurrentDirectoryA(MAX_PATH, Filename);
			strcat(Filename, "\\Test");
			if (GetFileAttributesA(Filename) == INVALID_FILE_ATTRIBUTES) CreateDirectoryA(Filename, NULL);
			D3DXSaveSurfaceToFileA(".\\Test\\shadowmap0.jpg", D3DXIFF_JPG, Shadows->ShadowMaps[MapNear].ShadowMapSurface, NULL, NULL);
			D3DXSaveSurfaceToFileA(".\\Test\\shadowmap1.jpg", D3DXIFF_JPG, Shadows->ShadowMaps[MapMiddle].ShadowMapSurface, NULL, NULL);
			D3DXSaveSurfaceToFileA(".\\Test\\shadowmap2.jpg", D3DXIFF_JPG, Shadows->ShadowMaps[MapFar].ShadowMapSurface, NULL, NULL);
			D3DXSaveSurfaceToFileA(".\\Test\\shadowmap3.jpg", D3DXIFF_JPG, Shadows->ShadowMaps[MapLod].ShadowMapSurface, NULL, NULL);
			D3DXSaveSurfaceToFileA(".\\Test\\shadowmap4.jpg", D3DXIFF_JPG, Shadows->ShadowMaps[MapOrtho].ShadowMapSurface, NULL, NULL);

			InterfaceManager->ShowMessage("Textures taken!");
		}
	}

	timer.LogTime("ShadowManager::RenderShadowMaps");
}


/*
* Filters the Shadow Map of given index using a 2 pass gaussian blur
*/
void ShadowManager::BlurShadowMap(ShadowsExteriorEffect::ShadowMapSettings* ShadowMap) {
    IDirect3DDevice9* Device = TheRenderManager->device;
    NiDX9RenderState* RenderState = TheRenderManager->renderState;
    IDirect3DTexture9* SourceShadowMap = ShadowMap->ShadowMapTexture;
    IDirect3DSurface9* TargetShadowMap = ShadowMap->ShadowMapSurface;

    Device->SetDepthStencilSurface(NULL);
    RenderState->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE, RenderStateArgs);
    RenderState->SetRenderState(D3DRS_ZWRITEENABLE, D3DZB_FALSE, RenderStateArgs);
    RenderState->SetVertexShader(ShadowMapBlurVertex->ShaderHandle, false);
    RenderState->SetPixelShader(ShadowMapBlurPixel->ShaderHandle, false);
	RenderState->SetFVF(FrameFVF, false);
	Device->SetStreamSource(0, ShadowMap->BlurShadowVertexBuffer, 0, sizeof(FrameVS));
	Device->SetRenderTarget(0, TargetShadowMap);
	
	// Pass map resolution to shader as a constant
	D3DXVECTOR4 inverseRes = { ShadowMap->ShadowMapInverseResolution, ShadowMap->ShadowMapInverseResolution, 0.0f, 0.0f };
	ShadowMapBlurPixel->SetShaderConstantF(0, &inverseRes, 1);
	RenderState->SetTexture(0, SourceShadowMap);

	// blur in two passes, vertically and horizontally
	D3DXVECTOR4 Blur[2] = {
		D3DXVECTOR4(1.0f, 0.0f, 0.0f, 0.0f),
		D3DXVECTOR4(0.0f, 1.0f, 0.0f, 0.0f),
	};

	for (int i = 0; i < 2; i++) {
		// set blur direction shader constants
		ShadowMapBlurPixel->SetShaderConstantF(1, &Blur[i], 1);

		Device->BeginScene();
		Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2); // draw call to execute the shader
		Device->EndScene();
	}

	RenderState->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE, RenderStateArgs);
    RenderState->SetRenderState(D3DRS_ZWRITEENABLE, D3DZB_TRUE, RenderStateArgs);
}

