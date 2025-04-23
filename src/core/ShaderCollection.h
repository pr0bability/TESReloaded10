#pragma once

#include<map>
#include<string>

#include "ShaderTemplate.h"

class ShaderCollection
{
public:
	ShaderCollection(const char* ShaderName) {
		Logger::Log("ShaderCollection init: %s", ShaderName);
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

	virtual std::map<std::string_view, ShaderTemplate> Templates() {
		return std::map<std::string_view, ShaderTemplate>();
	};
	ShaderTemplate GetTemplate(const char*);
};