#pragma once

#include "ShaderTemplate.h"

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

	static void				ReportError(HRESULT result);
	static bool				FileExists(const char* path);
	static bool				CheckPreprocessResult(const char* CachedPreprocessPath, ID3DXBuffer* ShaderSource);


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

	static ShaderRecord*	LoadShader(const char* Name, const char* SubPath, ShaderTemplate Template = ShaderTemplate{});

	const char* Name;
	bool					HasRenderedBuffer;
	bool					HasDepthBuffer;
	bool					ClearSamplers;
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

enum ShaderRecordType {
	Default,
	Exterior,
	Interior,
};

class NiD3DVertexShaderEx : public NiD3DVertexShader {
public:
	void					SetupShader(IDirect3DVertexShader9* CurrentVertexHandle);
	void					DisposeShader();

	ShaderRecordVertex*		GetShaderRecord(ShaderRecordType Type);

	static void __fastcall Free(NiD3DVertexShaderEx* shader);
};

class NiD3DPixelShaderEx : public NiD3DPixelShader {
public:
	void					SetupShader(IDirect3DPixelShader9* CurrentPixelHandle);
	void					DisposeShader();

	ShaderRecordPixel*		GetShaderRecord(ShaderRecordType Type);

	static void __fastcall Free(NiD3DPixelShaderEx* shader);
};