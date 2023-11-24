#pragma once

enum ShaderCompileType {
	AlwaysOff,
	AlwaysOn,
	RecompileChanged,
	RecompileInMenu,
	RecompileAbsent,
};


struct ShaderValue {
	UInt32				RegisterIndex;
	UInt32				RegisterCount;
	union {
		D3DXVECTOR4* Value;
		TextureRecord* Texture;
	};
};


class ShaderProgram {
public:
	ShaderProgram();
	virtual ~ShaderProgram();

	virtual void			SetCT() = 0;
	virtual void			CreateCT(ID3DXBuffer* ShaderSource, ID3DXConstantTable* ConstantTable) = 0;

	void					SetConstantTableValue(LPCSTR Name, UInt32 Index);
	static bool ShouldCompileShader(const char* fileBin, const char* fileHlsl, ShaderCompileType CompileStatus);

	ShaderValue* FloatShaderValues;
	UInt32					FloatShaderValuesCount;
	ShaderValue* TextureShaderValues;
	UInt32					TextureShaderValuesCount;
};

class ShaderRecord : public ShaderProgram {
public:
	ShaderRecord();
	virtual ~ShaderRecord();

	virtual void			SetCT();
	virtual void			CreateCT(ID3DXBuffer* ShaderSource, ID3DXConstantTable* ConstantTable);
	virtual void			SetShaderConstantF(UInt32 RegisterIndex, D3DXVECTOR4* Value, UInt32 RegisterCount) = 0;

	static ShaderRecord* LoadShader(const char* Name, const char* SubPath);

	bool					HasRenderedBuffer;
	bool					HasDepthBuffer;
	const char* Name;
};

class ShaderRecordVertex : public ShaderRecord {
public:
	ShaderRecordVertex(const char* shaderName);
	virtual ~ShaderRecordVertex();

	virtual void			SetShaderConstantF(UInt32 RegisterIndex, D3DXVECTOR4* Value, UInt32 RegisterCount);

	IDirect3DVertexShader9* ShaderHandle;
};

class ShaderRecordPixel : public ShaderRecord {
public:
	ShaderRecordPixel(const char* shaderName);
	virtual ~ShaderRecordPixel();

	virtual void			SetShaderConstantF(UInt32 RegisterIndex, D3DXVECTOR4* Value, UInt32 RegisterCount);

	IDirect3DPixelShader9* ShaderHandle;
};

class NiD3DVertexShaderEx : public NiD3DVertexShader {
public:
	void					SetupShader(IDirect3DVertexShader9* CurrentVertexHandle);
	void					DisposeShader();

	ShaderRecordVertex* ShaderProg;
	ShaderRecordVertex* ShaderProgE;
	ShaderRecordVertex* ShaderProgI;
	IDirect3DVertexShader9* ShaderHandleBackup;
	char					ShaderName[40];
};

class NiD3DPixelShaderEx : public NiD3DPixelShader {
public:
	void					SetupShader(IDirect3DPixelShader9* CurrentPixelHandle);
	void					DisposeShader();

	ShaderRecordPixel* ShaderProg;
	ShaderRecordPixel* ShaderProgE;
	ShaderRecordPixel* ShaderProgI;
	IDirect3DPixelShader9* ShaderHandleBackup;
	char					ShaderName[40];
};

class EffectRecord : public ShaderProgram {
public:
	EffectRecord(const char* effectName);
	virtual ~EffectRecord();

	virtual void			SetCT();
	virtual void			CreateCT(ID3DXBuffer* ShaderSource, ID3DXConstantTable* ConstantTable);
	virtual void			UpdateConstants();
	bool					SwitchEffect();
	void					Render(IDirect3DDevice9* Device, IDirect3DSurface9* RenderTarget, IDirect3DSurface9* RenderedSurface, UINT techniqueIndex, bool ClearRenderTarget, IDirect3DSurface9* SourceBuffer);
	void					DisposeEffect();
	bool					LoadEffect(bool alwaysCompile = false);

	static EffectRecord* LoadEffect(const char* Name);
	bool 					IsLoaded();
	bool					Enabled;

	ID3DXEffect* Effect;
	std::string* Path;
	std::string* SourcePath;
	const char* Name;
};
