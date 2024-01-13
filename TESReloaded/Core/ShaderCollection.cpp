bool ShaderCollection::SwitchShader() {
	Enabled = !TheSettingManager->GetMenuShaderEnabled(Name);
	TheSettingManager->SetMenuShaderEnabled(Name, Enabled);

	// TODO: handle unloading/reloading of shaders here

	return Enabled;
}

bool ShaderCollection::DisposeShaders() {

}