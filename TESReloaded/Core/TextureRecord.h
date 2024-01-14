#pragma once

class TextureRecord {
public:
	TextureRecord();

	enum TextureRecordType {
		None,
		PlanarBuffer,
		VolumeBuffer,
		CubeBuffer
	};
	static TextureRecordType	GetTextureType(UINT Type);
	void						GetSamplerStates(std::string samplerStateSubstring);
	bool						BindTexture(const char* Name);
	bool						LoadTexture(TextureRecordType Type, const char* TexturePath);

	IDirect3DBaseTexture9*		Texture;
	DWORD						SamplerStates[SamplerStatesMax];
};
