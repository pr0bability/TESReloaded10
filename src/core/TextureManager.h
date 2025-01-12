#pragma once

typedef std::map<std::string, IDirect3DBaseTexture9*> TextureList;
typedef std::map<std::string, IDirect3DBaseTexture9**> TexturePointersList;
typedef std::vector<TextureRecord*> WaterMapList;

class TextureManager { // Never disposed
public:
	static void				Initialize();
	void					InitTexture(const char* Name, IDirect3DTexture9** Texture, IDirect3DSurface9** Surface, int Width, int Height, D3DFORMAT format, bool mipmaps = false);
	void					RegisterTexture(const char* Name, IDirect3DBaseTexture9** Texture);
	void					SetWaterHeightMap(IDirect3DBaseTexture9* WaterHeightMap);
    void                    SetWaterReflectionMap(IDirect3DBaseTexture9* WaterReflectionMap);
	IDirect3DBaseTexture9*	GetFileTexture(std::string TexturePath, TextureRecord::TextureRecordType type);
	IDirect3DBaseTexture9* 	GetCachedTexture(std::string& pathS);
	IDirect3DBaseTexture9*	GetTextureByName(std::string& Name);
	void					DumpToFile(IDirect3DTexture9* Texture, const char* Name);

	IDirect3DTexture9*		SourceTexture;
	IDirect3DSurface9*		SourceSurface;
	IDirect3DTexture9* 		RenderedTexture;
	IDirect3DSurface9*		RenderedSurface;
	IDirect3DTexture9* 		BloomTexture;
	IDirect3DSurface9*		BloomSurface;
	IDirect3DTexture9*		DepthTexture;
	IDirect3DTexture9*		DepthTextureViewModel;
	IDirect3DSurface9*		CombinedDepthSurface;
	IDirect3DTexture9*		CombinedDepthTexture;
	IDirect3DTexture9*		DepthTextureINTZ;
	IDirect3DSurface9*		DepthSurface;
	TextureList				TextureCache;
	TexturePointersList		TextureNames;
    WaterMapList         	WaterHeightMapTextures;
    WaterMapList         	WaterReflectionMapTextures;

    IDirect3DBaseTexture9*  WaterHeightMapB;
    IDirect3DBaseTexture9*  WaterReflectionMapB;
};
