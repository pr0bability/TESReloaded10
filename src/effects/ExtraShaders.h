#pragma once

class ExtraShaders : public ShaderCollection
{
public:
	ExtraShaders() : ShaderCollection("ExtraShaders") {};

	struct ExtraShaderstruct {};
	ExtraShaderstruct	Constants;

	void	UpdateConstants() {};
	void	RegisterConstants() {};
	void	UpdateSettings() {};
};