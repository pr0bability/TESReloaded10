#pragma once

class ShaderCollection
{
public:
	ShaderCollection(const char* ShaderName) {
		Name = ShaderName;
		Enabled = TheSettingManager->GetMenuShaderEnabled(Name);
	};
	virtual ~ShaderCollection() {};

	virtual void			UpdateConstants() {};
	virtual void			UpdateSettings() {};
	virtual void			RegisterConstants() {};
	virtual void			RegisterTextures() {};

	bool					SwitchShader();
	void					DisposeShaders();
	bool 					IsLoaded();
	bool					Enabled;

	const char* Name;
	std::vector<NiD3DPixelShaderEx*>	PixelShaderList;
	std::vector<NiD3DVertexShaderEx*>	VertexShaderList;
};