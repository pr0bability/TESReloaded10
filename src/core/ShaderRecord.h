#pragma once

enum ShaderCompileType {
	AlwaysOff,
	AlwaysOn,
	RecompileChanged,
	RecompileInMenu,
	RecompileAbsent,
};


class ShaderValue {
public:
	ShaderValue() {};
	virtual ~ShaderValue() {};

	const char*			Name;
	UInt32				RegisterIndex;
	UInt32				RegisterCount;
};


class ShaderFloatValue : public ShaderValue {
public:
	ShaderFloatValue() {
		Value = nullptr;
	};
	virtual ~ShaderFloatValue() {};

	void				GetValueFromConstantTable();

	D3DXVECTOR4* Value;
	D3DXPARAMETER_TYPE	Type;
};


class ShaderTextureValue : public ShaderValue {
public:
	ShaderTextureValue() {
		Texture = nullptr;
		TexturePath = "";
	};
	virtual ~ShaderTextureValue() {};

	void				GetSamplerStateString(ID3DXBuffer* ShaderSource, UINT32 Index);
	void				GetTextureRecord();
	void				SaveToFile();

	std::string			SamplerString;
	std::string			TexturePath;
	TextureRecord*		Texture;
	TextureRecord::TextureRecordType	Type;
};


class ShaderProgram {
public:
	ShaderProgram();
	virtual ~ShaderProgram();

	virtual void			SetCT() = 0;
	virtual void			CreateCT(ID3DXBuffer* ShaderSource, ID3DXConstantTable* ConstantTable) = 0;

	static bool				ShouldCompileShader(const char* fileBin, const char* fileHlsl, ShaderCompileType CompileStatus);


	ShaderFloatValue*		FloatShaderValues;
	UInt32					FloatShaderValuesCount;
	ShaderTextureValue*		TextureShaderValues;
	UInt32					TextureShaderValuesCount;
};

class ShaderRecord : public ShaderProgram {
public:
	ShaderRecord();
	virtual ~ShaderRecord();

	virtual void			SetCT();
	virtual void			CreateCT(ID3DXBuffer* ShaderSource, ID3DXConstantTable* ConstantTable);
	virtual void			SetShaderConstantF(UInt32 RegisterIndex, D3DXVECTOR4* Value, UInt32 RegisterCount) = 0;

	static ShaderRecord*	LoadShader(const char* Name, const char* SubPath);

	const char* Name;
	bool					HasRenderedBuffer;
	bool					HasDepthBuffer;
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

	ShaderRecordVertex*		ShaderProg;
	ShaderRecordVertex*		ShaderProgE;
	ShaderRecordVertex*		ShaderProgI;
	IDirect3DVertexShader9* ShaderHandleBackup;
	char					ShaderName[40];
	bool					Enabled;
};

class NiD3DPixelShaderEx : public NiD3DPixelShader {
public:
	void					SetupShader(IDirect3DPixelShader9* CurrentPixelHandle);
	void					DisposeShader();

	ShaderRecordPixel*		ShaderProg;
	ShaderRecordPixel*		ShaderProgE;
	ShaderRecordPixel*		ShaderProgI;
	IDirect3DPixelShader9*	ShaderHandleBackup;
	char					ShaderName[40];
	bool					Enabled;
};