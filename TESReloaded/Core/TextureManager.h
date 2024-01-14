#pragma once

typedef std::map<std::string, IDirect3DBaseTexture9*> TextureList;
typedef std::map<std::string, IDirect3DBaseTexture9**> TexturePointersList;
typedef std::vector<TextureRecord*> WaterMapList;

class TextureManager { // Never disposed
public:
	static void				Initialize();
	void					InitTexture(const char* Name, IDirect3DTexture9** Texture, IDirect3DSurface9** Surface, int Width, int Height, D3DFORMAT format);
	TextureRecord*			LoadTexture(ShaderTextureValue* Constant);
	void					SetWaterHeightMap(IDirect3DBaseTexture9* WaterHeightMap);
    void                    SetWaterReflectionMap(IDirect3DBaseTexture9* WaterReflectionMap);
    IDirect3DBaseTexture9* 	GetCachedTexture(std::string& pathS);

	IDirect3DTexture9*		SourceTexture;
	IDirect3DSurface9*		SourceSurface;
	IDirect3DTexture9* 		RenderedTexture;
	IDirect3DSurface9*		RenderedSurface;
	IDirect3DTexture9* 		NormalsTexture;
	IDirect3DSurface9*		NormalsSurface;
	IDirect3DTexture9* 		ShadowPassTexture;
	IDirect3DSurface9*		ShadowPassSurface;
	IDirect3DTexture9* 		AvgLumaTexture;
	IDirect3DSurface9*		AvgLumaSurface;
	IDirect3DTexture9* 		BloomTexture;
	IDirect3DSurface9*		BloomSurface;
	IDirect3DTexture9*		DepthTexture;
	IDirect3DTexture9*		DepthTextureINTZ;
	IDirect3DSurface9*		DepthSurface;
	IDirect3DTexture9*		ShadowMapTexture[5];
	IDirect3DSurface9*		ShadowMapSurface[5];
	IDirect3DSurface9*		ShadowMapDepthSurface[5];
	IDirect3DCubeTexture9*	ShadowCubeMapTexture[ShadowCubeMapsMax];
	IDirect3DSurface9*		ShadowCubeMapSurface[ShadowCubeMapsMax][6];
	IDirect3DSurface9*		ShadowCubeMapDepthSurface;
	TextureList				TextureCache;
	TexturePointersList		TextureNames;
    WaterMapList         	WaterHeightMapTextures;
    WaterMapList         	WaterReflectionMapTextures;

    IDirect3DBaseTexture9*  WaterHeightMapB;
    IDirect3DBaseTexture9*  WaterReflectionMapB;
};
