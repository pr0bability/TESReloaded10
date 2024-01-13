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
	InitTexture(&TheTextureManager->SourceTexture, &TheTextureManager->SourceSurface, Width, Height, D3DFMT_A16B16G16R16F);
	InitTexture(&TheTextureManager->RenderedTexture, &TheTextureManager->RenderedSurface, Width, Height, D3DFMT_A16B16G16R16F);
	InitTexture(&TheTextureManager->ShadowPassTexture, &TheTextureManager->ShadowPassSurface, Width, Height, D3DFMT_A8R8G8B8);
	InitTexture(&TheTextureManager->NormalsTexture, &TheTextureManager->NormalsSurface, Width, Height, D3DFMT_A16B16G16R16F);
	InitTexture(&TheTextureManager->AvgLumaTexture, &TheTextureManager->AvgLumaSurface, 1, 1, D3DFMT_A16B16G16R16F);
	//InitTexture(&TheTextureManager->BloomTexture, &TheTextureManager->BloomSurface, Width, Height, D3DFMT_A8R8G8B8);

	TheTextureManager->DepthTexture = NULL;
	TheTextureManager->DepthSurface = NULL;
	Device->CreateTexture(Width, Height, 1, D3DUSAGE_DEPTHSTENCIL, (D3DFORMAT)MAKEFOURCC('I', 'N', 'T', 'Z'), D3DPOOL_DEFAULT, &TheTextureManager->DepthTexture, NULL);

	for (int i = 0; i <= ShadowManager::ShadowMapTypeEnum::MapOrtho; i++) {
		// create one texture per Exterior ShadowMap type
		float multiple = i == ShadowManager::ShadowMapTypeEnum::MapLod ? 2.0f : 1.0f; // double the size of lod map only
		ShadowMapSize = ShadowsExteriors->ShadowMapResolution * multiple;
		InitTexture(&TheTextureManager->ShadowMapTexture[i], &TheTextureManager->ShadowMapSurface[i], ShadowMapSize, ShadowMapSize, D3DFMT_G32R32F);
		Device->CreateDepthStencilSurface(ShadowMapSize, ShadowMapSize, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, true, &TheTextureManager->ShadowMapDepthSurface[i], NULL);

		// create textures to perform the blur
    }
	for (int i = 0; i < ShadowCubeMapsMax; i++) {
		Device->CreateCubeTexture(ShadowCubeMapSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &TheTextureManager->ShadowCubeMapTexture[i], NULL);
		for (int j = 0; j < 6; j++) {
			TheTextureManager->ShadowCubeMapTexture[i]->GetCubeMapSurface((D3DCUBEMAP_FACES)j, 0, &TheTextureManager->ShadowCubeMapSurface[i][j]);
		}
	}

	Device->CreateDepthStencilSurface(ShadowCubeMapSize, ShadowCubeMapSize, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, true, &TheTextureManager->ShadowCubeMapDepthSurface, NULL);
	timer.LogTime("TextureManager::Initialize");
}

/*
* Creates a texture of the given size and format and binds a surface to it, so it can be used as render target.
*/
void TextureManager::InitTexture(IDirect3DTexture9** Texture, IDirect3DSurface9** Surface, int Width, int Height, D3DFORMAT Format) {
	IDirect3DDevice9* Device = TheRenderManager->device;
	*Texture = NULL;
	*Surface = NULL;
	// create a texture to receive the surface contents
	Device->CreateTexture(Width, Height, 1, D3DUSAGE_RENDERTARGET, Format, D3DPOOL_DEFAULT, Texture, NULL);
	// set the surface level to the texture.
	(*Texture)->GetSurfaceLevel(0, Surface);
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
    if(Type >= TextureRecord::TextureRecordType::WaterHeightMapBuffer){ /*Texture assigned after init*/
        if(Type == TextureRecord::TextureRecordType::WaterHeightMapBuffer){
            NewTextureRecord->Texture = WaterHeightMapB; 
            WaterHeightMapTextures.push_back(NewTextureRecord);
        }
        else if(Type == TextureRecord::TextureRecordType::WaterReflectionMapBuffer){
            NewTextureRecord->Texture = WaterReflectionMapB; 
            WaterHeightMapTextures.push_back(NewTextureRecord);
		}
		else {
			Logger::Log("Game Texture %s Not recognized", Constant->Name);
			delete NewTextureRecord;
			return nullptr;
		}
        Logger::Log("Game Texture %s Attached", Constant->Name);
    }
	else if(Type >= TextureRecord::TextureRecordType::PlanarBuffer && Type <= TextureRecord::TextureRecordType::CubeBuffer){ //Cache only non game textures
		IDirect3DBaseTexture9* cached = GetCachedTexture(TexturePath);
		if(!cached) {
			if (NewTextureRecord->LoadTexture(Type, TexturePath.c_str())) {
				if (NewTextureRecord->Texture){
					Logger::Log("Texture loaded: %s", TexturePath.c_str());
					TextureCache[TexturePath] = NewTextureRecord->Texture;
				}
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
		if (NewTextureRecord->LoadTexture(Type, nullptr)) {
			Logger::Log("Game Texture %s Binded", Constant->Name);
		}
		else {
            Logger::Log("ERROR: Cannot bind texture %s", Constant->Name);
        }
	}

	GetSamplerStates(Constant->SamplerString, NewTextureRecord);
	timer.LogTime("TextureManager::LoadTexture");

	Constant->Texture = NewTextureRecord;
	return NewTextureRecord;
}

IDirect3DBaseTexture9* TextureManager::GetCachedTexture(std::string& pathS){
    TextureList::iterator t = TextureCache.find(pathS);
    if (t == TextureCache.end()) return nullptr;
    return t->second;
}


void TextureManager::GetSamplerStates(std::string& samplerStateSubstring, TextureRecord* textureRecord ) {
	std::string WordSamplerType[SamplerStatesMax];
	std::string WordTextureAddress[6];
	std::string WordTextureFilterType[4];
	std::string WordSRGBType[2];
	WordSamplerType[0] = "";
	WordSamplerType[D3DSAMP_ADDRESSU] = "ADDRESSU";
	WordSamplerType[D3DSAMP_ADDRESSV] = "ADDRESSV";
	WordSamplerType[D3DSAMP_ADDRESSW] = "ADDRESSW";
	WordSamplerType[D3DSAMP_BORDERCOLOR] = "BORDERCOLOR";
	WordSamplerType[D3DSAMP_MAGFILTER] = "MAGFILTER";
	WordSamplerType[D3DSAMP_MINFILTER] = "MINFILTER";
	WordSamplerType[D3DSAMP_MIPFILTER] = "MIPFILTER";
	WordSamplerType[D3DSAMP_MIPMAPLODBIAS] = "MIPMAPLODBIAS";
	WordSamplerType[D3DSAMP_MAXMIPLEVEL] = "MAXMIPLEVEL";
	WordSamplerType[D3DSAMP_MAXANISOTROPY] = "MAXANISOTROPY";
	WordSamplerType[D3DSAMP_SRGBTEXTURE] = "SRGBTEXTURE";
    WordSamplerType[D3DSAMP_ELEMENTINDEX] = "";
    WordSamplerType[D3DSAMP_DMAPOFFSET] = "";
	WordTextureAddress[0] = "";
	WordTextureAddress[D3DTADDRESS_WRAP] = "WRAP";
	WordTextureAddress[D3DTADDRESS_MIRROR] = "MIRROR";
	WordTextureAddress[D3DTADDRESS_CLAMP] = "CLAMP";
	WordTextureAddress[D3DTADDRESS_BORDER] = "BORDER";
	WordTextureAddress[D3DTADDRESS_MIRRORONCE] = "MIRRORONCE";
	WordTextureFilterType[D3DTEXF_NONE] = "NONE";
	WordTextureFilterType[D3DTEXF_POINT] = "POINT";
	WordTextureFilterType[D3DTEXF_LINEAR] = "LINEAR";
	WordTextureFilterType[D3DTEXF_ANISOTROPIC] = "ANISOTROPIC";
	WordSRGBType[0] = "FALSE";
	WordSRGBType[1] = "TRUE";

	std::stringstream samplerSettings = std::stringstream(trim(samplerStateSubstring));
	std::string setting;
	while (std::getline(samplerSettings, setting, ';')) {
		size_t newlinePos = setting.find("\n");
		if (newlinePos != std::string::npos) setting.erase(newlinePos, 1);
		std::string opt = trim(setting.substr(0, setting.find("=") - 1));
		std::string val = trim(setting.substr(setting.find("=") + 1, setting.length()));
        std::transform(opt.begin(), opt.end(),opt.begin(), ::toupper);
        std::transform(val.begin(), val.end(),val.begin(), ::toupper);
	//	Logger::Log("%s : %s", opt.c_str(), val.c_str());
		size_t optIdx = 0;
		for (size_t i = 1; i < 12; i++) {
			if (opt == WordSamplerType[i]) {
				optIdx = i;
				break;
			}
		}
		if (optIdx >= D3DSAMP_ADDRESSU && optIdx <= D3DSAMP_ADDRESSW) {
			for (size_t i = 1; i < 6; i++) {
				if (val == WordTextureAddress[i]) {
					textureRecord->SamplerStates[(D3DSAMPLERSTATETYPE)optIdx] = i;
					break;
				}
			}
		}
		else if (optIdx >= D3DSAMP_MAGFILTER && optIdx <= D3DSAMP_MIPFILTER) {
			for (size_t i = 0; i < 4; i++) {
				if (val == WordTextureFilterType[i]) {
					textureRecord->SamplerStates[(D3DSAMPLERSTATETYPE)optIdx] = i;
					break;
				}
			}
		}
		else if (optIdx == D3DSAMP_SRGBTEXTURE) {
			for (size_t i = 0; i < 2; i++) {
				if (val == WordSRGBType[i]) {
					textureRecord->SamplerStates[(D3DSAMPLERSTATETYPE)optIdx] = i;
					break;
				}
			}
		}
		else if(optIdx == D3DSAMP_BORDERCOLOR){
            float va = std::stof(val);
			textureRecord->SamplerStates[(D3DSAMPLERSTATETYPE)optIdx] = *((DWORD*)&va);
        }
		else if(optIdx == D3DSAMP_MAXANISOTROPY){
			textureRecord->SamplerStates[(D3DSAMPLERSTATETYPE)optIdx] = std::stoi(val);
        }
	}
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

