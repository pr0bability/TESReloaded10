#include <algorithm>


void TextureManager::Initialize() {

	Logger::Log("Starting the textures manager...");
	auto timer = TimeLogger();
	
	TheTextureManager = new TextureManager();

	IDirect3DDevice9* Device = TheRenderManager->device;
	UInt32 Width = TheRenderManager->width;
	UInt32 Height = TheRenderManager->height;
	
	// create textures used by NVR and bind them to surfaces
	TheTextureManager->InitTexture("TESR_SourceBuffer", &TheTextureManager->SourceTexture, &TheTextureManager->SourceSurface, Width, Height, D3DFMT_A16B16G16R16F);
	TheTextureManager->InitTexture("TESR_RenderedBuffer", &TheTextureManager->RenderedTexture, &TheTextureManager->RenderedSurface, Width, Height, D3DFMT_A16B16G16R16F);

	Device->CreateTexture(Width, Height, 1, D3DUSAGE_DEPTHSTENCIL, (D3DFORMAT)MAKEFOURCC('I', 'N', 'T', 'Z'), D3DPOOL_DEFAULT, &TheTextureManager->DepthTexture, NULL);
	TheTextureManager->RegisterTexture("TESR_DepthBuffer",(IDirect3DBaseTexture9**)&TheTextureManager->DepthTexture);
	Device->CreateTexture(Width, Height, 1, D3DUSAGE_DEPTHSTENCIL, (D3DFORMAT)MAKEFOURCC('I', 'N', 'T', 'Z'), D3DPOOL_DEFAULT, &TheTextureManager->DepthTextureViewModel, NULL);
	TheTextureManager->RegisterTexture("TESR_DepthBufferViewModel",(IDirect3DBaseTexture9**)&TheTextureManager->DepthTextureViewModel);
	TheTextureManager->RegisterTexture(WordWaterHeightMapBuffer, &TheTextureManager->WaterHeightMapB);
	TheTextureManager->RegisterTexture(WordWaterReflectionMapBuffer, &TheTextureManager->WaterReflectionMapB);

	timer.LogTime("TextureManager::Initialize");
}

/*
* Creates a texture of the given size and format and binds a surface to it, so it can be used as render target.
*/
void TextureManager::InitTexture(const char* Name, IDirect3DTexture9** Texture, IDirect3DSurface9** Surface, int Width, int Height, D3DFORMAT Format) {
	IDirect3DDevice9* Device = TheRenderManager->device;
	// create a texture to receive the surface contents
	HRESULT create = Device->CreateTexture(Width, Height, 1, D3DUSAGE_RENDERTARGET, Format, D3DPOOL_DEFAULT, Texture, NULL);

	if (FAILED(create)) {
		Logger::Log("[ERROR] : Failed to init texture %s", Name);
		return;
	}

	// set the surface level to the texture.
	(*Texture)->GetSurfaceLevel(0, Surface);
	RegisterTexture(Name, (IDirect3DBaseTexture9**)Texture);
}


/*
* Adds a texture to the list of sampler names recognized in shaders
*/
void TextureManager::RegisterTexture(const char* Name, IDirect3DBaseTexture9** Texture) {
	Logger::Log("Registering Texture %s", Name);
	TextureNames[Name] = Texture;
}

/*
* Gets a texture from the cache based on texture path
*/
IDirect3DBaseTexture9* TextureManager::GetCachedTexture(std::string& pathS) {
	TextureList::iterator t = TextureCache.find(pathS);
	if (t == TextureCache.end()) return nullptr;
	return t->second;
}


/*
* Gets a game dynamic texture by the sampler name
*/
IDirect3DBaseTexture9* TextureManager::GetTextureByName(std::string& Name) {
	TexturePointersList::iterator t = TextureNames.find(Name);
	if (t == TextureNames.end()) {
		Logger::Log("[ERROR] Texture %s not found.", Name.c_str());
		return nullptr;
	}
	return *(t->second);
}


/*
* Loads the actual texture file or get it from cache based on type/Name
*/
IDirect3DBaseTexture9* TextureManager::GetFileTexture(std::string TexturePath, TextureRecord::TextureRecordType Type) {

	IDirect3DBaseTexture9* Texture = GetCachedTexture(TexturePath);
	if (Texture) return Texture;

	switch (Type) {
	case TextureRecord::TextureRecordType::PlanarBuffer:
		D3DXCreateTextureFromFileA(TheRenderManager->device, TexturePath.data(), (IDirect3DTexture9**)&Texture);
		break;
	case TextureRecord::TextureRecordType::VolumeBuffer:
		D3DXCreateVolumeTextureFromFileA(TheRenderManager->device, TexturePath.data(), (IDirect3DVolumeTexture9**)&Texture);
		break;
	case TextureRecord::TextureRecordType::CubeBuffer:
		D3DXCreateCubeTextureFromFileA(TheRenderManager->device, TexturePath.data(), (IDirect3DCubeTexture9**)&Texture);
		break;
	default:
		Logger::Log("[ERROR] : Invalid texture type %i for %s", Type, TexturePath);
	}

	if (!Texture) Logger::Log("[ERROR] : Couldn't load texture file %s", TexturePath);
	else Logger::Log("Loaded texture file %s", TexturePath);

	// add texture to cache
	TextureCache[TexturePath] = Texture;
	return Texture;
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

