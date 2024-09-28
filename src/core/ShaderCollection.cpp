bool ShaderCollection::SwitchShader() {
	Enabled = !TheSettingManager->GetMenuShaderEnabled(Name);
	TheSettingManager->SetMenuShaderEnabled(Name, Enabled);

	// TODO: handle unloading/reloading of shaders here
	for (auto & pixelShader : PixelShaderList) {
		pixelShader->Enabled = Enabled;
	}

	for (auto& vertexShader : VertexShaderList) {
		vertexShader->Enabled = Enabled;
	}

	return Enabled;
}


void ShaderCollection::DisposeShaders() {

	for (auto& pixelShader : PixelShaderList) {
		pixelShader->DisposeShader();
	}

	for (auto& vertexShader : VertexShaderList) {
		vertexShader->DisposeShader();
	}
}

ShaderTemplate ShaderCollection::GetTemplate(const char* Name) {
	std::map<std::string, ShaderTemplate> templates = Templates();
	if (auto temp = templates.find(Name); temp != templates.end()) {
		return temp->second;
	}
	else {
		return ShaderTemplate{};
	}
}