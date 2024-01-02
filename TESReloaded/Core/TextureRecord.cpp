TextureRecord::TextureRecord() {

	Texture = NULL;
	SamplerStates[0] = 0; //This isn't used. Just to simplify  the matching between index and meaning
	SamplerStates[D3DSAMP_ADDRESSU] = D3DTADDRESS_WRAP;
	SamplerStates[D3DSAMP_ADDRESSV] = D3DTADDRESS_WRAP;
	SamplerStates[D3DSAMP_ADDRESSW] = D3DTADDRESS_WRAP;
	SamplerStates[D3DSAMP_BORDERCOLOR] = 0;
	SamplerStates[D3DSAMP_MAGFILTER] = D3DTEXF_POINT;
	SamplerStates[D3DSAMP_MINFILTER] = D3DTEXF_POINT;
	SamplerStates[D3DSAMP_MIPFILTER] = D3DTEXF_NONE;
	SamplerStates[D3DSAMP_MIPMAPLODBIAS] = 0;
	SamplerStates[D3DSAMP_MAXMIPLEVEL] = 0;
	SamplerStates[D3DSAMP_MAXANISOTROPY] = 1;
	SamplerStates[D3DSAMP_SRGBTEXTURE] = 0;

}

/*
* Detects which type the texture is based on DXParameter type or Name.
* Optionally can pass a pointer for wether the shader requires rendering the RenderedBuffer or DepthBuffer so it sets the value if detected.
*/
TextureRecord::TextureRecordType TextureRecord::GetTextureType(UINT Type, const char* Name, bool* HasRenderedBuffer, bool* HasDepthBuffer) {

	if (!strcmp(Name, "TESR_SourceBuffer")) return TextureRecordType::SourceBuffer;
	if (!strcmp(Name, "TESR_RenderedBuffer")) {
		if (HasRenderedBuffer) *HasRenderedBuffer = true;
		return TextureRecordType::RenderedBuffer;
	}
	if (!strcmp(Name, "TESR_DepthBuffer")) {
		if (HasDepthBuffer) *HasDepthBuffer = true;
		return TextureRecordType::DepthBuffer;
	}
	if (!strcmp(Name, "TESR_NormalsBuffer")) return TextureRecordType::NormalsBuffer;
	if (!strcmp(Name, "TESR_AvgLumaBuffer")) return TextureRecordType::AvgLumaBuffer;
	if (!strcmp(Name, "TESR_ShadowMapBufferNear")) return TextureRecordType::ShadowMapBufferNear;
	if (!strcmp(Name, "TESR_ShadowMapBufferMiddle")) return TextureRecordType::ShadowMapBufferMiddle;
	if (!strcmp(Name, "TESR_ShadowMapBufferFar")) return TextureRecordType::ShadowMapBufferFar;
	if (!strcmp(Name, "TESR_ShadowMapBufferLod")) return TextureRecordType::ShadowMapBufferLod;
	if (!strcmp(Name, "TESR_PointShadowBuffer")) return TextureRecordType::PointShadowBuffer;
	if (!strcmp(Name, "TESR_OrthoMapBuffer")) return TextureRecordType::OrthoMapBuffer;
	if (!strcmp(Name, "TESR_ShadowCubeMapBuffer0")) return TextureRecordType::ShadowCubeMapBuffer0;
	if (!strcmp(Name, "TESR_ShadowCubeMapBuffer1")) return TextureRecordType::ShadowCubeMapBuffer1;
	if (!strcmp(Name, "TESR_ShadowCubeMapBuffer2")) return TextureRecordType::ShadowCubeMapBuffer2;
	if (!strcmp(Name, "TESR_ShadowCubeMapBuffer3")) return TextureRecordType::ShadowCubeMapBuffer3;
	if (!strcmp(Name, "TESR_ShadowCubeMapBuffer4")) return TextureRecordType::ShadowCubeMapBuffer4;
	if (!strcmp(Name, "TESR_ShadowCubeMapBuffer5")) return TextureRecordType::ShadowCubeMapBuffer5;
	if (!strcmp(Name, "TESR_ShadowCubeMapBuffer6")) return TextureRecordType::ShadowCubeMapBuffer6;
	if (!strcmp(Name, "TESR_ShadowCubeMapBuffer7")) return TextureRecordType::ShadowCubeMapBuffer7;
	if (!strcmp(Name, "TESR_ShadowCubeMapBuffer8")) return TextureRecordType::ShadowCubeMapBuffer8;
	if (!strcmp(Name, "TESR_ShadowCubeMapBuffer9")) return TextureRecordType::ShadowCubeMapBuffer9;
	if (!strcmp(Name, "TESR_ShadowCubeMapBuffer10")) return TextureRecordType::ShadowCubeMapBuffer10;
	if (!strcmp(Name, "TESR_ShadowCubeMapBuffer11")) return TextureRecordType::ShadowCubeMapBuffer11;

	if (!strcmp(Name, WordWaterHeightMapBuffer)) return TextureRecordType::WaterHeightMapBuffer;
	if (!strcmp(Name, WordWaterReflectionMapBuffer)) return TextureRecordType::WaterReflectionMapBuffer;

	if (Type >= D3DXPT_SAMPLER && Type <= D3DXPT_SAMPLER2D) return TextureRecordType::PlanarBuffer;
	if (Type == D3DXPT_SAMPLER3D) return TextureRecordType::VolumeBuffer;
	if (Type == D3DXPT_SAMPLERCUBE) return TextureRecordType::CubeBuffer;

	return TextureRecordType::None;
}

/*
* Loads the actual texture file based on type/Name
*/
bool TextureRecord::LoadTexture(TextureRecordType Type, const char* Name) {
	IDirect3DTexture9* Tex = NULL;
	IDirect3DVolumeTexture9* TexV = NULL;
	IDirect3DCubeTexture9* TexC = NULL;

	// assigning shadow cube maps
	for (int i = 0; i < ShadowCubeMapsMax; i++) {
		if (Type == ShadowCubeMapBuffer0 + i) {
			Texture = TheTextureManager->ShadowCubeMapTexture[i];
			return true;
		}
	}

	// other buffers
	switch (Type) {
	case PlanarBuffer:
		D3DXCreateTextureFromFileA(TheRenderManager->device, Name, &Tex);
		if (Tex == NULL) return false;
		Texture = Tex;
		break;
	case VolumeBuffer:
		D3DXCreateVolumeTextureFromFileA(TheRenderManager->device, Name, &TexV);
		if (TexV == NULL) return false;
		Texture = TexV;
		break;
	case CubeBuffer:
		D3DXCreateCubeTextureFromFileA(TheRenderManager->device, Name, &TexC);
		if (TexC == NULL) return false;
		Texture = TexC;
		break;
	case SourceBuffer:
		Texture = TheTextureManager->SourceTexture;
		break;
	case RenderedBuffer:
		Texture = TheTextureManager->RenderedTexture;
		break;
	case DepthBuffer:
		Texture = TheTextureManager->DepthTexture;
		break;
	case NormalsBuffer:
		Texture = TheTextureManager->NormalsTexture;
		break;
	case AvgLumaBuffer:
		Texture = TheTextureManager->AvgLumaTexture;
		break;
	case ShadowMapBufferNear:
		Texture = TheTextureManager->ShadowMapTexture[ShadowManager::ShadowMapTypeEnum::MapNear];
		break;
	case ShadowMapBufferMiddle:
		Texture = TheTextureManager->ShadowMapTexture[ShadowManager::ShadowMapTypeEnum::MapMiddle];
		break;
	case ShadowMapBufferFar:
		Texture = TheTextureManager->ShadowMapTexture[ShadowManager::ShadowMapTypeEnum::MapFar];
		break;
	case ShadowMapBufferLod:
		Texture = TheTextureManager->ShadowMapTexture[ShadowManager::ShadowMapTypeEnum::MapLod];
		break;
	case OrthoMapBuffer:
		Texture = TheTextureManager->ShadowMapTexture[ShadowManager::ShadowMapTypeEnum::MapOrtho];
		break;
	case PointShadowBuffer:
		Texture = TheTextureManager->ShadowPassTexture;
		break;
	default:
		return false; //Texture is invalid or not assigned here.
	}
	return true;

}