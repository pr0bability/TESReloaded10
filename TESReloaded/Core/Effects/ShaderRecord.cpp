ShaderProgram::ShaderProgram() {

	FloatShaderValues = NULL;
	TextureShaderValues = NULL;
	FloatShaderValuesCount = 0;
	TextureShaderValuesCount = 0;

}

ShaderProgram::~ShaderProgram() {

	delete[] FloatShaderValues;
	delete[] TextureShaderValues;
}


ShaderRecord::ShaderRecord() {

	HasRenderedBuffer = false;
	HasDepthBuffer = false;

}
ShaderRecord::~ShaderRecord() {}


/**
* @param fileBin the name of the compiled shader file
* @param fileHlsl the name of the hlsl source file for this shader
* @param CompileStatus an integer for the status of the compilation. If set to 2, will compare file dates to return status.
* @returns wether the shader should be compiled, from a given binary shader and corresponding hlsl.
*/
bool ShaderProgram::ShouldCompileShader(const char* fileBin, const char* fileHlsl, ShaderCompileType CompileStatus) {
	if (CompileStatus == ShaderCompileType::AlwaysOn) return  true;
	if (CompileStatus == ShaderCompileType::AlwaysOff) return  false;
	if (CompileStatus == ShaderCompileType::RecompileInMenu) return TheShaderManager->IsMenuSwitch ? true : false;

	if (CompileStatus == ShaderCompileType::RecompileChanged) {
		WIN32_FILE_ATTRIBUTE_DATA attributesBin = { 0 };
		WIN32_FILE_ATTRIBUTE_DATA attributesSource = { 0 };
		BOOL hr = GetFileAttributesExA(fileBin, GetFileExInfoStandard, &attributesBin); // from winbase.h
		if (hr == FALSE) {
			// 			Logger::Log("File %s not present, compile", fileHlsl);
			return true; //File not present compile
		}
		else {
			BOOL hr = GetFileAttributesExA(fileHlsl, GetFileExInfoStandard, &attributesSource); // from winbase.h
			if (hr == FALSE) {
				//				Logger::Log("[ERROR] Can't open source %s", fileHlsl);
				return true; //BOH
			}

			ULARGE_INTEGER timeBin, timeSource;
			timeBin.LowPart = attributesBin.ftLastWriteTime.dwLowDateTime;
			timeBin.HighPart = attributesBin.ftLastWriteTime.dwHighDateTime;
			timeSource.LowPart = attributesSource.ftLastWriteTime.dwLowDateTime;
			timeSource.HighPart = attributesSource.ftLastWriteTime.dwHighDateTime;

			if (timeBin.QuadPart < timeSource.QuadPart) {
				Logger::Log("Binary older then source, compile %s", fileHlsl);
				return true;
			}
		}
	}
	return false;
}

/*
Loads the shader by name from a given subfolder (optionally). Shader will be compiled if needed.
@returns the ShaderRecord for this shader.
*/
ShaderRecord* ShaderRecord::LoadShader(const char* Name, const char* SubPath) {
	auto timer = TimeLogger();

	ShaderRecord* ShaderProg = NULL;
	ID3DXBuffer* ShaderSource = NULL;
	ID3DXBuffer* Shader = NULL;
	ID3DXBuffer* Errors = NULL;
	ID3DXConstantTable* ConstantTable = NULL;
	void* Function = NULL;
	UInt32 SourceSize = 0;
	char ShaderProfile[7];
	char FileName[MAX_PATH];
	char FileNameBinary[MAX_PATH];
	
	strcpy(FileName, ShadersPath);
	if (SubPath) strcat(FileName, SubPath);
	strcat(FileName, Name);
	strcpy(FileNameBinary, FileName);
	strcat(FileName, ".hlsl");

	HRESULT prepass = D3DXPreprocessShaderFromFileA(FileName, NULL, NULL, &ShaderSource, &Errors);
	bool Compile = ShouldCompileShader(FileNameBinary, FileName, (ShaderCompileType)TheSettingManager->SettingsMain.Develop.CompileShaders);
	if (prepass == D3D_OK) {
		if (strstr(Name, ".vso"))
			strcpy(ShaderProfile, "vs_3_0");
		else if (strstr(Name, ".pso"))
			strcpy(ShaderProfile, "ps_3_0");
		if (Compile) {
			D3DXCompileShaderFromFileA(FileName, NULL, NULL, "main", ShaderProfile, NULL, &Shader, &Errors, &ConstantTable);
			if (Errors) Logger::Log((char*)Errors->GetBufferPointer());
			if (Shader) {
				Function = Shader->GetBufferPointer();
				std::ofstream FileBinary(FileNameBinary, std::ios::out | std::ios::binary);
				FileBinary.write((const char*)Function, Shader->GetBufferSize());
				FileBinary.flush();
				FileBinary.close();
				Logger::Log("Shader compiled: %s", FileName);
			}
		}
		else {
			std::ifstream FileBinary(FileNameBinary, std::ios::in | std::ios::binary | std::ios::ate);
			if (FileBinary.is_open()) {
				std::streamoff Size = FileBinary.tellg();
				D3DXCreateBuffer(Size, &Shader);
				FileBinary.seekg(0, std::ios::beg);
				Function = Shader->GetBufferPointer();
				FileBinary.read((char*)Function, Size);
				FileBinary.close();
				D3DXGetShaderConstantTable((const DWORD*)Function, &ConstantTable);
			}
			else {
				Logger::Log("ERROR: Shader %s not found. Try to enable the CompileShader option to recompile the shaders.", FileNameBinary);
			}
		}

		timer.LogTime("ShaderRecord::LoadShader");

		if (Shader) {
			if (ShaderProfile[0] == 'v') {
				ShaderProg = new ShaderRecordVertex(Name);
				TheRenderManager->device->CreateVertexShader((const DWORD*)Function, &((ShaderRecordVertex*)ShaderProg)->ShaderHandle);
			}
			else {
				ShaderProg = new ShaderRecordPixel(Name);
				TheRenderManager->device->CreatePixelShader((const DWORD*)Function, &((ShaderRecordPixel*)ShaderProg)->ShaderHandle);
			}
			ShaderProg->CreateCT(ShaderSource, ConstantTable);
			Logger::Log("Shader loaded: %s", FileNameBinary);
		}
	}
	else {
		if (Errors) Logger::Log((char*)Errors->GetBufferPointer());
	}

	if (ShaderSource) ShaderSource->Release();
	if (Shader) Shader->Release();
	if (Errors) Errors->Release();
	return ShaderProg;
}

/**
* Creates the constants table for the Shader.
* @param ShaderSource
* @param ConstantTable
*/
void ShaderRecord::CreateCT(ID3DXBuffer* ShaderSource, ID3DXConstantTable* ConstantTable) {

	D3DXCONSTANTTABLE_DESC ConstantTableDesc;
	D3DXCONSTANT_DESC ConstantDesc;
	D3DXHANDLE Handle;
	UINT ConstantCount = 1;
	UInt32 FloatIndex = 0;
	UInt32 TextureIndex = 0;

	ConstantTable->GetDesc(&ConstantTableDesc);
	for (UINT c = 0; c < ConstantTableDesc.Constants; c++) {
		Handle = ConstantTable->GetConstant(NULL, c);
		ConstantTable->GetConstantDesc(Handle, &ConstantDesc, &ConstantCount);
		if (ConstantDesc.RegisterSet == D3DXRS_FLOAT4 && !memcmp(ConstantDesc.Name, "TESR_", 5)) FloatShaderValuesCount += 1;
		if (ConstantDesc.RegisterSet == D3DXRS_SAMPLER && !memcmp(ConstantDesc.Name, "TESR_", 5)) TextureShaderValuesCount += 1;
	}

	auto timer = TimeLogger();
	if (FloatShaderValuesCount) FloatShaderValues = new ShaderFloatValue[FloatShaderValuesCount];
	if (TextureShaderValuesCount) TextureShaderValues = new ShaderTextureValue[TextureShaderValuesCount];

	// Should be better but still crashes with NVHR -- TODO: Check how to fix it
	//if (FloatShaderValuesCount) FloatShaderValues = (ShaderValue*)Pointers::Functions::FormMemoryAlloc(FloatShaderValuesCount * sizeof(ShaderValue));
	//if (TextureShaderValuesCount) TextureShaderValues = (ShaderValue*)Pointers::Functions::FormMemoryAlloc(TextureShaderValuesCount * sizeof(ShaderValue));

	timer.LogTime("ShaderRecord::createCT Malloc");

	//Logger::Log("CreateCT: Shader has %i constants", ConstantTableDesc.Constants);

	for (UINT c = 0; c < ConstantTableDesc.Constants; c++) {
		Handle = ConstantTable->GetConstant(NULL, c);
		ConstantTable->GetConstantDesc(Handle, &ConstantDesc, &ConstantCount);
		if (!memcmp(ConstantDesc.Name, "TESR_", 5)) {
			switch (ConstantDesc.RegisterSet) {
			case D3DXRS_FLOAT4:
				FloatShaderValues[FloatIndex].Name = ConstantDesc.Name;
				FloatShaderValues[FloatIndex].Type = ConstantDesc.Type;
				FloatShaderValues[FloatIndex].RegisterIndex = ConstantDesc.RegisterIndex;
				FloatShaderValues[FloatIndex].RegisterCount = ConstantDesc.RegisterCount;
				FloatIndex++;
				break;
			case D3DXRS_SAMPLER:
				TextureShaderValues[TextureIndex].Name = ConstantDesc.Name;
				TextureShaderValues[TextureIndex].Type = TextureRecord::GetTextureType(ConstantDesc.Type, ConstantDesc.Name, &HasRenderedBuffer, &HasDepthBuffer);
				TextureShaderValues[TextureIndex].RegisterIndex = ConstantDesc.RegisterIndex;
				TextureShaderValues[TextureIndex].RegisterCount = 1;
				TextureShaderValues[TextureIndex].GetSamplerStateString(ShaderSource, ConstantDesc.RegisterIndex);
				TextureIndex++;
				break;
			default:
				Logger::Log("Found unsupported constant type in shader constant table: %s : %s", ConstantDesc.Name, ConstantDesc.RegisterSet);
				break;
			}
		}
	}

	timer.LogTime("ShaderRecord::createCT Done");
}


// extract info about texture from the shader source code
void ShaderTextureValue::GetSamplerStateString(ID3DXBuffer* ShaderSource, UINT32 Index) {
	std::string Source = std::string((const char*)ShaderSource->GetBufferPointer());

	size_t SamplerPos = Source.find(("register ( s" + std::to_string(Index) + " )"));
	if (SamplerPos == std::string::npos) {
		Logger::Log("[ERROR] %s  cannot be binded", Name);
		return;
	}
	if (Type >= TextureRecord::TextureRecordType::PlanarBuffer && Type <= TextureRecord::TextureRecordType::CubeBuffer) {
		//Only these samplers are bindable to an arbitrary texture
		size_t StartTexture = Source.find("<", SamplerPos + 1);
		size_t EndTexture = Source.find(">", SamplerPos + 1);
		if (StartTexture == std::string::npos || EndTexture == std::string::npos) {
			Logger::Log("[ERROR] %s cannot be binded", Name);
			return;
		}
		std::string TextureString = Source.substr(StartTexture + 1, EndTexture - StartTexture - 1);
		GetTexturePath(TextureString);
		//Logger::Log("texture path: %s", TexturePath);
	}
	size_t StartStatePos = Source.find("{", SamplerPos);
	size_t EndStatePos = Source.find("}", SamplerPos);
	if (EndStatePos == std::string::npos || StartStatePos == std::string::npos) {
		Logger::Log("[ERROR] %s cannot be binded", Name);
		return;
	}
	SamplerString = Source.substr(StartStatePos + 1, EndStatePos - StartStatePos - 1);
}


void ShaderTextureValue::GetTexturePath(std::string& resourceSubstring) {
	std::string PathS;
	if (resourceSubstring.find("ResourceName") != std::string::npos) {
		size_t StartPath = resourceSubstring.find("\"");
		size_t EndPath = resourceSubstring.rfind("\"");
		PathS = resourceSubstring.substr(StartPath + 1, EndPath - 1 - StartPath);
		PathS.insert(0, "Data\\Textures\\");  
	}
	else {
		Logger::Log("[ERROR] Cannot parse bindable texture");
	}
	TexturePath = PathS;
}


/*
* Associates a found shader constant name to a D3DXVECTOR4 pointer from the ConstantsTable.
*/
void ShaderFloatValue::GetValueFromConstantTable() {
	for (auto const& imap : TheShaderManager->ConstantsTable) {
		if (!strcmp(imap.first, Name)) {
			Value = TheShaderManager->ConstantsTable.at(imap.first);
			return;
		}
	}

	Logger::Log("Custom constant found: %s", Name);
	D3DXVECTOR4 v; v.x = v.y = v.z = v.w = 0.0f;
	TheShaderManager->CustomConst[Name] = v;
	Value = &TheShaderManager->CustomConst[Name];
}


/*
* Sets the Constant Table for the shader
*/
void ShaderRecord::SetCT() {

	if (HasRenderedBuffer) TheRenderManager->device->StretchRect(TheRenderManager->currentRTGroup->RenderTargets[0]->data->Surface, NULL, TheTextureManager->RenderedSurface, NULL, D3DTEXF_NONE);
	if (HasDepthBuffer) TheRenderManager->ResolveDepthBuffer();
	
	// binds textures
	ShaderTextureValue* Sampler;
	for (UInt32 c = 0; c < TextureShaderValuesCount; c++) {
		Sampler = &TextureShaderValues[c];
		if (!Sampler->Texture) Sampler->Texture = TheTextureManager->LoadTexture(Sampler);
	
		if (Sampler->Texture->Texture != nullptr) {
			TheRenderManager->renderState->SetTexture(Sampler->RegisterIndex, Sampler->Texture->Texture);
			for (int i = 1; i < SamplerStatesMax; i++) {
				TheRenderManager->SetSamplerState(Sampler->RegisterIndex, (D3DSAMPLERSTATETYPE)i, Sampler->Texture->SamplerStates[i]);
			}
		}
	}

	// update constants
	ShaderFloatValue* Constant;
	for (UInt32 c = 0; c < FloatShaderValuesCount; c++) {
		Constant = &FloatShaderValues[c];
		if (Constant->Value == nullptr) Constant->GetValueFromConstantTable();

		SetShaderConstantF(Constant->RegisterIndex, Constant->Value, Constant->RegisterCount); // TODO: for matrices, rows and columns are inverted because of the way this works.
	}
}


ShaderRecordVertex::ShaderRecordVertex(const char* shaderName) {

	Name = shaderName;
	ShaderHandle = NULL;

}

ShaderRecordVertex::~ShaderRecordVertex() {

	ShaderHandle->Release();

}

ShaderRecordPixel::ShaderRecordPixel(const char* shaderName) {
	Name = shaderName;
	ShaderHandle = NULL;

}

ShaderRecordPixel::~ShaderRecordPixel() {

	ShaderHandle->Release();

}


void ShaderRecordVertex::SetShaderConstantF(UInt32 RegisterIndex, D3DXVECTOR4* Value, UInt32 RegisterCount) {

	TheRenderManager->device->SetVertexShaderConstantF(RegisterIndex, (const float*)Value, RegisterCount);

}

void ShaderRecordPixel::SetShaderConstantF(UInt32 RegisterIndex, D3DXVECTOR4* Value, UInt32 RegisterCount) {

	TheRenderManager->device->SetPixelShaderConstantF(RegisterIndex, (const float*)Value, RegisterCount);

}


/*
* Class that wraps an effect shader, in order to load it/render it/set constants.
*/
EffectRecord::EffectRecord(const char* effectName) {

	Name = effectName;
	Path = new std::string(effectName);

	char FileName[MAX_PATH];
	strcpy(FileName, EffectsPath);
	strcat(FileName, Name);
	strcat(FileName, ".fx");
	strcat(FileName, ".hlsl");
	SourcePath = new std::string(FileName);

	Effect = NULL;
	Enabled = false;
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
		if ((ConstantDesc.Class == D3DXPC_VECTOR || ConstantDesc.Class == D3DXPC_MATRIX_ROWS) && !memcmp(ConstantDesc.Name, "TESR_", 5)) FloatShaderValuesCount += 1;
		if (ConstantDesc.Class == D3DXPC_OBJECT && ConstantDesc.Type >= D3DXPT_SAMPLER && ConstantDesc.Type <= D3DXPT_SAMPLERCUBE && !memcmp(ConstantDesc.Name, "TESR_", 5)) TextureShaderValuesCount += 1;
	}
	FloatShaderValues = new ShaderFloatValue[FloatShaderValuesCount];
	TextureShaderValues = new ShaderTextureValue[TextureShaderValuesCount];

	Logger::Debug("CreateCT: Effect has %i constants", ConstantTableDesc.Parameters);

	for (UINT c = 0; c < ConstantTableDesc.Parameters; c++) {
		Handle = Effect->GetParameter(NULL, c);
		Effect->GetParameterDesc(Handle, &ConstantDesc);
		if (!memcmp(ConstantDesc.Name, "TESR_", 5)) {
			switch (ConstantDesc.Class) {
			case D3DXPC_VECTOR:
			case D3DXPC_MATRIX_ROWS:
				FloatShaderValues[FloatIndex].Name = ConstantDesc.Name;
				FloatShaderValues[FloatIndex].Type = ConstantDesc.Type;
				FloatShaderValues[FloatIndex].RegisterIndex = (UInt32)Handle;
				FloatShaderValues[FloatIndex].RegisterCount = ConstantDesc.Rows;
				FloatIndex++;
				break;
			case D3DXPC_OBJECT:
				if (ConstantDesc.Class == D3DXPC_OBJECT && ConstantDesc.Type >= D3DXPT_SAMPLER && ConstantDesc.Type <= D3DXPT_SAMPLERCUBE) {
					TextureShaderValues[TextureIndex].Name = ConstantDesc.Name;
					TextureShaderValues[TextureIndex].Type = TextureRecord::GetTextureType(ConstantDesc.Type, ConstantDesc.Name, NULL, NULL);
					TextureShaderValues[TextureIndex].RegisterIndex = TextureIndex;
					TextureShaderValues[TextureIndex].RegisterCount = 1;
					TextureShaderValues[TextureIndex].GetSamplerStateString(ShaderSource, TextureIndex);
					TextureIndex++;
				}
				break;
			}
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

		if (!Sampler->Texture) Sampler->Texture = TheTextureManager->LoadTexture(Sampler);
		try {
			if (Sampler->Texture->Texture != nullptr) {
				TheRenderManager->device->SetTexture(Sampler->RegisterIndex, Sampler->Texture->Texture);
				for (int i = 1; i < SamplerStatesMax; i++) {
					TheRenderManager->SetSamplerState(Sampler->RegisterIndex, (D3DSAMPLERSTATETYPE)i, Sampler->Texture->SamplerStates[i]);
				}
			}
		}
		catch (const std::exception& e) {
			Logger::Log("[ERROR] Couldnt' bind texture %i to effect %s : %s", c, Path, e.what());
		}
	}

	ShaderFloatValue* Constant;
	for (UInt32 c = 0; c < FloatShaderValuesCount; c++) {
		Constant = &FloatShaderValues[c];
		if (Constant->Value == nullptr)  Constant->GetValueFromConstantTable();
		if (Constant->RegisterCount == 1)
			Effect->SetVector((D3DXHANDLE)Constant->RegisterIndex, Constant->Value);
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

	if (!Enabled || Effect == nullptr || !ShouldRender()) return; // skip rendering of disabled effects

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

	char name[255] = "EffectRecord::Render ";
	strcat(name, Path->c_str());
	timer.LogTime(name);
}


bool ShaderCollection::SwitchShader() {
	Enabled = !TheSettingManager->GetMenuShaderEnabled(Name);
	TheSettingManager->SetMenuShaderEnabled(Name, Enabled);

	// TODO: handle unloading/reloading of shaders here

	return Enabled;
}