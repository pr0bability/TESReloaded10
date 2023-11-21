/*
* Initializes the Shadow Manager by grabbing the relevant settings and shaders, and setting up map sizes.
*/
void ShadowManager::Initialize() {
	
	Logger::Log("Starting the shadows manager...");
	TheShadowManager = new ShadowManager();

	// initializes the settings
	SettingsShadowStruct::ExteriorsStruct* ShadowsExteriors = &TheSettingManager->SettingsShadows.Exteriors;
	SettingsShadowStruct::InteriorsStruct* ShadowsInteriors = &TheSettingManager->SettingsShadows.Interiors;
	UINT ShadowMapSize = ShadowsExteriors->ShadowMapResolution;
	UINT ShadowCubeMapSize = ShadowsInteriors->ShadowCubeMapSize;
	GetCascadeDepths();
	ShadowsExteriors->ShadowMapRadius[MapOrtho] = ShadowMapSize;

	// load the shaders
	TheShadowManager->ShadowMapVertex = (ShaderRecordVertex*)ShaderRecord::LoadShader("ShadowMap.vso", NULL);
	TheShadowManager->ShadowMapPixel = (ShaderRecordPixel*)ShaderRecord::LoadShader("ShadowMap.pso", NULL);
	TheShadowManager->ShadowCubeMapVertex = (ShaderRecordVertex*)ShaderRecord::LoadShader("ShadowCubeMap.vso", NULL);
	TheShadowManager->ShadowCubeMapPixel = (ShaderRecordPixel*)ShaderRecord::LoadShader("ShadowCubeMap.pso", NULL);

    TheShadowManager->ShadowMapBlurVertex = (ShaderRecordVertex*) ShaderRecord::LoadShader("ShadowMapBlur.vso", NULL);
    TheShadowManager->ShadowMapBlurPixel = (ShaderRecordPixel*) ShaderRecord::LoadShader("ShadowMapBlur.pso", NULL);

    if (TheShadowManager->ShadowMapVertex == nullptr || TheShadowManager->ShadowMapPixel == nullptr  || TheShadowManager->ShadowMapBlurVertex  == nullptr
        || TheShadowManager->ShadowCubeMapVertex == nullptr || TheShadowManager->ShadowCubeMapPixel == nullptr || TheShadowManager->ShadowMapBlurPixel  == nullptr ){
        Logger::Log("[ERROR]: Could not load one or more of the ShadowMap generation shaders. Reinstall the mod.");
    }

	// initialize the frame vertices for future shadow blurring
	for (int i = 0; i <= MapOrtho; i++) {
		float multiple = i == MapLod ? 2.0f : 1.0f; // double the size of lod map only
		ShadowMapSize = ShadowsExteriors->ShadowMapResolution * multiple;

		if (i != MapOrtho) TheShaderManager->CreateFrameVertex(ShadowMapSize, ShadowMapSize, &TheShadowManager->BlurShadowVertex[i]);
		TheShadowManager->ShadowMapViewPort[i] = { 0, 0, ShadowMapSize, ShadowMapSize, 0.0f, 1.0f };
        TheShadowManager->ShadowMapInverseResolution[i] = 1.0f / (float) ShadowMapSize;
	}
	TheShadowManager->ShadowCubeMapViewPort = { 0, 0, ShadowCubeMapSize, ShadowCubeMapSize, 0.0f, 1.0f };
	memset(TheShadowManager->ShadowCubeMapLights, NULL, sizeof(ShadowCubeMapLights));
}

/*
* Calculates the radius for each shadow map
*/
void ShadowManager::GetCascadeDepths() {
	SettingsShadowStruct::ExteriorsStruct* ShadowsExteriors = &TheSettingManager->SettingsShadows.Exteriors;
	float camFar = ShadowsExteriors->ShadowRadius;
	float logFactor = 0.9f;
	float camNear = 10.0f;
	int cascadeCount = 4;
	float cascadeNum = 0.0f;

	for (int i = 0; i < cascadeCount; i++) {
		// formula for cascade ratios adapted from https://www.asawicki.info/news_1283_cascaded_shadow_mapping
		cascadeNum += 1.0f;
		ShadowsExteriors->ShadowMapRadius[i] = lerp(
			camNear + (cascadeNum / cascadeCount) * (camFar - camNear),
			camNear * powf(camFar / camNear, cascadeNum / cascadeCount),
			logFactor);

		// filtering objects occupying less than 10 pixels in the shadow map
		ShadowsExteriors->Forms[i].MinRadius = 10.0f * ShadowsExteriors->ShadowMapRadius[i] / ShadowsExteriors->ShadowMapResolution;
	}
	ShadowsExteriors->ShadowMapRadius[cascadeCount] = camFar;

	// Store Shadow map sizes in Constants to pass to the Shaders
	TheShaderManager->ShaderConst.ShadowMap.ShadowMapRadius.x = ShadowsExteriors->ShadowMapRadius[MapNear];
	TheShaderManager->ShaderConst.ShadowMap.ShadowMapRadius.y = ShadowsExteriors->ShadowMapRadius[MapMiddle];
	TheShaderManager->ShaderConst.ShadowMap.ShadowMapRadius.z = ShadowsExteriors->ShadowMapRadius[MapFar];
	TheShaderManager->ShaderConst.ShadowMap.ShadowMapRadius.w = ShadowsExteriors->ShadowMapRadius[MapLod];
}

/**
* Generates the Frustrum planes from a matrix
*/
void ShadowManager::SetFrustum(ShadowMapTypeEnum ShadowMapType, D3DMATRIX* Matrix) {

	ShadowMapFrustum[ShadowMapType][PlaneNear].a = Matrix->_13;
	ShadowMapFrustum[ShadowMapType][PlaneNear].b = Matrix->_23;
	ShadowMapFrustum[ShadowMapType][PlaneNear].c = Matrix->_33;
	ShadowMapFrustum[ShadowMapType][PlaneNear].d = Matrix->_43;
	ShadowMapFrustum[ShadowMapType][PlaneFar].a = Matrix->_14 - Matrix->_13;
	ShadowMapFrustum[ShadowMapType][PlaneFar].b = Matrix->_24 - Matrix->_23;
	ShadowMapFrustum[ShadowMapType][PlaneFar].c = Matrix->_34 - Matrix->_33;
	ShadowMapFrustum[ShadowMapType][PlaneFar].d = Matrix->_44 - Matrix->_43;
	ShadowMapFrustum[ShadowMapType][PlaneLeft].a = Matrix->_14 + Matrix->_11;
	ShadowMapFrustum[ShadowMapType][PlaneLeft].b = Matrix->_24 + Matrix->_21;
	ShadowMapFrustum[ShadowMapType][PlaneLeft].c = Matrix->_34 + Matrix->_31;
	ShadowMapFrustum[ShadowMapType][PlaneLeft].d = Matrix->_44 + Matrix->_41;
	ShadowMapFrustum[ShadowMapType][PlaneRight].a = Matrix->_14 - Matrix->_11;
	ShadowMapFrustum[ShadowMapType][PlaneRight].b = Matrix->_24 - Matrix->_21;
	ShadowMapFrustum[ShadowMapType][PlaneRight].c = Matrix->_34 - Matrix->_31;
	ShadowMapFrustum[ShadowMapType][PlaneRight].d = Matrix->_44 - Matrix->_41;
	ShadowMapFrustum[ShadowMapType][PlaneTop].a = Matrix->_14 - Matrix->_12;
	ShadowMapFrustum[ShadowMapType][PlaneTop].b = Matrix->_24 - Matrix->_22;
	ShadowMapFrustum[ShadowMapType][PlaneTop].c = Matrix->_34 - Matrix->_32;
	ShadowMapFrustum[ShadowMapType][PlaneTop].d = Matrix->_44 - Matrix->_42;
	ShadowMapFrustum[ShadowMapType][PlaneBottom].a = Matrix->_14 + Matrix->_12;
	ShadowMapFrustum[ShadowMapType][PlaneBottom].b = Matrix->_24 + Matrix->_22;
	ShadowMapFrustum[ShadowMapType][PlaneBottom].c = Matrix->_34 + Matrix->_32;
	ShadowMapFrustum[ShadowMapType][PlaneBottom].d = Matrix->_44 + Matrix->_42;
	for (int i = 0; i < 6; ++i) {
		D3DXPLANE Plane(ShadowMapFrustum[ShadowMapType][i]);
		D3DXPlaneNormalize(&ShadowMapFrustum[ShadowMapType][i], &Plane);
	}

}

/*
* Checks wether the given node is in the frustrum using its radius for the current type of Shadow map.
*/
bool ShadowManager::InFrustum(ShadowMapTypeEnum ShadowMapType, NiNode* Node) {
	float Distance = 0.0f;
	bool R = false;
	NiBound* Bound = Node->GetWorldBound();

	if (Bound) {
		D3DXVECTOR3 Position = { Bound->Center.x - TheRenderManager->CameraPosition.x, Bound->Center.y - TheRenderManager->CameraPosition.y, Bound->Center.z - TheRenderManager->CameraPosition.z };
		
		R = true;
		for (int i = 0; i < 6; ++i) {
			Distance = D3DXPlaneDotCoord(&ShadowMapFrustum[ShadowMapType][i], &Position);
			if (Distance <= -Bound->Radius) {
				R = false;
				break;
			}
		}

		//if (ShadowMapType > MapNear && ShadowMapType < MapOrtho && R) { // Ensures to not be fully in the near frustum
		//	for (int i = 0; i < 6; ++i) {
		//		Distance = D3DXPlaneDotCoord(&ShadowMapFrustum[ShadowMapType - 1][i], &Position);
		//		if (Distance <= -Bound->Radius || std::fabs(Distance) < Bound->Radius) {
		//			R = false;
		//			break;
		//		}
		//	}
		//	R = !R;
		//}
	}
	return R;

}

/*
* Returns the given object ref if it passes the test for excluded form types, otherwise returns NULL.
*/
TESObjectREFR* ShadowManager::GetRef(TESObjectREFR* Ref, SettingsShadowStruct::FormsStruct* Forms, SettingsShadowStruct::ExcludedFormsList* ExcludedForms) {
	
	TESObjectREFR* R = NULL;

	if (Ref && Ref->GetNode()) {
		TESForm* Form = Ref->baseForm;
		ExtraRefractionProperty* RefractionExtraProperty = (ExtraRefractionProperty*)Ref->extraDataList.GetExtraData(BSExtraData::ExtraDataType::kExtraData_RefractionProperty);
		float Refraction = RefractionExtraProperty ? (1 - RefractionExtraProperty->refractionAmount) : 0.0f;

		if (!(Ref->flags & TESForm::FormFlags::kFormFlags_NotCastShadows) && Refraction <= 0.5) {
			UInt8 TypeID = Form->formType;
			if ((TypeID == TESForm::FormType::kFormType_Activator && Forms->Activators) ||
				(TypeID == TESForm::FormType::kFormType_Apparatus && Forms->Apparatus) ||
				(TypeID == TESForm::FormType::kFormType_Book && Forms->Books) ||
				(TypeID == TESForm::FormType::kFormType_Container && Forms->Containers) ||
				(TypeID == TESForm::FormType::kFormType_Door && Forms->Doors) ||
				(TypeID == TESForm::FormType::kFormType_Misc && Forms->Misc) ||
				(TypeID >= TESForm::FormType::kFormType_Stat && TypeID <= TESForm::FormType::kFormType_MoveableStatic && Forms->Statics) ||
				(TypeID == TESForm::FormType::kFormType_Tree && Forms->Trees) ||
				(TypeID == TESForm::FormType::kFormType_Furniture && Forms->Furniture) ||
                (TypeID == TESForm::FormType::kFormType_Land && Forms->Terrain) ||
				(TypeID >= TESForm::FormType::kFormType_NPC && TypeID <= TESForm::FormType::kFormType_LeveledCreature && Forms->Actors))
				R = Ref;
			if (R && ExcludedForms->size() > 0 && std::binary_search(ExcludedForms->begin(), ExcludedForms->end(), Form->refID)) R = NULL;
		}
	}
	return R;

}

// Detect which pass the object must be added to
void ShadowManager::AccumObject(std::stack<NiAVObject*>* containersAccum, NiAVObject* NiObject) {
	NiGeometry* geo;
	if (NiObject->IsGeometry()) {
		geo = static_cast<NiGeometry*>(NiObject);
		if (!geo->shader) return; // skip Geometry without a shader

		NiShadeProperty* pShaderProp = static_cast<NiShadeProperty*>(geo->GetProperty(NiProperty::kType_Shade));

#if defined(OBLIVION)
		if (geo->m_pcName && !memcmp(geo->m_pcName, "Torch", 5)) return; // No torch geo, it is too near the light and a bad square is rendered.
#endif
		// check data for rigged geometry
		if (geo->skinInstance && geo->skinInstance->SkinPartition && geo->skinInstance->SkinPartition->Partitions) {
			if (!geo->skinInstance->SkinPartition->Partitions[0].BuffData) return; // discard objects without buffer data
			skinnedGeoAccum.push(geo);
		}
		else if (pShaderProp->type == NiShadeProperty::kProp_SpeedTreeLeaf) {
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
	NiGeometry* geo;
	NiAVObject* object;
	NiNode* Node;

	AccumObject(&containers, NiObject); //list all objects contained, or sort the object if not a container

	// Gather geometry
	while (!containers.empty()) {
    		object = containers.top();
    		containers.pop();

    		Node = object ? object->IsNiNode() : nullptr;
    		if (!Node || Node->m_flags & NiAVObject::NiFlags::APP_CULLED || Node->GetWorldBoundRadius() < MinRadius) continue; // culling containers

		for (int i = 0; i < Node->m_children.numObjs; i++) {
			try {
				child = Node->m_children.data[i];
				if (!child || child->m_flags & NiAVObject::NiFlags::APP_CULLED || child->GetWorldBoundRadius() < MinRadius) continue; // culling children
				if (child->IsFadeNode() && static_cast<BSFadeNode*>(child)->FadeAlpha < 0.75f) continue; // stop rendering fadenodes below a certain opacity
				AccumObject(&containers, child);
			}
			catch (const std::exception& e) {
				Logger::Log("%s", e.what());
				InterfaceManager->ShowMessage(e.what());
			}
		}
	}
}

// Go through accumulations and render found objects
void ShadowManager::RenderAccums() {
	NiDX9RenderState* RenderState = TheRenderManager->renderState;
	IDirect3DDevice9* Device = TheRenderManager->device;

	// Render normal geometry
	if (!geometryAccum.empty()) {
		TheShaderManager->ShaderConst.Shadow.Data.x = 0.0f; // Type of geo (0 normal, 1 actors (skinned), 2 speedtree leaves)
		TheShaderManager->ShaderConst.Shadow.Data.y = 0.0f; // Alpha control
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
	TheShaderManager->ShaderConst.Shadow.Data.x = 1.0f; // Type of geo (0 normal, 1 actors (skinned), 2 speedtree leaves)
	TheShaderManager->ShaderConst.Shadow.Data.y = 0.0f; // Alpha control
	CurrentPixel->SetCT();
	CurrentVertex->SetCT();
	while (!skinnedGeoAccum.empty()) {
		RenderSkinnedGeometry(skinnedGeoAccum.top());
		skinnedGeoAccum.pop();
	}

	// Render speedtree
	if (!speedTreeAccum.empty()) {
		TheShaderManager->ShaderConst.Shadow.Data.x = 2.0f; // Type of geo (0 normal, 1 actors (skinned), 2 speedtree leaves)
		TheShaderManager->ShaderConst.Shadow.Data.y = 0.0f; // Alpha control
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

	TheRenderManager->CreateD3DMatrix(&TheShaderManager->ShaderConst.ShadowMap.ShadowWorld, &Geo->m_worldTransform);
	BSShaderProperty* ShaderProperty = (BSShaderProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Shade);
	if (!ShaderProperty || !ShaderProperty->IsLightingProperty()) return;

	TheShaderManager->ShaderConst.Shadow.Data.y = 0.0f; // Alpha Control
	if (AlphaEnabled) {
		NiAlphaProperty* AProp = (NiAlphaProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Alpha);
		if (AProp->flags & NiAlphaProperty::AlphaFlags::ALPHA_BLEND_MASK || AProp->flags & NiAlphaProperty::AlphaFlags::TEST_ENABLE_MASK) {
			if (NiTexture* Texture = *((BSShaderPPLightingProperty*)ShaderProperty)->ppTextures[0]) {
				
				TheShaderManager->ShaderConst.Shadow.Data.y = 1.0f; // Alpha Control

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

	TheRenderManager->CreateD3DMatrix(&TheShaderManager->ShaderConst.ShadowMap.ShadowWorld, &Geo->m_worldTransform);
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
	if (SkinInstance->SkinToWorldWorldToSkin) memcpy(&TheShaderManager->ShaderConst.ShadowMap.ShadowWorld, SkinInstance->SkinToWorldWorldToSkin, 0x40);
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


// calculates the minimum area viewproj matrix for a given cascade using cascade depth and frustum corners
D3DXMATRIX ShadowManager::GetCascadeViewProj(ShadowMapTypeEnum ShadowMapType, SettingsShadowStruct::ExteriorsStruct* ShadowsExteriors, D3DXMATRIX View) {
	D3DXMATRIX Proj;
	float FarPlane = ShadowsExteriors->ShadowMapFarPlane;
	float Radius = ShadowsExteriors->ShadowMapRadius[ShadowMapType];
	NiCamera* Camera = WorldSceneGraph->camera;

	// calculating the size of the shadow cascade
	float znear;
	float zfar;
	switch (ShadowMapType) {
	case ShadowMapTypeEnum::MapNear:
		znear = 10;
		zfar = ShadowsExteriors->ShadowMapRadius[ShadowMapTypeEnum::MapNear];
		break;
	case ShadowMapTypeEnum::MapMiddle:
		znear = ShadowsExteriors->ShadowMapRadius[ShadowMapTypeEnum::MapNear] * 0.9;
		zfar = ShadowsExteriors->ShadowMapRadius[ShadowMapTypeEnum::MapMiddle];
		break;
	case ShadowMapTypeEnum::MapFar:
		znear = ShadowsExteriors->ShadowMapRadius[ShadowMapTypeEnum::MapMiddle] * 0.9;
		zfar = ShadowsExteriors->ShadowMapRadius[ShadowMapTypeEnum::MapFar];
		break;
	case ShadowMapTypeEnum::MapLod:
		znear = ShadowsExteriors->ShadowMapRadius[ShadowMapTypeEnum::MapFar]* 0.9;
		zfar = ShadowsExteriors->ShadowMapRadius[ShadowMapTypeEnum::MapLod];
		break;
	case ShadowMapTypeEnum::MapOrtho:
		// shift the covered area in the direction of the camera vector
		D3DXVECTOR4 Center = D3DXVECTOR4(TheRenderManager->CameraForward.x, TheRenderManager->CameraForward.y, 0.0, 1.0);
		D3DXVec4Normalize(&Center, &Center);
		Radius *= 2;
		Center.x *= Radius;
		Center.y *= Radius;
		D3DXVec4Transform(&Center, &Center, &View);
		D3DXMatrixOrthoOffCenterRH(&Proj, Center.x - Radius, Center.x + Radius, Center.y -Radius, Center.y + Radius, FarPlane * 0.8f, 1.2f * FarPlane);
		return View * Proj;
	}

	float w = Camera->Frustum.Right - Camera->Frustum.Left;
	float h = Camera->Frustum.Top - Camera->Frustum.Bottom;

	float ar = h / w;

	//Logger::Log("fov %f   %f   %f", WorldSceneGraph->cameraFOV, Player->GetFoV(false), Player->GetFoV(true));
	float fov = TheRenderManager->FOVData.z;
	float fovY = TheRenderManager->FOVData.w;

	float tanHalfHFOV = tanf(fov * 0.5f);
	float tanHalfVFOV = tanf(fovY * 0.5f);

	float xn = znear * tanHalfHFOV;
	float xf = zfar * tanHalfHFOV;
	float yn = znear * tanHalfVFOV;
	float yf = zfar * tanHalfVFOV;

	D3DXVECTOR4 frustrumPoints[8];

	// near face
	frustrumPoints[0] = D3DXVECTOR4(xn, yn, znear, 1.0);
	frustrumPoints[1] = D3DXVECTOR4(-xn, yn, znear, 1.0);
	frustrumPoints[2] = D3DXVECTOR4(xn, -yn, znear, 1.0);
	frustrumPoints[3] = D3DXVECTOR4(-xn, -yn, znear, 1.0);

	// far face
	frustrumPoints[4] = D3DXVECTOR4(xf, yf, zfar, 1.0);
	frustrumPoints[5] = D3DXVECTOR4(-xf, yf, zfar, 1.0);
	frustrumPoints[6] = D3DXVECTOR4(xf, -yf, zfar, 1.0);
	frustrumPoints[7] = D3DXVECTOR4(-xf, -yf, zfar, 1.0);

	// values of the final light frustrum
	float left = 0.0f;
	float right = 0.0f;
	float bottom = 0.0f;
	float top = 0.0f;

	// transform from camera to world then to light space
	D3DXMATRIX m = TheRenderManager->InvViewMatrix * View;
	D3DXVECTOR4 p;
	for (int i = 0; i < 8; i++) {
		D3DXVec4Transform(&p, &frustrumPoints[i], &m);

		// extend frustrum to include all corners
		if (p.x < left || left == 0.0f) left = p.x;
		if (p.x > right || right == 0.0f) right = p.x;
		if (p.y > top || top == 0.0f) top = p.y;
		if (p.y < bottom || bottom == 0.0f) bottom = p.y;
	}


	D3DXMatrixOrthoOffCenterRH(&Proj, left, right, bottom, top, FarPlane * 0.6f, 1.4f * FarPlane);
	return View * Proj;
}

void ShadowManager::RenderShadowMap(ShadowMapTypeEnum ShadowMapType, SettingsShadowStruct::ExteriorsStruct* ShadowsExteriors, D3DXVECTOR3* At, D3DXVECTOR4* SunDir) {
	auto timer = TimeLogger();

	if (!TheTextureManager->ShadowMapDepthSurface[ShadowMapType]) {
		Logger::Log("ShadowDepthSurface missing for cascade %i", ShadowMapType);
		return;
	}
	if (!TheTextureManager->ShadowMapSurface[ShadowMapType]) {
		Logger::Log("ShadowSurface missing for cascade %i", ShadowMapType);
		return;
	}

	ShaderConstants::ShadowMapStruct* ShadowMap = &TheShaderManager->ShaderConst.ShadowMap;
	IDirect3DDevice9* Device = TheRenderManager->device;
	NiDX9RenderState* RenderState = TheRenderManager->renderState;
	GridCellArray* CellArray = Tes->gridCellArray;
	UInt32 CellArraySize = CellArray->size * CellArray->size;
	float FarPlane = ShadowsExteriors->ShadowMapFarPlane;
	D3DXVECTOR3 Up = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	D3DXMATRIX View;
	D3DXVECTOR3 Eye;
	AlphaEnabled = ShadowsExteriors->AlphaEnabled[ShadowMapType];

	Eye.x = At->x - FarPlane * SunDir->x * -1;
	Eye.y = At->y - FarPlane * SunDir->y * -1;
	Eye.z = At->z - FarPlane * SunDir->z * -1;

	// calculating the projection matrix for point of view of the light
	D3DXMatrixLookAtRH(&View, &Eye, At, &Up);
	ShadowMap->ShadowViewProj = GetCascadeViewProj(ShadowMapType, ShadowsExteriors, View); // calculating the size of the shadow cascade
	ShadowMap->ShadowCameraToLight[ShadowMapType] = TheRenderManager->InvViewProjMatrix * ShadowMap->ShadowViewProj;

	BillboardRight = { View._11, View._21, View._31, 0.0f };
	BillboardUp = { View._12, View._22, View._32, 0.0f };
	SetFrustum(ShadowMapType, &ShadowMap->ShadowViewProj);
    /*for (int i = 1; i < 4; i++){
        IDirect3DSurface9* targ= nullptr;
        Device->GetRenderTarget(i , &targ);
        Logger::Log("%u  : %08X", i, targ );
    }*/
    Device->SetRenderTarget(0, TheTextureManager->ShadowMapSurface[ShadowMapType]);

	Device->SetDepthStencilSurface(TheTextureManager->ShadowMapDepthSurface[ShadowMapType]);
	Device->SetViewport(&ShadowMapViewPort[ShadowMapType]);

	Device->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(1.0f, 0.25f, 0.25f, 0.55f), 1.0f, 0L);

	if (SunDir->z > 0.0f) {
		RenderState->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE, RenderStateArgs);
		RenderState->SetRenderState(D3DRS_ZWRITEENABLE, D3DZB_TRUE, RenderStateArgs);
		RenderState->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE, RenderStateArgs);
		RenderState->SetRenderState(D3DRS_ALPHABLENDENABLE, 0, RenderStateArgs);
		RenderState->SetVertexShader(ShadowMapVertex->ShaderHandle, false);
		RenderState->SetPixelShader(ShadowMapPixel->ShaderHandle, false);
		Device->BeginScene();

		if (Player->GetWorldSpace()) {
			for (UInt32 i = 0; i < CellArraySize; i++) {
				if (TESObjectCELL* Cell = CellArray->GetCell(i)) {
					RenderExteriorCell(Cell, ShadowsExteriors, ShadowMapType);
				}
			}
		}
		else {
			RenderExteriorCell(Player->parentCell, ShadowsExteriors, ShadowMapType);
		}

		Device->EndScene();
	}

	timer.LogTime("ShadowManager::RenderShadowMap ");
}


void ShadowManager::RenderExteriorCell(TESObjectCELL* Cell, SettingsShadowStruct::ExteriorsStruct* ShadowsExteriors, ShadowMapTypeEnum ShadowMapType) {
	if (!Cell || Cell->IsInterior())
		return;
	
	if (ShadowsExteriors->Forms[ShadowMapType].Terrain) {
		NiNode* LandNode = Cell->GetChildNode(TESObjectCELL::kCellNode_Land);
		// if (ShadowsExteriors->Forms[ShadowMapType].Lod) RenderLod(Tes->landLOD, ShadowMapType); //Render terrain LOD
		if (LandNode) AccumChildren((NiAVObject*)LandNode, 0);
	}

	TList<TESObjectREFR>::Entry* Entry = &Cell->objectList.First;
	while (Entry) {
		if (TESObjectREFR* Ref = GetRef(Entry->item, &ShadowsExteriors->Forms[ShadowMapType], &ShadowsExteriors->ExcludedForms)) {
			NiNode* RefNode = Ref->GetNode();
			if (InFrustum(ShadowMapType, RefNode)) AccumChildren(RefNode, ShadowsExteriors->Forms[ShadowMapType].MinRadius);
		}
		Entry = Entry->next;
	}

	RenderAccums();
}


void ShadowManager::RenderShadowCubeMap(NiPointLight** Lights, int LightIndex, SettingsShadowStruct::InteriorsStruct* ShadowsInteriors) {
	
	if (Lights[LightIndex] == NULL) return; // No light at current index
	if (!TheTextureManager->ShadowCubeMapDepthSurface) {
		Logger::Log("ShadowCubemapDepth missing for light %i", LightIndex);
		return;
	}

	auto timer = TimeLogger();

	ShaderConstants::ShadowMapStruct* ShadowMap = &TheShaderManager->ShaderConst.ShadowMap;
	IDirect3DDevice9* Device = TheRenderManager->device;
	NiDX9RenderState* RenderState = TheRenderManager->renderState;
	float Radius = 0.0f;
	float MinRadius = ShadowsInteriors->Forms.MinRadius;
	NiPoint3* LightPos = NULL;
	D3DXMATRIX View, Proj;
	D3DXVECTOR3 Eye, At, Up, CameraDirection;;

	Device->SetDepthStencilSurface(TheTextureManager->ShadowCubeMapDepthSurface);

	LightPos = &Lights[LightIndex]->m_worldTransform.pos;
	Radius = Lights[LightIndex]->Spec.r * ShadowsInteriors->LightRadiusMult;
	if (Lights[LightIndex]->CanCarry) Radius = 256.0f;
	Eye.x = LightPos->x - TheRenderManager->CameraPosition.x;
	Eye.y = LightPos->y - TheRenderManager->CameraPosition.y;
	Eye.z = LightPos->z - TheRenderManager->CameraPosition.z;
	ShadowMap->ShadowCubeMapLightPosition.x = Eye.x;
	ShadowMap->ShadowCubeMapLightPosition.y = Eye.y;
	ShadowMap->ShadowCubeMapLightPosition.z = Eye.z;
	ShadowMap->ShadowCubeMapLightPosition.w = Radius;
	TheShaderManager->ShaderConst.Shadow.Data.z = Radius;
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
		
		TList<TESObjectREFR>::Entry* Entry = &Player->parentCell->objectList.First;
		TESObjectREFR* Ref = NULL;
		while (Entry) {
			Ref = GetRef(Entry->item, &ShadowsInteriors->Forms, &ShadowsInteriors->ExcludedForms);
			if (Ref) {
				// Detect if the object is in front of the light in the direction of the current face
				// TODO: improve to base on frustum
				NiNode* RefNode = Ref->GetNode();
				D3DXVECTOR3 ObjectPos = RefNode->m_worldTransform.pos.toD3DXVEC3();
				D3DXVECTOR3 ObjectToLight = ObjectPos - Eye;

				D3DXVec3Normalize(&ObjectToLight, &ObjectToLight);
				float inFront = D3DXVec3Dot(&ObjectToLight, &CameraDirection);

				if (inFront && RefNode->GetDistance(LightPos) <= Radius * 1.2f) AccumChildren(RefNode, MinRadius);
			}
			Entry = Entry->next;
		}
		
		D3DXMatrixLookAtRH(&View, &Eye, &At, &Up);
		ShadowMap->ShadowViewProj = View * Proj;

		if (!TheTextureManager->ShadowCubeMapSurface[LightIndex][Face]) {
			Logger::Log("ShadowCubemapSurface missing for light %i, face %i", LightIndex, Face);
			continue;
		}

		Device->SetRenderTarget(0, TheTextureManager->ShadowCubeMapSurface[LightIndex][Face]);
		Device->SetViewport(&ShadowCubeMapViewPort);
		Device->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(1.0f, 0.25f, 0.25f, 0.55f), 1.0f, 0L);
		Device->BeginScene();

		RenderAccums();

		Device->EndScene();
	}

	timer.LogTime("RenderShadowCubeMap");	
}


void ShadowManager::GetNearbyLights(NiPointLight* ShadowLightsList[], NiPointLight* LightsList[]) {
	D3DXVECTOR4 PlayerPosition = Player->pos.toD3DXVEC4();
	//Logger::Log(" ==== Getting lights ====");
	auto timer = TimeLogger();

	// create a map of all nearby valid lights and sort them per distance to player
	std::map<int, NiPointLight*> SceneLights;
	NiTList<ShadowSceneLight>::Entry* Entry = SceneNode->lights.start;

	while (Entry) {
		NiPointLight* Light = Entry->data->sourceLight;
		D3DXVECTOR4 LightPosition = Light->m_worldTransform.pos.toD3DXVEC4();

		bool lightCulled = Light->m_flags & NiAVObject::NiFlags::APP_CULLED;
		bool lightOn = (Light->Diff.r + Light->Diff.g + Light->Diff.b) * Light->Dimmer > 5.0/255.0; // Check for low values in case of human error
		if (lightCulled || !lightOn) {
			Entry = Entry->next;
			continue;
		}

		D3DXVECTOR4 LightVector = LightPosition - PlayerPosition;
		D3DXVec4Normalize(&LightVector, &LightVector);
		float inFront = D3DXVec4Dot(&LightVector, &TheRenderManager->CameraForward);
		float Distance = Light->GetDistance(&Player->pos);
		float radius = Light->Spec.r * TheSettingManager->SettingsShadows.Interiors.LightRadiusMult;

		// select lights that will be tracked by removing culled lights and lights behind the player further away than their radius
		// TODO: handle using frustum check
		float drawDistance = TheShaderManager->isExterior ? TheSettingManager->SettingsShadows.Exteriors.ShadowMapRadius[ShadowMapTypeEnum::MapLod] : TheSettingManager->SettingsShadows.Interiors.DrawDistance;
		if ((inFront > 0 || Distance < radius) && (Distance + radius) < drawDistance) {
			SceneLights[(int)(Distance * 10000)] = Light; // mutliplying distance (used as key) before convertion to avoid duplicates in case of similar values
		}

		Entry = Entry->next;
	}

	// save only the n first lights (based on #define TrackedLightsMax)
	memset(&TheShaderManager->LightPosition, 0, TrackedLightsMax * sizeof(D3DXVECTOR4)); // clear previous lights from array

	// get the data for all tracked lights
	int ShadowIndex = 0;
	int LightIndex = 0;
	PointLightsNum = 0;

#if defined(OBLIVION)
	bool TorchOnBeltEnabled = TheSettingManager->SettingsMain.EquipmentMode.Enabled && TheSettingManager->SettingsMain.EquipmentMode.TorchKey != 255;
#endif

	D3DXVECTOR4 Empty = D3DXVECTOR4(0, 0, 0, 0);

	std::map<int, NiPointLight*>::iterator v = SceneLights.begin();
	for (int i = 0; i < TrackedLightsMax + ShadowCubeMapsMax; i++) {
		if (v != SceneLights.end())	{
			NiPointLight* Light = v->second;
			if (!Light || Light->EffectType != NiDynamicEffect::EffectTypes::POINT_LIGHT) {
				v++;
				continue;
			}

			// determin if light is a shadow caster
			bool CastShadow = TheSettingManager->SettingsShadows.Interiors.UseCastShadowFlag ? Light->CastShadows : true;
			
#if defined(OBLIVION)
			// Oblivion exception for carried torch lights 
			if (TorchOnBeltEnabled && Light->CanCarry == 2) {
				HighProcessEx* Process = (HighProcessEx*)Player->process;
				if (Process->OnBeltState == HighProcessEx::State::In) CastShadow = false;
			}
#endif

			D3DXVECTOR4 LightPosition = Light->m_worldTransform.pos.toD3DXVEC4();
			LightPosition.w = Light->Spec.r * TheSettingManager->SettingsShadows.Interiors.LightRadiusMult;

			if (CastShadow && ShadowIndex < ShadowCubeMapsMax) {
				// add found light to list of lights that cast shadows
				ShadowLightsList[ShadowIndex] = Light;
				TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[ShadowIndex] = LightPosition;
				ShadowIndex++;
				PointLightsNum++; // Constant to track number of shadow casting lights are present
			}
			else if (LightIndex < TrackedLightsMax){
				LightsList[LightIndex] = Light;
				TheShaderManager->LightPosition[LightIndex] = LightPosition;
				LightIndex++;
			};

			v++;
		}
		else {
			// set null values if number of lights in the scene becomes lower than previous iteration
			if (LightIndex < TrackedLightsMax) {
				//Logger::Log("clearing light at index %i", LightIndex);
				LightsList[LightIndex] = NULL;
				TheShaderManager->LightPosition[LightIndex] = Empty;
				LightIndex++;
			}
			if (ShadowIndex < ShadowCubeMapsMax) {
				//Logger::Log("clearing shadow casting light at index %i", ShadowIndex);
				ShadowLightsList[ShadowIndex] = NULL;
				TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[ShadowIndex] = Empty;
				ShadowIndex++;
			}
		}
	}

	timer.LogTime("ShadowManager::GetNearbyLights");

}

//static 	NiDX9RenderState::NiRenderStateSetting* RenderStateSettings = nullptr;

/*
* Renders the different shadow maps: Near, Far, Ortho.
*/
void ShadowManager::RenderShadowMaps() {
	if (!TheSettingManager->GetSettingI("Main.Main.Misc", "RenderEffects")) return; // cancel out if rendering effects is disabled

	SettingsShadowStruct::ExteriorsStruct* ShadowsExteriors = &TheSettingManager->SettingsShadows.Exteriors;
	SettingsShadowStruct::InteriorsStruct* ShadowsInteriors = &TheSettingManager->SettingsShadows.Interiors;

	bool isExterior = TheShaderManager->isExterior;// || currentCell->flags0 & TESObjectCELL::kFlags0_BehaveLikeExterior; // exterior flag currently broken

	bool ExteriorEnabled = isExterior && TheShaderManager->Effects.ShadowsExteriors->Enabled && ShadowsExteriors->Enabled;
	bool InteriorEnabled = !isExterior && TheShaderManager->Effects.ShadowsInteriors->Enabled;

	// early out in case shadow rendering is not required
	if (!ExteriorEnabled && !InteriorEnabled && !TheShaderManager->orthoRequired) return;

	auto timer = TimeLogger();

	// prepare some pointers to the device and surfaces
	SettingsShadowStruct::FormsStruct* ShadowsInteriorsForms = &ShadowsInteriors->Forms;
	SettingsShadowStruct::ExcludedFormsList* ShadowsInteriorsExcludedForms = &ShadowsInteriors->ExcludedForms;
	IDirect3DDevice9* Device = TheRenderManager->device;
	NiDX9RenderState* RenderState = TheRenderManager->renderState;
	IDirect3DSurface9* DepthSurface = NULL;
	IDirect3DSurface9* RenderSurface = NULL;
	D3DVIEWPORT9 viewport;

	D3DXVECTOR4* ShadowData = &TheShaderManager->ShaderConst.Shadow.Data;
	D3DXVECTOR4* OrthoData = &TheShaderManager->ShaderConst.Shadow.OrthoData;
	Device->GetDepthStencilSurface(&DepthSurface);
	Device->GetRenderTarget(0, &RenderSurface);
	Device->GetViewport(&viewport);	

	RenderState->SetRenderState(D3DRS_STENCILENABLE , 0 ,RenderStateArgs);
	RenderState->SetRenderState(D3DRS_STENCILREF , 0 ,RenderStateArgs);
 	RenderState->SetRenderState(D3DRS_STENCILFUNC , 8 ,RenderStateArgs);

	TheRenderManager->UpdateSceneCameraData();
	TheRenderManager->SetupSceneCamera();
	
	D3DXVECTOR4 PlayerPosition = Player->pos.toD3DXVEC4();
	TESObjectCELL* currentCell = Player->parentCell;

	// Render directional shadows for Sun/Moon

	ShadowData->w = ShadowsExteriors->ShadowMode;	// Mode (0:off, 1:VSM, 2:ESM, 3: ESSM);
	NiNode* PlayerNode = Player->GetNode();
	D3DXVECTOR3 At;

	At.x = PlayerNode->m_worldTransform.pos.x - TheRenderManager->CameraPosition.x;
	At.y = PlayerNode->m_worldTransform.pos.y - TheRenderManager->CameraPosition.y;
	At.z = PlayerNode->m_worldTransform.pos.z - TheRenderManager->CameraPosition.z;

	CurrentVertex = ShadowMapVertex;
	CurrentPixel = ShadowMapPixel;

	// track point lights for interiors and exteriors
	NiPointLight* ShadowLights[ShadowCubeMapsMax] = { NULL };
	NiPointLight* Lights[TrackedLightsMax] = { NULL };
	GetNearbyLights(ShadowLights, Lights);

	// Render all shadow maps
	if (isExterior && ExteriorEnabled) {
		ShadowData->z = 0; // set shader constant to identify other shadow maps
		D3DXVECTOR4* SunDir = &TheShaderManager->ShaderConst.SunDir;
		for (int i = MapNear; i < MapOrtho; i++) {
			ShadowMapTypeEnum ShadowMapType = static_cast<ShadowMapTypeEnum>(i);
			RenderShadowMap(ShadowMapType, ShadowsExteriors, &At, SunDir);
			if(ShadowsExteriors->BlurShadowMaps) BlurShadowMap(ShadowMapType);
		}
	}

	// render ortho map if one of the effects using ortho is active
	if (isExterior && TheShaderManager->orthoRequired) {
		ShadowData->z = 1; // identify ortho map in shader constant
		D3DXVECTOR4 OrthoDir = D3DXVECTOR4(0.05f, 0.05f, 1.0f, 1.0f);
		RenderShadowMap(MapOrtho, ShadowsExteriors, &At, &OrthoDir);
		OrthoData->x = ShadowsExteriors->ShadowMapRadius[MapOrtho] * 2;
	}

	// Render shadow maps for point lights
	bool usePointLights = (TheShaderManager->isDayTime > 0.5) ? TheSettingManager->SettingsShadows.Exteriors.UsePointShadowsDay : TheSettingManager->SettingsShadows.Exteriors.UsePointShadowsNight;

	if ((isExterior && usePointLights) || (!isExterior && InteriorEnabled)) {
		CurrentVertex = ShadowCubeMapVertex;
		CurrentPixel = ShadowCubeMapPixel;
		AlphaEnabled = ShadowsInteriors->AlphaEnabled;

		// render the cubemaps for each light
		for (int i = 0; i < ShadowsInteriors->LightPoints; i++) {
			RenderShadowCubeMap(ShadowLights, i, ShadowsInteriors);
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
			D3DXSaveSurfaceToFileA(".\\Test\\shadowmap0.jpg", D3DXIFF_JPG, TheTextureManager->ShadowMapSurface[MapNear], NULL, NULL);
			D3DXSaveSurfaceToFileA(".\\Test\\shadowmap1.jpg", D3DXIFF_JPG, TheTextureManager->ShadowMapSurface[MapMiddle], NULL, NULL);
			D3DXSaveSurfaceToFileA(".\\Test\\shadowmap2.jpg", D3DXIFF_JPG, TheTextureManager->ShadowMapSurface[MapFar], NULL, NULL);
			D3DXSaveSurfaceToFileA(".\\Test\\shadowmap3.jpg", D3DXIFF_JPG, TheTextureManager->ShadowMapSurface[MapLod], NULL, NULL);
			D3DXSaveSurfaceToFileA(".\\Test\\shadowmap4.jpg", D3DXIFF_JPG, TheTextureManager->ShadowMapSurface[MapOrtho], NULL, NULL);

			InterfaceManager->ShowMessage("Textures taken!");
		}
	}

	timer.LogTime("ShadowManager::RenderShadowMaps");
}


float ShadowManager::lerp(float a, float b, float t) {
	return (1 - t) * a + t * b;
}


/*
* Filters the Shadow Map of given index using a 2 pass gaussian blur
*/
void ShadowManager::BlurShadowMap(ShadowMapTypeEnum ShadowMapType) {
    IDirect3DDevice9* Device = TheRenderManager->device;
    NiDX9RenderState* RenderState = TheRenderManager->renderState;
    IDirect3DTexture9* SourceShadowMap = TheTextureManager->ShadowMapTexture[ShadowMapType];
    IDirect3DSurface9* TargetShadowMap = TheTextureManager->ShadowMapSurface[ShadowMapType];

    Device->SetDepthStencilSurface(NULL);
    RenderState->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE, RenderStateArgs);
    RenderState->SetRenderState(D3DRS_ZWRITEENABLE, D3DZB_FALSE, RenderStateArgs);
    RenderState->SetVertexShader(ShadowMapBlurVertex->ShaderHandle, false);
    RenderState->SetPixelShader(ShadowMapBlurPixel->ShaderHandle, false);
	RenderState->SetFVF(FrameFVF, false);
	Device->SetStreamSource(0, BlurShadowVertex[ShadowMapType], 0, sizeof(FrameVS));
	Device->SetRenderTarget(0, TargetShadowMap);
	
	// Pass map resolution to shader as a constant
	D3DXVECTOR4 inverseRes = { ShadowMapInverseResolution[ShadowMapType], ShadowMapInverseResolution[ShadowMapType], 0.0f, 0.0f };
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

