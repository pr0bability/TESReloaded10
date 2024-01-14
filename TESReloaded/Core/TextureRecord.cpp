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
TextureRecord::TextureRecordType TextureRecord::GetTextureType(UINT Type) {

	if (Type >= D3DXPT_SAMPLER && Type <= D3DXPT_SAMPLER2D) return TextureRecordType::PlanarBuffer;
	if (Type == D3DXPT_SAMPLER3D) return TextureRecordType::VolumeBuffer;
	if (Type == D3DXPT_SAMPLERCUBE) return TextureRecordType::CubeBuffer;

	return TextureRecordType::None;
}

void TextureRecord::GetSamplerStates(std::string samplerStateSubstring) {
	std::map<std::string, int> WordSamplerType;
	WordSamplerType["ADDRESSU"] = D3DSAMP_ADDRESSU;
	WordSamplerType["ADDRESSV"] = D3DSAMP_ADDRESSV;
	WordSamplerType["ADDRESSW"] = D3DSAMP_ADDRESSW;
	WordSamplerType["BORDERCOLOR"] = D3DSAMP_BORDERCOLOR;
	WordSamplerType["MAGFILTER"] = D3DSAMP_MAGFILTER;
	WordSamplerType["MINFILTER"] = D3DSAMP_MINFILTER;
	WordSamplerType["MIPFILTER"] = D3DSAMP_MIPFILTER;
	WordSamplerType["MIPMAPLODBIAS"] = D3DSAMP_MIPMAPLODBIAS;
	WordSamplerType["MAXMIPLEVEL"] = D3DSAMP_MAXMIPLEVEL;
	WordSamplerType["MAXANISOTROPY"] = D3DSAMP_MAXANISOTROPY;
	WordSamplerType["SRGBTEXTURE"] = D3DSAMP_SRGBTEXTURE;

	std::map<std::string, int> WordTextureAddress;
	WordTextureAddress["WRAP"] = D3DTADDRESS_WRAP;
	WordTextureAddress["MIRROR"] = D3DTADDRESS_MIRROR;
	WordTextureAddress["CLAMP"] = D3DTADDRESS_CLAMP;
	WordTextureAddress["BORDER"] = D3DTADDRESS_BORDER;
	WordTextureAddress["MIRRORONCE"] = D3DTADDRESS_MIRRORONCE;

	std::map<std::string, int> WordTextureFilterType;
	WordTextureFilterType["NONE"] = D3DTEXF_NONE;
	WordTextureFilterType["POINT"] = D3DTEXF_POINT;
	WordTextureFilterType["LINEAR"] = D3DTEXF_LINEAR;
	WordTextureFilterType["ANISOTROPIC"] = D3DTEXF_ANISOTROPIC;

	std::map<std::string, int> WordSRGBType;
	WordSRGBType["FALSE"] = 0;
	WordSRGBType["TRUE"] = 1;

	std::stringstream samplerSettings = std::stringstream(samplerStateSubstring);
	std::string setting;
	while (std::getline(samplerSettings, setting, ';')) {
		size_t newlinePos = setting.find("\n");
		if (newlinePos != std::string::npos) setting.erase(newlinePos, 1);
		std::string opt = trim(setting.substr(0, setting.find("=") - 1));
		std::string val = trim(setting.substr(setting.find("=") + 1, setting.length()));
		std::transform(opt.begin(), opt.end(), opt.begin(), ::toupper);
		std::transform(val.begin(), val.end(), val.begin(), ::toupper);
		//	Logger::Log("%s : %s", opt.c_str(), val.c_str());

		size_t optIdx = WordSamplerType[opt];
		if (optIdx >= D3DSAMP_ADDRESSU && optIdx <= D3DSAMP_ADDRESSW)
			SamplerStates[(D3DSAMPLERSTATETYPE)optIdx] = WordTextureAddress[val];

		if (optIdx >= D3DSAMP_MAGFILTER && optIdx <= D3DSAMP_MIPFILTER)
			SamplerStates[(D3DSAMPLERSTATETYPE)optIdx] = WordTextureFilterType[val];

		if (optIdx == D3DSAMP_SRGBTEXTURE)
			SamplerStates[(D3DSAMPLERSTATETYPE)optIdx] = WordSRGBType[val];

		if (optIdx == D3DSAMP_BORDERCOLOR) {
			float va = std::stof(val);
			SamplerStates[(D3DSAMPLERSTATETYPE)optIdx] = *((DWORD*)&va);
		}

		if (optIdx == D3DSAMP_MAXANISOTROPY)
			SamplerStates[(D3DSAMPLERSTATETYPE)optIdx] = std::stoi(val);

	}

}



bool TextureRecord::BindTexture(const char* Name) {
	for (auto const& imap : TheTextureManager->TextureNames) {
		if (!strcmp(imap.first.c_str(), Name)) {
			Texture = *(IDirect3DBaseTexture9**)(TheTextureManager->TextureNames.at(imap.first));
			return true;
		}
	}
	return false;
}


/*
* Loads the actual texture file based on type/Name
*/
bool TextureRecord::LoadTexture(TextureRecordType Type, const char* TexturePath) {
	IDirect3DTexture9* Tex = NULL;
	IDirect3DVolumeTexture9* TexV = NULL;
	IDirect3DCubeTexture9* TexC = NULL;

	switch (Type) {
	case PlanarBuffer:
		D3DXCreateTextureFromFileA(TheRenderManager->device, TexturePath, &Tex);
		if (Tex == NULL) return false;
		Texture = Tex;
		break;
	case VolumeBuffer:
		D3DXCreateVolumeTextureFromFileA(TheRenderManager->device, TexturePath, &TexV);
		if (TexV == NULL) return false;
		Texture = TexV;
		break;
	case CubeBuffer:
		D3DXCreateCubeTextureFromFileA(TheRenderManager->device, TexturePath, &TexC);
		if (TexC == NULL) return false;
		Texture = TexC;
		break;
	default:
		return false;
	}

	return true;
}