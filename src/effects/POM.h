#pragma once

class POMShaders : public ShaderCollection
{
public:
	POMShaders() : ShaderCollection("POM") {};

	std::map<std::string_view, ShaderTemplate> Templates() {
		return std::map<std::string_view, ShaderTemplate>{
			{ "PAR2000.vso", ShaderTemplate{ "ParallaxTemplate", {{"VS", ""}} } },
			{ "PAR2001.vso", ShaderTemplate{ "ParallaxTemplate", {{"VS", ""}, {"PROJ_SHADOW", ""}} } },
			{ "PAR2002.vso", ShaderTemplate{ "ParallaxTemplate", {{"VS", ""}, {"LIGHTS", "2"}} } },
			{ "PAR2003.vso", ShaderTemplate{ "ParallaxTemplate", {{"VS", ""}, {"LIGHTS", "2"}, {"PROJ_SHADOW", ""}} } },
			{ "PAR2004.vso", ShaderTemplate{ "ParallaxTemplate", {{"VS", ""}, {"SPECULAR", ""}} } },
			{ "PAR2005.vso", ShaderTemplate{ "ParallaxTemplate", {{"VS", ""}, {"SPECULAR", ""}, {"PROJ_SHADOW", ""}} } },
			{ "PAR2006.vso", ShaderTemplate{ "ParallaxTemplate", {{"VS", ""}, {"SPECULAR", ""}, {"LIGHTS", "2"}} } },
			{ "PAR2007.vso", ShaderTemplate{ "ParallaxTemplate", {{"VS", ""}, {"SPECULAR", ""}, {"LIGHTS", "2"}, {"PROJ_SHADOW", ""}} } },
			{ "PAR2008.vso", ShaderTemplate{ "ParallaxTemplate", {{"VS", ""}, {"AD", ""}, {"LIGHTS", "2"}} } },
			{ "PAR2009.vso", ShaderTemplate{ "ParallaxTemplate", {{"VS", ""}, {"AD", ""}, {"PROJ_SHADOW", ""}, {"LIGHTS", "2"}} } },
			{ "PAR2010.vso", ShaderTemplate{ "ParallaxTemplate", {{"VS", ""}, {"AD", ""}, {"LIGHTS", "3"}} } },
			{ "PAR2011.vso", ShaderTemplate{ "ParallaxTemplate", {{"VS", ""}, {"AD", ""}, {"PROJ_SHADOW", ""}, {"LIGHTS", "3"}} } },
			{ "PAR2012.vso", ShaderTemplate{ "ParallaxTemplate", {{"VS", ""}, {"DIFFUSE", ""}, {"LIGHTS", "2"}} } },
			{ "PAR2013.vso", ShaderTemplate{ "ParallaxTemplate", {{"VS", ""}, {"DIFFUSE", ""}, {"LIGHTS", "3"}} } },
			{ "PAR2014.vso", ShaderTemplate{ "ParallaxTemplate", {{"VS", ""}, {"NO_FOG", ""}, {"NO_LIGHT", ""}} } },
			{ "PAR2015.vso", ShaderTemplate{ "ParallaxTemplate", {{"VS", ""}, {"ONLY_SPECULAR", ""}} } },
			{ "PAR2016.vso", ShaderTemplate{ "ParallaxTemplate", {{"VS", ""}, {"ONLY_SPECULAR", ""}, {"PROJ_SHADOW", ""}} } },
			{ "PAR2017.vso", ShaderTemplate{ "ParallaxTemplate", {{"VS", ""}, {"ONLY_SPECULAR", ""}, {"POINT", ""}} } },
			{ "PAR2018.vso", ShaderTemplate{ "ParallaxTemplate", {{"VS", ""}, {"ONLY_SPECULAR", ""}, {"POINT", ""}, {"NUM_PT_LIGHTS", "2"}} } },
			{ "PAR2019.vso", ShaderTemplate{ "ParallaxTemplate", {{"VS", ""}, {"ONLY_SPECULAR", ""}, {"POINT", ""}, {"NUM_PT_LIGHTS", "3"}} } },
			{ "PAR2000.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}} } },
			{ "PAR2001.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"OPT", ""}} } },
			{ "PAR2002.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"SI", ""}} } },
			{ "PAR2003.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"PROJ_SHADOW", ""}} } },
			{ "PAR2004.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"SI", ""}, {"PROJ_SHADOW", ""}} } },
			{ "PAR2005.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"LIGHTS", "2"}} } },
			{ "PAR2006.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"LIGHTS", "2"}, {"SI", ""}}} },
			{ "PAR2007.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"LIGHTS", "2"}, {"PROJ_SHADOW", ""}}} },
			{ "PAR2008.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"LIGHTS", "2"}, {"SI", ""}, {"PROJ_SHADOW", ""}}} },
			{ "PAR2009.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"SPECULAR", ""}} } },
			{ "PAR2010.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"SPECULAR", ""}, {"SI", ""}} } },
			{ "PAR2011.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"SPECULAR", ""}, {"PROJ_SHADOW", ""}} } },
			{ "PAR2012.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"SPECULAR", ""}, {"SI", ""}, {"PROJ_SHADOW", ""}} } },
			{ "PAR2013.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"AD", ""}, {"LIGHTS", "2"}} } },
			{ "PAR2014.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"AD", ""}, {"LIGHTS", "2"}, {"SI", ""}} } },
			{ "PAR2015.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"AD", ""}, {"LIGHTS", "2"}, {"PROJ_SHADOW", ""}} } },
			{ "PAR2016.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"AD", ""}, {"LIGHTS", "2"}, {"SI", ""}, {"PROJ_SHADOW", ""}} } },
			{ "PAR2017.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"AD", ""}, {"LIGHTS", "3"}} } },
			{ "PAR2018.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"AD", ""}, {"LIGHTS", "3"}, {"SI", ""}} } },
			{ "PAR2019.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"AD", ""}, {"LIGHTS", "3"}, {"PROJ_SHADOW", ""}} } },
			{ "PAR2020.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"AD", ""}, {"LIGHTS", "3"}, {"SI", ""}, {"PROJ_SHADOW", ""}} } },
			{ "PAR2021.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"DIFFUSE", ""}, {"LIGHTS", "2"}} } },
			{ "PAR2022.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"DIFFUSE", ""}, {"LIGHTS", "3"}} } },
			{ "PAR2023.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"NO_LIGHT", ""}} } },
			{ "PAR2024.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"ONLY_SPECULAR", ""}} } },
			{ "PAR2025.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"ONLY_SPECULAR", ""}, {"PROJ_SHADOW", ""}} } },
			{ "PAR2026.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"ONLY_SPECULAR", ""}, {"POINT", ""}} } },
			{ "PAR2027.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"ONLY_SPECULAR", ""}, {"POINT", ""}, {"NUM_PT_LIGHTS", "2"}} } },
			{ "PAR2028.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"ONLY_SPECULAR", ""}, {"POINT", ""}, {"NUM_PT_LIGHTS", "3"}} } },
			{ "PAR2029.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"SPECULAR", ""}, {"LIGHTS", "2"}} } },
			{ "PAR2030.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"SPECULAR", ""}, {"LIGHTS", "2"}, {"SI", ""}} } },
			{ "PAR2031.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"SPECULAR", ""}, {"LIGHTS", "2"}, {"PROJ_SHADOW", ""}} } },
			{ "PAR2032.pso", ShaderTemplate{ "ParallaxTemplate", {{"PS", ""}, {"SPECULAR", ""}, {"LIGHTS", "2"}, {"SI", ""}, {"PROJ_SHADOW", ""}}} },
		};
	};

	struct POMStruct {
		D3DXVECTOR4		Data;
	};
	POMStruct	Constants;

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();
};