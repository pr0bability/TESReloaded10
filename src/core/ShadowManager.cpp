#define ShadowMapFarPlane 32768;

/*
* Initializes the Shadow Manager by grabbing the relevant settings and shaders, and setting up map sizes.
*/
void ShadowManager::Initialize() {
	
	Logger::Log("Starting the shadows manager...");
	TheShadowManager = new ShadowManager();

	// setup the shadow render passes for the shadowmaps
	TheShadowManager->geometryPass = new ShadowRenderPass();
	TheShadowManager->alphaPass = new AlphaShadowRenderPass();
	TheShadowManager->skinnedGeoPass = new SkinnedGeoShadowRenderPass();
	TheShadowManager->speedTreePass = new SpeedTreeShadowRenderPass();
	TheShadowManager->terrainLODPass = new TerrainLODPass();

	// load the shaders
	TheShadowManager->ShadowMapVertex = (ShaderRecordVertex*)ShaderRecord::LoadShader("ShadowMap.vso", "Shadows\\");
	TheShadowManager->ShadowMapPixel = (ShaderRecordPixel*)ShaderRecord::LoadShader("ShadowMap.pso", "Shadows\\");
	TheShadowManager->ShadowCubeMapVertex = (ShaderRecordVertex*)ShaderRecord::LoadShader("ShadowCubeMap.vso", "Shadows\\");
	TheShadowManager->ShadowCubeMapPixel = (ShaderRecordPixel*)ShaderRecord::LoadShader("ShadowCubeMap.pso", "Shadows\\");

    TheShadowManager->ShadowMapBlurVertex = (ShaderRecordVertex*) ShaderRecord::LoadShader("ShadowMapBlur.vso", "Shadows\\");
    TheShadowManager->ShadowMapBlurPixel = (ShaderRecordPixel*) ShaderRecord::LoadShader("ShadowMapBlur.pso", "Shadows\\");

	// Make sure samplers are not reset on SetCT as that causes errors.
	TheShadowManager->ShadowMapVertex->ClearSamplers = false;
	TheShadowManager->ShadowMapPixel->ClearSamplers = false;
	TheShadowManager->ShadowCubeMapVertex->ClearSamplers = false;
	TheShadowManager->ShadowCubeMapPixel->ClearSamplers = false;
	TheShadowManager->ShadowMapBlurVertex->ClearSamplers = false;
	TheShadowManager->ShadowMapBlurPixel->ClearSamplers = false;

	TheShadowManager->ShadowShadersLoaded = true;
    if (TheShadowManager->ShadowMapVertex == nullptr || TheShadowManager->ShadowMapPixel == nullptr  || TheShadowManager->ShadowMapBlurVertex  == nullptr
        || TheShadowManager->ShadowCubeMapVertex == nullptr || TheShadowManager->ShadowCubeMapPixel == nullptr || TheShadowManager->ShadowMapBlurPixel  == nullptr ){
		TheShadowManager->ShadowShadersLoaded = false;
		Logger::Log("[ERROR]: Could not load one or more of the ShadowMap generation shaders. Reinstall the mod.");
    }

	UINT ShadowCubeMapSize = TheShaderManager->Effects.ShadowsExteriors->Settings.Interiors.ShadowCubeMapSize;
	TheShadowManager->ShadowCubeMapViewPort = { 0, 0, ShadowCubeMapSize, ShadowCubeMapSize, 0.0f, 1.0f };

	TheShadowManager->shadowMapsRenderTime = 0;
}


/*
* Returns the given object ref's NiNode if it passes the test for excluded form types, otherwise returns NULL.
*/
NiNode* ShadowManager::GetRefNode(TESObjectREFR* Ref, ShadowsExteriorEffect::FormsStruct* Forms) {
	
	if (!Ref) return NULL;
	NiNode* Node = Ref->GetNode();

	if (!Node) return NULL;
	if (Ref->flags & TESForm::FormFlags::kFormFlags_NotCastShadows) return NULL;

	TESForm* Form = Ref->baseForm;
	UInt8 TypeID = Form->formType;
	switch (TypeID) {
	case TESForm::FormType::kFormType_Land:
		return NULL; // land is handled separately
		break;
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
	case TESForm::FormType::kFormType_NPC:
	case TESForm::FormType::kFormType_Creature:
	case TESForm::FormType::kFormType_LeveledCreature:
		if (!Forms->Actors) return NULL;
		break;
	case TESForm::FormType::kFormType_Stat:
	case TESForm::FormType::kFormType_StaticCollection:
	case TESForm::FormType::kFormType_MoveableStatic:
		//return NULL;
		if (!Forms->Statics) return NULL;
		break;
	default:
		break;
	}

	ExtraRefractionProperty* RefractionExtraProperty = (ExtraRefractionProperty*)Ref->extraDataList.GetExtraData(BSExtraData::ExtraDataType::kExtraData_RefractionProperty);
	float Refraction = RefractionExtraProperty ? (1 - RefractionExtraProperty->refractionAmount) : 0.0f;
	if (Refraction >= 0.5) return NULL;

	return Node;
}


// Detect which pass the object must be added to
void ShadowManager::AccumObject(std::stack<NiAVObject*>* containersAccum, NiAVObject* NiObject, ShadowsExteriorEffect::FormsStruct* Forms, bool isLODLand) {
	auto timelog = TimeLogger();

	NiGeometry* geo = static_cast<NiGeometry*>(NiObject);
	if (!geo->shader) return; // skip Geometry without a shader

#if defined(OBLIVION)
	if (geo->m_pcName && !memcmp(geo->m_pcName, "Torch", 5)) return; // No torch geo, it is too near the light and a bad square is rendered.
#endif

	if (skinnedGeoPass->AccumObject(geo)) {}
	else if (speedTreePass->AccumObject(geo)) {}
	else if (Forms->Lod && isLODLand && terrainLODPass->AccumObject(geo)) {}
	else if (Forms->AlphaEnabled && alphaPass->AccumObject(geo)) {}
	else geometryPass->AccumObject(geo);

	//timelog.LogTime("ShadowManager::AccumObject");
}


// go through the Object children and sort the ones that will be rendered based on their properties
void ShadowManager::AccumChildren(NiAVObject* NiObject, ShadowsExteriorEffect::FormsStruct* Forms, bool isLand, bool isLOD, NiFrustumPlanes *arPlanes) {
	if (!NiObject) return;

	std::stack<NiAVObject*> containers;
	NiAVObject* child;
	NiAVObject* object;
	NiNode* Node;

	//list all objects contained, or sort the object if not a container
	if (!NiObject->IsGeometry())
		containers.push(NiObject);
	else
		AccumObject(&containers, NiObject, Forms, isLand && isLOD);
		

	// Gather geometry
	while (!containers.empty()) {
    	object = containers.top();
    	containers.pop();

		if (!object) continue;

		Node = object->IsNiNode();
    	if (!Node || Node->m_flags & NiAVObject::NiFlags::APP_CULLED) continue; // culling containers
		if (!isLand && Node->GetWorldBoundRadius() < Forms->MinRadius) continue;

		if (Node->IsKindOf<NiSwitchNode>()) {
			// NiSwitchNode - only render active children (if exists) to the shadow map.
			NiSwitchNode* SwitchNode = static_cast<NiSwitchNode*>(Node);
			if (SwitchNode->m_iIndex < 0)
				continue;

			child = Node->m_children.data[SwitchNode->m_iIndex];
			if (!child->IsGeometry())
				containers.push(child);
			else
				AccumObject(&containers, child, Forms, false);
			continue;
		}

		for (int i = 0; i < Node->m_children.end; i++) {
			child = Node->m_children.data[i];
			if (!child || child->m_flags & NiAVObject::NiFlags::APP_CULLED) continue; // culling children
			if (!isLand && child->GetWorldBoundRadius() < Forms->MinRadius) continue;

			// Frustum culling.
			if (arPlanes && (isLand || isLOD)) {
				BSMultiBoundNode* multibound = child->IsMultiBoundNode();

				if (multibound && !multibound->spMultiBound->spShape->WithinFrustum(*arPlanes)) continue;
			}
			else if (arPlanes && !child->WithinFrustum(arPlanes)) continue;

			if (child->IsFadeNode() && static_cast<BSFadeNode*>(child)->FadeAlpha < 0.75f) continue; // stop rendering fadenodes below a certain opacity
			if (!child->IsGeometry())
				containers.push(child);
			else
				AccumObject(&containers, child, Forms, isLand && isLOD);
		}
	}
}

// Go through accumulations and render found objects
void ShadowManager::RenderAccums(D3DVIEWPORT9* ViewPort) {
	IDirect3DDevice9* Device = TheRenderManager->device;

	Device->SetViewport(ViewPort);

	Device->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 0L);

	Device->BeginScene();
	
	geometryPass->RenderAccum();
	terrainLODPass->RenderAccum();
	alphaPass->RenderAccum();
	skinnedGeoPass->RenderAccum();
	speedTreePass->RenderAccum();

	Device->EndScene();
}


void ShadowManager::RenderShadowMap(ShadowsExteriorEffect::ShadowMapSettings* ShadowMap, D3DXMATRIX* ViewProj) {
	ShadowsExteriorEffect* Shadows = TheShaderManager->Effects.ShadowsExteriors;
	NiDX9RenderState* RenderState = TheRenderManager->renderState;

	ShadowMap->ShadowCameraToLight = (*ViewProj);
	TheCameraManager->SetFrustum(&ShadowMap->ShadowMapFrustum, ViewProj);

	RenderState->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_ZWRITEENABLE, D3DZB_TRUE, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_ALPHABLENDENABLE, 0, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_DEPTHBIAS, (DWORD)0.0f, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, (DWORD)0.0f, RenderStateArgs);

	if (ShadowMap->Forms.Lod) {
		AccumChildren(BGSTerrainManager::GetRootLandLODNode(), &ShadowMap->Forms, true, true, &ShadowMap->ShadowMapFrustumPlanes);
		AccumChildren(BGSTerrainManager::GetRootObjectLODNode(), &ShadowMap->Forms, false, true, &ShadowMap->ShadowMapFrustumPlanes);
	}

	if (Player->GetWorldSpace()) {
		GridCellArray* CellArray = Tes->gridCellArray;
		UInt32 CellArraySize = CellArray->size * CellArray->size;

		for (UInt32 i = 0; i < CellArraySize; i++) {
			AccumExteriorCell(CellArray->GetCell(i), ShadowMap);
		}
	}
	else {
		AccumExteriorCell(Player->parentCell, ShadowMap);
	}

	RenderAccums(&ShadowMap->ShadowMapViewPort);
}


void ShadowManager::AccumExteriorCell(TESObjectCELL* Cell, ShadowsExteriorEffect::ShadowMapSettings* ShadowMap) {
	if (!Cell || Cell->IsInterior())
		return;
	
	if (ShadowMap->Forms.Terrain)
		AccumChildren(Cell->GetChildNode(TESObjectCELL::kCellNode_Land), &ShadowMap->Forms, true, false, &ShadowMap->ShadowMapFrustumPlanes);

	TList<TESObjectREFR>::Entry* Entry = &Cell->objectList.First;
	while (Entry) {
		NiNode* RefNode = GetRefNode(Entry->item, &ShadowMap->Forms);

		if (!RefNode) {
			Entry = Entry->next;
			continue;
		}

		if (RefNode && RefNode->WithinFrustum(&ShadowMap->ShadowMapFrustumPlanes))
			AccumChildren(RefNode, &ShadowMap->Forms, false, false, &ShadowMap->ShadowMapFrustumPlanes);

		Entry = Entry->next;
	}
}


void ShadowManager::RenderShadowSpotlight(NiSpotLight** Lights, UInt32 LightIndex) {
	NiSpotLight* pNiLight = Lights[LightIndex];
	if (pNiLight == NULL || !pNiLight->CastShadows) return;

	ShadowsExteriorEffect* Shadows = TheShaderManager->Effects.ShadowsExteriors;
	ShadowsExteriorEffect::InteriorsStruct* Settings = &Shadows->Settings.Interiors;

	IDirect3DDevice9* Device = TheRenderManager->device;
	NiDX9RenderState* RenderState = TheRenderManager->renderState;
	D3DXMATRIX View, Proj;

	NiPoint3* LightPos = &pNiLight->m_worldTransform.pos;
	float Radius = pNiLight->Spec.r;

#if defined(OBLIVION)
	if (pNiLight->CanCarry)
		Radius = 256.0f;
#endif

	D3DXVECTOR3 Up = D3DXVECTOR3(0, 0, 1);
	D3DXVECTOR3 Eye = LightPos->toD3DXVEC3();
	Eye.x -= TheRenderManager->CameraPosition.x;
	Eye.y -= TheRenderManager->CameraPosition.y;
	Eye.z -= TheRenderManager->CameraPosition.z;
	Shadows->Constants.ShadowCubeMapLightPosition.x = Eye.x;
	Shadows->Constants.ShadowCubeMapLightPosition.y = Eye.y;
	Shadows->Constants.ShadowCubeMapLightPosition.z = Eye.z;
	Shadows->Constants.ShadowCubeMapLightPosition.w = Radius;
	Shadows->Constants.Data.z = Radius;
	D3DXMatrixPerspectiveFovRH(&Proj, D3DXToRadian(pNiLight->OuterSpotAngle * 2), 1.0f, 0.1f, Radius);

	RenderState->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_ZWRITEENABLE, D3DZB_TRUE, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_ALPHABLENDENABLE, 0, RenderStateArgs);

	D3DXVECTOR3 CameraDirection = D3DXVECTOR3(pNiLight->m_worldTransform.rot.data[0][0], pNiLight->m_worldTransform.rot.data[1][0], pNiLight->m_worldTransform.rot.data[2][0]);
	D3DXVECTOR3 At = Eye + CameraDirection;

	TList<TESObjectREFR>::Entry* Entry = &Player->parentCell->objectList.First;
	while (Entry) {
		NiNode* RefNode = GetRefNode(Entry->item, &Settings->Forms);
		if (!RefNode) {
			Entry = Entry->next;
			continue;
		}

		// Detect if the object is in front of the light in the direction of the current face
		// TODO: improve to base on frustum
		D3DXVECTOR3 ObjectPos = RefNode->m_worldTransform.pos.toD3DXVEC3();
		D3DXVECTOR3 ObjectToLight = ObjectPos - LightPos->toD3DXVEC3();

		D3DXVec3Normalize(&ObjectToLight, &ObjectToLight);
		bool inFront = D3DXVec3Dot(&ObjectToLight, &CameraDirection) > 0;
		if (inFront && RefNode->GetDistance(LightPos) <= Radius + RefNode->GetWorldBoundRadius()) 
			AccumChildren(RefNode, &Settings->Forms, false, false);

		Entry = Entry->next;
	}

	D3DXMatrixLookAtRH(&View, &Eye, &At, &Up);
	Shadows->Constants.ShadowViewProj = View * Proj;
	TheShaderManager->SpotLightWorldToLightMatrix[LightIndex] = (Shadows->Constants.ShadowViewProj);

	Device->SetRenderTarget(0, Shadows->Textures.ShadowSpotlightSurface[LightIndex]);
	Device->SetDepthStencilSurface(Shadows->Textures.ShadowCubeMapDepthSurface);

	RenderAccums(&ShadowCubeMapViewPort);
}


void ShadowManager::RenderShadowCubeMap(ShadowSceneLight** Lights, UInt32 LightIndex) {
	if (Lights[LightIndex] == NULL) return; // No light at current index
	
	ShadowsExteriorEffect* Shadows = TheShaderManager->Effects.ShadowsExteriors;
	ShadowsExteriorEffect::InteriorsStruct* Settings = &Shadows->Settings.Interiors;

	IDirect3DDevice9* Device = TheRenderManager->device;
	NiDX9RenderState* RenderState = TheRenderManager->renderState;
	float Radius = 0.0f;
	float MinRadius = Settings->Forms.MinRadius;
	NiPoint3* LightPos = NULL;
	D3DXMATRIX View, Proj;
	D3DXVECTOR3 Eye, At, Up, CameraDirection;

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
		if (iter) {
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
				if (isFirstPerson) continue;

				if (!Player->isThirdPerson && !Settings->PlayerShadowFirstPerson && isThirdPerson)
					continue;

				if (Player->isThirdPerson && !Settings->PlayerShadowThirdPerson && isThirdPerson)
					continue;

				if (skinnedGeoPass->AccumObject(geo)) {}
				else if (speedTreePass->AccumObject(geo)) {}
				else if (Settings->Forms.AlphaEnabled && alphaPass->AccumObject(geo)) {}
				else geometryPass->AccumObject(geo);
			}
		}
		else {
			// old form based geo accumulation when the one perform by the game has not handled this light
			TList<TESObjectREFR>::Entry* Entry = &Player->parentCell->objectList.First;
			while (Entry) {
				if (NiNode* RefNode = GetRefNode(Entry->item, &Settings->Forms)) {
					// Detect if the object is in front of the light in the direction of the current face
					// TODO: improve to base on frustum
					D3DXVECTOR3 ObjectPos = RefNode->m_worldTransform.pos.toD3DXVEC3();
					D3DXVECTOR3 ObjectToLight = ObjectPos - LightPos->toD3DXVEC3();

					D3DXVec3Normalize(&ObjectToLight, &ObjectToLight);
					bool inFront = D3DXVec3Dot(&ObjectToLight, &CameraDirection) > 0;
					if (RefNode->GetDistance(LightPos) <= Radius + RefNode->GetWorldBoundRadius()) AccumChildren(RefNode, &Settings->Forms, false, false);
				}
				Entry = Entry->next;
			}
		}


		D3DXMatrixLookAtRH(&View, &Eye, &At, &Up);
		Shadows->Constants.ShadowViewProj = View * Proj;

		Device->SetRenderTarget(0, Shadows->Textures.ShadowCubeMapSurface[LightIndex][Face]);
		Device->SetDepthStencilSurface(Shadows->Textures.ShadowCubeMapDepthSurface);

		RenderAccums(&ShadowCubeMapViewPort);
	}
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

	// track point lights for interiors and exteriors
	ShadowSceneLight* ShadowLights[ShadowCubeMapsMax] = { NULL };
	NiPointLight* Lights[TrackedLightsMax] = { NULL };
	NiSpotLight* SpotLights[SpotLightsMax] = { NULL };

	TheShaderManager->GetNearbyLights(ShadowLights, Lights, SpotLights);

	ShadowsExteriorEffect* Shadows = TheShaderManager->Effects.ShadowsExteriors;
	ShadowsExteriorEffect::ExteriorsStruct* ShadowsExteriors = &Shadows->Settings.Exteriors;
	ShadowsExteriorEffect::InteriorsStruct* ShadowsInteriors = &Shadows->Settings.Interiors;

	bool isExterior = TheShaderManager->GameState.isExterior;// || currentCell->flags0 & TESObjectCELL::kFlags0_BehaveLikeExterior; // exterior flag currently broken
	bool ExteriorEnabled = isExterior && TheShaderManager->Effects.ShadowsExteriors->Enabled && ShadowsExteriors->Enabled;
	bool InteriorEnabled = !isExterior && TheShaderManager->Effects.ShadowsInteriors->Enabled;

	// early out in case shadow rendering is not required
	if (!ExteriorEnabled && !InteriorEnabled && !TheShaderManager->orthoRequired || !ShadowShadersLoaded) {
		return;
	}
	if (!Player->parentCell) return;

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

	DWORD zfunc;
	Device->GetRenderState(D3DRS_ZFUNC, &zfunc); // backup in case of inverted depth

	if (1.0 - NiDX9Renderer::GetSingleton()->m_fZClear) // inverted depth
		RenderState->SetRenderState(D3DRS_ZFUNC, D3DCMP_GREATEREQUAL, RenderStateArgs);
	else
		RenderState->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL, RenderStateArgs);

	RenderState->SetRenderState(D3DRS_STENCILENABLE, 1, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_STENCILREF, 0, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_ALPHAREF, 0, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_NORMALIZENORMALS, 1, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_POINTSIZE, 810365505, RenderStateArgs);

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

	// Render all shadow maps

	// Quantize sun direction angle to reduce shimmer by a large factor.
	D3DXVECTOR3 SunDir = Shadows->CalculateSmoothedSunDir();

	if (isExterior && (ExteriorEnabled || TheShaderManager->orthoRequired)) {

		// Update cascade depths based on current camera.
		Shadows->GetCascadeDepths();

		geometryPass->VertexShader = ShadowMapVertex;
		geometryPass->PixelShader = ShadowMapPixel;
		alphaPass->VertexShader = ShadowMapVertex;
		alphaPass->PixelShader = ShadowMapPixel;
		skinnedGeoPass->VertexShader = ShadowMapVertex;
		skinnedGeoPass->PixelShader = ShadowMapPixel;
		speedTreePass->VertexShader = ShadowMapVertex;
		speedTreePass->PixelShader = ShadowMapPixel;
		terrainLODPass->VertexShader = ShadowMapVertex;
		terrainLODPass->PixelShader = ShadowMapPixel;

		if (ExteriorEnabled && SunDir.z > 0.0f) {
			ShadowData->z = 0; // set shader constant to identify other shadow maps
			auto shadowMapTimer = TimeLogger();

			if (Shadows->ShadowAtlasSurfaceMSAA)
				Device->SetRenderTarget(0, Shadows->ShadowAtlasSurfaceMSAA);
			else
				Device->SetRenderTarget(0, Shadows->ShadowAtlasSurface);

			Device->SetDepthStencilSurface(Shadows->ShadowAtlasDepthSurface);

			for (int i = MapNear; i < MapOrtho; i++) {
				ShadowsExteriorEffect::ShadowMapSettings* ShadowMap = &Shadows->ShadowMaps[i];

				if (!Shadows->Settings.ShadowMaps.LimitFrequency || i != MapLod || !(FrameCounter % 4)) {
					Shadows->Constants.ShadowViewProj = Shadows->GetCascadeViewProj(ShadowMap, &SunDir);
					RenderShadowMap(ShadowMap, &Shadows->Constants.ShadowViewProj);
				}
				else {
					// We need to update the shadowprojmatrix of MapLod by the camera translation between frames to avoid jumps in the shadows.
					D3DXVECTOR3 newCameraTranslation = WorldSceneGraph->camera->m_worldTransform.pos.toD3DXVEC3();
					D3DXVECTOR3 difference = newCameraTranslation - ShadowMap->CameraTranslation;
					D3DXMATRIX translationMatrix;
					D3DXMatrixTranslation(&translationMatrix, difference.x, difference.y, difference.z);
					ShadowMap->ShadowCameraToLight = translationMatrix * ShadowMap->ShadowCameraToLight;
					ShadowMap->CameraTranslation = newCameraTranslation;
				}

				std::string message = "ShadowManager::RenderShadowMap ";
				message += std::to_string(i);
				shadowMapTimer.LogTime(message.c_str());
			}
		}

		// Resolve MSAA.
		if (Shadows->ShadowAtlasSurfaceMSAA)
			Device->StretchRect(Shadows->ShadowAtlasSurfaceMSAA, NULL, Shadows->ShadowAtlasSurface, NULL, D3DTEXF_LINEAR);

		if (Shadows->Settings.ShadowMaps.Prefilter) BlurShadowAtlas();

		if (Shadows->Settings.ShadowMaps.Mipmaps)
			Shadows->ShadowAtlasTexture->GenerateMipSubLevels();

		// render ortho map if one of the effects using ortho is active
		if (TheShaderManager->orthoRequired) {
			auto shadowMapTimer = TimeLogger();

			Device->SetRenderTarget(0, Shadows->ShadowMapOrthoSurface);
			Device->SetDepthStencilSurface(Shadows->ShadowMapOrthoDepthSurface);
			Device->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 0L);

			ShadowData->z = 1; // identify ortho map in shader constant
			D3DXVECTOR4 OrthoDir = D3DXVECTOR4(0.05f, 0.05f, 1.0f, 1.0f);
			D3DMATRIX View = GetViewMatrix(&At, &OrthoDir);
			Shadows->Constants.ShadowViewProj = Shadows->GetOrthoViewProj(View);

			RenderShadowMap(&Shadows->ShadowMaps[MapOrtho], &Shadows->Constants.ShadowViewProj);
			OrthoData->x = Shadows->ShadowMaps[MapOrtho].ShadowMapRadius * 2;
	
			shadowMapTimer.LogTime("ShadowManager::RenderShadowMap Ortho");
		}

	}

	// Render shadow maps for point lights
	bool usePointLights = (TheShaderManager->GameState.isDayTime > 0.5) ? ShadowsExteriors->UsePointShadowsDay : ShadowsExteriors->UsePointShadowsNight;

	AlphaEnabled = ShadowsInteriors->Forms.AlphaEnabled;
	geometryPass->VertexShader = ShadowCubeMapVertex;
	geometryPass->PixelShader = ShadowCubeMapPixel;
	alphaPass->VertexShader = ShadowCubeMapVertex;
	alphaPass->PixelShader = ShadowCubeMapPixel;
	skinnedGeoPass->VertexShader = ShadowCubeMapVertex;
	skinnedGeoPass->PixelShader = ShadowCubeMapPixel;
	speedTreePass->VertexShader = ShadowCubeMapVertex;
	speedTreePass->PixelShader = ShadowCubeMapPixel;

	auto shadowMapTimer = TimeLogger();
	if ((isExterior && usePointLights) || (!isExterior && InteriorEnabled)) {
		// render the cubemaps for each light
		for (int i = 0; i < ShadowsInteriors->LightPoints; i++) {

			// Render targets set in function due to rendering multiple faces.
			RenderShadowCubeMap(ShadowLights, i);

			std::string message = "ShadowManager::RenderShadowCubeMap ";
			message += std::to_string(i);
			shadowMapTimer.LogTime(message.c_str());
		}
	}

	if (TheShaderManager->Effects.Flashlight->Enabled && TheShaderManager->Effects.Flashlight->spotLightActive && TheShaderManager->Effects.Flashlight->Settings.renderShadows) {
		// render shadow maps for spotlights
		
		for (int i = 0; i < SpotLightsMax; i++) {
			if (!SpotLights[i] || SpotLights[i]->Spec.r == 0) continue; //bypass lights with no radius

			// Render targets set in function.
			RenderShadowSpotlight(SpotLights, i);

			std::string message = "ShadowManager::RenderShadowSpotLight";
			message += std::to_string(i);
			shadowMapTimer.LogTime(message.c_str());
		}
	}

	// reset renderer to previous state
	Device->SetDepthStencilSurface(DepthSurface);
	Device->SetRenderTarget(0, RenderSurface);
	Device->SetViewport(&viewport);
	Device->SetRenderState(D3DRS_ZFUNC, zfunc);

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
			D3DXSaveSurfaceToFileA(".\\Test\\shadowmapatlas.jpg", D3DXIFF_JPG, Shadows->ShadowAtlasSurface, NULL, NULL);
			D3DXSaveSurfaceToFileA(".\\Test\\shadowmaportho.jpg", D3DXIFF_JPG, Shadows->ShadowMapOrthoSurface, NULL, NULL);

			InterfaceManager->ShowMessage("Textures taken!");
		}
	}

	FrameCounter = (FrameCounter + 1) % 4;
	shadowMapsRenderTime = timer.LogTime("ShadowManager::RenderShadowMaps");
}


/*
* Filters the Shadow Map of given index using a 2 pass gaussian blur
*/
void ShadowManager::BlurShadowAtlas() {
	ShadowsExteriorEffect* Shadows = TheShaderManager->Effects.ShadowsExteriors;
	
	IDirect3DDevice9* Device = TheRenderManager->device;
    NiDX9RenderState* RenderState = TheRenderManager->renderState;
    IDirect3DTexture9* SourceShadowMap = Shadows->ShadowAtlasTexture;
    IDirect3DSurface9* TargetShadowMap = Shadows->ShadowAtlasSurface;

    Device->SetDepthStencilSurface(NULL);
    RenderState->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE, RenderStateArgs);
    RenderState->SetRenderState(D3DRS_ZWRITEENABLE, D3DZB_FALSE, RenderStateArgs);
    RenderState->SetVertexShader(ShadowMapBlurVertex->ShaderHandle, false);
    RenderState->SetPixelShader(ShadowMapBlurPixel->ShaderHandle, false);
	RenderState->SetFVF(FrameFVF, false);
	Device->SetStreamSource(0, Shadows->ShadowAtlasVertexBuffer, 0, sizeof(FrameVS));
	Device->SetRenderTarget(0, TargetShadowMap);
	
	// Pass map resolution to shader as a constant
	D3DXVECTOR4 inverseRes = { Shadows->ShadowAtlasCascadeTexelSize, Shadows->ShadowAtlasCascadeTexelSize, 0.0f, 0.0f };
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

