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
		if (memcmp(ConstantDesc.Name, "TESR_", 5)) continue; // Only treat constants named with TESR prefix
		if (ConstantDesc.RegisterSet == D3DXRS_FLOAT4) FloatShaderValuesCount += 1;
		if (ConstantDesc.RegisterSet == D3DXRS_SAMPLER) TextureShaderValuesCount += 1;
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
		if (memcmp(ConstantDesc.Name, "TESR_", 5)) continue; // Only treat constants named with TESR prefix

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
			TextureShaderValues[TextureIndex].Type = TextureRecord::GetTextureType(ConstantDesc.Type);
			TextureShaderValues[TextureIndex].RegisterIndex = ConstantDesc.RegisterIndex;
			TextureShaderValues[TextureIndex].RegisterCount = 1;
			TextureShaderValues[TextureIndex].GetSamplerStateString(ShaderSource, ConstantDesc.RegisterIndex);
			TextureShaderValues[TextureIndex].GetTextureRecord();

			// mark this shader as needing to render depth/a buffer of the scene before the object can be rendered
			if (!memcmp(ConstantDesc.Name, "TESR_DepthBuffer", 17)) HasDepthBuffer = true;
			if (!memcmp(ConstantDesc.Name, "TESR_RenderedBuffer", 20)) HasRenderedBuffer = true;
	
			TextureIndex++;
			break;
		default:
			Logger::Log("Found unsupported constant type in shader constant table: %s : %s", ConstantDesc.Name, ConstantDesc.RegisterSet);
			break;
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

	size_t SamplerEnd = Source.find("\n", SamplerPos + 1);
	std::string SamplerLine = Source.substr(SamplerPos, SamplerEnd - SamplerPos);

	//Only these samplers are bindable to an arbitrary texture
	size_t StartTexture = SamplerLine.find("<");
	size_t EndTexture = SamplerLine.rfind(">");
	if (StartTexture != std::string::npos && EndTexture != std::string::npos) {
		std::string TextureString = SamplerLine.substr(StartTexture + 1, EndTexture - StartTexture - 1);

		// find texture path
		if (TextureString.find("ResourceName") != std::string::npos) {
			size_t StartPath = TextureString.find("\"");
			size_t EndPath = TextureString.rfind("\"");
			std::string PathS;
			PathS = TextureString.substr(StartPath + 1, EndPath - 1 - StartPath);
			PathS.insert(0, "Data\\Textures\\");
			TexturePath = PathS;
					}
	}

	size_t StartStatePos = SamplerLine.find("{");
	size_t EndStatePos = SamplerLine.rfind("}");
	if (EndStatePos == std::string::npos || StartStatePos == std::string::npos) {
		Logger::Log("[ERROR] %s cannot be binded", Name);
		return;
	}
	SamplerString = SamplerLine.substr(StartStatePos + 1, EndStatePos - StartStatePos - 1);
}


/*
* Gets a texture record with information from the Sampler and source
*/
void ShaderTextureValue::GetTextureRecord() {
	auto timer = TimeLogger();

	//Logger::Log("Loading texture %s (type:%i) (path: %s)", Name, Type, TexturePath);

	if (!Type) {
		Logger::Log("[ERROR] Sampler %s doesn't have a valid type", Name);
		return;
	}

	Texture = new TextureRecord();

	// preload file textures, game textures will get bind during constant table setting
	if (TexturePath != "") Texture->Texture = TheTextureManager->GetFileTexture(TexturePath, Type);

	Texture->GetSamplerStates(trim(SamplerString));

	timer.LogTime("ShaderTextureValue::GetTextureRecord");
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
		if (Sampler->Texture->Texture == nullptr) 
			Sampler->Texture->BindTexture(Sampler->Name);

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


/*
* Hook - Function to replace shader handles during pass initialisation. 
*/
void NiD3DVertexShaderEx::SetupShader(IDirect3DVertexShader9* CurrentVertexHandle) {

	if (!Enabled || !TheSettingManager->SettingsMain.Main.RenderEffects) {
		ShaderHandle = ShaderHandleBackup;
		return;
	}

	if (ShaderProgE && Player->GetWorldSpace()) {
		ShaderHandle = ShaderProgE->ShaderHandle;
		if (CurrentVertexHandle != ShaderHandle) ShaderProgE->SetCT();
	}
	else if (ShaderProgI && !Player->GetWorldSpace()) {
		ShaderHandle = ShaderProgI->ShaderHandle;
		if (CurrentVertexHandle != ShaderHandle) ShaderProgI->SetCT();
	}
	else if (ShaderProg) {
		ShaderHandle = ShaderProg->ShaderHandle;
		if (CurrentVertexHandle != ShaderHandle) ShaderProg->SetCT();
	}
	else {
		ShaderHandle = ShaderHandleBackup;
	}

}

void NiD3DVertexShaderEx::DisposeShader() {

	if (ShaderProgE) delete ShaderProgE; ShaderProgE = NULL;
	if (ShaderProgI) delete ShaderProgI; ShaderProgI = NULL;
	if (ShaderProg) delete ShaderProg; ShaderProg = NULL;

}


/*
* Hook - Function to replace shader handles during pass initialisation.
*/
void NiD3DPixelShaderEx::SetupShader(IDirect3DPixelShader9* CurrentPixelHandle) {

	if (!Enabled || !TheSettingManager->SettingsMain.Main.RenderEffects) {
		ShaderHandle = ShaderHandleBackup;
		return;
	}

	if (ShaderProgE && Player->GetWorldSpace()) {
		ShaderHandle = ShaderProgE->ShaderHandle;
		if (CurrentPixelHandle != ShaderHandle) ShaderProgE->SetCT();
	}
	else if (ShaderProgI && !Player->GetWorldSpace()) {
		ShaderHandle = ShaderProgI->ShaderHandle;
		if (CurrentPixelHandle != ShaderHandle) ShaderProgI->SetCT();
	}
	else if (ShaderProg) {
		ShaderHandle = ShaderProg->ShaderHandle;
		if (CurrentPixelHandle != ShaderHandle) ShaderProg->SetCT();
	}
	else {
		ShaderHandle = ShaderHandleBackup;
	}
}


void NiD3DPixelShaderEx::DisposeShader() {

	if (ShaderProgE) delete ShaderProgE; ShaderProgE = NULL;
	if (ShaderProgI) delete ShaderProgI; ShaderProgI = NULL;
	if (ShaderProg) delete ShaderProg; ShaderProg = NULL;

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