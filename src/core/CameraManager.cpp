void CameraManager::Initialize() {

	Logger::Log("Starting the camera manager...");
	TheCameraManager = new CameraManager();

	TheCameraManager->DialogTarget = NULL;
	TheCameraManager->From = { 0.0f, 0.0f, 0.0f };
	TheCameraManager->FromOffset = { 0.0f, 0.0f, 0.0f };
	TheCameraManager->CameraCommands = new CameraManagerCommands();
	TheCameraManager->FirstPersonView = false;
	TheCameraManager->TogglePOV = false;

}

bool CameraManager::IsFirstPerson() {

	return TheSettingManager->SettingsMain.CameraMode.Enabled ? FirstPersonView : Player->IsFirstPerson();

}

bool CameraManager::IsVanity() {

	return Player->IsVanity();

}

void CameraManager::SetFirstPerson(bool FirstPerson) {

	FirstPersonView = FirstPerson;

}

void CameraManager::ResetCamera() {
	
	Player->ResetCamera();

}

void CameraManager::SetSceneGraph() {
	
	SettingsMainStruct::CameraModeStruct* CameraMode = &TheSettingManager->SettingsMain.CameraMode;
	float FoV = CameraMode->FoV;
	
	if (CameraMode->Enabled) {
		WorldSceneGraph->SetNearDistance(2.5f);
		if (FoV != WorldSceneGraph->cameraFOV && !Player->IsAiming() && InterfaceManager->IsActive(Menu::MenuType::kMenuType_None)) {
			WorldSceneGraph->UpdateParticleShaderFoV(FoV);
			Player->SetFoV(FoV);
		}
	}
	TheShaderManager->ShaderConst.ReciprocalResolution.w = Player->GetFoV(false);

}

void CameraManager::ExecuteCommands(NiAVObject* CameraNode) {

	CameraCommands->Execute(CameraNode);

}

CameraManager::CameraManagerCommands::CameraManagerCommands() {

	memset(this, NULL, sizeof(CameraManagerCommands));

}

void CameraManager::CameraManagerCommands::Execute(NiAVObject* CameraNode) {

	if (!IsTranslating && !IsRotating && !IsLookingAt && !IsTranslatingToPosition && !IsRotatingToPosition && !IsLookingAtPosition) return;

	NiMatrix33* CameraRotationW = &CameraNode->m_worldTransform.rot;
	NiPoint3* CameraPositionW = &CameraNode->m_worldTransform.pos;
	NiMatrix33* CameraRotationL = &CameraNode->m_localTransform.rot;
	NiPoint3* CameraPositionL = &CameraNode->m_localTransform.pos;
	NiMatrix33* NodeRotationW = NULL;
	NiPoint3* NodePositionW = NULL;
	NiMatrix33 m;
	NiPoint3 v;

	if (Ref) {
		NiNode* RootNode = Ref->GetNiNode();
		if (Ref->IsActor() && (IsTranslating || IsLookingAt)) {
			NiAVObject* Head = RootNode->GetObjectByName("Bip01 Head");
			NodeRotationW = &Head->m_worldTransform.rot;
			NodePositionW = &Head->m_worldTransform.pos;
		}
		else {
			NodeRotationW = &RootNode->m_worldTransform.rot;
			NodePositionW = &RootNode->m_worldTransform.pos;
		}
	}

	if (IsTranslating) {
		v = *NodeRotationW * Position;
		v.x += NodePositionW->x;
		v.y += NodePositionW->y;
		v.z += NodePositionW->z;
		memcpy(CameraPositionW, &v, sizeof(NiPoint3));
		memcpy(CameraPositionL, &v, sizeof(NiPoint3));
	}

	if (IsRotating) {
		m.GenerateRotationMatrixZXY(&Rotation, 1);
		m = *NodeRotationW * m;
		memcpy(CameraRotationW, &m, sizeof(NiMatrix33));
		memcpy(CameraRotationL, &m, sizeof(NiMatrix33));
	}

	if (IsTranslatingToPosition) {
		memcpy(CameraPositionW, &Position, sizeof(NiPoint3));
		memcpy(CameraPositionL, &Position, sizeof(NiPoint3));
	}

	if (IsRotatingToPosition) {
		m.GenerateRotationMatrixZXY(&Rotation, 1);
		memcpy(CameraRotationW, &m, sizeof(NiMatrix33));
		memcpy(CameraRotationL, &m, sizeof(NiMatrix33));
	}

	if (IsLookingAt || IsLookingAtPosition) {
		if (!IsTranslating && !IsTranslatingToPosition) {
			Position.x = CameraPositionW->x;
			Position.y = CameraPositionW->y;
			Position.z = CameraPositionW->z;
		}
		if (IsLookingAt) {
			LookingAtPosition = *NodeRotationW * LookingAtOffset;
			LookingAtPosition.x += NodePositionW->x;
			LookingAtPosition.y += NodePositionW->y;
			LookingAtPosition.z += NodePositionW->z;
		}
		Position.GetLookAt(&LookingAtPosition, &v);
		m.GenerateRotationMatrixZXY(&v, 1);
		memcpy(CameraRotationW, &m, sizeof(NiMatrix33));
		memcpy(CameraRotationL, &m, sizeof(NiMatrix33));
	}

}


void CameraManager::GetPlane(D3DXPLANE* plane, float a, float b, float c, float d) {
	D3DXPLANE newPlane = D3DXPLANE(a, b, c, d);
	D3DXPlaneNormalize(plane, &newPlane);
}

/**
* Generates the Frustrum planes from a matrix
*/
void CameraManager::SetFrustum(frustum* Frustum, D3DMATRIX* Matrix) {
	
	GetPlane(&Frustum->plane[frustum::PLANENEAR],
		Matrix->_13,
		Matrix->_23,
		Matrix->_33,
		Matrix->_43
	);
	GetPlane(&(Frustum->plane[frustum::PLANEFAR]),
		Matrix->_14 - Matrix->_13,
		Matrix->_24 - Matrix->_23,
		Matrix->_34 - Matrix->_33,
		Matrix->_44 - Matrix->_43
	);
	GetPlane(&Frustum->plane[frustum::PLANELEFT],
		Matrix->_14 + Matrix->_11,
		Matrix->_24 + Matrix->_21,
		Matrix->_34 + Matrix->_31,
		Matrix->_44 + Matrix->_41
	);
	GetPlane(&Frustum->plane[frustum::PLANERIGHT],
		Matrix->_14 - Matrix->_11,
		Matrix->_24 - Matrix->_21,
		Matrix->_34 - Matrix->_31,
		Matrix->_44 - Matrix->_41
	);
	GetPlane(&Frustum->plane[frustum::PLANETOP],
		Matrix->_14 - Matrix->_12,
		Matrix->_24 - Matrix->_22,
		Matrix->_34 - Matrix->_32,
		Matrix->_44 - Matrix->_42
	);
	GetPlane(&Frustum->plane[frustum::PLANEBOTTOM],
		Matrix->_14 + Matrix->_12,
		Matrix->_24 + Matrix->_22,
		Matrix->_34 + Matrix->_32,
		Matrix->_44 + Matrix->_42
	);
}


/*
* Checks wether the given node is in the frustrum using its radius for the current type of Shadow map.
*/
bool CameraManager::InFrustum(frustum* frustum, NiNode* Node, bool skipNear) {
	NiBound* Bound = Node->GetWorldBound();
	if (!Bound) return false;

	D3DXVECTOR3 Position = { 
		Bound->Center.x - TheRenderManager->CameraPosition.x, 
		Bound->Center.y - TheRenderManager->CameraPosition.y, 
		Bound->Center.z - TheRenderManager->CameraPosition.z 
	};

	for (int i = skipNear ? 1 : 0; i < 6; ++i) {
		if (D3DXPlaneDotCoord(&frustum->plane[i], &Position) <= -Bound->Radius)
			return false;
	}
	return true;

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
