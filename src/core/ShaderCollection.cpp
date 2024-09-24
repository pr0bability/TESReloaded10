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

ShaderTemplate* ShaderCollection::GetTemplate(const char* Name) {
	Logger::Log("Looking for template for %s", Name);
	if (auto Template = Templates.find(Name); Template != Templates.end()) {
		Logger::Log("TEMPLATE FOUND");
		return &(Template->second);
	}
	else {
		return NULL;
	}
}