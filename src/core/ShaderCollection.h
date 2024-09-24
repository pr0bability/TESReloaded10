#pragma once

#include<string>
#include<map>

#include "ShaderTemplate.h"

class ShaderCollection
{
public:
	ShaderCollection(const char* ShaderName, std::map<std::string, ShaderTemplate> Temps = std::map<std::string, ShaderTemplate>()) {
		Name = ShaderName;
		Enabled = TheSettingManager->GetMenuShaderEnabled(Name);
		Templates = Temps;
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

	ShaderTemplate* GetTemplate(const char*);
	std::map<std::string, ShaderTemplate> Templates;
};