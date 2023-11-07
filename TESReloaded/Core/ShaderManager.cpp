#define RESZ_CODE 0x7FA05000

/*
* Constructor of Animator class. Starts an animator for a given value.
*/
Animator::Animator() {
	time = TimeGlobals::Get();
	startValue = 0;
	endValue = 0;
	startTime = 0;
	endTime = 0;
	running = false;
	switched = false;
};


Animator::~Animator() {
};


void Animator::Initialize(float value) {
	startValue = value;
	endValue = value;
}

/*
* Starts the animator by setting a target value and a duration to reach it.
*/
void Animator::Start(float duration, float finalValue) {
	float currenttime = time->GameDaysPassed->data * 24;

	startTime = currenttime;
	endTime = currenttime + duration;
	running = true;

	startValue = GetValue();
	endValue = finalValue;
}


/*
* Gets the value for the animated value at the current time.
*/
float Animator::GetValue() {
	float currenttime = time->GameDaysPassed->data * 24;
	if (!running) return startValue;
	
	if (currenttime > endTime) {
		running = false;
		startValue = endValue;
		return endValue;
	}

	running = true;
	currenttime = ShaderManager::invLerp(startTime, endTime, currenttime);
	return ShaderManager::lerp(startValue, endValue, currenttime);
};


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


/* 
Declares the constants that can be accessed from the shader code from the Constant Table, and gives them a readable name.
*/
void ShaderProgram::SetConstantTableValue(LPCSTR Name, UInt32 Index) {

	//Logger::Log("Constants contains name %s? %i", Name, TheShaderManager->ConstantsTable.count(Name));
	//Logger::Log("Constants map size? %i", TheShaderManager->ConstantsTable.size());

	for (auto const& imap : TheShaderManager->ConstantsTable) {
		if (!strcmp(imap.first, Name)) {
			FloatShaderValues[Index].Value = TheShaderManager->ConstantsTable.at(imap.first);
			return;
		}
	}

	Logger::Log("Custom constant found: %s", Name);
	D3DXVECTOR4 v; v.x = v.y = v.z = v.w = 0.0f;
	TheShaderManager->CustomConst[Name] = v;
	FloatShaderValues[Index].Value = &TheShaderManager->CustomConst[Name];
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
bool ShaderProgram::ShouldCompileShader(const char* fileBin, const char* fileHlsl, ShaderCompileType CompileStatus){
	if(CompileStatus == ShaderCompileType::AlwaysOn) return  true;
	if(CompileStatus == ShaderCompileType::AlwaysOff) return  false;
	if(CompileStatus == ShaderCompileType::RecompileInMenu) return TheShaderManager->IsMenuSwitch ? true : false;

	if(CompileStatus == ShaderCompileType::RecompileChanged) {
		WIN32_FILE_ATTRIBUTE_DATA attributesBin = {0};
		WIN32_FILE_ATTRIBUTE_DATA attributesSource = {0};
		BOOL hr = GetFileAttributesExA(fileBin, GetFileExInfoStandard, &attributesBin); // from winbase.h
		if(hr == FALSE) {
// 			Logger::Log("File %s not present, compile", fileHlsl);
			return true; //File not present compile
		}
		else{
			BOOL hr = GetFileAttributesExA(fileHlsl, GetFileExInfoStandard, &attributesSource); // from winbase.h
			if(hr == FALSE) {
//				Logger::Log("[ERROR] Can't open source %s", fileHlsl);
				return true; //BOH
			}

			ULARGE_INTEGER timeBin, timeSource;
			timeBin.LowPart = attributesBin.ftLastWriteTime.dwLowDateTime;
			timeBin.HighPart = attributesBin.ftLastWriteTime.dwHighDateTime;
			timeSource.LowPart = attributesSource.ftLastWriteTime.dwLowDateTime;
			timeSource.HighPart = attributesSource.ftLastWriteTime.dwHighDateTime;

			if(timeBin.QuadPart < timeSource.QuadPart){
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
	if (!memcmp(Name, "WATER", 5)) {
		if (!TheSettingManager->GetMenuShaderEnabled("Water")) return NULL;
	}
	else if (!memcmp(Name, "GRASS", 5)) {
		if (!TheSettingManager->GetMenuShaderEnabled("Grass")) return NULL;
	}
	else if (!memcmp(Name, "HDR", 3) || !memcmp(Name, "ISHDR", 5)) {
		// load tonemapping shaders, with different names between New vegas and Oblivion
		if (!TheSettingManager->GetMenuShaderEnabled("HDR")) return NULL;
	}
	else if (!memcmp(Name, "PAR", 3)) {
		if (!TheSettingManager->GetMenuShaderEnabled("POM")) return NULL;
	}
	else if (!memcmp(Name, "SKIN", 4)) {
		if (!TheSettingManager->GetMenuShaderEnabled("Skin")) return NULL;
	}
	else if (!memcmp(Name, "SKY", 3)) {
		if (!TheSettingManager->GetMenuShaderEnabled("Sky")) return NULL;
	}
	else if (strstr(TerrainShaders, Name)) {
		if (!TheSettingManager->GetMenuShaderEnabled("Terrain")) return NULL;
	}
	else if (strstr(BloodShaders, Name)) {
		if (!TheSettingManager->GetMenuShaderEnabled("Blood")) return NULL;
	}
	else if (!memcmp(Name, "NIGHTEYE", 8)) {
		if (!TheSettingManager->GetMenuShaderEnabled("NightEye")) return NULL;
	}
	else if (!memcmp(Name, "Shadow", 6)) {
		strcat(FileName, "Shadows\\");
	}
	else if (!memcmp(Name, "Occlusion", 9)) {
		strcat(FileName, "Occlusion\\");
	}
	else if (!memcmp(Name, "Bink", 4)) {
		strcat(FileName, "Bink\\");
	}
	else if(!TheSettingManager->SettingsMain.Shaders.Extra) return NULL;
    
	if (SubPath) strcat(FileName, SubPath);
	strcat(FileName, Name);
	strcpy(FileNameBinary, FileName);
	strcat(FileName, ".hlsl");

    HRESULT prepass = D3DXPreprocessShaderFromFileA(FileName, NULL, NULL, &ShaderSource , &Errors);
	bool Compile = ShouldCompileShader(FileNameBinary, FileName, (ShaderCompileType) TheSettingManager->SettingsMain.Develop.CompileShaders);
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
	FloatShaderValues = new ShaderValue [FloatShaderValuesCount];
	TextureShaderValues = new ShaderValue [TextureShaderValuesCount];
	
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
					SetConstantTableValue(ConstantDesc.Name, FloatIndex);
					FloatShaderValues[FloatIndex].RegisterIndex = ConstantDesc.RegisterIndex;
					FloatShaderValues[FloatIndex].RegisterCount = ConstantDesc.RegisterCount;
					FloatIndex++;
 					break;
				case D3DXRS_SAMPLER:
					TextureShaderValues[TextureIndex].Texture = TheTextureManager->LoadTexture(ShaderSource, ConstantDesc.Type, ConstantDesc.Name, ConstantDesc.RegisterIndex, &HasRenderedBuffer, &HasDepthBuffer);
					TextureShaderValues[TextureIndex].RegisterIndex = ConstantDesc.RegisterIndex;
					TextureShaderValues[TextureIndex].RegisterCount = 1;
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

/* 
* Sets the Constant Table for the shader
*/
void ShaderRecord::SetCT() {
	ShaderValue* Value;

	if (HasRenderedBuffer) TheRenderManager->device->StretchRect(TheRenderManager->currentRTGroup->RenderTargets[0]->data->Surface, NULL, TheTextureManager->RenderedSurface, NULL, D3DTEXF_NONE);
	if (HasDepthBuffer) TheRenderManager->ResolveDepthBuffer();
	// binds textures
	for (UInt32 c = 0; c < TextureShaderValuesCount; c++) {
		Value = &TextureShaderValues[c];
		if (Value->Texture->Texture) TheRenderManager->renderState->SetTexture(Value->RegisterIndex, Value->Texture->Texture);
		for (int i = 1; i < SamplerStatesMax; i++) {
			TheRenderManager->SetSamplerState(Value->RegisterIndex, (D3DSAMPLERSTATETYPE)i, Value->Texture->SamplerStates[i]);
		}
	}
	// update constants
	for (UInt32 c = 0; c < FloatShaderValuesCount; c++) {
		Value = &FloatShaderValues[c];
		SetShaderConstantF(Value->RegisterIndex, Value->Value, Value->RegisterCount); // TODO: for matrices, rows and columns are inverted because of the way this works.
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
	strcpy(FileName, effectName);
	strcat(FileName, ".hlsl");
	SourcePath = new std::string(FileName);

	Enabled = false;
	Effect = NULL;

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
void EffectRecord::DisposeEffect(){
	if (Effect) Effect->Release();
	Effect = nullptr;
	delete[] FloatShaderValues;
	delete[] TextureShaderValues;
	Enabled = false;
}

/*
 * Compile and Load the Effect shader
 */
bool EffectRecord::LoadEffect(bool alwaysCompile){
	auto timer = TimeLogger();

	ID3DXBuffer* ShaderSource = NULL;
	ID3DXBuffer* Errors = NULL;
	ID3DXEffect* Effect = NULL;
	bool success = false;
    HRESULT prepass = D3DXPreprocessShaderFromFileA(SourcePath->data(), NULL, NULL, &ShaderSource , &Errors);
	ID3DXEffectCompiler* Compiler = NULL;
	ID3DXBuffer* EffectBuffer = NULL;
	HRESULT load = NULL;

	if(alwaysCompile || ShouldCompileShader(Path->data(), SourcePath->data(), (ShaderCompileType)TheSettingManager->SettingsMain.Develop.CompileEffects) ){
		HRESULT comp  = D3DXCreateEffectCompilerFromFileA(SourcePath->data(), NULL, NULL, NULL, &Compiler, &Errors);
		if(FAILED(comp)) goto cleanup;
		if (Errors){
			Logger::Log((char*)Errors->GetBufferPointer());
			Errors->Release();
			Errors = nullptr;
		}

		HRESULT compiled = Compiler->CompileEffect(NULL, &EffectBuffer, &Errors);
		if(FAILED(compiled)) goto cleanup;
		if (Errors){
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
	if(FAILED(load)) goto cleanup;

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
	if (Errors){
		Logger::Log((char*)Errors->GetBufferPointer());
		Errors->Release();
	}

	timer.LogTime("EffectRecord::LoadSEffect");

	return success;
}

/**
* Loads an effect shader by name (The post process effects stored in the Effects folder)
* @param Name the name for the effect
* @returns an EffectRecord describing the effect shader.
*/
EffectRecord* EffectRecord::LoadEffect(const char* Name) {	
	EffectRecord* EffectProg = new EffectRecord(Name);
	EffectProg->LoadEffect();
	return EffectProg;
}

bool EffectRecord::IsLoaded(){
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
	FloatShaderValues = new ShaderValue[FloatShaderValuesCount];
	TextureShaderValues = new ShaderValue[TextureShaderValuesCount];

	Logger::Debug("CreateCT: Effect has %i constants", ConstantTableDesc.Parameters);

	for (UINT c = 0; c < ConstantTableDesc.Parameters; c++) {
		Handle = Effect->GetParameter(NULL, c);
		Effect->GetParameterDesc(Handle, &ConstantDesc);
		if (!memcmp(ConstantDesc.Name, "TESR_", 5)) {
			switch (ConstantDesc.Class) {
				case D3DXPC_VECTOR:
				case D3DXPC_MATRIX_ROWS:
					SetConstantTableValue(ConstantDesc.Name, FloatIndex);
					FloatShaderValues[FloatIndex].RegisterIndex = (UInt32)Handle;
					FloatShaderValues[FloatIndex].RegisterCount = ConstantDesc.Rows;
					FloatIndex++;
					break;
				case D3DXPC_OBJECT:
					if (ConstantDesc.Class == D3DXPC_OBJECT && ConstantDesc.Type >= D3DXPT_SAMPLER && ConstantDesc.Type <= D3DXPT_SAMPLERCUBE) {
						TextureShaderValues[TextureIndex].Texture = TheTextureManager->LoadTexture(ShaderSource, ConstantDesc.Type, ConstantDesc.Name, TextureIndex, NULL, NULL);
						TextureShaderValues[TextureIndex].RegisterIndex = TextureIndex;
						TextureShaderValues[TextureIndex].RegisterCount = 1;
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
	ShaderValue* Value;
	if (!Enabled || Effect == nullptr) return;
	for (UInt32 c = 0; c < TextureShaderValuesCount; c++) {
		Value = &TextureShaderValues[c];
		try {
			if (Value->Texture->Texture) TheRenderManager->device->SetTexture(Value->RegisterIndex, Value->Texture->Texture);
			for (int i = 1; i < SamplerStatesMax; i++) {
				TheRenderManager->SetSamplerState(Value->RegisterIndex, (D3DSAMPLERSTATETYPE)i, Value->Texture->SamplerStates[i]);
			}
		}
		catch (const std::exception& e) {
			Logger::Log("[ERRROR] Couldnt' bind texture %i to effect %s : %s", c, Path, e.what());
		}
	}
	for (UInt32 c = 0; c < FloatShaderValuesCount; c++) {
		Value = &FloatShaderValues[c];
		if (Value->RegisterCount == 1)
			Effect->SetVector((D3DXHANDLE)Value->RegisterIndex, Value->Value);
		else
			Effect->SetMatrix((D3DXHANDLE)Value->RegisterIndex, (D3DXMATRIX*)Value->Value);
	}
}

/*
 * Enable or Disable Effect, with reloading it if it's changed on disk
 */
bool EffectRecord::SwitchEffect(){
	bool change = true;
	if (!IsLoaded() || (!Enabled && ShouldCompileShader(Path->data(), SourcePath->data(), ShaderCompileType::RecompileChanged)) ) {
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

	if (!Enabled || Effect == nullptr) return; // skip rendering of disabled effects

	auto timer = TimeLogger();
	if (SourceBuffer) Device->StretchRect(RenderTarget, NULL, SourceBuffer, NULL, D3DTEXF_NONE);

	try {
		D3DXHANDLE technique = Effect->GetTechnique(techniqueIndex);
		Effect->SetTechnique(technique);
		SetCT(); // update the constant table
		UINT Passes;
		Effect->Begin(&Passes, NULL);
		for (UINT p = 0; p < Passes; p++) {
			if (ClearRenderTarget) Device->Clear(0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 1), 1.0f, 0L);
			Effect->BeginPass(p);
			Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
			Effect->EndPass();
			Device->StretchRect(RenderTarget, NULL, RenderedSurface, NULL, D3DTEXF_NONE);
		}
		Effect->End();
	}
	catch (const std::exception& e) {
		Logger::Log("Error during rendering of effect %s: %s", Path->c_str(), e.what());
	}

	char name[255] = "EffectRecord::Render ";
	strcat(name, Path->c_str());
	timer.LogTime(name);
}

/**
* Initializes the Shader Manager Singleton.
* The Shader Manager creates and holds onto the Effects activated in the Settings Manager, and sets the constants.
*/
void ShaderManager::Initialize() {

	auto timer = TimeLogger();

	Logger::Log("Starting the shaders manager...");
	TheShaderManager = new ShaderManager();

	TheShaderManager->FrameVertex = NULL;

	// initializing the list of effect names
	TheShaderManager->EffectsNames["AvgLuma"] = &TheShaderManager->Effects.AvgLuma;
	TheShaderManager->EffectsNames["AmbientOcclusion"] = &TheShaderManager->Effects.AmbientOcclusion;
	TheShaderManager->EffectsNames["BloodLens"] = &TheShaderManager->Effects.BloodLens;
	TheShaderManager->EffectsNames["Bloom"] = &TheShaderManager->Effects.Bloom;
	TheShaderManager->EffectsNames["Coloring"] = &TheShaderManager->Effects.Coloring;
	TheShaderManager->EffectsNames["Cinema"] = &TheShaderManager->Effects.Cinema;
	TheShaderManager->EffectsNames["DepthOfField"] = &TheShaderManager->Effects.DepthOfField;
	TheShaderManager->EffectsNames["Debug"] = &TheShaderManager->Effects.Debug;
	TheShaderManager->EffectsNames["Exposure"] = &TheShaderManager->Effects.Exposure;
	TheShaderManager->EffectsNames["GodRays"] = &TheShaderManager->Effects.GodRays;
	TheShaderManager->EffectsNames["ImageAdjust"] = &TheShaderManager->Effects.ImageAdjust;
	TheShaderManager->EffectsNames["Lens"] = &TheShaderManager->Effects.Lens;
	TheShaderManager->EffectsNames["LowHF"] = &TheShaderManager->Effects.LowHF;
	TheShaderManager->EffectsNames["MotionBlur"] = &TheShaderManager->Effects.MotionBlur;
	TheShaderManager->EffectsNames["Normals"] = &TheShaderManager->Effects.Normals;
	TheShaderManager->EffectsNames["Precipitations"] = &TheShaderManager->Effects.Rain;
	TheShaderManager->EffectsNames["Sharpening"] = &TheShaderManager->Effects.Sharpening;
	TheShaderManager->EffectsNames["ShadowsExteriors"] = &TheShaderManager->Effects.ShadowsExteriors;
	TheShaderManager->EffectsNames["ShadowsInteriors"] = &TheShaderManager->Effects.ShadowsInteriors;
	TheShaderManager->EffectsNames["PointShadows"] = &TheShaderManager->Effects.PointShadows;
	TheShaderManager->EffectsNames["PointShadows2"] = &TheShaderManager->Effects.PointShadows2;
	TheShaderManager->EffectsNames["SunShadows"] = &TheShaderManager->Effects.SunShadows;
	TheShaderManager->EffectsNames["Specular"] = &TheShaderManager->Effects.Specular;
	TheShaderManager->EffectsNames["Snow"] = &TheShaderManager->Effects.Snow;
	TheShaderManager->EffectsNames["SnowAccumulation"] = &TheShaderManager->Effects.SnowAccumulation;
	TheShaderManager->EffectsNames["Underwater"] = &TheShaderManager->Effects.Underwater;
	TheShaderManager->EffectsNames["VolumetricFog"] = &TheShaderManager->Effects.VolumetricFog;
	TheShaderManager->EffectsNames["WaterLens"] = &TheShaderManager->Effects.WaterLens;
	TheShaderManager->EffectsNames["WetWorld"] = &TheShaderManager->Effects.WetWorld;

	//setup map of constant names
	TheShaderManager->ConstantsTable["TESR_ToneMapping"] = &TheShaderManager->ShaderConst.HDR.ToneMapping;
	TheShaderManager->ConstantsTable["TESR_ParallaxData"] = &TheShaderManager->ShaderConst.POM.ParallaxData;
	TheShaderManager->ConstantsTable["TESR_GrassScale"] = &TheShaderManager->ShaderConst.Grass.Scale;
	TheShaderManager->ConstantsTable["TESR_TerrainData"] = &TheShaderManager->ShaderConst.Terrain.Data;
	TheShaderManager->ConstantsTable["TESR_SkinData"] = &TheShaderManager->ShaderConst.Skin.SkinData;
	TheShaderManager->ConstantsTable["TESR_SkinColor"] = &TheShaderManager->ShaderConst.Skin.SkinColor;
	TheShaderManager->ConstantsTable["TESR_ShadowData"] = &TheShaderManager->ShaderConst.Shadow.Data;
	TheShaderManager->ConstantsTable["TESR_ShadowScreenSpaceData"] = &TheShaderManager->ShaderConst.Shadow.ScreenSpaceData;
	TheShaderManager->ConstantsTable["TESR_ShadowRadius"] = &TheShaderManager->ShaderConst.ShadowMap.ShadowMapRadius;
	TheShaderManager->ConstantsTable["TESR_OrthoData"] = &TheShaderManager->ShaderConst.Shadow.OrthoData;
	TheShaderManager->ConstantsTable["TESR_RainData"] = &TheShaderManager->ShaderConst.Rain.RainData;
	TheShaderManager->ConstantsTable["TESR_RainAspect"] = &TheShaderManager->ShaderConst.Rain.RainAspect;
	TheShaderManager->ConstantsTable["TESR_SnowData"] = &TheShaderManager->ShaderConst.Snow.SnowData;
	TheShaderManager->ConstantsTable["TESR_WorldTransform"] = (D3DXVECTOR4*)&TheRenderManager->worldMatrix;
	TheShaderManager->ConstantsTable["TESR_ViewTransform"] = (D3DXVECTOR4*)&TheRenderManager->viewMatrix;
	TheShaderManager->ConstantsTable["TESR_ProjectionTransform"] = (D3DXVECTOR4*)&TheRenderManager->projMatrix;
	TheShaderManager->ConstantsTable["TESR_InvProjectionTransform"] = (D3DXVECTOR4*)&TheRenderManager->InvProjMatrix;
	TheShaderManager->ConstantsTable["TESR_WorldViewProjectionTransform"] = (D3DXVECTOR4*)&TheRenderManager->WorldViewProjMatrix;
	TheShaderManager->ConstantsTable["TESR_InvViewProjectionTransform"] = (D3DXVECTOR4*)&TheRenderManager->InvViewProjMatrix;
	TheShaderManager->ConstantsTable["TESR_ViewProjectionTransform"] = (D3DXVECTOR4*)&TheRenderManager->ViewProjMatrix;
	TheShaderManager->ConstantsTable["TESR_ViewSpaceLightDir"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ViewSpaceLightDir;
	TheShaderManager->ConstantsTable["TESR_ScreenSpaceLightDir"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ScreenSpaceLightDir;
	TheShaderManager->ConstantsTable["TESR_ShadowWorldTransform"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowWorld;
	TheShaderManager->ConstantsTable["TESR_ShadowViewProjTransform"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowViewProj;
	TheShaderManager->ConstantsTable["TESR_ShadowCameraToLightTransform"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowCameraToLight;
	TheShaderManager->ConstantsTable["TESR_ShadowCameraToLightTransformNear"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowCameraToLight[0];
	TheShaderManager->ConstantsTable["TESR_ShadowCameraToLightTransformMiddle"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowCameraToLight[1];
	TheShaderManager->ConstantsTable["TESR_ShadowCameraToLightTransformFar"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowCameraToLight[2];
	TheShaderManager->ConstantsTable["TESR_ShadowCameraToLightTransformLod"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowCameraToLight[3];
	TheShaderManager->ConstantsTable["TESR_ShadowCameraToLightTransformOrtho"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowCameraToLight[4];
	TheShaderManager->ConstantsTable["TESR_ShadowCubeMapLightPosition"] = &TheShaderManager->ShaderConst.ShadowMap.ShadowCubeMapLightPosition;
	TheShaderManager->ConstantsTable["TESR_ShadowLightPosition"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition;
	TheShaderManager->ConstantsTable["TESR_ShadowLightPosition0"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[0];
	TheShaderManager->ConstantsTable["TESR_ShadowLightPosition1"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[1];
	TheShaderManager->ConstantsTable["TESR_ShadowLightPosition2"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[2];
	TheShaderManager->ConstantsTable["TESR_ShadowLightPosition3"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[3];
	TheShaderManager->ConstantsTable["TESR_ShadowLightPosition4"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[4];
	TheShaderManager->ConstantsTable["TESR_ShadowLightPosition5"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[5];
	TheShaderManager->ConstantsTable["TESR_ShadowLightPosition6"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[6];
	TheShaderManager->ConstantsTable["TESR_ShadowLightPosition7"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[7];
	TheShaderManager->ConstantsTable["TESR_ShadowLightPosition8"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[8];
	TheShaderManager->ConstantsTable["TESR_ShadowLightPosition9"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[9];
	TheShaderManager->ConstantsTable["TESR_ShadowLightPosition10"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[10];
	TheShaderManager->ConstantsTable["TESR_ShadowLightPosition11"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[11];
	TheShaderManager->ConstantsTable["TESR_LightPosition0"] = (D3DXVECTOR4*)&TheShaderManager->LightPosition[0];
	TheShaderManager->ConstantsTable["TESR_LightPosition1"] = (D3DXVECTOR4*)&TheShaderManager->LightPosition[1];
	TheShaderManager->ConstantsTable["TESR_LightPosition2"] = (D3DXVECTOR4*)&TheShaderManager->LightPosition[2];
	TheShaderManager->ConstantsTable["TESR_LightPosition3"] = (D3DXVECTOR4*)&TheShaderManager->LightPosition[3];
	TheShaderManager->ConstantsTable["TESR_LightPosition4"] = (D3DXVECTOR4*)&TheShaderManager->LightPosition[4];
	TheShaderManager->ConstantsTable["TESR_LightPosition5"] = (D3DXVECTOR4*)&TheShaderManager->LightPosition[5];
	TheShaderManager->ConstantsTable["TESR_LightPosition6"] = (D3DXVECTOR4*)&TheShaderManager->LightPosition[6];
	TheShaderManager->ConstantsTable["TESR_LightPosition7"] = (D3DXVECTOR4*)&TheShaderManager->LightPosition[7];
	TheShaderManager->ConstantsTable["TESR_LightPosition8"] = (D3DXVECTOR4*)&TheShaderManager->LightPosition[8];
	TheShaderManager->ConstantsTable["TESR_LightPosition9"] = (D3DXVECTOR4*)&TheShaderManager->LightPosition[9];
	TheShaderManager->ConstantsTable["TESR_LightPosition10"] = (D3DXVECTOR4*)&TheShaderManager->LightPosition[10];
	TheShaderManager->ConstantsTable["TESR_LightPosition11"] = (D3DXVECTOR4*)&TheShaderManager->LightPosition[11];
	TheShaderManager->ConstantsTable["TESR_LightAttenuation0"] = (D3DXVECTOR4*)&TheShaderManager->LightAttenuation[0];
	TheShaderManager->ConstantsTable["TESR_LightAttenuation1"] = (D3DXVECTOR4*)&TheShaderManager->LightAttenuation[1];
	TheShaderManager->ConstantsTable["TESR_LightAttenuation2"] = (D3DXVECTOR4*)&TheShaderManager->LightAttenuation[2];
	TheShaderManager->ConstantsTable["TESR_LightAttenuation3"] = (D3DXVECTOR4*)&TheShaderManager->LightAttenuation[3];
	TheShaderManager->ConstantsTable["TESR_LightAttenuation4"] = (D3DXVECTOR4*)&TheShaderManager->LightAttenuation[4];
	TheShaderManager->ConstantsTable["TESR_LightAttenuation5"] = (D3DXVECTOR4*)&TheShaderManager->LightAttenuation[5];
	TheShaderManager->ConstantsTable["TESR_LightAttenuation6"] = (D3DXVECTOR4*)&TheShaderManager->LightAttenuation[6];
	TheShaderManager->ConstantsTable["TESR_LightAttenuation7"] = (D3DXVECTOR4*)&TheShaderManager->LightAttenuation[7];
	TheShaderManager->ConstantsTable["TESR_ShadowCubeMapBlend"] = &TheShaderManager->ShaderConst.ShadowMap.ShadowCubeMapBlend;
	TheShaderManager->ConstantsTable["TESR_OcclusionWorldViewProjTransform"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.OcclusionMap.OcclusionWorldViewProj;
	TheShaderManager->ConstantsTable["TESR_ReciprocalResolution"] = &TheShaderManager->ShaderConst.ReciprocalResolution;
	TheShaderManager->ConstantsTable["TESR_CameraForward"] = &TheRenderManager->CameraForward;
	TheShaderManager->ConstantsTable["TESR_DepthConstants"] = &TheRenderManager->DepthConstants;
	TheShaderManager->ConstantsTable["TESR_CameraData"] = &TheRenderManager->CameraData;
	TheShaderManager->ConstantsTable["TESR_CameraPosition"] = &TheRenderManager->CameraPosition;
	TheShaderManager->ConstantsTable["TESR_SunPosition"] = &TheShaderManager->ShaderConst.SunPosition;
	TheShaderManager->ConstantsTable["TESR_SunDirection"] = &TheShaderManager->ShaderConst.SunDir;
	TheShaderManager->ConstantsTable["TESR_SunTiming"] = &TheShaderManager->ShaderConst.SunTiming;
	TheShaderManager->ConstantsTable["TESR_SunAmount"] = &TheShaderManager->ShaderConst.SunAmount;
	TheShaderManager->ConstantsTable["TESR_SunsetColor"] = &TheShaderManager->ShaderConst.Sky.SunsetColor;
	TheShaderManager->ConstantsTable["TESR_ShadowFade"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowFade;
	TheShaderManager->ConstantsTable["TESR_GameTime"] = &TheShaderManager->ShaderConst.GameTime;
	TheShaderManager->ConstantsTable["TESR_WaterCoefficients"] = &TheShaderManager->ShaderConst.Water.waterCoefficients;
	TheShaderManager->ConstantsTable["TESR_WaveParams"] = &TheShaderManager->ShaderConst.Water.waveParams;
	TheShaderManager->ConstantsTable["TESR_WaterVolume"] = &TheShaderManager->ShaderConst.Water.waterVolume;
	TheShaderManager->ConstantsTable["TESR_WaterSettings"] = &TheShaderManager->ShaderConst.Water.waterSettings;
	TheShaderManager->ConstantsTable["TESR_WaterDeepColor"] = &TheShaderManager->ShaderConst.Water.deepColor;
	TheShaderManager->ConstantsTable["TESR_WaterShallowColor"] = &TheShaderManager->ShaderConst.Water.shallowColor;
	TheShaderManager->ConstantsTable["TESR_WaterShorelineParams"] = &TheShaderManager->ShaderConst.Water.shorelineParams;
	TheShaderManager->ConstantsTable["TESR_FogColor"] = &TheShaderManager->ShaderConst.fogColor;
	TheShaderManager->ConstantsTable["TESR_HorizonColor"] = &TheShaderManager->ShaderConst.horizonColor;
	TheShaderManager->ConstantsTable["TESR_SunColor"] = &TheShaderManager->ShaderConst.sunColor;
	TheShaderManager->ConstantsTable["TESR_SkyColor"] = &TheShaderManager->ShaderConst.skyColor;
	TheShaderManager->ConstantsTable["TESR_SkyData"] = &TheShaderManager->ShaderConst.Sky.SkyData;
	TheShaderManager->ConstantsTable["TESR_CloudData"] = &TheShaderManager->ShaderConst.Sky.CloudData;
	TheShaderManager->ConstantsTable["TESR_SkyLowColor"] = &TheShaderManager->ShaderConst.skyLowColor;
	TheShaderManager->ConstantsTable["TESR_SunAmbient"] = &TheShaderManager->ShaderConst.sunAmbient;
	TheShaderManager->ConstantsTable["TESR_FogData"] = &TheShaderManager->ShaderConst.fogData;
	TheShaderManager->ConstantsTable["TESR_FogDistance"] = &TheShaderManager->ShaderConst.fogDistance;
	TheShaderManager->ConstantsTable["TESR_AmbientOcclusionAOData"] = &TheShaderManager->ShaderConst.AmbientOcclusion.AOData;
	TheShaderManager->ConstantsTable["TESR_AmbientOcclusionData"] = &TheShaderManager->ShaderConst.AmbientOcclusion.Data;
	TheShaderManager->ConstantsTable["TESR_BloodLensParams"] = &TheShaderManager->ShaderConst.BloodLens.Params;
	TheShaderManager->ConstantsTable["TESR_BloodLensColor"] = &TheShaderManager->ShaderConst.BloodLens.BloodColor;
	TheShaderManager->ConstantsTable["TESR_BloomData"] = &TheShaderManager->ShaderConst.Bloom.BloomData;
	TheShaderManager->ConstantsTable["TESR_BloomValues"] = &TheShaderManager->ShaderConst.Bloom.BloomValues;
	TheShaderManager->ConstantsTable["TESR_HDRBloomData"] = &TheShaderManager->ShaderConst.HDR.BloomData;
	TheShaderManager->ConstantsTable["TESR_HDRData"] = &TheShaderManager->ShaderConst.HDR.HDRData;
	TheShaderManager->ConstantsTable["TESR_LotteData"] = &TheShaderManager->ShaderConst.HDR.LotteData;
	TheShaderManager->ConstantsTable["TESR_CinemaData"] = &TheShaderManager->ShaderConst.Cinema.Data;
	TheShaderManager->ConstantsTable["TESR_CinemaSettings"] = &TheShaderManager->ShaderConst.Cinema.Settings;
	TheShaderManager->ConstantsTable["TESR_ColoringColorCurve"] = &TheShaderManager->ShaderConst.Coloring.ColorCurve;
	TheShaderManager->ConstantsTable["TESR_ColoringEffectGamma"] = &TheShaderManager->ShaderConst.Coloring.EffectGamma;
	TheShaderManager->ConstantsTable["TESR_ColoringData"] = &TheShaderManager->ShaderConst.Coloring.Data;
	TheShaderManager->ConstantsTable["TESR_ColoringValues"] = &TheShaderManager->ShaderConst.Coloring.Values;
	TheShaderManager->ConstantsTable["TESR_DepthOfFieldBlur"] = &TheShaderManager->ShaderConst.DepthOfField.Blur;
	TheShaderManager->ConstantsTable["TESR_DepthOfFieldData"] = &TheShaderManager->ShaderConst.DepthOfField.Data;
	TheShaderManager->ConstantsTable["TESR_ExposureData"] = &TheShaderManager->ShaderConst.Exposure.Data;
	TheShaderManager->ConstantsTable["TESR_GodRaysRay"] = &TheShaderManager->ShaderConst.GodRays.Ray;
	TheShaderManager->ConstantsTable["TESR_GodRaysRayColor"] = &TheShaderManager->ShaderConst.GodRays.RayColor;
	TheShaderManager->ConstantsTable["TESR_GodRaysData"] = &TheShaderManager->ShaderConst.GodRays.Data;
	TheShaderManager->ConstantsTable["TESR_ImageAdjustData"] = &TheShaderManager->ShaderConst.ImageAdjust.Data;
	TheShaderManager->ConstantsTable["TESR_DarkAdjustColor"] = &TheShaderManager->ShaderConst.ImageAdjust.DarkColor;
	TheShaderManager->ConstantsTable["TESR_LightAdjustColor"] = &TheShaderManager->ShaderConst.ImageAdjust.LightColor;
	TheShaderManager->ConstantsTable["TESR_LensData"] = &TheShaderManager->ShaderConst.Lens.Data;
	TheShaderManager->ConstantsTable["TESR_LowHFData"] = &TheShaderManager->ShaderConst.LowHF.Data;
	TheShaderManager->ConstantsTable["TESR_MotionBlurParams"] = &TheShaderManager->ShaderConst.MotionBlur.BlurParams;
	TheShaderManager->ConstantsTable["TESR_MotionBlurData"] = &TheShaderManager->ShaderConst.MotionBlur.Data;
	TheShaderManager->ConstantsTable["TESR_SharpeningData"] = &TheShaderManager->ShaderConst.Sharpening.Data;
	TheShaderManager->ConstantsTable["TESR_SpecularData"] = &TheShaderManager->ShaderConst.Specular.Data;
	TheShaderManager->ConstantsTable["TESR_SpecularEffects"] = &TheShaderManager->ShaderConst.Specular.EffectStrength;
	TheShaderManager->ConstantsTable["TESR_SnowAccumulationParams"] = &TheShaderManager->ShaderConst.SnowAccumulation.Params;
	TheShaderManager->ConstantsTable["TESR_SnowAccumulationColor"] = &TheShaderManager->ShaderConst.SnowAccumulation.Color;
	TheShaderManager->ConstantsTable["TESR_VolumetricFogData"] = &TheShaderManager->ShaderConst.VolumetricFog.Data;
	TheShaderManager->ConstantsTable["TESR_WaterLensData"] = &TheShaderManager->ShaderConst.WaterLens.Time;
	TheShaderManager->ConstantsTable["TESR_WetWorldCoeffs"] = &TheShaderManager->ShaderConst.WetWorld.Coeffs;
	TheShaderManager->ConstantsTable["TESR_WetWorldData"] = &TheShaderManager->ShaderConst.WetWorld.Data;
	TheShaderManager->ConstantsTable["TESR_DebugVar"] = &TheShaderManager->ShaderConst.DebugVar;

	// Initialize all effects to NULL
	EffectsList::iterator v = TheShaderManager->EffectsNames.begin();
	while (v != TheShaderManager->EffectsNames.end()) {
		*v->second = NULL;
		v++;
	}

	memset(TheShaderManager->WaterVertexShaders, NULL, sizeof(WaterVertexShaders));
	memset(TheShaderManager->WaterPixelShaders, NULL, sizeof(WaterPixelShaders));
	TheShaderManager->InitializeConstants();
	TheShaderManager->ShaderConst.ReciprocalResolution.x = 1.0f / (float)TheRenderManager->width;
	TheShaderManager->ShaderConst.ReciprocalResolution.y = 1.0f / (float)TheRenderManager->height;
	TheShaderManager->ShaderConst.ReciprocalResolution.z = (float)TheRenderManager->width / (float)TheRenderManager->height;
	TheShaderManager->ShaderConst.ReciprocalResolution.w = 0.0f; // Reserved to store the FoV
	TheShaderManager->CreateFrameVertex(TheRenderManager->width, TheRenderManager->height, &TheShaderManager->FrameVertex);

    TheShaderManager->PreviousCell = nullptr;
    TheShaderManager->IsMenuSwitch = false;

	timer.LogTime("ShaderManager::Initialize");

}

void ShaderManager::CreateFrameVertex(UInt32 Width, UInt32 Height, IDirect3DVertexBuffer9** FrameVertex) {
	
	void* VertexData = NULL;
	float OffsetX = (1.0f / (float)Width) * 0.5f;
	float OffsetY = (1.0f / (float)Height) * 0.5f;
	
	FrameVS FrameVertices[] = {
		{ -1.0f,  1.0f, 1.0f, 0.0f + OffsetX, 0.0f + OffsetY },
		{ -1.0f, -1.0f, 1.0f, 0.0f + OffsetX, 1.0f + OffsetY },
		{  1.0f,  1.0f, 1.0f, 1.0f + OffsetX, 0.0f + OffsetY },
		{  1.0f, -1.0f, 1.0f, 1.0f + OffsetX, 1.0f + OffsetY }
	};
	TheRenderManager->device->CreateVertexBuffer(4 * sizeof(FrameVS), D3DUSAGE_WRITEONLY, FrameFVF, D3DPOOL_DEFAULT, FrameVertex, NULL);
	(*FrameVertex)->Lock(0, 0, &VertexData, NULL);
	memcpy(VertexData, FrameVertices, sizeof(FrameVertices));
	(*FrameVertex)->Unlock();

}

/*
* Initializes the Effect Record for each effect activated in the settings.
*/
void ShaderManager::CreateEffects() {

	auto timer = TimeLogger();

	// create effect records for effects shaders based on name
	EffectsList::iterator v = TheShaderManager->EffectsNames.begin();
	while (v != TheShaderManager->EffectsNames.end()) {
		const char* Name = v->first.c_str();
		bool enabled = TheSettingManager->GetMenuShaderEnabled(Name);

		EffectRecord* effect = CreateEffect(Name, enabled);
		*v->second = effect; // assign pointer from Effects struct to the newly created effect record

		enabled = effect->Enabled;
		TheSettingManager->SetMenuShaderEnabled(Name, enabled); // disable settings of effects that couldn't load
		v++;
	}

	/*TODO*/
	//CreateEffect(EffectRecord::EffectRecordType::Extra);
	//if (EffectsSettings->Extra) CreateEffect(EffectRecord::EffectRecordType::Extra);

	timer.LogTime("ShaderManager::CreateEffects");
}

void ShaderManager::InitializeConstants() {

	ShaderConst.pWeather = NULL;
	ShaderConst.WaterLens.Percent = 0.0f;
	ShaderConst.BloodLens.Percent = 0.0f;
	ShaderConst.SnowAccumulation.Params.w = 0.0f;
	ShaderConst.WetWorld.Data.x = 0.0f;
	ShaderConst.WetWorld.Data.y = 0.0f;
	ShaderConst.WetWorld.Data.z = 0.0f;
	ShaderConst.Rain.RainData.x = 0.0f;
	ShaderConst.Rain.RainData.y = 0.0f;
	ShaderConst.Rain.RainData.z = 0.0f;
	ShaderConst.Rain.RainData.w = 0.0f;

	ShaderConst.Animators.PuddlesAnimator.Initialize(0);
	ShaderConst.Animators.RainAnimator.Initialize(0);
	ShaderConst.Animators.SnowAnimator.Initialize(0);
	ShaderConst.Animators.SnowAccumulationAnimator.Initialize(0);
}


/*
Updates the values of the constants that can be accessed from shader code, with values representing the state of the game's elements.
*/
void ShaderManager::UpdateConstants() {
	
	auto timer = TimeLogger();

	bool IsThirdPersonView = !TheCameraManager->IsFirstPerson();
	Sky* WorldSky = Tes->sky;
	NiNode* SunRoot = WorldSky->sun->RootNode;
	TESClimate* currentClimate = WorldSky->firstClimate;
	TESWeather* currentWeather = WorldSky->firstWeather;
	TESWeather* previousWeather = WorldSky->secondWeather;
	TESObjectCELL* currentCell = Player->parentCell;
	TESWorldSpace* currentWorldSpace = Player->GetWorldSpace();
	TESRegion* currentRegion = Player->GetRegion();
	float weatherPercent = WorldSky->weatherPercent;
	float lastGameTime = ShaderConst.GameTime.y;
	const char* sectionName = NULL;
	avglumaRequired = false; // toggle for rendering AvgLuma
	orthoRequired = false; // toggle for rendering Ortho map

	if (Effects.Debug->Enabled) avglumaRequired = true;

	if (!currentCell) return; // if no cell is present, we skip update to avoid trying to access values that don't exist

	// context variables
	isExterior = !currentCell->IsInterior();// || Player->parentCell->flags0 & TESObjectCELL::kFlags0_BehaveLikeExterior; // < use exterior flag, broken for now
	isUnderwater = Tes->sky->GetIsUnderWater();
	isCellChanged = currentCell != PreviousCell;
	PreviousCell = currentCell;

	PipBoyIsOn = InterfaceManager->getIsMenuOpen();
	VATSIsOn = InterfaceManager->IsActive(Menu::kMenuType_VATS);
	OverlayIsOn = InterfaceManager->IsActive(Menu::kMenuType_Computers) ||
		InterfaceManager->IsActive(Menu::kMenuType_LockPick) ||
		InterfaceManager->IsActive(Menu::kMenuType_Surgery) ||
		InterfaceManager->IsActive(Menu::kMenuType_SlotMachine) ||
		InterfaceManager->IsActive(Menu::kMenuType_Blackjack) ||
		InterfaceManager->IsActive(Menu::kMenuType_Roulette) ||
		InterfaceManager->IsActive(Menu::kMenuType_Caravan);
	isDialog = InterfaceManager->IsActive(Menu::MenuType::kMenuType_Dialog);
	isPersuasion = InterfaceManager->IsActive(Menu::MenuType::kMenuType_Persuasion);

	if (isCellChanged) TheSettingManager->SettingsChanged = true; // force update constants during cell transition

	bool isRainy = false;
	bool isSnow = false;
	if (currentWeather) {
		isRainy = currentWeather->GetWeatherType() == TESWeather::WeatherType::kType_Rainy;
		isSnow = currentWeather->GetWeatherType() == TESWeather::WeatherType::kType_Snow;
	}

	TimeGlobals* GameTimeGlobals = TimeGlobals::Get();
	float GameHour = GameTimeGlobals->GameHour->data;
	float DaysPassed = GameTimeGlobals->GameDaysPassed ? GameTimeGlobals->GameDaysPassed->data : 1.0f;

	float SunriseStart = WorldSky->GetSunriseBegin();
	float SunriseEnd = WorldSky->GetSunriseEnd();
	float SunsetStart = WorldSky->GetSunsetBegin();
	float SunsetEnd = WorldSky->GetSunsetEnd();

	// calculating sun amount for shaders (currently not used by any shaders)
	float sunRiseTransitionTime = SunriseEnd - SunriseStart; // sunriseEnd is only the middle point of the transition before nights get fully dark
	float sunSetTransitionTime = SunsetEnd - SunsetStart; // sunsetStart is only the middle point of the transition

	float sunRise = invLerp(SunriseStart - sunRiseTransitionTime, SunriseEnd, GameHour); // 0 at night to 1 after sunrise
	float sunSet = invLerp(SunsetEnd + sunSetTransitionTime, SunsetStart, GameHour);  // 1 before sunset to 0 at night
	float newDayTime = sunRise * sunSet;

	bool isDayTimeChanged = false;
	if (isDayTime != newDayTime) {
		isDayTime = newDayTime;
		isDayTimeChanged = true;
	}
	float transitionCurve = smoothStep(0, 0.6, isDayTime); // a curve for day/night transitions that occurs mostly during second half of sunset

	ShaderConst.GameTime.x = TimeGlobals::GetGameTime(); //time in milliseconds
	ShaderConst.GameTime.y = GameHour; //time in hours
	ShaderConst.GameTime.z = (float)TheFrameRateManager->Time;
	ShaderConst.GameTime.w = TheFrameRateManager->ElapsedTime; // frameTime in seconds

	// get water height based on player position
	ShaderConst.Water.waterSettings.x = Tes->GetWaterHeight(Player, WorldSceneGraph);
	ShaderConst.Water.waterSettings.z = isUnderwater;

	//bool isExterior = !currentCell->IsInterior();

	ShaderConst.SunTiming.x = WorldSky->GetSunriseColorBegin();
	ShaderConst.SunTiming.y = SunriseEnd;
	ShaderConst.SunTiming.z = SunsetStart;
	ShaderConst.SunTiming.w = WorldSky->GetSunsetColorEnd();

	if (lastGameTime != ShaderConst.GameTime.y) {
		// update Sun position
		float deltaz = ShaderConst.SunDir.z;

		ShaderConst.SunPosition = SunRoot->m_localTransform.pos.toD3DXVEC4();
		D3DXVec4Normalize(&ShaderConst.SunPosition, &ShaderConst.SunPosition);

		if (isDayTime < 0.5) {
			// use lighting direction at night time
			ShaderConst.SunDir = Tes->directionalLight->direction.toD3DXVEC4() * -1;
		}
		else {
			ShaderConst.SunDir = ShaderConst.SunPosition;
			D3DXVec4Normalize(&ShaderConst.SunDir, &ShaderConst.SunDir);
		}
	}

	// expose the light vector in view space for screen space lighting
	D3DXVec4Transform(&ShaderConst.ScreenSpaceLightDir, &ShaderConst.SunDir, &TheRenderManager->ViewProjMatrix);
	D3DXVec4Normalize(&ShaderConst.ScreenSpaceLightDir, &ShaderConst.ScreenSpaceLightDir);

	D3DXVec4Transform(&ShaderConst.ViewSpaceLightDir, &ShaderConst.SunDir, &TheRenderManager->ViewMatrix);
	D3DXVec4Normalize(&ShaderConst.ViewSpaceLightDir, &ShaderConst.ViewSpaceLightDir);

	ShaderConst.ShadowFade.x = 0;

	if (isExterior) {
		ShaderConst.ShadowFade.x = smoothStep(0.5, 0, abs(isDayTime - 0.5)); // fade shadows to 0 at sunrise/sunset

		// at night time, fade based on moonphase
		if (isDayTime < 0.5) {
			// moonphase goes from 0 to 8
			float MoonPhase = (fmod(DaysPassed, 8 * currentClimate->phaseLength & 0x3F)) / (currentClimate->phaseLength & 0x3F);

			float PI = 3.1416; // use cos curve to fade moon light shadows strength
			MoonPhase = std::lerp(-PI, PI, MoonPhase / 8) - PI / 4; // map moonphase to 1/2PI/2PI + 1/2

			// map MoonVisibility to MinNightDarkness/1 range
			float nightMinDarkness = TheSettingManager->GetSettingF("Shaders.ShadowsExteriors.Main", "NightMinDarkness");
			float MoonVisibility = lerp(0.0, (double)nightMinDarkness, cos(MoonPhase) * 0.5 + 0.5);
			ShaderConst.ShadowFade.x = lerp (0, MoonVisibility, ShaderConst.ShadowFade.x);
		}

		// pass the enabled/disabled property of the shadow maps to the shadowfade constant
		const char* PointLightsSettingName = (isDayTime > 0.5) ? "UsePointShadowsDay" : "UsePointShadowsNight";
		bool usePointLights = TheSettingManager->GetSettingI("Shaders.ShadowsExteriors.Main", PointLightsSettingName);
			
		ShaderConst.ShadowFade.y = TheSettingManager->SettingsShadows.Exteriors.Enabled && Effects.ShadowsExteriors->Enabled;
		ShaderConst.ShadowFade.z = usePointLights;
		ShaderConst.ShadowFade.w = ShaderConst.ShadowMap.ShadowMapRadius.w; //furthest distance for point lights shadows
	}
	else {
		// pass the enabled/disabled property of the shadow maps to the shadowfade constant
		ShaderConst.ShadowFade.y = Effects.ShadowsInteriors->Enabled;
		ShaderConst.ShadowFade.z = 1;
		ShaderConst.ShadowFade.w = TheSettingManager->SettingsShadows.Interiors.DrawDistance; //furthest distance for point lights shadows
	}

	ShaderConst.sunGlare = currentWeather ? (currentWeather->GetSunGlare() / 255.0f) : 0.5;

	isDayTime = smoothStep(0, 1, isDayTime); // smooth daytime progression
	ShaderConst.SunAmount.x = isDayTime; 
	ShaderConst.SunAmount.y = ShaderConst.sunGlare;
	if (TheSettingManager->SettingsChanged) {
		// sky settings are used in several shaders whether the shader is active or not
		ShaderConst.SunAmount.z = TheSettingManager->GetSettingI("Shaders.Sky.Main", "ReplaceSun");

		ShaderConst.Sky.SkyData.x = TheSettingManager->GetSettingF("Shaders.Sky.Main", "AthmosphereThickness");
		ShaderConst.Sky.SkyData.y = TheSettingManager->GetSettingF("Shaders.Sky.Main", "SunInfluence");
		ShaderConst.Sky.SkyData.z = TheSettingManager->GetSettingF("Shaders.Sky.Main", "SunStrength");
		ShaderConst.Sky.SkyData.w = TheSettingManager->GetSettingF("Shaders.Sky.Main", "StarStrength");

		ShaderConst.Sky.CloudData.x = TheSettingManager->GetSettingF("Shaders.Sky.Clouds", "UseNormals");
		ShaderConst.Sky.CloudData.y = TheSettingManager->GetSettingF("Shaders.Sky.Clouds", "SphericalNormals");
		ShaderConst.Sky.CloudData.z = TheSettingManager->GetSettingF("Shaders.Sky.Clouds", "Transparency");
		ShaderConst.Sky.CloudData.w = TheSettingManager->GetSettingF("Shaders.Sky.Clouds", "Brightness");
	}

	// only add sunset color boost in exteriors
	if (isExterior) {
		ShaderConst.Sky.SunsetColor.x = TheSettingManager->GetSettingF("Shaders.Sky.Main", "SunsetR");
		ShaderConst.Sky.SunsetColor.y = TheSettingManager->GetSettingF("Shaders.Sky.Main", "SunsetG");
		ShaderConst.Sky.SunsetColor.z = TheSettingManager->GetSettingF("Shaders.Sky.Main", "SunsetB");

		// TODO : fix sun culling for sun replacing?
		//if (TheSettingManager->GetMenuShaderEnabled("Sky")) {
		//	if (ShaderConst.SunAmount.z) WorldSky->sun->RootNode->m_flags |= ~NiAVObject::NiFlags::DISPLAY_OBJECT; // cull Sun node
		//	else WorldSky->sun->RootNode->m_flags &= NiAVObject::NiFlags::DISPLAY_OBJECT; // disable Sun node
		//}
	}
	else {
		ShaderConst.Sky.SunsetColor.x = 0;
		ShaderConst.Sky.SunsetColor.y = 0;
		ShaderConst.Sky.SunsetColor.z = 0;
	}

	ShaderConst.sunColor.x = WorldSky->sunDirectional.r;
	ShaderConst.sunColor.y = WorldSky->sunDirectional.g;
	ShaderConst.sunColor.z = WorldSky->sunDirectional.b;
	ShaderConst.sunColor.w = ShaderConst.sunGlare;

	ShaderConst.windSpeed = WorldSky->windSpeed;

	ShaderConst.fogColor.x = WorldSky->fogColor.r;
	ShaderConst.fogColor.y = WorldSky->fogColor.g;
	ShaderConst.fogColor.z = WorldSky->fogColor.b;
	ShaderConst.fogColor.w = 1.0f;

	ShaderConst.horizonColor.x = WorldSky->Horizon.r;
	ShaderConst.horizonColor.y = WorldSky->Horizon.g;
	ShaderConst.horizonColor.z = WorldSky->Horizon.b;
	ShaderConst.horizonColor.w = 1.0f;

	ShaderConst.sunAmbient.x = WorldSky->sunAmbient.r;
	ShaderConst.sunAmbient.y = WorldSky->sunAmbient.g;
	ShaderConst.sunAmbient.z = WorldSky->sunAmbient.b;
	ShaderConst.sunAmbient.w = 1.0f;

	ShaderConst.skyLowColor.x = WorldSky->SkyLower.r;
	ShaderConst.skyLowColor.y = WorldSky->SkyLower.g;
	ShaderConst.skyLowColor.z = WorldSky->SkyLower.b;
	ShaderConst.skyLowColor.w = 1.0f;

	ShaderConst.skyColor.x = WorldSky->skyUpper.r;
	ShaderConst.skyColor.y = WorldSky->skyUpper.g;
	ShaderConst.skyColor.z = WorldSky->skyUpper.b;
	ShaderConst.skyColor.w = 1.0f;

	// for near plane, ensure that far > near
	ShaderConst.fogData.x = WorldSky->fogFarPlane > WorldSky->fogNearPlane ? WorldSky->fogNearPlane : WorldSky->fogFarPlane * 0.7;
	ShaderConst.fogData.y = WorldSky->fogFarPlane;
	ShaderConst.fogData.z = ShaderConst.sunGlare;
	ShaderConst.fogData.w = WorldSky->fogPower;

	ShaderConst.fogDistance.x = ShaderConst.fogData.x;
	ShaderConst.fogDistance.y = ShaderConst.fogData.y;
	ShaderConst.fogDistance.z = 1.0f;
	ShaderConst.fogDistance.w = ShaderConst.sunGlare;

	//if (weatherPercent == 1.0f) ShaderConst.pWeather = currentWeather;

	if (TheSettingManager->GetMenuShaderEnabled("Water") || Effects.Underwater->Enabled) {
		RGBA* rgba = NULL;
		SettingsWaterStruct* sws = NULL;
		TESWaterForm* currentWater = currentCell->GetWaterForm();
			
		sectionName = "Shaders.Water.Default";
		if (currentWater) {
			UInt32 WaterType = currentWater->GetWaterType();
			if (WaterType == TESWaterForm::WaterType::kWaterType_Blood)
				sectionName = "Shaders.Water.Blood";
			else if (WaterType == TESWaterForm::WaterType::kWaterType_Lava)
				sectionName = "Shaders.Water.Lava";

			// world space specific settings. TODO Reimplement with Toml
			//else if (!(sws = TheSettingManager->GetSettingsWater(currentCell->GetEditorName())) && currentWorldSpace)
			//	sws = TheSettingManager->GetSettingsWater(currentWorldSpace->GetEditorName());

			rgba = currentWater->GetDeepColor();
			ShaderConst.Water.deepColor.x = rgba->r / 255.0f;
			ShaderConst.Water.deepColor.y = rgba->g / 255.0f;
			ShaderConst.Water.deepColor.z = rgba->b / 255.0f;
			ShaderConst.Water.deepColor.w = rgba->a / 255.0f;
			rgba = currentWater->GetShallowColor();
			ShaderConst.Water.shallowColor.x = rgba->r / 255.0f;
			ShaderConst.Water.shallowColor.y = rgba->g / 255.0f;
			ShaderConst.Water.shallowColor.z = rgba->b / 255.0f;
			ShaderConst.Water.shallowColor.w = rgba->a / 255.0f;
		}

		if (TheSettingManager->SettingsChanged) {
			ShaderConst.Water.waterCoefficients.x = TheSettingManager->GetSettingF(sectionName, "inExtCoeff_R");
			ShaderConst.Water.waterCoefficients.y = TheSettingManager->GetSettingF(sectionName, "inExtCoeff_G");
			ShaderConst.Water.waterCoefficients.z = TheSettingManager->GetSettingF(sectionName, "inExtCoeff_B");
			ShaderConst.Water.waterCoefficients.w = TheSettingManager->GetSettingF(sectionName, "inScattCoeff");

			ShaderConst.Water.waveParams.x = TheSettingManager->GetSettingF(sectionName, "choppiness");
			ShaderConst.Water.waveParams.y = TheSettingManager->GetSettingF(sectionName, "waveWidth");
			ShaderConst.Water.waveParams.z = TheSettingManager->GetSettingF(sectionName, "waveSpeed");
			ShaderConst.Water.waveParams.w = TheSettingManager->GetSettingF(sectionName, "reflectivity");

			ShaderConst.Water.waterSettings.y = TheSettingManager->GetSettingF(sectionName, "depthDarkness");

			ShaderConst.Water.waterVolume.x = TheSettingManager->GetSettingF(sectionName, "causticsStrength") * ShaderConst.sunGlare;
			ShaderConst.Water.waterVolume.y = TheSettingManager->GetSettingF(sectionName, "shoreFactor");
			ShaderConst.Water.waterVolume.z = TheSettingManager->GetSettingF(sectionName, "turbidity");
			ShaderConst.Water.waterVolume.w = TheSettingManager->GetSettingF(sectionName, "causticsStrengthS");

			ShaderConst.Water.shorelineParams.x = TheSettingManager->GetSettingF(sectionName, "shoreMovement");
		}
	}		

	if (isUnderwater) {
		ShaderConst.BloodLens.Percent = 0.0f;
		ShaderConst.WaterLens.Percent = -1.0f;
		ShaderConst.Animators.WaterLensAnimator.switched = true;
		ShaderConst.Animators.WaterLensAnimator.Start(0.0, 0);
	}

	if (Effects.WaterLens->Enabled) {
		if (!isUnderwater && ShaderConst.Animators.WaterLensAnimator.switched == true) {
			ShaderConst.Animators.WaterLensAnimator.switched = false;
			// start the waterlens effect and animate it fading
			ShaderConst.Animators.WaterLensAnimator.Initialize(1);
			ShaderConst.Animators.WaterLensAnimator.Start(0.01, 0);
		}
		ShaderConst.WaterLens.Percent = ShaderConst.Animators.WaterLensAnimator.GetValue();
		ShaderConst.WaterLens.Time.w = TheSettingManager->GetSettingF("Shaders.WaterLens.Main", "Amount") * ShaderConst.WaterLens.Percent;
	}
			
	if (isExterior) {
		// Rain fall & puddles
		if (isRainy && ShaderConst.Animators.RainAnimator.switched == false) {
			// it just started raining
			ShaderConst.WetWorld.Data.y = 1.0f;
			ShaderConst.Animators.PuddlesAnimator.Start(TheSettingManager->GetSettingF("Shaders.WetWorld.Main", "Increase"), 1);
			ShaderConst.Animators.RainAnimator.switched = true;
			ShaderConst.Animators.RainAnimator.Start(0.05, 1);
		}
		else if (!isRainy && ShaderConst.Animators.RainAnimator.switched) {
			// it just stopped raining
			ShaderConst.WetWorld.Data.y = 0.0f;
			ShaderConst.Animators.PuddlesAnimator.Start(TheSettingManager->GetSettingF("Shaders.WetWorld.Main", "Decrease"), 0);
			ShaderConst.Animators.RainAnimator.switched = false;
			ShaderConst.Animators.RainAnimator.Start(0.07, 0);
		}
		ShaderConst.WetWorld.Data.x = ShaderConst.Animators.RainAnimator.GetValue();
		ShaderConst.WetWorld.Data.z = ShaderConst.Animators.PuddlesAnimator.GetValue();
		ShaderConst.WetWorld.Data.w = TheSettingManager->GetSettingF("Shaders.WetWorld.Main", "Amount");

		if (ShaderConst.WetWorld.Data.x || ShaderConst.WetWorld.Data.z) orthoRequired = true; // mark ortho map calculation as necessary
		ShaderConst.Rain.RainData.x = ShaderConst.Animators.RainAnimator.GetValue();

		if (Effects.Snow->Enabled) {
			// Snow fall
			if (isSnow && ShaderConst.Animators.SnowAnimator.switched == false) {
				// it just started snowing
				ShaderConst.Animators.PuddlesAnimator.Start(0.3, 0); // fade out any puddles if they exist
				ShaderConst.Animators.SnowAnimator.switched = true;
				ShaderConst.Animators.SnowAnimator.Initialize(0);
				ShaderConst.Animators.SnowAnimator.Start(0.5, 1);
			}
			else if (!isSnow && ShaderConst.Animators.SnowAnimator.switched) {
				// it just stopped snowing
				ShaderConst.Animators.SnowAnimator.switched = false;
				ShaderConst.Animators.SnowAnimator.Start(0.2, 0);
			}
			ShaderConst.Snow.SnowData.x = ShaderConst.Animators.SnowAnimator.GetValue();

			if (ShaderConst.Snow.SnowData.x) orthoRequired = true; // mark ortho map calculation as necessary

			if (TheSettingManager->SettingsChanged) {
				ShaderConst.Snow.SnowData.z = TheSettingManager->GetSettingF("Shaders.Snow.Main", "Speed");
			}
		}

		if (Effects.SnowAccumulation->Enabled) {
			// Snow Accumulation
			if (isSnow && !ShaderConst.Animators.SnowAccumulationAnimator.switched) {
				// it just started snowing
				ShaderConst.Animators.SnowAccumulationAnimator.switched = true;
				ShaderConst.Animators.SnowAccumulationAnimator.Start(TheSettingManager->GetSettingF("Shaders.SnowAccumulation.Main", "Increase"), 1);
			}
			else if (!isSnow && ShaderConst.Animators.SnowAccumulationAnimator.switched) {
				// it just stopped snowing
				ShaderConst.Animators.SnowAccumulationAnimator.switched = false;
				ShaderConst.Animators.SnowAccumulationAnimator.Start(TheSettingManager->GetSettingF("Shaders.SnowAccumulation.Main", "Decrease"), 0);
			}

			ShaderConst.SnowAccumulation.Params.w = ShaderConst.Animators.SnowAccumulationAnimator.GetValue();
			if (ShaderConst.SnowAccumulation.Params.w) orthoRequired = true; // mark ortho map calculation as necessary
		}
	}

	if (TheSettingManager->SettingsChanged) {
		// Static constants that will only change when settings are edited

		if (TheSettingManager->GetMenuShaderEnabled("Grass")) {
			ShaderConst.Grass.Scale.x = TheSettingManager->GetSettingF("Shaders.Grass.Main", "ScaleX");
			ShaderConst.Grass.Scale.y = TheSettingManager->GetSettingF("Shaders.Grass.Main", "ScaleY");
			ShaderConst.Grass.Scale.z = TheSettingManager->GetSettingF("Shaders.Grass.Main", "ScaleZ");

			switch (TheSettingManager->GetSettingI("Shaders.Grass.Main", "GrassDensity")) {
			case 1:
				*Pointers::Settings::MinGrassSize = 240;
				*Pointers::Settings::TexturePctThreshold = 0.3f;
				break;
			case 2:
				*Pointers::Settings::MinGrassSize = 240;
				*Pointers::Settings::TexturePctThreshold = 0.2f;
				break;
			case 3:
				*Pointers::Settings::MinGrassSize = 120;
				*Pointers::Settings::TexturePctThreshold = 0.3f;
				break;
			case 4:
				*Pointers::Settings::MinGrassSize = 120;
				*Pointers::Settings::TexturePctThreshold = 0.2f;
				break;
			case 5:
				*Pointers::Settings::MinGrassSize = 80;
				*Pointers::Settings::TexturePctThreshold = 0.3f;
				break;
			case 6:
				*Pointers::Settings::MinGrassSize = 80;
				*Pointers::Settings::TexturePctThreshold = 0.2f;
				break;
			case 7:
				*Pointers::Settings::MinGrassSize = 20;
				*Pointers::Settings::TexturePctThreshold = 0.3f;
				break;
			case 8:
				*Pointers::Settings::MinGrassSize = 20;
				*Pointers::Settings::TexturePctThreshold = 0.2f;
				break;
			default:
				break;
			}

			float minDistance = TheSettingManager->GetSettingF("Shaders.Grass.Main", "MinDistance");
			if (minDistance) *Pointers::Settings::GrassStartFadeDistance = minDistance;
			float maxDistance = TheSettingManager->GetSettingF("Shaders.Grass.Main", "MaxDistance");
			if (maxDistance) *Pointers::Settings::GrassEndDistance = maxDistance;

			if (TheSettingManager->GetSettingI("Shaders.Grass.Main", "WindEnabled")) {
				*Pointers::Settings::GrassWindMagnitudeMax = *Pointers::ShaderParams::GrassWindMagnitudeMax = TheSettingManager->GetSettingF("Shaders.Grass.Main", "WindCoefficient") * ShaderConst.windSpeed;
				*Pointers::Settings::GrassWindMagnitudeMin = *Pointers::ShaderParams::GrassWindMagnitudeMin = *Pointers::Settings::GrassWindMagnitudeMax * 0.5f;
			}
		}


		if (TheSettingManager->GetMenuShaderEnabled("POM")) {
			ShaderConst.POM.ParallaxData.x = TheSettingManager->GetSettingF("Shaders.POM.Main", "HeightMapScale");
			ShaderConst.POM.ParallaxData.y = TheSettingManager->GetSettingF("Shaders.POM.Main", "MinSamples");
			ShaderConst.POM.ParallaxData.z = TheSettingManager->GetSettingF("Shaders.POM.Main", "MaxSamples");
		}

		if (TheSettingManager->GetMenuShaderEnabled("Terrain")) {
			ShaderConst.Terrain.Data.x = TheSettingManager->GetSettingF("Shaders.Terrain.Main", "DistantSpecular");
			ShaderConst.Terrain.Data.y = TheSettingManager->GetSettingF("Shaders.Terrain.Main", "DistantNoise");
			ShaderConst.Terrain.Data.z = TheSettingManager->GetSettingF("Shaders.Terrain.Main", "NearSpecular");
			ShaderConst.Terrain.Data.w = TheSettingManager->GetSettingF("Shaders.Terrain.Main", "MiddleSpecular");
		}

		if (TheSettingManager->GetMenuShaderEnabled("Skin")) {
			ShaderConst.Skin.SkinData.x = TheSettingManager->GetSettingF("Shaders.Skin.Main", "Attenuation");
			ShaderConst.Skin.SkinData.y = TheSettingManager->GetSettingF("Shaders.Skin.Main", "SpecularPower");
			ShaderConst.Skin.SkinData.z = TheSettingManager->GetSettingF("Shaders.Skin.Main", "MaterialThickness");
			ShaderConst.Skin.SkinData.w = TheSettingManager->GetSettingF("Shaders.Skin.Main", "RimScalar");
			ShaderConst.Skin.SkinColor.x = TheSettingManager->GetSettingF("Shaders.Skin.Main", "CoeffRed");
			ShaderConst.Skin.SkinColor.y = TheSettingManager->GetSettingF("Shaders.Skin.Main", "CoeffGreen");
			ShaderConst.Skin.SkinColor.z = TheSettingManager->GetSettingF("Shaders.Skin.Main", "CoeffBlue");
		}

		if (Effects.GodRays->Enabled) {
			ShaderConst.GodRays.Ray.x = TheSettingManager->GetSettingF("Shaders.GodRays.Main", "RayIntensity");
			ShaderConst.GodRays.Ray.y = TheSettingManager->GetSettingF("Shaders.GodRays.Main", "RayLength");
			ShaderConst.GodRays.Ray.z = TheSettingManager->GetSettingF("Shaders.GodRays.Main", "RayDensity");
			ShaderConst.GodRays.Ray.w = TheSettingManager->GetSettingF("Shaders.GodRays.Main", "RayVisibility");
			ShaderConst.GodRays.RayColor.x = TheSettingManager->GetSettingF("Shaders.GodRays.Coloring", "RayR");
			ShaderConst.GodRays.RayColor.y = TheSettingManager->GetSettingF("Shaders.GodRays.Coloring", "RayG");
			ShaderConst.GodRays.RayColor.z = TheSettingManager->GetSettingF("Shaders.GodRays.Coloring", "RayB");
			ShaderConst.GodRays.RayColor.w = TheSettingManager->GetSettingF("Shaders.GodRays.Coloring", "Saturate");
			ShaderConst.GodRays.Data.x = TheSettingManager->GetSettingF("Shaders.GodRays.Main", "LightShaftPasses");
			ShaderConst.GodRays.Data.y = TheSettingManager->GetSettingF("Shaders.GodRays.Main", "Luminance");
			ShaderConst.GodRays.Data.w = TheSettingManager->GetSettingF("Shaders.GodRays.Main", "TimeEnabled");
		}

		if (Effects.Sharpening->Enabled) {
			ShaderConst.Sharpening.Data.x = TheSettingManager->GetSettingF("Shaders.Sharpening.Main", "Strength");
			ShaderConst.Sharpening.Data.y = TheSettingManager->GetSettingF("Shaders.Sharpening.Main", "Clamp");
			ShaderConst.Sharpening.Data.z = TheSettingManager->GetSettingF("Shaders.Sharpening.Main", "Offset");
		}

		if (Effects.VolumetricFog->Enabled) {
			ShaderConst.VolumetricFog.Data.x = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "Exponent");
			ShaderConst.VolumetricFog.Data.y = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "ColorCoeff");
			ShaderConst.VolumetricFog.Data.z = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "Amount");
			ShaderConst.VolumetricFog.Data.w = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "MaxDistance");
			//if (weatherPercent == 1.0f && ShaderConst.fogData.y > TheSettingManager->SettingsVolumetricFog.MaxDistance) ShaderConst.VolumetricFog.Data.w = 0.0f;
		}

		// blur settings are used to blur normals for all effects using them
		ShaderConst.SnowAccumulation.Params.x = TheSettingManager->GetSettingF("Shaders.SnowAccumulation.Main", "BlurNormDropThreshhold");
		ShaderConst.SnowAccumulation.Params.y = TheSettingManager->GetSettingF("Shaders.SnowAccumulation.Main", "BlurRadiusMultiplier");

		if (Effects.SnowAccumulation->Enabled) {
			ShaderConst.SnowAccumulation.Params.z = TheSettingManager->GetSettingF("Shaders.SnowAccumulation.Main", "SunPower");
			ShaderConst.SnowAccumulation.Color.x = TheSettingManager->GetSettingF("Shaders.SnowAccumulation.Main", "SnowColorR");
			ShaderConst.SnowAccumulation.Color.y = TheSettingManager->GetSettingF("Shaders.SnowAccumulation.Main", "SnowColorG");
			ShaderConst.SnowAccumulation.Color.z = TheSettingManager->GetSettingF("Shaders.SnowAccumulation.Main", "SnowColorB");
		}
		
		if (Effects.ShadowsExteriors->Enabled) {
			ShaderConst.Shadow.ScreenSpaceData.x = TheSettingManager->GetSettingI("Shaders.ShadowsExteriors.ScreenSpace", "Enabled") && Effects.ShadowsExteriors->Enabled;
			ShaderConst.Shadow.ScreenSpaceData.y = TheSettingManager->GetSettingF("Shaders.ShadowsExteriors.ScreenSpace", "BlurRadius");
			ShaderConst.Shadow.ScreenSpaceData.z = TheSettingManager->GetSettingF("Shaders.ShadowsExteriors.ScreenSpace", "RenderDistance");
		}

		if (Effects.WetWorld->Enabled) {
			ShaderConst.WetWorld.Coeffs.x = TheSettingManager->GetSettingF("Shaders.WetWorld.Main", "PuddleCoeff_R");
			ShaderConst.WetWorld.Coeffs.y = TheSettingManager->GetSettingF("Shaders.WetWorld.Main", "PuddleCoeff_G");
			ShaderConst.WetWorld.Coeffs.z = TheSettingManager->GetSettingF("Shaders.WetWorld.Main", "PuddleCoeff_B");
			ShaderConst.WetWorld.Coeffs.w = TheSettingManager->GetSettingF("Shaders.WetWorld.Main", "PuddleSpecularMultiplier");
		}

		if (Effects.WaterLens->Enabled) {
			ShaderConst.WaterLens.Time.x = TheSettingManager->GetSettingF("Shaders.WaterLens.Main", "TimeMultA");
			ShaderConst.WaterLens.Time.y = TheSettingManager->GetSettingF("Shaders.WaterLens.Main", "TimeMultB");
			ShaderConst.WaterLens.Time.z = TheSettingManager->GetSettingF("Shaders.WaterLens.Main", "Viscosity");
		}

		if (Effects.Rain->Enabled) {
			ShaderConst.Rain.RainData.y = TheSettingManager->GetSettingF("Shaders.Precipitations.Main", "VerticalScale");
			ShaderConst.Rain.RainData.z = TheSettingManager->GetSettingF("Shaders.Precipitations.Main", "Speed");
			ShaderConst.Rain.RainData.w = TheSettingManager->GetSettingF("Shaders.Precipitations.Main", "Opacity");

			ShaderConst.Rain.RainAspect.x = TheSettingManager->GetSettingF("Shaders.Precipitations.Main", "Refraction");
			ShaderConst.Rain.RainAspect.y = TheSettingManager->GetSettingF("Shaders.Precipitations.Main", "Coloring");
			ShaderConst.Rain.RainAspect.z = TheSettingManager->GetSettingF("Shaders.Precipitations.Main", "Bloom");
		}
			
		if (Effects.Cinema->Enabled) {
			ShaderConst.Cinema.Data.y = TheSettingManager->GetSettingF("Shaders.Cinema.Main", "VignetteRadius");
			ShaderConst.Cinema.Data.z = TheSettingManager->GetSettingF("Shaders.Cinema.Main", "VignetteDarkness");
			ShaderConst.Cinema.Data.w = TheSettingManager->GetSettingF("Shaders.Cinema.Main", "OverlayStrength");
			ShaderConst.Cinema.Settings.y = TheSettingManager->GetSettingF("Shaders.Cinema.Main", "FilmGrainAmount");
			ShaderConst.Cinema.Settings.z = TheSettingManager->GetSettingF("Shaders.Cinema.Main", "ChromaticAberration");
			ShaderConst.Cinema.Settings.w = TheSettingManager->GetSettingF("Shaders.Cinema.Main", "LetterBoxDepth");
		}

		if (Effects.AmbientOcclusion->Enabled) {
			sectionName = "Shaders.AmbientOcclusion.Exteriors";
			if (!isExterior) sectionName = "Shaders.AmbientOcclusion.Interiors";

			ShaderConst.AmbientOcclusion.Enabled = TheSettingManager->GetSettingI(sectionName, "Enabled");
			if (ShaderConst.AmbientOcclusion.Enabled) {
				ShaderConst.AmbientOcclusion.AOData.x = TheSettingManager->GetSettingF(sectionName, "Samples");
				ShaderConst.AmbientOcclusion.AOData.y = TheSettingManager->GetSettingF(sectionName, "StrengthMultiplier");
				ShaderConst.AmbientOcclusion.AOData.z = TheSettingManager->GetSettingF(sectionName, "ClampStrength");
				ShaderConst.AmbientOcclusion.AOData.w = TheSettingManager->GetSettingF(sectionName, "Range");
				ShaderConst.AmbientOcclusion.Data.x = TheSettingManager->GetSettingF(sectionName, "AngleBias");
				ShaderConst.AmbientOcclusion.Data.y = TheSettingManager->GetSettingF(sectionName, "LumThreshold");
				ShaderConst.AmbientOcclusion.Data.z = TheSettingManager->GetSettingF(sectionName, "BlurDropThreshold");
				ShaderConst.AmbientOcclusion.Data.w = TheSettingManager->GetSettingF(sectionName, "BlurRadiusMultiplier");
			}
		}

		if (Effects.Bloom->Enabled) {
			sectionName = "Shaders.Bloom.Exteriors";
			if (!isExterior) sectionName = "Shaders.Bloom.Interiors";

			ShaderConst.Bloom.BloomData.x = TheSettingManager->GetSettingF(sectionName, "Luminance");
			ShaderConst.Bloom.BloomData.y = TheSettingManager->GetSettingF(sectionName, "MiddleGray");
			ShaderConst.Bloom.BloomData.z = TheSettingManager->GetSettingF(sectionName, "WhiteCutOff");
			ShaderConst.Bloom.BloomValues.x = TheSettingManager->GetSettingF(sectionName, "BloomIntensity");
			ShaderConst.Bloom.BloomValues.y = TheSettingManager->GetSettingF(sectionName, "OriginalIntensity");
			ShaderConst.Bloom.BloomValues.z = TheSettingManager->GetSettingF(sectionName, "BloomSaturation");
			ShaderConst.Bloom.BloomValues.w = TheSettingManager->GetSettingF(sectionName, "OriginalSaturation");
		}



		ShaderConst.DebugVar.x = TheSettingManager->GetSettingF("Main.Develop.Main", "DebugVar1");
		ShaderConst.DebugVar.y = TheSettingManager->GetSettingF("Main.Develop.Main", "DebugVar2");
		ShaderConst.DebugVar.z = TheSettingManager->GetSettingF("Main.Develop.Main", "DebugVar3");
		ShaderConst.DebugVar.w = TheSettingManager->GetSettingF("Main.Develop.Main", "DebugVar4");
	}

	if (TheSettingManager->SettingsChanged || isDayTimeChanged) {
		if (TheSettingManager->GetMenuShaderEnabled("HDR")) {
			ShaderConst.HDR.PointLightMult = TheSettingManager->GetSettingTransition("Shaders.HDR", "PointLightMultiplier", isExterior, transitionCurve);
			ShaderConst.HDR.ToneMapping.x = TheSettingManager->GetSettingTransition("Shaders.HDR", "ToneMapping", isExterior, transitionCurve);
			ShaderConst.HDR.ToneMapping.y = TheSettingManager->GetSettingTransition("Shaders.HDR", "ToneMappingBlur", isExterior, transitionCurve);
			ShaderConst.HDR.ToneMapping.z = TheSettingManager->GetSettingTransition("Shaders.HDR", "ToneMappingColor", isExterior, transitionCurve);
			ShaderConst.HDR.ToneMapping.w = TheSettingManager->GetSettingTransition("Shaders.HDR", "Linearization", isExterior, transitionCurve);
			ShaderConst.HDR.BloomData.x = TheSettingManager->GetSettingTransition("Shaders.HDR", "BloomStrength", isExterior, transitionCurve);
			ShaderConst.HDR.BloomData.y = TheSettingManager->GetSettingTransition("Shaders.HDR", "BloomExponent", isExterior, transitionCurve);
			ShaderConst.HDR.BloomData.z = TheSettingManager->GetSettingTransition("Shaders.HDR", "SkyMultiplier", isExterior, transitionCurve);
			ShaderConst.HDR.BloomData.w = TheSettingManager->GetSettingTransition("Shaders.HDR", "WhitePoint", isExterior, transitionCurve);
			ShaderConst.HDR.HDRData.x = TheSettingManager->GetSettingTransition("Shaders.HDR", "TonemappingMode", isExterior, transitionCurve);
			ShaderConst.HDR.HDRData.y = TheSettingManager->GetSettingTransition("Shaders.HDR", "Exposure", isExterior, transitionCurve);
			ShaderConst.HDR.HDRData.z = TheSettingManager->GetSettingTransition("Shaders.HDR", "Saturation", isExterior, transitionCurve);
			ShaderConst.HDR.HDRData.w = TheSettingManager->GetSettingTransition("Shaders.HDR", "Gamma", isExterior, transitionCurve);
			ShaderConst.HDR.LotteData.x = TheSettingManager->GetSettingTransition("Shaders.HDR", "LotteContrast", isExterior, transitionCurve);
			ShaderConst.HDR.LotteData.y = TheSettingManager->GetSettingTransition("Shaders.HDR", "LotteBrightness", isExterior, transitionCurve);
			ShaderConst.HDR.LotteData.z = TheSettingManager->GetSettingTransition("Shaders.HDR", "LotteMidpoint", isExterior, transitionCurve);
			ShaderConst.HDR.LotteData.w = TheSettingManager->GetSettingTransition("Shaders.HDR", "LotteShoulder", isExterior, transitionCurve);
		}

		if (Effects.ImageAdjust->Enabled) {
			ShaderConst.ImageAdjust.Data.x = TheSettingManager->GetSettingTransition("Shaders.ImageAdjust", "Brightness", isExterior, isDayTime);
			ShaderConst.ImageAdjust.Data.y = TheSettingManager->GetSettingTransition("Shaders.ImageAdjust", "Contrast", isExterior, isDayTime);
			ShaderConst.ImageAdjust.Data.z = TheSettingManager->GetSettingTransition("Shaders.ImageAdjust", "Saturation", isExterior, isDayTime);
			ShaderConst.ImageAdjust.Data.w = TheSettingManager->GetSettingTransition("Shaders.ImageAdjust", "Strength", isExterior, isDayTime);

			ShaderConst.ImageAdjust.DarkColor.x = TheSettingManager->GetSettingTransition("Shaders.ImageAdjust", "DarkColorR", isExterior, isDayTime);
			ShaderConst.ImageAdjust.DarkColor.y = TheSettingManager->GetSettingTransition("Shaders.ImageAdjust", "DarkColorG", isExterior, isDayTime);
			ShaderConst.ImageAdjust.DarkColor.z = TheSettingManager->GetSettingTransition("Shaders.ImageAdjust", "DarkColorB", isExterior, isDayTime);

			ShaderConst.ImageAdjust.LightColor.x = TheSettingManager->GetSettingTransition("Shaders.ImageAdjust", "LightColorR", isExterior, isDayTime);
			ShaderConst.ImageAdjust.LightColor.y = TheSettingManager->GetSettingTransition("Shaders.ImageAdjust", "LightColorG", isExterior, isDayTime);
			ShaderConst.ImageAdjust.LightColor.z = TheSettingManager->GetSettingTransition("Shaders.ImageAdjust", "LightColorB", isExterior, isDayTime);
		}

		if (Effects.GodRays->Enabled) {
			float dayMult = TheSettingManager->GetSettingF("Shaders.GodRays.Main", "DayMultiplier");
			float nightMult = TheSettingManager->GetSettingF("Shaders.GodRays.Main", "NightMultiplier");
			ShaderConst.GodRays.Data.z = lerp(nightMult, dayMult, transitionCurve);

			if (TheSettingManager->GetSettingI("Shaders.GodRays.Main", "SunGlareEnabled")) {
				ShaderConst.GodRays.Ray.z *= ShaderConst.sunGlare;
				ShaderConst.GodRays.Ray.w *= ShaderConst.sunGlare;
			}
		}
	}


	if (Effects.Coloring->Enabled) {
		SettingsColoringStruct* scs = TheSettingManager->GetSettingsColoring(currentCell->GetEditorName());

		if (!scs && isExterior) scs = TheSettingManager->GetSettingsColoring(currentWorldSpace->GetEditorName());
		if (!scs) scs = TheSettingManager->GetSettingsColoring("Default");
		ShaderConst.Coloring.Data.x = scs->Strength;
		ShaderConst.Coloring.Data.y = scs->BaseGamma;
		ShaderConst.Coloring.Data.z = scs->Fade;
		ShaderConst.Coloring.Data.w = scs->Contrast;
		ShaderConst.Coloring.Values.x = scs->Saturation;
		ShaderConst.Coloring.Values.y = scs->Bleach;
		ShaderConst.Coloring.Values.z = scs->BleachLuma;
		ShaderConst.Coloring.Values.w = scs->Linearization;
		ShaderConst.Coloring.ColorCurve.x = scs->ColorCurve;
		ShaderConst.Coloring.ColorCurve.y = scs->ColorCurveR;
		ShaderConst.Coloring.ColorCurve.z = scs->ColorCurveG;
		ShaderConst.Coloring.ColorCurve.w = scs->ColorCurveB;
		ShaderConst.Coloring.EffectGamma.x = scs->EffectGamma;
		ShaderConst.Coloring.EffectGamma.y = scs->EffectGammaR;
		ShaderConst.Coloring.EffectGamma.z = scs->EffectGammaG;
		ShaderConst.Coloring.EffectGamma.w = scs->EffectGammaB;
	}

	if (Effects.BloodLens->Enabled) {
		if (ShaderConst.BloodLens.Percent > 0.0f) {
			ShaderConst.BloodLens.Time.z = TheSettingManager->GetSettingF("Shaders.BloodLens.Main", "Time");
			if (ShaderConst.BloodLens.Percent == 1.0f) {
				ShaderConst.BloodLens.Time.w = 0.0f;
				srand(time(NULL));
				ShaderConst.BloodLens.Params.x = (double)rand() / (RAND_MAX + 1) * (0.75f - 0.25f) + 0.25f; //from 0.25 to 0.75
				ShaderConst.BloodLens.Params.y = (double)rand() / (RAND_MAX + 1) * (0.5f + 0.1f) - 0.1f; //from -0.1 to 0.5
				ShaderConst.BloodLens.Params.z = (double)rand() / (RAND_MAX + 1) * (2.0f + 2.0f) - 2.0f; //from -2 to 2
			}
			ShaderConst.BloodLens.Time.w += 1.0f;
			ShaderConst.BloodLens.Percent = 1.0f - ShaderConst.BloodLens.Time.w / ShaderConst.BloodLens.Time.z;
			if (ShaderConst.BloodLens.Percent < 0.0f)
				ShaderConst.BloodLens.Percent = 0.0f;
			ShaderConst.BloodLens.Params.w = TheSettingManager->GetSettingF("Shaders.BloodLens.Main", "Intensity") * ShaderConst.BloodLens.Percent;
			ShaderConst.BloodLens.BloodColor.x = TheSettingManager->GetSettingF("Shaders.BloodLens.Main", "ColorR");
			ShaderConst.BloodLens.BloodColor.y = TheSettingManager->GetSettingF("Shaders.BloodLens.Main", "ColorG");
			ShaderConst.BloodLens.BloodColor.z = TheSettingManager->GetSettingF("Shaders.BloodLens.Main", "ColorB");
		}
	}

	if (Effects.LowHF->Enabled) {
		float PlayerHealthPercent = (float)Player->GetActorValue(Actor::ActorVal::kActorVal_Health) / (float)Player->GetBaseActorValue(Actor::ActorVal::kActorVal_Health);
		float PlayerFatiguePercent = (float)Player->GetActorValue(Actor::ActorVal::kActorVal_Stamina) / (float)Player->GetBaseActorValue(Actor::ActorVal::kActorVal_Stamina);

		ShaderConst.LowHF.Data.x = 0.0f;
		ShaderConst.LowHF.Data.y = 0.0f;
		ShaderConst.LowHF.Data.z = 0.0f;
		ShaderConst.LowHF.Data.w = 0.0f;

		float healthLimit = TheSettingManager->GetSettingF("Shaders.LowHF.Main", "HealthLimit");
		if (Player->IsAlive()) {
			ShaderConst.LowHF.HealthCoeff = 1.0f - PlayerHealthPercent / healthLimit;
			ShaderConst.LowHF.FatigueCoeff = 1.0f - PlayerFatiguePercent / TheSettingManager->GetSettingF("Shaders.LowHF.Main", "FatigueLimit");
			if (PlayerHealthPercent < healthLimit) {
				ShaderConst.LowHF.Data.x = ShaderConst.LowHF.HealthCoeff * TheSettingManager->GetSettingF("Shaders.LowHF.Main", "LumaMultiplier");
				ShaderConst.LowHF.Data.y = ShaderConst.LowHF.HealthCoeff * 0.01f * TheSettingManager->GetSettingF("Shaders.LowHF.Main", "BlurMultiplier");
				ShaderConst.LowHF.Data.z = ShaderConst.LowHF.HealthCoeff * 20.0f * TheSettingManager->GetSettingF("Shaders.LowHF.Main", "VignetteMultiplier");
				ShaderConst.LowHF.Data.w = (1.0f - ShaderConst.LowHF.HealthCoeff) * TheSettingManager->GetSettingF("Shaders.LowHF.Main", "DarknessMultiplier");
			}
			if (!ShaderConst.LowHF.Data.x && PlayerFatiguePercent < TheSettingManager->GetSettingF("Shaders.LowHF.Main", "FatigueLimit"))
				ShaderConst.LowHF.Data.x = ShaderConst.LowHF.FatigueCoeff * TheSettingManager->GetSettingF("Shaders.LowHF.Main", "LumaMultiplier");
		}
	}

	if (Effects.DepthOfField->Enabled) {
		avglumaRequired = true;

		sectionName = "Shaders.DepthOfField.FirstPersonView";
		if (TheCameraManager->IsVanity())
			sectionName = "Shaders.DepthOfField.VanityView";
		else if (IsThirdPersonView)
			sectionName = "Shaders.DepthOfField.ThirdPersonView";

		int Mode = TheSettingManager->GetSettingI(sectionName, "Mode");
		bool dofActive = TheSettingManager->GetSettingI(sectionName, "Enabled");

		// disable based on settings/context
		if ((Mode == 1 && (isDialog || isPersuasion)) ||
			(Mode == 2 && (!isDialog)) ||
			(Mode == 3 && (!isPersuasion)) ||
			(Mode == 4 && (!isDialog || !isPersuasion))) dofActive = false;

		ShaderConst.DepthOfField.Enabled = dofActive;

		if (dofActive) {
			ShaderConst.DepthOfField.Blur.x = TheSettingManager->GetSettingF(sectionName, "DistantBlur");
			ShaderConst.DepthOfField.Blur.y = TheSettingManager->GetSettingF(sectionName, "DistantBlurStartRange");
			ShaderConst.DepthOfField.Blur.z = TheSettingManager->GetSettingF(sectionName, "DistantBlurEndRange");
			ShaderConst.DepthOfField.Blur.w = TheSettingManager->GetSettingF(sectionName, "BaseBlurRadius");
			ShaderConst.DepthOfField.Data.x = TheSettingManager->GetSettingF(sectionName, "BlurFallOff");
			ShaderConst.DepthOfField.Data.y = TheSettingManager->GetSettingF(sectionName, "Radius");
			ShaderConst.DepthOfField.Data.z = TheSettingManager->GetSettingF(sectionName, "DiameterRange");
			ShaderConst.DepthOfField.Data.w = TheSettingManager->GetSettingF(sectionName, "NearBlurCutOff");
		}
	}

	ShaderConst.Cinema.Data.x = 1.0f; // set cinema aspect ratio to native ar
	if (Effects.Cinema->Enabled) {
		int Mode = TheSettingManager->GetSettingI("Shaders.Cinema.Main", "Mode");
		float aspectRatio = TheSettingManager->GetSettingF("Shaders.Cinema.Main", "AspectRatio");

		// disable based on settings/context
		if ((Mode == 1 && (isDialog || isPersuasion)) ||
			(Mode == 2 && (!isDialog)) || 
			(Mode == 3 && (!isPersuasion)) ||
			(Mode == 4 && (!isDialog && !isPersuasion))) aspectRatio = 1.0f;

		ShaderConst.Cinema.Data.x = aspectRatio;
	}

	// camera/position change data
	if (Effects.MotionBlur->Enabled) {
		sectionName = "Shaders.MotionBlur.FirstPersonView";
		if (IsThirdPersonView) sectionName = "Shaders.MotionBlur.ThirdPersonView";

		float AngleZ = D3DXToDegree(Player->rot.z);
		float AngleX = D3DXToDegree(Player->rot.x);
		float fMotionBlurAmtX = ShaderConst.MotionBlur.oldAngleZ - AngleZ;
		float fMotionBlurAmtY = ShaderConst.MotionBlur.oldAngleX - AngleX;
		float fBlurDistScratchpad = fMotionBlurAmtX + 360.0f;
		float fBlurDistScratchpad2 = (AngleZ - ShaderConst.MotionBlur.oldAngleZ + 360.0f) * -1.0f;

		if (abs(fMotionBlurAmtX) > abs(fBlurDistScratchpad))
			fMotionBlurAmtX = fBlurDistScratchpad;
		else if (abs(fMotionBlurAmtX) > abs(fBlurDistScratchpad2))
			fMotionBlurAmtX = fBlurDistScratchpad2;

		if (pow(fMotionBlurAmtX, 2) + pow(fMotionBlurAmtY, 2) < TheSettingManager->GetSettingF(sectionName, "BlurCutOff")) {
			fMotionBlurAmtX = 0.0f;
			fMotionBlurAmtY = 0.0f;
		}

		ShaderConst.MotionBlur.Data.x = (ShaderConst.MotionBlur.oldoldAmountX + ShaderConst.MotionBlur.oldAmountX + fMotionBlurAmtX) / 3.0f;
		ShaderConst.MotionBlur.Data.y = (ShaderConst.MotionBlur.oldoldAmountY + ShaderConst.MotionBlur.oldAmountY + fMotionBlurAmtY) / 3.0f;
		ShaderConst.MotionBlur.oldAngleZ = AngleZ;
		ShaderConst.MotionBlur.oldAngleX = AngleX;
		ShaderConst.MotionBlur.oldoldAmountX = ShaderConst.MotionBlur.oldAmountX;
		ShaderConst.MotionBlur.oldoldAmountY = ShaderConst.MotionBlur.oldAmountY;
		ShaderConst.MotionBlur.oldAmountX = fMotionBlurAmtX;
		ShaderConst.MotionBlur.oldAmountY = fMotionBlurAmtY;
		ShaderConst.MotionBlur.BlurParams.x = TheSettingManager->GetSettingF(sectionName, "GaussianWeight");
		ShaderConst.MotionBlur.BlurParams.y = TheSettingManager->GetSettingF(sectionName, "BlurScale");
		ShaderConst.MotionBlur.BlurParams.z = TheSettingManager->GetSettingF(sectionName, "BlurOffsetMax");
	}

	if (Effects.Lens->Enabled) {
		avglumaRequired = true;

		ShaderConst.Lens.Data.x = TheSettingManager->GetSettingF("Shaders.Lens.Main", "DirtLensAmount");
		if (isExterior)	ShaderConst.Lens.Data.y = lerp(TheSettingManager->GetSettingF("Shaders.Lens.Main", "NightBloomTreshold"), TheSettingManager->GetSettingF("Shaders.Lens.Main", "ExteriorBloomTreshold"), isDayTime);
		else ShaderConst.Lens.Data.y = TheSettingManager->GetSettingF("Shaders.Lens.Main", "InteriorBloomTreshold");
	}

	if (Effects.Exposure->Enabled) {
		avglumaRequired = true; // mark average luma calculation as necessary
		if (TheSettingManager->SettingsChanged || isDayTimeChanged) {
			ShaderConst.Exposure.Data.x = TheSettingManager->GetSettingTransition("Shaders.Exposure", "MinBrightness", isExterior, isDayTime);
			ShaderConst.Exposure.Data.y = TheSettingManager->GetSettingTransition("Shaders.Exposure", "MaxBrightness", isExterior, isDayTime);
			ShaderConst.Exposure.Data.z = TheSettingManager->GetSettingTransition("Shaders.Exposure", "DarkAdaptSpeed", isExterior, isDayTime);
			ShaderConst.Exposure.Data.w = TheSettingManager->GetSettingTransition("Shaders.Exposure", "LightAdaptSpeed", isExterior, isDayTime);
		}
	}

	if (Effects.Specular->Enabled) {
		float rainyPercent = ShaderConst.Animators.RainAnimator.GetValue();
		const char* ext = "Shaders.Specular.Exterior";
		const char* rain = "Shaders.Specular.Rain";

		// handle transition by interpolating previous and current weather settings
		ShaderConst.Specular.Data.x = lerp(TheSettingManager->GetSettingF(ext, "SpecLumaTreshold"), TheSettingManager->GetSettingF(rain, "SpecLumaTreshold"), rainyPercent);
		ShaderConst.Specular.Data.y = lerp(TheSettingManager->GetSettingF(ext, "BlurMultiplier"), TheSettingManager->GetSettingF(rain, "BlurMultiplier"), rainyPercent);
		ShaderConst.Specular.Data.z = lerp(TheSettingManager->GetSettingF(ext, "Glossiness"), TheSettingManager->GetSettingF(rain, "Glossiness"), rainyPercent);
		ShaderConst.Specular.Data.w = lerp(TheSettingManager->GetSettingF(ext, "DistanceFade"), TheSettingManager->GetSettingF(rain, "DistanceFade"), rainyPercent);
		ShaderConst.Specular.EffectStrength.x = lerp(TheSettingManager->GetSettingF(ext, "SpecularStrength"), TheSettingManager->GetSettingF(rain, "SpecularStrength"), rainyPercent);
		ShaderConst.Specular.EffectStrength.y = lerp(TheSettingManager->GetSettingF(ext, "SkyTintStrength"), TheSettingManager->GetSettingF(rain, "SkyTintStrength"), rainyPercent);
		ShaderConst.Specular.EffectStrength.z = lerp(TheSettingManager->GetSettingF(ext, "FresnelStrength"), TheSettingManager->GetSettingF(rain, "FresnelStrength"), rainyPercent);
		ShaderConst.Specular.EffectStrength.w = lerp(TheSettingManager->GetSettingF(ext, "SkyTintSaturation"), TheSettingManager->GetSettingF(rain, "SkyTintSaturation"), rainyPercent);
	}

	TheSettingManager->SettingsChanged = false;
	timer.LogTime("ShaderManager::UpdateConstants");
}

bool ShaderManager::CreateShader(const char* Name) {
	
	NiD3DVertexShader** Vertex = NULL;
	NiD3DPixelShader** Pixel = NULL;
	int WaterVertexShadersSize = sizeof(WaterVertexShaders) / 4;
	int WaterPixelShadersSize = sizeof(WaterPixelShaders) / 4;
	int Upperbound = 0;
	bool success = true;

	if (!strcmp(Name, "Terrain")) {
		Upperbound = GetShader(Name, &Vertex, NULL, 0);
		for (int i = 0; i < Upperbound; i++) if (Vertex[i] && strstr(TerrainShaders, ((NiD3DVertexShaderEx*)Vertex[i])->ShaderName)) success = LoadShader(Vertex[i]) && success;
		Upperbound = GetShader(Name, &Pixel, NULL, 0);
		for (int i = 0; i < Upperbound; i++) if (Pixel[i] && strstr(TerrainShaders, ((NiD3DPixelShaderEx*)Pixel[i])->ShaderName)) success = LoadShader(Pixel[i]) && success;
	}
	else if (!strcmp(Name, "ExtraShaders")) {
		Upperbound = GetShader(Name, &Vertex, NULL, 0);
		for (int i = 0; i < Upperbound; i++) if (Vertex[i] && !strstr(TerrainShaders, ((NiD3DVertexShaderEx*)Vertex[i])->ShaderName)) success = LoadShader(Vertex[i]) && success;
		Upperbound = GetShader(Name, &Pixel, NULL, 0);
		for (int i = 0; i < Upperbound; i++) if (Pixel[i] && !strstr(TerrainShaders, ((NiD3DPixelShaderEx*)Pixel[i])->ShaderName)) success = LoadShader(Pixel[i]) && success;
	}
	else {
		Upperbound = GetShader(Name, &Vertex, WaterVertexShaders, WaterVertexShadersSize);
		for (int i = 0; i < Upperbound; i++) if (Vertex[i]) success = LoadShader(Vertex[i]) && success;
		Upperbound = GetShader(Name, &Pixel, WaterPixelShaders, WaterPixelShadersSize);
		for (int i = 0; i < Upperbound; i++) if (Pixel[i]) success = LoadShader(Pixel[i]) && success;
		if (!strcmp(Name, "Water")) {
			Upperbound = GetShader("WaterHeightMap", &Vertex, WaterVertexShaders, WaterVertexShadersSize);
			for (int i = 0; i < Upperbound; i++) if (Vertex[i]) success = LoadShader(Vertex[i]) && success;
			Upperbound = GetShader("WaterHeightMap", &Pixel, WaterPixelShaders, WaterPixelShadersSize);
			for (int i = 0; i < Upperbound; i++) if (Pixel[i]) success = LoadShader(Pixel[i]) && success;
			Upperbound = GetShader("WaterDisplacement", &Vertex, WaterVertexShaders, WaterVertexShadersSize);
			for (int i = 0; i < Upperbound; i++) if (Vertex[i]) success = LoadShader(Vertex[i]) && success;
			Upperbound = GetShader("WaterDisplacement", &Pixel, WaterPixelShaders, WaterPixelShadersSize);
			for (int i = 0; i < Upperbound; i++) if (Pixel[i]) success = LoadShader(Pixel[i]) && success;
		}
	}

	if (!success) {
		char Message[256] = "Error: Could not load shader ";
		strcat(Message, Name);
		InterfaceManager->ShowMessage(Message);
		Logger::Log(Message);
	}

	return success;
}

/*
* Load generic Vertex Shaders as well as the ones for interiors and exteriors if the exist. Returns false if generic one isn't found (as other ones are optional)
*/
bool ShaderManager::LoadShader(NiD3DVertexShader* Shader) {
	
	NiD3DVertexShaderEx* VertexShader = (NiD3DVertexShaderEx*)Shader;
	
	// Load generic, interior and exterior shaders
	VertexShader->ShaderProg  = (ShaderRecordVertex*)ShaderRecord::LoadShader(VertexShader->ShaderName, NULL);
	VertexShader->ShaderProgE = (ShaderRecordVertex*)ShaderRecord::LoadShader(VertexShader->ShaderName, "Exteriors\\");
	VertexShader->ShaderProgI = (ShaderRecordVertex*)ShaderRecord::LoadShader(VertexShader->ShaderName, "Interiors\\");

	return VertexShader->ShaderProg != nullptr;
}

/*
* Load generic Pixel Shaders as well as the ones for interiors and exteriors if the exist. Returns false if generic one isn't found (as other ones are optional)
*/
bool ShaderManager::LoadShader(NiD3DPixelShader* Shader) {

	NiD3DPixelShaderEx* PixelShader = (NiD3DPixelShaderEx*)Shader;

	PixelShader->ShaderProg  = (ShaderRecordPixel*)ShaderRecord::LoadShader(PixelShader->ShaderName, NULL);
	PixelShader->ShaderProgE = (ShaderRecordPixel*)ShaderRecord::LoadShader(PixelShader->ShaderName, "Exteriors\\");
	PixelShader->ShaderProgI = (ShaderRecordPixel*)ShaderRecord::LoadShader(PixelShader->ShaderName, "Interiors\\");

	return PixelShader->ShaderProg != nullptr;
}

void ShaderManager::DisposeShader(const char* Name) {

	NiD3DVertexShader** Vertex = NULL;
	NiD3DPixelShader** Pixel = NULL;
	int WaterVertexShadersSize = sizeof(WaterVertexShaders) / 4;
	int WaterPixelShadersSize = sizeof(WaterPixelShaders) / 4;
	int Upperbound = 0;

	if (!strcmp(Name, "Terrain")) {
		Upperbound = GetShader(Name, &Vertex, NULL, 0);
		for (int i = 0; i < GetShader(Name, &Vertex, NULL, 0); i++) if (Vertex[i] && strstr(TerrainShaders, ((NiD3DVertexShaderEx*)Vertex[i])->ShaderName)) ((NiD3DVertexShaderEx*)Vertex[i])->DisposeShader();
		Upperbound = GetShader(Name, &Pixel, NULL, 0);
		for (int i = 0; i < GetShader(Name, &Pixel, NULL, 0); i++) if (Pixel[i] && strstr(TerrainShaders, ((NiD3DPixelShaderEx*)Pixel[i])->ShaderName)) ((NiD3DPixelShaderEx*)Pixel[i])->DisposeShader();
	}
	else if (!strcmp(Name, "ExtraShaders")) {
		Upperbound = GetShader(Name, &Vertex, NULL, 0);
		for (int i = 0; i < GetShader(Name, &Vertex, NULL, 0); i++) if (Vertex[i] && !strstr(TerrainShaders, ((NiD3DVertexShaderEx*)Vertex[i])->ShaderName)) ((NiD3DVertexShaderEx*)Vertex[i])->DisposeShader();
		Upperbound = GetShader(Name, &Pixel, NULL, 0);
		for (int i = 0; i < GetShader(Name, &Pixel, NULL, 0); i++) if (Pixel[i] && !strstr(TerrainShaders, ((NiD3DPixelShaderEx*)Pixel[i])->ShaderName)) ((NiD3DPixelShaderEx*)Pixel[i])->DisposeShader();
	}
	else {
		Upperbound = GetShader(Name, &Vertex, WaterVertexShaders, WaterVertexShadersSize);
		for (int i = 0; i < GetShader(Name, &Vertex, WaterVertexShaders, WaterVertexShadersSize); i++) if (Vertex[i]) ((NiD3DVertexShaderEx*)Vertex[i])->DisposeShader();
		Upperbound = GetShader(Name, &Pixel, WaterPixelShaders, WaterPixelShadersSize);
		for (int i = 0; i < GetShader(Name, &Pixel, WaterPixelShaders, WaterPixelShadersSize); i++) if (Pixel[i]) ((NiD3DPixelShaderEx*)Pixel[i])->DisposeShader();
		if (!strcmp(Name, "Water")) {
			Upperbound = GetShader("WaterHeightMap", &Vertex, WaterVertexShaders, WaterVertexShadersSize);
			for (int i = 0; i < GetShader("WaterHeightMap", &Vertex, WaterVertexShaders, WaterVertexShadersSize); i++) if (Vertex[i]) ((NiD3DVertexShaderEx*)Vertex[i])->DisposeShader();
			Upperbound = GetShader("WaterHeightMap", &Pixel, WaterPixelShaders, WaterPixelShadersSize);
			for (int i = 0; i < GetShader("WaterHeightMap", &Pixel, WaterPixelShaders, WaterPixelShadersSize); i++) if (Pixel[i]) ((NiD3DPixelShaderEx*)Pixel[i])->DisposeShader();
			Upperbound = GetShader("WaterDisplacement", &Vertex, WaterVertexShaders, WaterVertexShadersSize);
			for (int i = 0; i < GetShader("WaterDisplacement", &Vertex, WaterVertexShaders, WaterVertexShadersSize); i++) if (Vertex[i]) ((NiD3DVertexShaderEx*)Vertex[i])->DisposeShader();
			Upperbound = GetShader("WaterDisplacement", &Pixel, WaterPixelShaders, WaterPixelShadersSize);
			for (int i = 0; i < GetShader("WaterDisplacement", &Pixel, WaterPixelShaders, WaterPixelShadersSize); i++) if (Pixel[i]) ((NiD3DPixelShaderEx*)Pixel[i])->DisposeShader();
		}
	}

}

/*
* Loads an Effect Shader from the corresponding fx file based on the Effect Record effect Type.
*/
EffectRecord* ShaderManager::CreateEffect(const char* Name, bool setEnabled) {
	
	char Filename[MAX_PATH];
	strcpy(Filename, EffectsPath);
	strcat(Filename, Name);
	strcat(Filename, ".fx");

	EffectRecord* effect = EffectRecord::LoadEffect(Filename);
	if (!effect->IsLoaded()) setEnabled = false;

	effect->Enabled = setEnabled;
	return effect;

	// TODO: simplify and streamline extra shaders creation
	//SettingsMainStruct* SettingsMain = &TheSettingManager->SettingsMain;
	//	case EffectRecord::EffectRecordType::Extra:
	//		WIN32_FIND_DATAA File;
	//		HANDLE H;
	//		char* cFileName = NULL;
	//		EffectRecord* ExtraEffect = NULL;

	//		if (SettingsMain->Develop.CompileEffects)
	//			strcat(Filename, "Extra\\*.hlsl");
	//		else
	//			strcat(Filename, "Extra\\*.fx");
	//		H = FindFirstFileA((LPCSTR)Filename, &File);
	//		if (H != INVALID_HANDLE_VALUE) {
	//			cFileName = (char*)File.cFileName;
	//			if (SettingsMain->Develop.CompileEffects) File.cFileName[strlen(cFileName) - 5] = '\0';
	//			strcpy(Filename, EffectsPath);
	//			strcat(Filename, "Extra\\");
	//			strcat(Filename, cFileName);
	//			ExtraEffect = EffectRecord::LoadEffect(Filename);
	//			if (ExtraEffect) ExtraEffects[std::string(cFileName).substr(0, strlen(cFileName) - 3)] = ExtraEffect;
	//			while (FindNextFileA(H, &File)) {
	//				cFileName = (char*)File.cFileName;
	//				if (SettingsMain->Develop.CompileEffects) File.cFileName[strlen(cFileName) - 5] = '\0';
	//				strcpy(Filename, EffectsPath);
	//				strcat(Filename, "Extra\\");
	//				strcat(Filename, cFileName);
	//				ExtraEffect = EffectRecord::LoadEffect(Filename);
	//				if (ExtraEffect) ExtraEffects[std::string(cFileName).substr(0, strlen(cFileName) - 3)] = ExtraEffect;
	//			}
	//			FindClose(H);
	//		}
	//		break;
	//}
}

/*
* Deletes an Effect based on the Effect Record effect type. 
*/
void ShaderManager::DisposeEffect(EffectRecord** Effect) {
	*Effect = NULL;
	delete* Effect;

	//	case EffectRecord::EffectRecordType::Extra:
	//		ExtraEffectsList::iterator v = ExtraEffects.begin();
	//		while (v != ExtraEffects.end()) {
	//			delete v->second;
	//			v++;
	//		}
	//		ExtraEffects.clear();
	//		break;
	//}
}

/*
* Renders a given effect to an arbitrary render target
*/
void ShaderManager::RenderEffectToRT(IDirect3DSurface9* RenderTarget, EffectRecord* Effect, bool clearRenderTarget) {
	IDirect3DDevice9* Device = TheRenderManager->device;
	Device->SetRenderTarget(0, RenderTarget);
	Device->StretchRect(RenderTarget, NULL, RenderTarget, NULL, D3DTEXF_NONE);
	Effect->Render(Device, RenderTarget, RenderTarget, 0, clearRenderTarget, NULL);
};


void ShaderManager::RenderEffectsPreTonemapping(IDirect3DSurface9* RenderTarget) {
	TheRenderManager->UpdateSceneCameraData();
	TheRenderManager->SetupSceneCamera();

	if (!TheSettingManager->GetSettingI("Main.Main.Misc", "RenderEffects")) return; // Main toggle

	auto timer = TimeLogger();

	IDirect3DDevice9* Device = TheRenderManager->device;
	NiDX9RenderState* RenderState = TheRenderManager->renderState;
	IDirect3DSurface9* SourceSurface = TheTextureManager->SourceSurface;
	IDirect3DSurface9* RenderedSurface = TheTextureManager->RenderedSurface;

	// prepare device for effects
	Device->SetStreamSource(0, FrameVertex, 0, sizeof(FrameVS));
	Device->SetFVF(FrameFVF);

	// Disable Depth render state settings and enable full color
	RenderState->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_ZWRITEENABLE, D3DZB_FALSE, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_COLORWRITEENABLE, 15, RenderStateArgs);

	// render post process normals for use by shaders
	RenderEffectToRT(TheTextureManager->NormalsSurface, Effects.Normals, false);

	// render a shadow pass for point lights
	if ((isExterior && Effects.ShadowsExteriors->Enabled) || (!isExterior && Effects.ShadowsInteriors->Enabled)) {
		// separate lights in 2 batches
		RenderEffectToRT(TheTextureManager->ShadowPassSurface, Effects.PointShadows, true);
		if (TheShadowManager->PointLightsNum > 6) RenderEffectToRT(TheTextureManager->ShadowPassSurface, Effects.PointShadows2, false);
		if (isExterior) RenderEffectToRT(TheTextureManager->ShadowPassSurface, Effects.SunShadows, false);
	}

	Device->SetRenderTarget(0, RenderTarget);

	if (!Player->parentCell) return;
	if (OverlayIsOn) return; // disable all effects during terminal/lockpicking sequences because they bleed through the overlay

	// copy the source render target to both the rendered and source textures (rendered gets updated after every pass, source once per effect)
	Device->StretchRect(RenderTarget, NULL, RenderedSurface, NULL, D3DTEXF_NONE);
	Device->StretchRect(RenderTarget, NULL, SourceSurface, NULL, D3DTEXF_NONE);

	Effects.AmbientOcclusion->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);

	if (isExterior) Effects.ShadowsExteriors->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);
	else Effects.ShadowsInteriors->Render(Device, RenderTarget, RenderedSurface, 0, true, SourceSurface);

	if (isUnderwater) {
		// underwater only effects
		Effects.Underwater->Render(Device, RenderTarget, RenderedSurface, 0, false, NULL);
	} else {
		if (isExterior) {
			Effects.Specular->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);
			if (ShaderConst.WetWorld.Data.z > 0.0f && !VATSIsOn) Effects.WetWorld->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);
			if (ShaderConst.SnowAccumulation.Params.w > 0.0f) Effects.SnowAccumulation->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);
		}

		if (!PipBoyIsOn) Effects.VolumetricFog->Render(Device, RenderTarget, RenderedSurface, 0, false, NULL);
	}

	Effects.Bloom->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);

	// Restore render state settings
	RenderState->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_ZWRITEENABLE, D3DZB_TRUE, RenderStateArgs);

	timer.LogTime("ShaderManager::RenderEffectsPreTonemapping");
}


/*
* Renders the effect that have been set to enabled.
*/
void ShaderManager::RenderEffects(IDirect3DSurface9* RenderTarget) {
	if (!TheSettingManager->GetSettingI("Main.Main.Misc", "RenderEffects")) return; // Main toggle
	if (!Player->parentCell) return;
	if (OverlayIsOn) return; // disable all effects during terminal/lockpicking sequences because they bleed through the overlay

	auto timer = TimeLogger();

	TheRenderManager->UpdateSceneCameraData();
	TheRenderManager->SetupSceneCamera();

	IDirect3DDevice9* Device = TheRenderManager->device;
	NiDX9RenderState* RenderState = TheRenderManager->renderState;
	IDirect3DSurface9* SourceSurface = TheTextureManager->SourceSurface;
	IDirect3DSurface9* RenderedSurface = TheTextureManager->RenderedSurface;

	Device->SetStreamSource(0, FrameVertex, 0, sizeof(FrameVS));
	Device->SetFVF(FrameFVF);

	// prepare device for effects
	Device->SetRenderTarget(0, RenderTarget);

	// copy the source render target to both the rendered and source textures (rendered gets updated after every pass, source once per effect)
	Device->StretchRect(RenderTarget, NULL, RenderedSurface, NULL, D3DTEXF_NONE);
	Device->StretchRect(RenderTarget, NULL, SourceSurface, NULL, D3DTEXF_NONE);

	if (!isUnderwater && isExterior) {
		Effects.GodRays->Render(Device, RenderTarget, RenderedSurface, 0, true, SourceSurface);
		if (ShaderConst.Rain.RainData.x > 0.0f) Effects.Rain->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);
		if (ShaderConst.Snow.SnowData.x > 0.0f) Effects.Snow->Render(Device, RenderTarget, RenderedSurface, 0, false, NULL);
	}

	// calculate average luma for use by shaders
	if (avglumaRequired) {
		RenderEffectToRT(TheTextureManager->AvgLumaSurface, Effects.AvgLuma, NULL);
		Device->SetRenderTarget(0, RenderTarget); 	// restore device used for effects
	}

	Effects.Exposure->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);

	// screenspace coloring/blurring effects get rendered last
	Effects.Coloring->Render(Device, RenderTarget, RenderedSurface, 0, false, NULL);
	if (ShaderConst.DepthOfField.Enabled) Effects.DepthOfField->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);
	if (ShaderConst.MotionBlur.Data.x || ShaderConst.MotionBlur.Data.y) Effects.MotionBlur->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);

	// lens effects
	if (ShaderConst.BloodLens.Percent > 0.0f) Effects.BloodLens->Render(Device, RenderTarget, RenderedSurface, 0, false, NULL);
	if (ShaderConst.WaterLens.Percent > 0.0f) Effects.WaterLens->Render(Device, RenderTarget, RenderedSurface, 0, false, NULL);
	if (ShaderConst.LowHF.Data.x) Effects.LowHF->Render(Device, RenderTarget, RenderedSurface, 0, false, NULL);
	if (!isUnderwater) Effects.Lens->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);
	Effects.Sharpening->Render(Device, RenderTarget, RenderedSurface, 0, false, NULL);

	// cinema effect gets rendered very last because of vignetting/letterboxing
	Effects.Cinema->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);

	// final adjustments
	Effects.ImageAdjust->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);

	// debug shader allows to display some of the buffers
	Effects.Debug->Render(Device, RenderTarget, RenderedSurface, 0, false, NULL);

	//if (EffectsSettings->Extra) {
	//	for (EffectsList::iterator iter = Effects.ExtraEffects.begin(); iter != Effects.ExtraEffects.end(); ++iter) {
	//		if (iter->second->Enabled) {
	//			iter->second->Render(Device, RenderTarget, RenderedSurface, false, true);
	//		}
	//	}
	//}

	timer.LogTime("ShaderManager::RenderEffects");
}

/*
* Writes the settings corresponding to the shader/effect name, to switch it between enabled/disabled.*
* Also creates or deletes the corresponding Effect Record.
*/
void ShaderManager::SwitchShaderStatus(const char* Name) {
	IsMenuSwitch = true;

	// effects
	try {
		EffectRecord* effect = *EffectsNames.at(Name);
		bool setting = effect->SwitchEffect();
		TheSettingManager->SetMenuShaderEnabled(Name, setting);
	}
	catch (const std::exception& e){
		// shaders
		Logger::Log("Toggling Shader %s", Name);
		bool enable = !TheSettingManager->GetMenuShaderEnabled(Name);
		DisposeShader(Name);
		if (enable) CreateShader(Name);
		TheSettingManager->SetMenuShaderEnabled(Name, enable);
	}

	//else if (!strcmp(Name, "ExtraEffectsSettings")) { //TODO change to new effect switch
	//	EffectsSettings->Extra = !EffectsSettings->Extra;
	//	DisposeEffect(EffectRecord::EffectRecordType::Extra);
	//	if (EffectsSettings->Extra) CreateEffect(EffectRecord::EffectRecordType::Extra);
	//}

	IsMenuSwitch = false;
}

void ShaderManager::SetCustomConstant(const char* Name, D3DXVECTOR4 Value) {
	CustomConstants::iterator v = CustomConst.find(std::string(Name));
	if (v != CustomConst.end()) v->second = Value;
}

void ShaderManager::SetExtraEffectEnabled(const char* Name, bool Value) {
	//EffectsList::iterator v = Effects.ExtraEffects.find(std::string(Name));
	//if (v != Effects.ExtraEffects.end()) v->second->Enabled = Value;
}

float ShaderManager::lerp(float a, float b, float t) {
	return std::lerp(a, b, t);
}

D3DXVECTOR4 ShaderManager::lerp(D3DXVECTOR4 a, D3DXVECTOR4 b, float t) {
	D3DXVECTOR4 result;
	result.x = std::lerp(a.x, b.x, t);
	result.y = std::lerp(a.y, b.y, t);
	result.z = std::lerp(a.z, b.z, t);
	result.w = std::lerp(a.w, b.w, t);

	return result;
}

float ShaderManager::invLerp(float a, float b, float t) {
	return(t - a) / (b - a);
}

float ShaderManager::clamp(float a, float b, float t) {
	return min(max(a, t), b);
}

float ShaderManager::smoothStep(float a, float b, float t) {
	t = clamp(0.0, 1.0, invLerp(a, b, t));
	return t * t * (3.0 - 2.0 * t);
}
