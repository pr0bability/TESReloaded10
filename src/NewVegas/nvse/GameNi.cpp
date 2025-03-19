#include "GameNi.h"

NiMemManager* NiMemManager::GetSingleton() {
	return *(NiMemManager**)0x11F6080;
}

// 0xAA13E0
void* NiNew(size_t stSize) {
	if (!stSize)
		stSize = 1;

	size_t stAlignment = 4;
	return NiMemManager::GetSingleton()->m_pkAllocator->Allocate(stSize, stAlignment, NI_OPER_NEW, true, 0, -1, 0);
}

// 0xAA1070
void* NiAlloc(size_t stSize) {
	if (!stSize)
		stSize = 1;

	size_t stAlignment = 4;
	return NiMemManager::GetSingleton()->m_pkAllocator->Allocate(stSize, stAlignment, NI_MALLOC, false, 0, -1, 0);
}

// 0xAA10B0
void* NiAlignedAlloc(size_t stSize, size_t stAlignment) {
	if (!stSize)
		stSize = 1;

	return NiMemManager::GetSingleton()->m_pkAllocator->Allocate(stSize, stAlignment, NI_ALIGNEDMALLOC, false, 0, -1, 0);
}

// 0xAA10F0
void NiFree(void* pvMem) {
	if (pvMem)
		NiMemManager::GetSingleton()->m_pkAllocator->Deallocate(pvMem, NI_FREE, -1);
}

// 0xAA1110
void NiAlignedFree(void* pvMem) {
	if (pvMem)
		NiMemManager::GetSingleton()->m_pkAllocator->Deallocate(pvMem, NI_ALIGNEDFREE, -1);
}

// 0xAA1460
void NiDelete(void* pvMem, size_t stElementSize) {
	if (pvMem)
		NiMemManager::GetSingleton()->m_pkAllocator->Deallocate(pvMem, NI_OPER_DELETE, stElementSize);
}

NiD3DRSEntry* NiD3DRenderStateGroup::FindRenderStateEntry(UInt32 auiState, bool& abInSaveList) {
	return ThisStdCall<NiD3DRSEntry*>(0xE7F000, this, auiState, &abInSaveList);
}

// 0xE76700
NiDX9ShaderDeclaration* NiDX9ShaderDeclaration::Create(NiDX9Renderer* apRenderer, UInt32 auiMaxStreamEntryCount, UInt32 auiStreamCount) {
	return CdeclCall<NiDX9ShaderDeclaration*>(0xE76700, apRenderer, auiMaxStreamEntryCount, auiStreamCount);
}

float* const BSShaderManager::fDepthBias = (float*)0x1200458;
float* const BSShaderManager::fLODLandDrop = (float*)0x11AD808;
NiPoint3* const BSShaderManager::kCameraPos = (NiPoint3*)0x11F474C;
NiPoint4* const BSShaderManager::kLoadedRange = (NiPoint4*)0x11F95F4;
BSShader** BSShaderManager::pspShaders = (BSShader**)0x11F9548;

ShadowSceneNode* BSShaderManager::GetShadowSceneNode(UInt32 aeType) {
	return ((ShadowSceneNode**)0x11F91C8)[aeType];
}

NiDX9Renderer* BSShaderManager::GetRenderer() {
	return *(NiDX9Renderer**)0x11F9508;
}

BSShader* BSShaderManager::GetShader(ShaderType aiShaderIndex) {
	if (!GetRenderer() || aiShaderIndex > BSSM_SHADER_VOLUMETRIC_FOG)
		return nullptr;

	if (pspShaders[aiShaderIndex])
		return pspShaders[aiShaderIndex];

	BSShader* pShader = nullptr;
	switch (aiShaderIndex) {
	case BSSM_SHADER_TALL_GRASS:
		pShader = TallGrassShader::CreateShader();
		break;
	case BSSM_SHADER_GEOM_DECAL:
		pShader = GeometryDecalShader::CreateShader();
		break;
	default:
		// Do not handle other shader types, not required for now.
		return nullptr;
	}

	pspShaders[aiShaderIndex] = pShader;

	return pShader;
}

// 0xBCC0A0
GeometryDecalShader* __stdcall GeometryDecalShader::CreateShader() {
	return StdCall<GeometryDecalShader*>(0xBCC0A0);
}

GeometryDecalShader* GeometryDecalShader::GetShader() {
	return static_cast<GeometryDecalShader*>(BSShaderManager::GetShader(BSShaderManager::BSSM_SHADER_GEOM_DECAL));
}

NiD3DPass** Lighting30Shader::GetAllPasses() {
	return (NiD3DPass**)0x1200EC0;
}

// 0xBACD00
TallGrassShader* TallGrassShader::Create(NiDX9ShaderDeclaration* apShaderDeclaration) {
	
	return NiCreate<TallGrassShader, 0xBACD00>(apShaderDeclaration);
}

// 0xBACF00
TallGrassShader* __stdcall TallGrassShader::CreateShader() {
	TallGrassShader* pShader = nullptr;
	NiDX9ShaderDeclaration* pShaderDeclaration = nullptr;

	pShaderDeclaration = NiDX9ShaderDeclaration::Create(NiDX9Renderer::GetSingleton(), 5, 1);
	pShaderDeclaration->SetEntry(0, 0, NiShaderDeclaration::SHADERPARAM_NI_POSITION, NiShaderDeclaration::SPTYPE_FLOAT3, 0);
	pShaderDeclaration->SetEntry(1u, 3u, NiShaderDeclaration::SHADERPARAM_NI_NORMAL, NiShaderDeclaration::SPTYPE_FLOAT3, 0);
	pShaderDeclaration->SetEntry(2u, 5u, NiShaderDeclaration::SHADERPARAM_NI_COLOR, NiShaderDeclaration::SPTYPE_FLOAT4, 0);
	pShaderDeclaration->SetEntry(3u, 7u, NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD0, NiShaderDeclaration::SPTYPE_FLOAT2, 0);
	pShaderDeclaration->SetEntryAlt(0, 4u, 6u, NiShaderDeclaration::SHADERPARAM_EXTRA_DATA_MASK, NiShaderDeclaration::SPTYPE_FLOAT1, NiShaderDeclaration::SPUSAGE_TEXCOORD, 1u, NiShaderDeclaration::SPTESS_DEFAULT);
	pShaderDeclaration->GetD3DDeclaration();

	pShader = TallGrassShader::Create(pShaderDeclaration);
	pShader->Initialize();
	pShader->InitShaders();
	pShader->InitShaderConstants();
	pShader->InitPasses();
	pShader->SetShaderDecl(pShaderDeclaration);

	return pShader;
}

TallGrassShader* TallGrassShader::GetShader() {
	return static_cast<TallGrassShader*>(BSShaderManager::GetShader(BSShaderManager::BSSM_SHADER_TALL_GRASS));
}

NiPoint3 const NiPoint3::ZERO = NiPoint3(0, 0, 0);
void NiPoint3::GetLookAt(NiPoint3* LookAt, NiPoint3* Rotation) {
	float x, y, z, r;
	
	x = this->x - LookAt->x;
	y = this->y - LookAt->y;
	z = this->z - LookAt->z;
	r = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
	Rotation->x = D3DXToDegree(atan2(y, x)) + 90.0f;
	Rotation->y = D3DXToDegree(acos(z / r)) - 90.0f;
	Rotation->z = 0;
}

void NiVector4::Normalize() {
	float len = sqrt((x * x) + (y * y) + (z * z));
	if (len > 0.000001f) {
		x = x / len;
		y = y / len;
		z = z / len;
	}
	else {
		x = 0.0f;
		y = 0.0f;
		z = 1.0f;
	}
}

void NiMatrix33::GenerateRotationMatrixZXY(NiPoint3* v, bool degrees) {
	float a = v->x;
	float b = v->y;
	float c = v->z;

	if (degrees) { a = D3DXToRadian(a); b = D3DXToRadian(b); c = D3DXToRadian(c); }
	data[0][0] = cos(a) * cos(c) - sin(a) * sin(b) * sin(c);
	data[0][1] = -cos(b) * sin(a);
	data[0][2] = cos(a) * sin(c) + cos(c) * sin(a) * sin(b);
	data[1][0] = cos(c) * sin(a) + cos(a) * sin(b) * sin(c);
	data[1][1] = cos(a) * cos(b);
	data[1][2] = sin(a) * sin(c) - cos(a) * cos(c) * sin(b);
	data[2][0] = -cos(b) * sin(c);
	data[2][1] = sin(b);
	data[2][2] = cos(b) * cos(c);
}

float NiPlane::Distance(const NiPoint3& arPoint) const {
	return Normal * arPoint - Constant;
}

int NiBound::WhichSide(const NiPlane& kPlane) const {
	float fDistance = kPlane.Distance(Center);
	return (fDistance > -Radius) ? (Radius <= fDistance) : 2;
}

template <typename T>
UInt16	NiTArray<T>::Add(T* Item) {
	return (UInt16)ThisCall(0x00A5EB20, this, Item);
}
template class NiTArray<NiAVObject*>; //CURSED

float NiAVObject::GetDistance(NiPoint3* Point) {

	NiPoint3 v;

	v.x = this->m_worldTransform.pos.x - Point->x;
	v.y = this->m_worldTransform.pos.y - Point->y;
	v.z = this->m_worldTransform.pos.z - Point->z;
	return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

NiBound* NiBound::GetGlobalWorldBound() {
	return (NiBound*)0x11F4288;
}

NiBound* NiAVObject::GetWorldBound() const { 
	return m_kWorldBound ? m_kWorldBound : NiBound::GetGlobalWorldBound();
}

float NiAVObject::GetWorldBoundRadius() {
	return (m_kWorldBound ? m_kWorldBound->Radius : 0.0f); 
}

void NiNode::New(UInt16 Children) { 
	ThisCall(0x00A5ECB0, this, Children);
}

void	SceneGraph::UpdateParticleShaderFoV(float FoV) {
	void (__cdecl* UpdateParticleShaderFoVData)(float) = (void (__cdecl*)(float))0x00B54000;
	ThisCall(0x00C52020, this, FoV, 0, NULL, 0);
	UpdateParticleShaderFoVData(FoV);
}

void	SceneGraph::SetNearDistance(float Distance) { 
	float* SettingNearDistance = (float*)0x01203148;
	*SettingNearDistance = Distance;
}

NiProperty*	 NiGeometry::GetProperty(NiProperty::PropertyType Type) {
	return propertyState.m_aspProps[Type];
}


bool NiSkinInstance::IsPartitionEnabled(UInt32 partitionIndex) {
	void* VFT = *(void**)this;
	if (VFT == Pointers::VirtualTables::BSDismemberSkinInstance){
		BSDismemberSkinInstance* t = (BSDismemberSkinInstance*) this;
		if (t->IsRenderable == 0) return false;
		if (t->partitions && t->partitionNumber < partitionIndex){
			DismemberPartition p = t->partitions[partitionIndex];
			if (!p.Enabled) return false;
		}
	}
	return true;
}

// 0x6532C0
bool NiObject::IsKindOf(const NiRTTI& apRTTI) const {
	return GetRTTI()->IsKindOf(apRTTI);
}

// 0x6532C0
bool NiObject::IsKindOf(const NiRTTI* const apRTTI) const {
	return GetRTTI()->IsKindOf(apRTTI);
}

// 0x45BAF0
bool NiObject::IsExactKindOf(const NiRTTI* const apRTTI) const {
	return GetRTTI()->IsExactKindOf(apRTTI);
}

// 0x45BAF0
bool NiObject::IsExactKindOf(const NiRTTI& apRTTI) const {
	return GetRTTI()->IsExactKindOf(apRTTI);
}

// 0x45BAD0
bool NiObject::IsExactKindOf(const NiRTTI& apRTTI, NiObject* apObject) {
	return apObject && apObject->IsExactKindOf(apRTTI);
}

// 0x45BAD0
bool NiObject::IsExactKindOf(const NiRTTI* const apRTTI, NiObject* apObject) {
	return apObject && apObject->IsExactKindOf(apRTTI);
}

void NiObject::LogObjectAttributes(){
	NiTArray<char*>* debug = (NiTArray<char*>*) Pointers::Functions::FormMemoryAlloc(sizeof(NiTArray<char*>));
	memset(debug, 0, sizeof(NiTArray<char*>));
	*(void**)debug = (void*)0x0102102C;  //NiTArray<char*> vtbl;
	debug->growSize = 16; //Must be initialized > 0    
	this->DumpAttributes(debug);
	for (int i = 0; i < debug->numObjs; i++){Logger::Log("%s", debug->data[i]); }
	Pointers::Functions::FormMemoryDeAlloc(debug);
}

void NiDX9Renderer::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE State, DWORD Value) {
	renderState->SetSamplerState(Sampler, State, Value, false);
}

void NiDX9Renderer::PackGeometryBuffer(NiGeometryBufferData* GeoData, NiGeometryData* ModelData, NiSkinInstance* SkinInstance, NiD3DShaderDeclaration* ShaderDeclaration) {
	
}

void NiDX9Renderer::PackSkinnedGeometryBuffer(NiGeometryBufferData* GeoData, NiGeometryData* ModelData, NiSkinInstance* SkinInstance, NiSkinPartition::Partition* Partition, NiD3DShaderDeclaration* ShaderDeclaration){
	
}

void NiDX9Renderer::CalculateBoneMatrixes(NiSkinInstance* SkinInstance, NiTransform* WorldTrasform) {
	ThisCall(0x00E6FE30, this, SkinInstance, WorldTrasform, false, 3, true); 
}

void NiDX9RenderState::SetCullMode(NiStencilProperty::DrawMode aeMode) {
	SetRenderState(D3DRS_CULLMODE, m_auiCullModeMapping[aeMode][LeftHanded], RenderStateArgs);
}

bool BSShaderProperty::IsLightingProperty() {
	return (uiShaderIndex == ShaderDefinitionEnum::kShaderDefinition_ShadowLightShader || uiShaderIndex == ShaderDefinitionEnum::kShaderDefinition_Lighting30Shader || uiShaderIndex == ShaderDefinitionEnum::kShaderDefinition_ParallaxShader);
}

bool BSShaderProperty::GetFlag(uint32_t auiFlag) const {
	return ((1 << (auiFlag % 0x20u)) & ulFlags[auiFlag >> 5]) != 0;
}

// GAME - 0xA74E10
void NiFrustumPlanes::Set(const NiFrustum& kFrust, const NiTransform& kXform) {
	ThisCall(0xA74E10, this, &kFrust, &kXform);
}

const NiPlane& NiFrustumPlanes::GetPlane(UInt32 ePlane) const {
	return CullingPlanes[ePlane];
}

bool NiFrustumPlanes::IsPlaneActive(UInt32 ePlane) const {
	return (ActivePlanes & (1 << ePlane)) ? true : false;
}

bool NiFrustumPlanes::IsAnyPlaneActive() const {
	return ActivePlanes ? true : false;
}

void NiFrustumPlanes::EnablePlane(UInt32 ePlane) {
	ActivePlanes = ActivePlanes | (1 << ePlane);
}

void NiFrustumPlanes::DisablePlane(UInt32 ePlane) {
	ActivePlanes = ActivePlanes & ~(1 << ePlane);
}

void NiFrustumPlanes::SetActivePlaneState(UInt32 uiState) {
	ActivePlanes = uiState;
}

bool NiAVObject::WithinFrustum(NiFrustumPlanes* arPlanes) {
	return GetWorldBound()->WithinFrustum(arPlanes);
}

bool NiBound::WithinFrustum(NiFrustumPlanes* arPlanes) {
	bool bInFrustum = true;
	for (UInt32 uiFace = 0; uiFace < arPlanes->MaxPlanes; ++uiFace) {
		if (arPlanes->IsPlaneActive(uiFace) && WhichSide(arPlanes->CullingPlanes[uiFace]) == NiPlane::NegativeSide) {
			bInFrustum = false;
			break;
		}
	}
	return bInFrustum;
}

// GAME - 0xA701B0
// GECK - 0x816B00
bool NiCamera::LookAtWorldPoint(const NiPoint3& kWorldPt, const NiPoint3& kWorldUp) {
	return ThisCall<bool>(0xA701B0, this, &kWorldPt, &kWorldUp);
}


D3DXMATRIX* const ShadowLightShader::VertexConstants::WorldTranspose = (D3DXMATRIX*)0x11FECC8;
NiPoint4* const ShadowLightShader::VertexConstants::LODLandParams = (NiPoint4*)0x11FA0B0;
