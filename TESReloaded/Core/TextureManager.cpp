#include <algorithm>


void TextureManager::Initialize() {

	Logger::Log("Starting the textures manager...");
	auto timer = TimeLogger();
	
	TheTextureManager = new TextureManager();

	IDirect3DDevice9* Device = TheRenderManager->device;
	UInt32 Width = TheRenderManager->width;
	UInt32 Height = TheRenderManager->height;
	SettingsShadowStruct::ExteriorsStruct* ShadowsExteriors = &TheSettingManager->SettingsShadows.Exteriors;
	SettingsShadowStruct::InteriorsStruct* ShadowsInteriors = &TheSettingManager->SettingsShadows.Interiors;
	UINT ShadowMapSize = 0;
	UINT ShadowCubeMapSize = ShadowsInteriors->ShadowCubeMapSize;
	
	// create textures used by NVR and bind them to surfaces
	TheTextureManager->InitTexture("TESR_SourceBuffer", &TheTextureManager->SourceTexture, &TheTextureManager->SourceSurface, Width, Height, D3DFMT_A16B16G16R16F);
	TheTextureManager->InitTexture("TESR_RenderedBuffer", &TheTextureManager->RenderedTexture, &TheTextureManager->RenderedSurface, Width, Height, D3DFMT_A16B16G16R16F);
	TheTextureManager->InitTexture("TESR_PointShadowBuffer", &TheTextureManager->ShadowPassTexture, &TheTextureManager->ShadowPassSurface, Width, Height, D3DFMT_A8R8G8B8);
	TheTextureManager->InitTexture("TESR_NormalsBuffer", &TheTextureManager->NormalsTexture, &TheTextureManager->NormalsSurface, Width, Height, D3DFMT_A16B16G16R16F);
	TheTextureManager->InitTexture("TESR_AvgLumaBuffer", &TheTextureManager->AvgLumaTexture, &TheTextureManager->AvgLumaSurface, 1, 1, D3DFMT_A16B16G16R16F);
	//InitTexture(&TheTextureManager->BloomTexture, &TheTextureManager->BloomSurface, Width, Height, D3DFMT_A8R8G8B8);

	TheTextureManager->DepthTexture = NULL;
	TheTextureManager->DepthSurface = NULL;
	Device->CreateTexture(Width, Height, 1, D3DUSAGE_DEPTHSTENCIL, (D3DFORMAT)MAKEFOURCC('I', 'N', 'T', 'Z'), D3DPOOL_DEFAULT, &TheTextureManager->DepthTexture, NULL);
	
	TheTextureManager->TextureNames[WordWaterHeightMapBuffer] = &TheTextureManager->WaterHeightMapB;
	TheTextureManager->TextureNames[WordWaterReflectionMapBuffer] = &TheTextureManager->WaterReflectionMapB;
	TheTextureManager->TextureNames["TESR_DepthBuffer"] = (IDirect3DBaseTexture9**)&TheTextureManager->DepthTexture;


	std::vector<const char*>ShadowBufferNames = {
		"TESR_ShadowMapBufferNear",
		"TESR_ShadowMapBufferMiddle",
		"TESR_ShadowMapBufferFar",
		"TESR_ShadowMapBufferLod",
		"TESR_OrthoMapBuffer",
	};

	for (int i = 0; i <= ShadowManager::ShadowMapTypeEnum::MapOrtho; i++) {
		// create one texture per Exterior ShadowMap type
		float multiple = i == ShadowManager::ShadowMapTypeEnum::MapLod ? 2.0f : 1.0f; // double the size of lod map only
		ShadowMapSize = ShadowsExteriors->ShadowMapResolution * multiple;
		TheTextureManager->InitTexture(ShadowBufferNames[i], &TheTextureManager->ShadowMapTexture[i], &TheTextureManager->ShadowMapSurface[i], ShadowMapSize, ShadowMapSize, D3DFMT_G32R32F);
		Device->CreateDepthStencilSurface(ShadowMapSize, ShadowMapSize, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, true, &TheTextureManager->ShadowMapDepthSurface[i], NULL);

		// create textures to perform the blur
    }
	for (int i = 0; i < ShadowCubeMapsMax; i++) {
		Device->CreateCubeTexture(ShadowCubeMapSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &TheTextureManager->ShadowCubeMapTexture[i], NULL);
		for (int j = 0; j < 6; j++) {
			TheTextureManager->ShadowCubeMapTexture[i]->GetCubeMapSurface((D3DCUBEMAP_FACES)j, 0, &TheTextureManager->ShadowCubeMapSurface[i][j]);
		}
		std::string textureName = "TESR_ShadowCubeMapBuffer" + std::to_string(i);
		TheTextureManager->TextureNames[textureName] = (IDirect3DBaseTexture9**)&TheTextureManager->ShadowCubeMapTexture[i];
	}

	Device->CreateDepthStencilSurface(ShadowCubeMapSize, ShadowCubeMapSize, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, true, &TheTextureManager->ShadowCubeMapDepthSurface, NULL);
	timer.LogTime("TextureManager::Initialize");
}

/*
* Creates a texture of the given size and format and binds a surface to it, so it can be used as render target.
*/
void TextureManager::InitTexture(const char* Name, IDirect3DTexture9** Texture, IDirect3DSurface9** Surface, int Width, int Height, D3DFORMAT Format) {
	IDirect3DDevice9* Device = TheRenderManager->device;
	*Texture = NULL;
	*Surface = NULL;
	// create a texture to receive the surface contents
	Device->CreateTexture(Width, Height, 1, D3DUSAGE_RENDERTARGET, Format, D3DPOOL_DEFAULT, Texture, NULL);
	// set the surface level to the texture.
	(*Texture)->GetSurfaceLevel(0, Surface);
	TextureNames[Name] = (IDirect3DBaseTexture9**)Texture;
}

/*
* Binds texture buffers to a given register name
*/
TextureRecord* TextureManager::LoadTexture(ShaderTextureValue* Constant) {
	auto timer = TimeLogger();

	//Logger::Log("Loading texture %s (type:%i) (path: %s)", Constant->Name, Constant->Type, Constant->TexturePath);

	std::string TexturePath = Constant->TexturePath;
	TextureRecord::TextureRecordType Type = Constant->Type;

	if (!Type) {
		Logger::Log("[ERROR] Sampler %s doesn't have a valid type", Constant->Name);
		return nullptr;
	}
	
	TextureRecord* NewTextureRecord = new TextureRecord();

	if (Constant->TexturePath != "") { //Cache only non game textures
		IDirect3DBaseTexture9* cached = GetCachedTexture(TexturePath);
		if(!cached) {
			NewTextureRecord->LoadTexture(Type, TexturePath.c_str());
			if (NewTextureRecord->Texture){
				Logger::Log("Texture loaded: %s", TexturePath.c_str());
				TextureCache[TexturePath] = NewTextureRecord->Texture;
			}
			else {
				Logger::Log("ERROR: Cannot load texture %s", TexturePath.c_str());
			}
		}
		else {
			NewTextureRecord->Texture = cached;
			Logger::Log("Texture linked: %s", TexturePath.c_str());
		}
	}
	else {
		if (NewTextureRecord->BindTexture(Constant->Name)) {
			Logger::Log("Game Texture %s Binded", Constant->Name);
		}
		else {
            Logger::Log("ERROR: Cannot bind texture %s", Constant->Name);
        }
	}

	NewTextureRecord->GetSamplerStates(trim(Constant->SamplerString));

	timer.LogTime("TextureManager::LoadTexture");

	Constant->Texture = NewTextureRecord;
	return NewTextureRecord;
}

IDirect3DBaseTexture9* TextureManager::GetCachedTexture(std::string& pathS){
    TextureList::iterator t = TextureCache.find(pathS);
    if (t == TextureCache.end()) return nullptr;
    return t->second;
}


void TextureManager::SetWaterHeightMap(IDirect3DBaseTexture9* WaterHeightMap) {
    if (WaterHeightMapB == WaterHeightMap) return;
    WaterHeightMapB = WaterHeightMap;  //This may cause crashes on certain conditions
//    Logger::Log("Binding %0X", WaterHeightMap);
	for (WaterMapList::iterator it = TheTextureManager->WaterHeightMapTextures.begin(); it != TheTextureManager->WaterHeightMapTextures.end(); it++){
		 (*it)->Texture = WaterHeightMap;
	}
}

void TextureManager::SetWaterReflectionMap(IDirect3DBaseTexture9* WaterReflectionMap) {
    if (WaterReflectionMapB == WaterReflectionMap) return;
    WaterReflectionMapB = WaterReflectionMap;
//    Logger::Log("Binding %0X", WaterReflectionMap);
	for (WaterMapList::iterator it = TheTextureManager->WaterReflectionMapTextures.begin(); it != TheTextureManager->WaterReflectionMapTextures.end(); it++){
		 (*it)->Texture = WaterReflectionMap;
	}
}

