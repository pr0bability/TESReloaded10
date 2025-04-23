// Shader To compute a shadow pass for point light shadows (Supports 5 more lights)

float4 TESR_ShadowLightPosition[12];
float4 TESR_LightColor[24];
float4 TESR_ShadowFade;

sampler2D TESR_DepthBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_NormalsBuffer : register(s1) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = NONE; MINFILTER = NONE; MIPFILTER = NONE; };
sampler2D TESR_PointShadowBuffer : register(s2) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; ADDRESSW = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
samplerCUBE TESR_ShadowCubeMapBuffer6 : register(s3) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; ADDRESSW = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
samplerCUBE TESR_ShadowCubeMapBuffer7 : register(s4) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; ADDRESSW = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
samplerCUBE TESR_ShadowCubeMapBuffer8 : register(s5) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; ADDRESSW = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
samplerCUBE TESR_ShadowCubeMapBuffer9 : register(s6) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; ADDRESSW = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
samplerCUBE TESR_ShadowCubeMapBuffer10: register(s7) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; ADDRESSW = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
// samplerCUBE TESR_ShadowCubeMapBuffer11 : register(s8) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; ADDRESSW = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

#include "Includes/Helpers.hlsl"
#include "Includes/Depth.hlsl"
#include "Includes/Normals.hlsl"
#include "Includes/Shadows.hlsl"

struct VSOUT
{
	float4 vertPos : POSITION;
	float2 UVCoord : TEXCOORD0;
};

struct VSIN
{
	float4 vertPos : POSITION0;
	float2 UVCoord : TEXCOORD0;
};

VSOUT FrameVS(VSIN IN)
{
	VSOUT OUT = (VSOUT)0.0f;
	OUT.vertPos = IN.vertPos;
	OUT.UVCoord = IN.UVCoord;
	return OUT;
}

float4 Shadow( VSOUT IN ) : COLOR0 {

	float2 uv = IN.UVCoord;
	float depth = readDepth(uv);
    float3 camera_vector = toWorld(uv) * depth;
    float4 world_pos = float4(TESR_CameraPosition.xyz + camera_vector, 1.0f);	
	float4 normal = float4(GetWorldNormal(uv), 1);

	float Shadow = tex2D(TESR_PointShadowBuffer, IN.UVCoord).r;
	Shadow += GetPointLightAmount(TESR_ShadowCubeMapBuffer6, world_pos, TESR_ShadowLightPosition[6], normal) * luma(TESR_LightColor[6].rgb) * TESR_LightColor[6].w;
	Shadow += GetPointLightAmount(TESR_ShadowCubeMapBuffer7, world_pos, TESR_ShadowLightPosition[7], normal) * luma(TESR_LightColor[7].rgb) * TESR_LightColor[7].w;
	Shadow += GetPointLightAmount(TESR_ShadowCubeMapBuffer8, world_pos, TESR_ShadowLightPosition[8], normal) * luma(TESR_LightColor[8].rgb) * TESR_LightColor[8].w;
	Shadow += GetPointLightAmount(TESR_ShadowCubeMapBuffer9, world_pos, TESR_ShadowLightPosition[9], normal) * luma(TESR_LightColor[9].rgb) * TESR_LightColor[9].w;
	Shadow += GetPointLightAmount(TESR_ShadowCubeMapBuffer10, world_pos, TESR_ShadowLightPosition[10], normal) * luma(TESR_LightColor[10].rgb) * TESR_LightColor[10].w;
	// Shadow += GetPointLightAmount(TESR_ShadowCubeMapBuffer11, world_pos, TESR_ShadowLightPosition11, normal);
	Shadow += GetPointLightContribution(world_pos, TESR_ShadowLightPosition[11], normal);


	Shadow = saturate(Shadow);
	
	return float4(Shadow, Shadow, Shadow, 1.0f);
}

technique {
	pass {
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Shadow();
	}
}
