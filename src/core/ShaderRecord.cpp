#include "ShaderRecord.h"
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
	ClearSamplers = true;

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

	if (CompileStatus != ShaderCompileType::RecompileChanged) return false;

	WIN32_FILE_ATTRIBUTE_DATA attributesBin = { 0 };
	WIN32_FILE_ATTRIBUTE_DATA attributesSource = { 0 };
	BOOL hr = GetFileAttributesExA(fileBin, GetFileExInfoStandard, &attributesBin); // from winbase.h
	if (!hr) 
		return true; //File not present, compile

	hr = GetFileAttributesExA(fileHlsl, GetFileExInfoStandard, &attributesSource); // from winbase.h
	if (!hr) {
		Logger::Log("[ERROR] Can't Compile %s, source cannot be read", fileHlsl);
		return false;
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


void reportError(HRESULT result) {
	if (result == E_ABORT) Logger::Log("Operation aborted");
	if (result == E_ACCESSDENIED) Logger::Log("Access Denied");
	if (result == E_FAIL) Logger::Log("Operation Failed");
	if (result == E_HANDLE) Logger::Log("Handle invalid");
	if (result == E_INVALIDARG) Logger::Log("Argument invalid");
	if (result == E_NOINTERFACE) Logger::Log("Interface invalid");
	if (result == E_NOTIMPL) Logger::Log("Not implemented");
	if (result == E_OUTOFMEMORY) Logger::Log("Out of memory");
	if (result == E_POINTER) Logger::Log("Invalid pointer");
	if (result == E_UNEXPECTED) Logger::Log("Unexpected fail");
}

/*
Loads the shader by name from a given subfolder (optionally). Shader will be compiled if needed.
@returns the ShaderRecord for this shader.
*/
ShaderRecord* ShaderRecord::LoadShader(const char* Name, const char* SubPath, ShaderTemplate Template) {
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
	
	// Build filenames.
	strcpy(FileName, ShadersPath);
	if (SubPath) strcat(FileName, SubPath);
	strcat(FileName, Name);
	strcpy(FileNameBinary, FileName);

	// If we have a template, we want to use the corresponding input file.
	if (Template.Name != NULL) {
		memset(FileName, 0, MAX_PATH);
		strcpy(FileName, ShadersPath);
		if (SubPath) strcat(FileName, SubPath);
		strcat(FileName, Template.Name);
	}
	
	strcat(FileName, ".hlsl");

	HRESULT prepass = D3DXPreprocessShaderFromFileA(FileName, NULL, NULL, &ShaderSource, &Errors);
	bool Compile = ShouldCompileShader(FileNameBinary, FileName, (ShaderCompileType)TheSettingManager->SettingsMain.Develop.CompileShaders);
	if (prepass == D3D_OK) {

		if (strstr(Name, ".vso"))
			strcpy(ShaderProfile, "vs_3_0");
		else if (strstr(Name, ".pso"))
			strcpy(ShaderProfile, "ps_3_0");

		if (!Compile){
			std::ifstream FileBinary(FileNameBinary, std::ios::in | std::ios::binary | std::ios::ate);
			if (FileBinary.is_open()) {
				std::streamoff Size = FileBinary.tellg();
				D3DXCreateBuffer(Size, &Shader);
				FileBinary.seekg(0, std::ios::beg);
				Function = Shader->GetBufferPointer();
				FileBinary.read((char*)Function, Size);
				FileBinary.close();
				//D3DXGetShaderConstantTable((const DWORD*)Function, &ConstantTable);
			}
			else {
				Logger::Log("ERROR: Shader binary %s not found.", FileNameBinary);
			}
		}

		if (Compile || !Function) {
			// compile if option was enabled or compiled version not found
			D3DXMACRO* defines = NULL;
			if (Template.Name != NULL && TheRenderManager->IsReversedDepth()) {
				int i = 0;
				bool nullFound = false;
				while (!nullFound) {
					nullFound = Template.Defines[i].Name == NULL;
					if (!nullFound) i++;
				}
				Template.Defines[i] = {"REVERSED_DEPTH", ""};
				defines = &(Template.Defines[0]);
			}
			else if (Template.Name != NULL) {
				defines = &(Template.Defines[0]);
			}
			else if (TheRenderManager->IsReversedDepth()) {
				D3DXMACRO reversed_depth[2] = { {"REVERSED_DEPTH", ""} };
				defines = &(reversed_depth[0]);
			}

			D3DXCompileShaderFromFileA(FileName, defines, NULL, "main", ShaderProfile, NULL, &Shader, &Errors, &ConstantTable);
			if (Errors) Logger::Log((char*)Errors->GetBufferPointer());
			if (Shader) {
				Function = Shader->GetBufferPointer();
				std::ofstream FileBinary(FileNameBinary, std::ios::out | std::ios::binary);
				FileBinary.write((const char*)Function, Shader->GetBufferSize());
				FileBinary.flush();
				FileBinary.close();
				if (Template.Name == NULL)
					Logger::Log("Shader compiled: %s", FileName);
				else
					Logger::Log("Shader compiled: %s using template: %s", FileNameBinary, FileName);
			}
		}

		HRESULT get = D3DXGetShaderConstantTableEx((const DWORD*)Function, D3DXCONSTTABLE_LARGEADDRESSAWARE, &ConstantTable);

		if (FAILED(get)) {
			Logger::Log("Encountered an issue getting constant table for %s", Name);
			reportError(get);
		}
		else {
			D3DXCONSTANTTABLE_DESC ConstantTableDesc;
			get = ConstantTable->GetDesc(&ConstantTableDesc);
			reportError(get);

			if (FAILED(get)) {
				Logger::Log("Issues getting constants descriptions for %s", Name);
			}
			else {
				//Logger::Log("Compile time: Shader %s - %s has %i constants", Name, ConstantTableDesc.Creator, ConstantTableDesc.Constants);

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
		}
	}
	else {
		if (Errors) Logger::Log((char*)Errors->GetBufferPointer());
	}

	timer.LogTime("ShaderRecord::LoadShader");

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

	HRESULT get = ConstantTable->GetDesc(&ConstantTableDesc);
	if (FAILED(get)) Logger::Log("CreateCT : failed to get constants desc");
	reportError(get);

	//Logger::Log("CreateCT: Shader %s has %i constants", Name, ConstantTableDesc.Constants);
	//Logger::Log("%s", (const char*)ShaderSource->GetBufferPointer());

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
		Logger::Log("[ERROR] %s  cannot be binded: sampler index not found", Name);
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
		Logger::Log("[ERROR] during binding of %s : Samplerstate description not found", Name);
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

	// override default sampler states if the sampler string was found
	if (!SamplerString.empty()) Texture->GetSamplerStates(trim(SamplerString));

	timer.LogTime("ShaderTextureValue::GetTextureRecord");
}


/*
* Associates a found shader constant name to a D3DXVECTOR4 pointer from the ConstantsTable.
*/
void ShaderFloatValue::GetValueFromConstantTable() {
	std::string constantName = Name;
	std::map<std::string, D3DXVECTOR4*>::iterator iter = TheShaderManager->ConstantsTable.find(constantName);

	if (iter != TheShaderManager->ConstantsTable.end()) {
		Value = TheShaderManager->ConstantsTable.at(Name);
	}
	else {
		Logger::Log("Custom constant found: %s", Name);
		D3DXVECTOR4 v; v.x = v.y = v.z = v.w = 0.0f;
		TheShaderManager->CustomConst[Name] = v;
		Value = &TheShaderManager->CustomConst[Name];
	}
}


/*
* Sets the Constant Table for the shader
*/
void ShaderRecord::SetCT() {

	if (HasRenderedBuffer) TheRenderManager->device->StretchRect(TheRenderManager->currentRTGroup->RenderTargets[0]->data->Surface, NULL, TheTextureManager->RenderedSurface, NULL, D3DTEXF_NONE);
	if (HasDepthBuffer) {
		//Logger::Log("Resolving depth buffer for shader %s", Name);
		TheRenderManager->ResolveDepthBuffer(TheTextureManager->DepthTexture);
	}

	// reset samplers
	if (ClearSamplers) {
		for (int i = 0; i < 16; i++)
			TheRenderManager->renderState->SetTexture(i, nullptr);
	}
	
	// binds textures
	ShaderTextureValue* Sampler;
	for (UInt32 c = 0; c < TextureShaderValuesCount; c++) {
		Sampler = &TextureShaderValues[c];
		if (Sampler->Texture->Texture == nullptr) {
			Sampler->Texture->BindTexture(Sampler->Name);

			if (Sampler->Texture->Texture) Logger::Log("%s : Texture %s Successfully bound", Name, Sampler->Name);
			//else Logger::Log("[ERROR] : Could not bind texture %s for shader %s", Sampler->Name, Name);
		}

		if (Sampler->Texture->Texture != nullptr) {
			TheRenderManager->renderState->SetTexture(Sampler->RegisterIndex, Sampler->Texture->Texture);

			for (int i = 1; i < SamplerStatesMax; i++) {
				if (TheRenderManager->renderState->GetSamplerState(Sampler->RegisterIndex, (D3DSAMPLERSTATETYPE)i) != Sampler->Texture->SamplerStates[i])
					TheRenderManager->SetSamplerState(Sampler->RegisterIndex, (D3DSAMPLERSTATETYPE)i, Sampler->Texture->SamplerStates[i]);
			}
		}
		else {
			Logger::Log("%s : Texture %s was not bound", Name, Sampler->Name);
		}
	}

	// update constants
	ShaderFloatValue* Constant;
	for (UInt32 c = 0; c < FloatShaderValuesCount; c++) {
		Constant = &FloatShaderValues[c];

		if (Constant->Value == nullptr) Constant->GetValueFromConstantTable();
		if (Constant->Value == nullptr) {
			Logger::Log("[Error] %s : Couldn't get value for Constant %s", Name, Constant->Name);
			continue;
		}

		SetShaderConstantF(Constant->RegisterIndex, Constant->Value, Constant->RegisterCount); // TODO: for matrices, rows and columns are inverted because of the way this works.
	}
}


/*
* Hook - Function to replace shader handles during pass initialisation. 
*/
void NiD3DVertexShaderEx::SetupShader(IDirect3DVertexShader9* CurrentVertexHandle) {

	if ((!Enabled || !TheSettingManager->SettingsMain.Main.RenderEffects) && ShaderHandleBackup) {
		ShaderHandle = (IDirect3DVertexShader9*)ShaderHandleBackup;
		return;
	}

	if (GetShaderRecord(ShaderRecordType::Exterior) && Player->GetWorldSpace()) {
		ShaderHandle = GetShaderRecord(ShaderRecordType::Exterior)->ShaderHandle;
		if (CurrentVertexHandle != ShaderHandle) GetShaderRecord(ShaderRecordType::Exterior)->SetCT();
	}
	else if (GetShaderRecord(ShaderRecordType::Interior) && !Player->GetWorldSpace()) {
		ShaderHandle = GetShaderRecord(ShaderRecordType::Interior)->ShaderHandle;
		if (CurrentVertexHandle != ShaderHandle) GetShaderRecord(ShaderRecordType::Interior)->SetCT();
	}
	else if (GetShaderRecord(ShaderRecordType::Default)) {
		ShaderHandle = GetShaderRecord(ShaderRecordType::Default)->ShaderHandle;
		if (CurrentVertexHandle != ShaderHandle) GetShaderRecord(ShaderRecordType::Default)->SetCT();
	}
	else if (ShaderHandleBackup) {
		ShaderHandle = (IDirect3DVertexShader9*)ShaderHandleBackup;
	}

}

void NiD3DVertexShaderEx::DisposeShader() {
	for (UInt32 i = 0; i < 3; i++) {
		if (ShaderProg[i]) {
			delete ShaderProg[i];
			ShaderProg[i] = NULL;
		}
	}
}

ShaderRecordVertex* NiD3DVertexShaderEx::GetShaderRecord(ShaderRecordType Type) {
	return (ShaderRecordVertex*)NiD3DShaderProgram::GetShaderRecord(Type);
}

void __fastcall NiD3DVertexShaderEx::Free(NiD3DVertexShaderEx* shader) {
	shader->DisposeShader();
	ShaderCollection* Collection = TheShaderManager->GetShaderCollection(shader->Name);
	std::vector<NiD3DVertexShaderEx*>* pList = &Collection->VertexShaderList;
	pList->erase(std::remove(pList->begin(), pList->end(), shader), pList->end());
	ThisCall(0xE89B70, shader);
}


/*
* Hook - Function to replace shader handles during pass initialisation.
*/
void NiD3DPixelShaderEx::SetupShader(IDirect3DPixelShader9* CurrentPixelHandle) {

	if (!Enabled || !TheSettingManager->SettingsMain.Main.RenderEffects) {
		ShaderHandle = (IDirect3DPixelShader9*)ShaderHandleBackup;
		return;
	}

	if (GetShaderRecord(ShaderRecordType::Exterior) && Player->GetWorldSpace()) {
		ShaderHandle = GetShaderRecord(ShaderRecordType::Exterior)->ShaderHandle;
		if (CurrentPixelHandle != ShaderHandle) GetShaderRecord(ShaderRecordType::Exterior)->SetCT();
	}
	else if (GetShaderRecord(ShaderRecordType::Interior) && !Player->GetWorldSpace()) {
		ShaderHandle = GetShaderRecord(ShaderRecordType::Interior)->ShaderHandle;
		if (CurrentPixelHandle != ShaderHandle) GetShaderRecord(ShaderRecordType::Interior)->SetCT();
	}
	else if (GetShaderRecord(ShaderRecordType::Default)) {
		ShaderHandle = GetShaderRecord(ShaderRecordType::Default)->ShaderHandle;
		if (CurrentPixelHandle != ShaderHandle) GetShaderRecord(ShaderRecordType::Default)->SetCT();
	}
	else {
		ShaderHandle = (IDirect3DPixelShader9*)ShaderHandleBackup;
	}
}


void NiD3DPixelShaderEx::DisposeShader() {
	for (UInt32 i = 0; i < 3; i++) {
		if (ShaderProg[i]) {
			delete ShaderProg[i];
			ShaderProg[i] = NULL;
		}
	}
}

ShaderRecordPixel* NiD3DPixelShaderEx::GetShaderRecord(ShaderRecordType Type) {
	return (ShaderRecordPixel*)NiD3DShaderProgram::GetShaderRecord(Type);
}

void __fastcall NiD3DPixelShaderEx::Free(NiD3DPixelShaderEx* shader) {
	shader->DisposeShader();
	ShaderCollection* Collection = TheShaderManager->GetShaderCollection(shader->Name);
	std::vector<NiD3DPixelShaderEx*>* pList = &Collection->PixelShaderList;
	pList->erase(std::remove(pList->begin(), pList->end(), shader), pList->end());
	ThisCall(0xE89970, shader);
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