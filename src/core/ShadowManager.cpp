#define ShadowMapFarPlane 32768;
#include <tracy/Tracy.hpp>

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

	TheShadowManager->ShadowMapBlurVertex = (ShaderRecordVertex*)ShaderRecord::LoadShader("ShadowMapBlur.vso", "Shadows\\");
	TheShadowManager->ShadowMapBlurPixel = (ShaderRecordPixel*)ShaderRecord::LoadShader("ShadowMapBlur.pso", "Shadows\\");

	TheShadowManager->ShadowMapClearPixel = (ShaderRecordPixel*)ShaderRecord::LoadShader("ShadowMapClear.pso", "Shadows\\");

	// Make sure samplers are not reset on SetCT as that causes errors.
	TheShadowManager->ShadowMapVertex->ClearSamplers = false;
	TheShadowManager->ShadowMapPixel->ClearSamplers = false;
	TheShadowManager->ShadowCubeMapVertex->ClearSamplers = false;
	TheShadowManager->ShadowCubeMapPixel->ClearSamplers = false;
	TheShadowManager->ShadowMapBlurVertex->ClearSamplers = false;
	TheShadowManager->ShadowMapBlurPixel->ClearSamplers = false;
	TheShadowManager->ShadowMapClearPixel->ClearSamplers = false;

	TheShadowManager->ShadowShadersLoaded = true;
	if (TheShadowManager->ShadowMapVertex == nullptr || TheShadowManager->ShadowMapPixel == nullptr || TheShadowManager->ShadowMapBlurVertex == nullptr
		|| TheShadowManager->ShadowCubeMapVertex == nullptr || TheShadowManager->ShadowCubeMapPixel == nullptr || TheShadowManager->ShadowMapBlurPixel == nullptr) {
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
			if (!Forms->Actors) return NULL; // D3D9 breaks on actors for some unknown reason
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


// Detech shader flags that we do not want.
bool ShadowManager::CheckShaderFlags(NiGeometry* Geometry) {
	BSShaderProperty* shaderProp = static_cast<BSShaderProperty*>(Geometry->GetProperty(NiProperty::kType_Shade));

	if (!shaderProp)
		return false;

	return !(shaderProp->GetFlag(BSSP_REFRACTION) ||
		shaderProp->GetFlag(BSSP_FIRE_REFRACTION) ||
		shaderProp->GetFlag(BSSP_DECAL) ||
		shaderProp->GetFlag(BSSP_DYNAMIC_DECAL));
}


// Detect which pass the object must be added to
void ShadowManager::AccumObject(NiGeometry* geo, CullTask* apTask, uint32_t mask, bool isLODLand) {
	if (!geo->shader || !CheckShaderFlags(geo)) {
		return;
	}

#if defined(OBLIVION)
	if (geo->m_pcName && !memcmp(geo->m_pcName, "Torch", 5)) return; // No torch geo, it is too near the light and a bad square is rendered.
#endif

	bool bAppend = false;
	if (skinnedGeoPass->AccumObject(geo, &bAppend)) {
		if (bAppend) {
			for (auto& accum : apTask->accums) {
				if (mask & static_cast<uint32_t>(accum.mask)) {
					accum.skinnedGeometry.push_back(geo);
				}
			}
		}
	}
	else if (speedTreePass->AccumObject(geo, &bAppend)) {
		if (bAppend) {
			for (auto& accum : apTask->accums) {
				if (mask & static_cast<uint32_t>(accum.mask)) {
					accum.speedtreeGeometry.push_back(geo);
				}
			}
		}
	}
	else if (terrainLODPass->AccumObject(geo, &bAppend)) {
		if (bAppend) {
			for (auto& accum : apTask->accums) {
				if (mask & static_cast<uint32_t>(accum.mask)) {
					if (accum.pConfig->Lod && isLODLand) {
						accum.terrainGeometry.push_back(geo);
					}
				}
			}
		}
	}
	else if (alphaPass->AccumObject(geo, &bAppend)) {
		if (bAppend) {
			for (auto& accum : apTask->accums) {
				if (mask & static_cast<uint32_t>(accum.mask)) {
					if (accum.pConfig->AlphaEnabled) {
						accum.alphaGeometry.push_back(geo);
					}
				}
			}
		}
	}
	else if (geometryPass->AccumObject(geo, &bAppend)) {
		if (bAppend) {
			for (auto& accum : apTask->accums) {
				if (mask & static_cast<uint32_t>(accum.mask)) {
					accum.geometry.push_back(geo);
				}
			}
		}
	}
}

void ShadowManager::AccumChildren(std::vector<AccumNode>& nodes, CullTask* apTask, const bool abIsLand, const bool abIsLod) {
	while (!nodes.empty()) {
		//ZoneScopedN("Process Node");

		auto [apObject, mask] = nodes[nodes.size() - 1];
		nodes.pop_back();

		if (!apObject || !mask || apObject->m_flags & NiAVObject::NiFlags::APP_CULLED) {
			continue;
		}

		//list all objects contained, or sort the object if not a container
		if (const auto geom = apObject->IsGeometry()) {
			//ZoneScopedN("Geom");
			AccumObject(geom, apTask, mask, abIsLand && abIsLod);
			continue;
		}

		NiNode* apNode = apObject->IsNiNode();
		if (!apNode) {
			continue;
		}

		// stop rendering fadenodes below a certain opacity
		if (apObject->IsFadeNode() && static_cast<BSFadeNode*>(apObject)->FadeAlpha < 0.75f) {
			continue;
		}

		const auto worldBound = apNode->GetWorldBoundRadius();
		const auto multibound = apObject->IsMultiBoundNode();

		for (const auto& accum : apTask->accums) {
			//ZoneScopedN("Mask Update");

			if ((mask & static_cast<uint32_t>(accum.mask)) == 0) {
				continue;
			}

			const auto apConfig = accum.pConfig;

			if (!abIsLand && worldBound < apConfig->MinRadius) {
				mask &= ~static_cast<uint32_t>(accum.mask);
				continue;
			}

			if (accum.pPlanes) {
				//ZoneScopedN("Multibound / Frustum Culling");

				if (multibound && (abIsLand || abIsLod)) {
					if (!multibound->spMultiBound->spShape->WithinFrustum(*accum.pPlanes)) {
						mask &= ~static_cast<uint32_t>(accum.mask);
					}
				}
				else {
					if (!apObject->WithinFrustum(accum.pPlanes)) {
						mask &= ~static_cast<uint32_t>(accum.mask);
					}
				}
			}
		}

		if (mask == 0) {
			continue;
		}

		// NiSwitchNode - only render active children (if exists) to the shadow map.
		if (const auto switchNode = apNode->NiDynamicCast<NiSwitchNode>()) {
			if (switchNode->m_iIndex >= 0) {
				nodes.emplace_back(AccumNode{ apNode->m_children.data[switchNode->m_iIndex], mask });
			}

			continue;
		}

		for (int i = 0; i < apNode->m_children.end; i++) {
			nodes.emplace_back(AccumNode{ apNode->m_children.data[i], mask });
		}
	}
}

// go through the Object children and sort the ones that will be rendered based on their properties
void ShadowManager::AccumChildren(NiAVObject* apObject, CullTask* apTask, uint32_t mask, const bool abIsLand, const bool abIsLod) {
	ZoneScoped;

	std::vector<AccumNode> nodes{};
	nodes.push_back(AccumNode{apObject, mask});

	AccumChildren(nodes, apTask, abIsLand, abIsLod);
}

// Go through accumulations and render found objects
void ShadowManager::RenderAccums(ShadowAccumulator& accum) {
	ZoneScoped;

	{
		ZoneScopedN("Geometry Pass");
		geometryPass->RenderAccum(accum.geometry);
	}
	{
		ZoneScopedN("Terrain LOD Pass");
		terrainLODPass->RenderAccum(accum.terrainGeometry);
	}
	{
		ZoneScopedN("Alpha Pass");
		alphaPass->RenderAccum(accum.alphaGeometry);
	}
	{
		ZoneScopedN("Skinned Geo Pass");
		skinnedGeoPass->RenderAccum(accum.skinnedGeometry);
	}
	{
		ZoneScopedN("Speed Tree Pass");
		speedTreePass->RenderAccum(accum.speedtreeGeometry);
	}
}


void ShadowManager::RenderShadowMap(CullTask* apTask, uint32_t mask) {
	ZoneScoped;

	erase_if(apTask->accums, [=](ShadowAccumulator& accum) {
		return !(mask & static_cast<uint32_t>(accum.mask));
	});

	apTask->UpdateMask();

	ShadowsExteriorEffect* Shadows = TheShaderManager->Effects.ShadowsExteriors;

	IDirect3DDevice9* Device = TheRenderManager->device;
	NiDX9RenderState* RenderState = TheRenderManager->renderState;

	RenderState->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_ZWRITEENABLE, D3DZB_TRUE, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE, RenderStateArgs);

	RenderState->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_ALPHAREF, 0, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS, RenderStateArgs);

	RenderState->SetRenderState(D3DRS_DEPTHBIAS, (DWORD)0.0f, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, (DWORD)0.0f, RenderStateArgs);

	for (auto& accum : apTask->accums) {
		TheCameraManager->SetFrustum(&accum.pShadowMap->ShadowMapFrustum, &accum.ViewProj);
	}

	AccumChildren(BGSTerrainManager::GetRootLandLODNode(), apTask, apTask->GetLodMask(), true, true);
	AccumChildren(BGSTerrainManager::GetRootObjectLODNode(), apTask, apTask->GetLodMask(), true, true);

	if (Player->GetWorldSpace()) {
		GridCellArray* CellArray = Tes->gridCellArray;
		UInt32 CellArraySize = CellArray->size * CellArray->size;

		for (UInt32 i = 0; i < CellArraySize; i++) {
			AccumExteriorCell(apTask, CellArray->GetCell(i));
		}
	}
	else {
		AccumExteriorCell(apTask, Player->parentCell);
	}

	for (auto& accum : apTask->accums) {
		D3DXVECTOR4* ShadowData = &TheShaderManager->Effects.ShadowsExteriors->Constants.Data;

		ShadowData->z = accum.zIndex;
		Shadows->Constants.ShadowViewProj = accum.ViewProj;

		Device->SetRenderTarget(0, accum.pSurface);
		Device->SetDepthStencilSurface(accum.pDepthSurface);

		Device->SetViewport(&accum.pShadowMap->ShadowMapViewPort);
		Device->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 0L);

		if (accum.pShadowMap->CustomClearRequired) {
			ClearShadowCascade(&accum.pShadowMap->ShadowMapViewPort, &accum.pShadowMap->ClearColor);
		}

		RenderAccums(accum);
	}
}


void ShadowManager::AccumExteriorCell(CullTask* apTask, TESObjectCELL* Cell) {
	ZoneScoped;

	if (!Cell || Cell->IsInterior()) {
		return;
	}

	AccumChildren(Cell->GetChildNode(TESObjectCELL::kCellNode_Land), apTask, apTask->GetTerrainMask(), true, false);

	ShadowsExteriorEffect::FormsStruct allForms{
		.AlphaEnabled = true,
		.Activators = true,
		.Actors = true,
		.Apparatus = true,
		.Books = true,
		.Containers = true,
		.Doors = true,
		.Furniture = true,
		.Misc = true,
		.Statics = true,
		.Terrain = true,
		.Trees = true,
		.Lod = true,
		.MinRadius = true,
		.OrigMinRadius = true
	};

	std::vector<AccumNode> nodes {};
	TList<TESObjectREFR>::Entry* Entry = &Cell->objectList.First;
	while (Entry) {
		auto maskInner = apTask->mask;

		if (const auto pRefNode = GetRefNode(Entry->item, &allForms)) {
			for (const auto& accum : apTask->accums) {
				if (!GetRefNode(Entry->item, accum.pConfig)) {
					maskInner &= ~static_cast<uint32_t>(accum.mask);
				}
			}

			if (maskInner) {
				nodes.emplace_back(AccumNode {pRefNode, maskInner });
			}
		}

		Entry = Entry->next;
	}

	if (nodes.size() > 10) {
		nodes.reserve(1024);
	}

	AccumChildren(nodes, apTask, false, false);
}


void ShadowManager::RenderShadowSpotlight(NiSpotLight** Lights, UInt32 LightIndex) {
	ZoneScoped;

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

	CullTask task{
		.accums = {
			ShadowAccumulator{
				.mask = ShadowMapTypeMask::Near,
				.geometry = {},
				.pConfig = &Settings->Forms,
				.pPlanes = nullptr,
				.pShadowMap = nullptr
			}
		}
	};

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
		if (inFront && RefNode->GetDistance(LightPos) <= Radius + RefNode->GetWorldBoundRadius()) {
			AccumChildren(RefNode, &task, task.GetMask(), false, false);
		}

		Entry = Entry->next;
	}

	D3DXMatrixLookAtRH(&View, &Eye, &At, &Up);
	Shadows->Constants.ShadowViewProj = View * Proj;
	TheShaderManager->SpotLightWorldToLightMatrix[LightIndex] = (Shadows->Constants.ShadowViewProj);

	Device->SetRenderTarget(0, Shadows->Textures.ShadowSpotlightSurface[LightIndex]);
	Device->SetDepthStencilSurface(Shadows->Textures.ShadowCubeMapDepthSurface);

	Device->SetViewport(&ShadowCubeMapViewPort);
	Device->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 0L);

	RenderAccums(task.accums[0]);
}


void ShadowManager::RenderShadowCubeMap(ShadowSceneLight** Lights, UInt32 LightIndex) {
	ZoneScoped;

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

	RenderState->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_ALPHAREF, 0, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS, RenderStateArgs);

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

		CullTask task{
			.accums = {
				ShadowAccumulator{
					.mask = ShadowMapTypeMask::Near,
					.geometry = {},
					.pConfig = &Settings->Forms,
					.pPlanes = nullptr,
					.pShadowMap = nullptr
				}
			}
		};

		// Since this is pure geometry, getting reference data will be difficult (read: slow)
		auto iter = Lights[LightIndex]->kGeometryList.start;
		if (iter) {
			while (iter) {
				NiGeometry* geo = iter->data;
				iter = iter->next;
				if (!geo || geo->m_flags & NiAVObject::APP_CULLED)
					continue;

				BSShaderProperty* shaderProp = static_cast<BSShaderProperty*>(geo->GetProperty(NiProperty::kType_Shade));
				NiMaterialProperty* matProp = static_cast<NiMaterialProperty*>(geo->GetProperty(NiProperty::kType_Material));

				if (!shaderProp)
					continue;

				// Skip refraction and fire refraction.
				if (!CheckShaderFlags(geo))
					continue;

				bool isFirstPerson = shaderProp->m_usFlags.GetBit(NiShadeProperty::kFirstPerson);
				bool isThirdPerson = shaderProp->m_usFlags.GetBit(NiShadeProperty::kThirdPerson);

				// Skip objects if they are barely visible. 
				if ((matProp && matProp->fAlpha < 0.05f))
					continue;

				// Also skip viewmodel due to issues, and render player's model only in 3rd person
				if (isFirstPerson) continue;

				if (!Player->isThirdPerson && !Settings->PlayerShadowFirstPerson && isThirdPerson)
					continue;

				if (Player->isThirdPerson && !Settings->PlayerShadowThirdPerson && isThirdPerson)
					continue;

				auto &accum = task.accums[0];
				bool bAppend = false;
				if (skinnedGeoPass->AccumObject(geo, &bAppend)) {
					if (bAppend) {
						accum.skinnedGeometry.push_back(geo);
					}
				}
				else if (speedTreePass->AccumObject(geo, &bAppend)) {
					if (bAppend) {
						accum.speedtreeGeometry.push_back(geo);
					}
				}
				else if (alphaPass->AccumObject(geo, &bAppend)) {
					if (Settings->Forms.AlphaEnabled && bAppend) {
						accum.alphaGeometry.push_back(geo);
					}
				}
				else if (geometryPass->AccumObject(geo, &bAppend)) {
					if (bAppend) {
						accum.geometry.push_back(geo);
					}
				}
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
					if (RefNode->GetDistance(LightPos) <= Radius + RefNode->GetWorldBoundRadius()) {
						AccumChildren(RefNode, &task, task.GetMask(), false, false);
					}
				}
				Entry = Entry->next;
			}
		}


		D3DXMatrixLookAtRH(&View, &Eye, &At, &Up);
		Shadows->Constants.ShadowViewProj = View * Proj;

		Device->SetRenderTarget(0, Shadows->Textures.ShadowCubeMapSurface[LightIndex][Face]);
		Device->SetDepthStencilSurface(Shadows->Textures.ShadowCubeMapDepthSurface);

		Device->SetViewport(&ShadowCubeMapViewPort);
		Device->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 0L);

		RenderAccums(task.accums[0]);
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
				shaderProperty->m_usFlags.Set(auiFlags);
			else
				shaderProperty->m_usFlags.Clear(auiFlags);
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
	ZoneScoped;

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

void ShadowManager::RecalculateBillboardVectors(D3DXVECTOR3* SunDir) {
	ZoneScoped;

	D3DXVECTOR3 WorldUp = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	// Calculate BillboardRight as perpendicular to SunDir and WorldUp
	D3DXVECTOR3 BillboardRightVec;
	D3DXVec3Cross(&BillboardRightVec, &WorldUp, SunDir);

	// Handle case where sun is directly above/below
	if (D3DXVec3LengthSq(&BillboardRightVec) < 0.0001f) {
		BillboardRightVec = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	}
	D3DXVec3Normalize(&BillboardRightVec, &BillboardRightVec);

	// Calculate BillboardUp perpendicular to both SunDir and BillboardRightVec
	D3DXVECTOR3 BillboardUpVec;
	D3DXVec3Cross(&BillboardUpVec, SunDir, &BillboardRightVec);
	D3DXVec3Normalize(&BillboardUpVec, &BillboardUpVec);

	// Set shader constants
	BillboardRight = NiVector4(BillboardRightVec.x, BillboardRightVec.y, BillboardRightVec.z, 0.0f);
	BillboardUp = NiVector4(BillboardUpVec.x, BillboardUpVec.y, BillboardUpVec.z, 0.0f);
}

/*
* Renders the different shadow maps: Near, Far, Ortho.
*/
void ShadowManager::RenderShadowMaps() {
	ZoneScoped;

	if (!TheSettingManager->SettingsMain.Main.RenderEffects) return; // cancel out if rendering effects is disabled

	// track point lights for interiors and exteriors
	ShadowSceneLight* ShadowLights[ShadowCubeMapsMax] = { NULL };
	NiPointLight* Lights[TrackedLightsMax] = { NULL };
	NiSpotLight* SpotLights[SpotLightsMax] = { NULL };

	TheShaderManager->GetNearbyLights(ShadowLights, Lights, SpotLights);

	ShadowsExteriorEffect* Shadows = TheShaderManager->Effects.ShadowsExteriors;
	ShadowsExteriorEffect::ExteriorsStruct* ShadowsExteriors = &Shadows->Settings.Exteriors;
	ShadowsExteriorEffect::InteriorsStruct* ShadowsInteriors = &Shadows->Settings.Interiors;

	bool isExterior = TheShaderManager->GameState.isExterior; // || currentCell->flags0 & TESObjectCELL::kFlags0_BehaveLikeExterior; // exterior flag currently broken
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
	NiNode* PlayerNode = Player->GetNode();
	D3DXVECTOR3 At;
	At.x = PlayerNode->m_worldTransform.pos.x - TheRenderManager->CameraPosition.x;
	At.y = PlayerNode->m_worldTransform.pos.y - TheRenderManager->CameraPosition.y;
	At.z = PlayerNode->m_worldTransform.pos.z - TheRenderManager->CameraPosition.z;

	// Render all shadow maps
	Device->BeginScene();

	// Quantize sun direction angle to reduce shimmer by a large factor.
	D3DXVECTOR3 SunDir = Shadows->CalculateSmoothedSunDir();

	CullTask task{
		.SunDir = SunDir
	};

	if (isExterior && (ExteriorEnabled || TheShaderManager->orthoRequired)) {
		for (int i = 0; i <= MapOrtho; i++) {
			ShadowsExteriorEffect::ShadowMapSettings* ShadowMap = &Shadows->ShadowMaps[i];

			ShadowAccumulator accum {
				.mask = static_cast<ShadowMapTypeMask>(1 << i),
				.geometry = {},
				.pConfig = &ShadowMap->Forms,
				.pPlanes = &ShadowMap->ShadowMapFrustumPlanes,
				.pShadowMap = ShadowMap,
				.pDepthSurface = Shadows->ShadowAtlasDepthSurface
			};

			if (i != MapOrtho) {
				accum.pSurface = Shadows->ShadowAtlasSurfaceMSAA ? Shadows->ShadowAtlasSurfaceMSAA : Shadows->ShadowAtlasSurface;
				accum.pDepthSurface = Shadows->ShadowAtlasDepthSurface;
				accum.zIndex = 0;
				accum.ViewProj = Shadows->GetCascadeViewProj(accum.pShadowMap, &SunDir);
			} else {
				accum.pSurface = Shadows->ShadowMapOrthoSurface;
				accum.pDepthSurface = Shadows->ShadowMapOrthoDepthSurface;
				accum.zIndex = 1;

				auto OrthoDir = D3DXVECTOR3(0.05f, 0.05f, 1.0f);
				accum.ViewProj = Shadows->GetCascadeViewProj(ShadowMap, &OrthoDir);
			}

			accum.Init();

			task.accums.push_back(accum);
		}

		auto renderMask = static_cast<uint32_t>(ShadowMapTypeMask::Primary);

		auto drawLod = (!Shadows->Settings.ShadowMaps.LimitFrequency || (FrameCounter % 4 == 0)) && ExteriorEnabled && SunDir.z > 0.0f;
		if (drawLod) {
			renderMask |= static_cast<uint32_t>(ShadowMapTypeMask::Lod);
		}

		const auto ortho = (!Shadows->Settings.OrthoMap.LimitFrequency || !((FrameCounter + 2) % 4)) && TheShaderManager->orthoRequired;
		if (ortho) {
			renderMask |= static_cast<uint32_t>(ShadowMapTypeMask::Ortho);
		}

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
			// Recalculate billboard vectors for speedtree leaves shader.
			RecalculateBillboardVectors(&SunDir);

			ShadowsExteriorEffect::ShadowMapSettings* ShadowMap = &Shadows->ShadowMaps[MapLod];

			// We need to update the shadowprojmatrix of MapLod by the camera translation between frames to avoid jumps in the shadows.
			D3DXVECTOR3 newCameraTranslation = WorldSceneGraph->camera->m_worldTransform.pos.toD3DXVEC3();
			D3DXVECTOR3 difference = newCameraTranslation - ShadowMap->CameraTranslation;
			D3DXMATRIX translationMatrix;
			D3DXMatrixTranslation(&translationMatrix, difference.x, difference.y, difference.z);
			ShadowMap->ShadowCameraToLight = translationMatrix * ShadowMap->ShadowCameraToLight;
			ShadowMap->CameraTranslation = newCameraTranslation;
			Shadows->Constants.ShadowBlur.y = Shadows->ShadowAtlasSurfaceMSAA ? 1.0f : 0.0f; // Disable blur for last cascade if MSAA is off.

			// Resolve MSAA.
			if (Shadows->ShadowAtlasSurfaceMSAA)
				Device->StretchRect(Shadows->ShadowAtlasSurfaceMSAA, NULL, Shadows->ShadowAtlasSurface, NULL, D3DTEXF_NONE);

			if (Shadows->Settings.ShadowMaps.Prefilter) BlurShadowAtlas();

			if (Shadows->Settings.ShadowMaps.Mipmaps)
				Shadows->ShadowAtlasTexture->GenerateMipSubLevels();
		}

		// render ortho map if one of the effects using ortho is active
		if (TheShaderManager->orthoRequired) {
			ShadowsExteriorEffect::ShadowMapSettings* ShadowMap = &Shadows->ShadowMaps[MapOrtho];

			D3DXVECTOR3 newCameraTranslation = WorldSceneGraph->camera->m_worldTransform.pos.toD3DXVEC3();
			D3DXVECTOR3 difference = newCameraTranslation - ShadowMap->CameraTranslation;
			D3DXMATRIX translationMatrix;
			D3DXMatrixTranslation(&translationMatrix, difference.x, difference.y, difference.z);
			ShadowMap->ShadowCameraToLight = translationMatrix * ShadowMap->ShadowCameraToLight;
			ShadowMap->CameraTranslation = newCameraTranslation;

			OrthoData->x = Shadows->Settings.OrthoMap.Distance * 2;
			OrthoData->y = ShadowMap->ShadowMapInverseResolution;
		}

		RenderShadowMap(&task, renderMask);
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
		if (Global->OnKeyDown(0x17)) {
			// TODO: setting for debug key ?
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

	Device->EndScene();

	FrameCounter = (FrameCounter + 1) % 4;
	shadowMapsRenderTime = timer.LogTime("ShadowManager::RenderShadowMaps");
}


/*
 * Clear a part of the shadow map atlas based on the shadow mapping mode.
 * 
 * Note: Render target, view port should be set beforehand. Scene has to be already being rendered.
 */
void ShadowManager::ClearShadowCascade(D3DVIEWPORT9* ViewPort, D3DXVECTOR4* ClearColor) {
	ZoneScoped;

	ShadowsExteriorEffect* Shadows = TheShaderManager->Effects.ShadowsExteriors;

	IDirect3DDevice9* Device = TheRenderManager->device;
	NiDX9RenderState* RenderState = TheRenderManager->renderState;
	IDirect3DSurface9* TargetShadowMap = Shadows->ShadowAtlasSurface;

	Device->SetDepthStencilSurface(NULL);
	RenderState->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_ZWRITEENABLE, D3DZB_FALSE, RenderStateArgs);
	RenderState->SetPixelShader(ShadowMapClearPixel->ShaderHandle, false);

	Device->SetPixelShaderConstantF(0, (const float*)ClearColor, 1);

	// Draw a full-screen quad (inside the viewport)
	struct VERTEX {
		float x, y, z, rhw;
	};
	VERTEX vertices[] = {
		{ (float)ViewPort->X - 0.5f, (float)ViewPort->Y - 0.5f, 0.5f, 1.0f },
		{ (float)(ViewPort->X + ViewPort->Width) - 0.5f, (float)ViewPort->Y - 0.5f, 0.5f, 1.0f },
		{ (float)ViewPort->X - 0.5f, (float)(ViewPort->Y + ViewPort->Height) - 0.5f, 0.5f, 1.0f },
		{ (float)(ViewPort->X + ViewPort->Width) - 0.5f, (float)(ViewPort->Y + ViewPort->Height) - 0.5f, 0.5f, 1.0f }
	};

	RenderState->SetFVF(D3DFVF_XYZRHW, false);

	Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(VERTEX));

	Device->SetDepthStencilSurface(Shadows->ShadowAtlasDepthSurface);
	RenderState->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_ZWRITEENABLE, D3DZB_TRUE, RenderStateArgs);
}


/*
* Filters the Shadow Map of given index using a 2 pass gaussian blur
*/
void ShadowManager::BlurShadowAtlas() {
	ZoneScoped;

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
	ShadowMapBlurPixel->SetShaderConstantF(0, &Shadows->Constants.ShadowBlur, 1);
	RenderState->SetTexture(0, SourceShadowMap);

	// blur in two passes, vertically and horizontally
	D3DXVECTOR4 Blur[2] = {
		D3DXVECTOR4(1.0f, 0.0f, 0.0f, 0.0f),
		D3DXVECTOR4(0.0f, 1.0f, 0.0f, 0.0f),
	};

	for (int i = 0; i < 2; i++) {
		// set blur direction shader constants
		ShadowMapBlurPixel->SetShaderConstantF(1, &Blur[i], 1);

		Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2); // draw call to execute the shader
	}

	RenderState->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_ZWRITEENABLE, D3DZB_TRUE, RenderStateArgs);
}
