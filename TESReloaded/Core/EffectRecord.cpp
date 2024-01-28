/*
* Class that wraps an effect shader, in order to load it/render it/set constants.
*/
EffectRecord::EffectRecord(const char* effectName) {

	Name = effectName;

	char FileName[MAX_PATH];
	strcpy(FileName, EffectsPath);
	strcat(FileName, Name);
	strcat(FileName, ".fx");
	Path = new std::string(FileName);

	strcat(FileName, ".hlsl");
	SourcePath = new std::string(FileName);

	Effect = NULL;
	Enabled = false;
	renderTime = 0;
	constantUpdateTime = 0;
}

/*Shader Values arrays are freed in the superclass Destructor*/
EffectRecord::~EffectRecord() {
	if (Effect) Effect->Release();
	delete Path;
	delete SourcePath;
}

/*
 * Unload effects, allowing it to be reloaded from  a blank state.
 */
void EffectRecord::DisposeEffect() {
	if (Effect) Effect->Release();
	Effect = nullptr;
	delete[] FloatShaderValues;
	delete[] TextureShaderValues;
	Enabled = false;
}

/*
 * Compile and Load the Effect shader
 */
bool EffectRecord::LoadEffect(bool alwaysCompile) {
	auto timer = TimeLogger();

	ID3DXBuffer* ShaderSource = NULL;
	ID3DXBuffer* Errors = NULL;
	ID3DXEffect* Effect = NULL;
	bool success = false;
	HRESULT prepass = D3DXPreprocessShaderFromFileA(SourcePath->data(), NULL, NULL, &ShaderSource, &Errors);
	ID3DXEffectCompiler* Compiler = NULL;
	ID3DXBuffer* EffectBuffer = NULL;
	HRESULT load = NULL;

	if (alwaysCompile || ShouldCompileShader(Path->data(), SourcePath->data(), (ShaderCompileType)TheSettingManager->SettingsMain.Develop.CompileEffects)) {
		HRESULT comp = D3DXCreateEffectCompilerFromFileA(SourcePath->data(), NULL, NULL, NULL, &Compiler, &Errors);
		if (FAILED(comp)) goto cleanup;
		if (Errors) {
			Logger::Log((char*)Errors->GetBufferPointer());
			Errors->Release();
			Errors = nullptr;
		}

		HRESULT compiled = Compiler->CompileEffect(NULL, &EffectBuffer, &Errors);
		if (FAILED(compiled)) goto cleanup;
		if (Errors) {
			Logger::Log((char*)Errors->GetBufferPointer());
			Errors->Release();
			Errors = nullptr;
		}

		if (EffectBuffer) {
			std::ofstream FileBinary(Path->data(), std::ios::out | std::ios::binary);
			FileBinary.write((char*)EffectBuffer->GetBufferPointer(), EffectBuffer->GetBufferSize());
			FileBinary.flush();
			FileBinary.close();
			Logger::Log("Effect compiled: %s", SourcePath->data());
		}
	}

	load = D3DXCreateEffectFromFileA(TheRenderManager->device, Path->data(), NULL, NULL, NULL, NULL, &Effect, &Errors);
	if (FAILED(load)) goto cleanup;

	if (Errors) Logger::Log((char*)Errors->GetBufferPointer()); // LAst can be cleaned in the cleanup section
	if (Effect) {
		this->Effect = Effect;
		CreateCT(ShaderSource, NULL); //Create the object which will associate a register index to a float pointer for constants updates;
		Logger::Log("Effect loaded: %s", Path->data());
	}

	success = true;
cleanup:
	if (EffectBuffer) EffectBuffer->Release();
	if (Compiler) Compiler->Release();

	if (ShaderSource) ShaderSource->Release();
	if (Errors) {
		Logger::Log((char*)Errors->GetBufferPointer());
		Errors->Release();
	}

	// set enabled status of effect based on success and setting
	Enabled = Effect != nullptr && TheSettingManager->GetMenuShaderEnabled(Name);

	timer.LogTime("EffectRecord::LoadSEffect");

	return success;
}


/**
* Loads an effect shader by name (The post process effects stored in the Effects folder)
* @param Name the name for the effect
* @returns an EffectRecord describing the effect shader.
*/
bool EffectRecord::IsLoaded() {
	return Effect != nullptr;
}


/**
Creates the Constant Table for the Effect Record.
*/
void EffectRecord::CreateCT(ID3DXBuffer* ShaderSource, ID3DXConstantTable* ConstantTable) {
	auto timer = TimeLogger();

	D3DXEFFECT_DESC ConstantTableDesc;
	D3DXPARAMETER_DESC ConstantDesc;
	D3DXHANDLE Handle;
	UINT ConstantCount = 1;
	UInt32 FloatIndex = 0;
	UInt32 TextureIndex = 0;

	Effect->GetDesc(&ConstantTableDesc);
	for (UINT c = 0; c < ConstantTableDesc.Parameters; c++) {
		Handle = Effect->GetParameter(NULL, c);
		Effect->GetParameterDesc(Handle, &ConstantDesc);
		if (memcmp(ConstantDesc.Name, "TESR_", 5)) continue;
		if ((ConstantDesc.Class == D3DXPC_VECTOR || ConstantDesc.Class == D3DXPC_MATRIX_ROWS)) FloatShaderValuesCount += 1;
		if (ConstantDesc.Class == D3DXPC_OBJECT && ConstantDesc.Type >= D3DXPT_SAMPLER && ConstantDesc.Type <= D3DXPT_SAMPLERCUBE) TextureShaderValuesCount += 1;
	}
	FloatShaderValues = new ShaderFloatValue[FloatShaderValuesCount];
	TextureShaderValues = new ShaderTextureValue[TextureShaderValuesCount];

	//Logger::Debug("CreateCT: Effect has %i constants", ConstantTableDesc.Parameters);

	for (UINT c = 0; c < ConstantTableDesc.Parameters; c++) {
		Handle = Effect->GetParameter(NULL, c);
		Effect->GetParameterDesc(Handle, &ConstantDesc);
		if (memcmp(ConstantDesc.Name, "TESR_", 5)) continue; // Only treat constants named with TESR prefix

		switch (ConstantDesc.Class) {
		case D3DXPC_VECTOR:
			FloatShaderValues[FloatIndex].Name = ConstantDesc.Name;
			FloatShaderValues[FloatIndex].Type = (D3DXPARAMETER_TYPE)D3DXPC_VECTOR;
			FloatShaderValues[FloatIndex].RegisterIndex = (UInt32)Handle;
			FloatShaderValues[FloatIndex].RegisterCount = ConstantDesc.Elements;
			FloatIndex++;
			break;
		case D3DXPC_MATRIX_ROWS:
			FloatShaderValues[FloatIndex].Name = ConstantDesc.Name;
			FloatShaderValues[FloatIndex].Type = (D3DXPARAMETER_TYPE)D3DXPC_MATRIX_ROWS;
			FloatShaderValues[FloatIndex].RegisterIndex = (UInt32)Handle;
			FloatShaderValues[FloatIndex].RegisterCount = ConstantDesc.Rows;
			FloatIndex++;
			break;
		case D3DXPC_OBJECT:
			if (ConstantDesc.Class == D3DXPC_OBJECT && ConstantDesc.Type >= D3DXPT_SAMPLER && ConstantDesc.Type <= D3DXPT_SAMPLERCUBE) {
				TextureShaderValues[TextureIndex].Name = ConstantDesc.Name;
				TextureShaderValues[TextureIndex].Type = TextureRecord::GetTextureType(ConstantDesc.Type);
				TextureShaderValues[TextureIndex].RegisterIndex = TextureIndex;
				TextureShaderValues[TextureIndex].RegisterCount = 1;
				TextureShaderValues[TextureIndex].GetSamplerStateString(ShaderSource, TextureIndex);
				TextureShaderValues[TextureIndex].GetTextureRecord();

				TextureIndex++;
			}
			break;
		}
	}

	timer.LogTime("EffectRecord::ConstantTableDesc");
}

/*
*Sets the Effect Shader constants table and texture registers.
*/
void EffectRecord::SetCT() {
	if (!Enabled || Effect == nullptr) return;

	ShaderTextureValue* Sampler;
	for (UInt32 c = 0; c < TextureShaderValuesCount; c++) {
		Sampler = &TextureShaderValues[c];
		if (!Sampler->Texture->Texture) {
			Sampler->Texture->BindTexture(Sampler->Name);

			if (Sampler->Texture->Texture) Logger::Log("%s : Texture %s Succesfully bound", Name, Sampler->Name);
			else Logger::Log("[ERROR] : Could not bind texture %s for effect %s", Sampler->Name, Name);
		}

		if (Sampler->Texture->Texture != nullptr) {
			TheRenderManager->device->SetTexture(Sampler->RegisterIndex, Sampler->Texture->Texture);
			for (int i = 1; i < SamplerStatesMax; i++) {
				TheRenderManager->SetSamplerState(Sampler->RegisterIndex, (D3DSAMPLERSTATETYPE)i, Sampler->Texture->SamplerStates[i]);
			}
		}
	}

	ShaderFloatValue* Constant;
	for (UInt32 c = 0; c < FloatShaderValuesCount; c++) {
		Constant = &FloatShaderValues[c];
		if (Constant->Value == nullptr)  Constant->GetValueFromConstantTable();
		if (Constant->Type == (D3DXPARAMETER_TYPE)D3DXPC_VECTOR) {
			if (Constant->RegisterCount > 1)
				Effect->SetVectorArray((D3DXHANDLE)Constant->RegisterIndex, Constant->Value, Constant->RegisterCount);
			else
				Effect->SetVector((D3DXHANDLE)Constant->RegisterIndex, Constant->Value);
		}
		else
			Effect->SetMatrix((D3DXHANDLE)Constant->RegisterIndex, (D3DXMATRIX*)Constant->Value);
	}
}

/*
 * Enable or Disable Effect, with reloading it if it's changed on disk
 */
bool EffectRecord::SwitchEffect() {
	bool change = true;
	if (!IsLoaded() || (!Enabled && ShouldCompileShader(Path->data(), SourcePath->data(), ShaderCompileType::RecompileChanged))) {
		Logger::Log("Effect %s is not loaded", Path->data());
		DisposeEffect();
		change = LoadEffect(true);
	}
	if (change) {
		Enabled = !Enabled;
	}
	else {
		char Message[256] = "Error: Couldn't enable effect ";
		strcat(Message, Path->data());

		InterfaceManager->ShowMessage(Message);
		Logger::Log(Message);
	}
	return Enabled;
}


/**
* Renders the given effect shader.
*/
void EffectRecord::Render(IDirect3DDevice9* Device, IDirect3DSurface9* RenderTarget, IDirect3DSurface9* RenderedSurface, UINT techniqueIndex, bool ClearRenderTarget, IDirect3DSurface9* SourceBuffer) {

	if (!Enabled || Effect == nullptr || !ShouldRender()) {
		renderTime = 0.0f;
		return; // skip rendering of disabled effects
	}

	auto timer = TimeLogger();
	if (SourceBuffer) Device->StretchRect(RenderTarget, NULL, SourceBuffer, NULL, D3DTEXF_NONE);

	try {
		D3DXHANDLE technique = Effect->GetTechnique(techniqueIndex);
		Effect->SetTechnique(technique);
		SetCT(); // update the constant table
		UINT Passes;
		Effect->Begin(&Passes, NULL);
		for (UINT p = 0; p < Passes; p++) {
			if (ClearRenderTarget) Device->Clear(0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(1, 0, 0, 0), 1.0f, 0L);
			Effect->BeginPass(p);
			Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
			Effect->EndPass();
			Device->StretchRect(RenderTarget, NULL, RenderedSurface, NULL, D3DTEXF_NONE);
		}
		Effect->End();
	}
	catch (const std::exception& e) {
		Logger::Log("Error during rendering of effect %s: %s", Name, e.what());
	}

	std::string name = "EffectRecord::Render " + *Path;
	renderTime = timer.LogTime(name.c_str());
}
