// Shader To compute a shadow pass for point light shadows (Only for 6 lights)

float4 TESR_ShadowLightPosition0;
float4 TESR_ShadowLightPosition1;
float4 TESR_ShadowLightPosition2;
float4 TESR_ShadowLightPosition3;
float4 TESR_ShadowLightPosition4;
float4 TESR_ShadowLightPosition5;
float4 TESR_LightPosition0;
float4 TESR_LightPosition1;
float4 TESR_LightPosition2;
float4 TESR_LightPosition3;
float4 TESR_LightPosition4;
float4 TESR_LightPosition5;
float4 TESR_LightPosition6;
float4 TESR_LightPosition7;
float4 TESR_LightPosition8;
float4 TESR_LightPosition9;
float4 TESR_LightPosition10;
float4 TESR_LightPosition11;
float4 TESR_ShadowFade;

//sampler_state removed to avoid a artifact. TODO investigate
sampler2D TESR_DepthBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_NormalsBuffer : register(s1) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = NONE; MINFILTER = NONE; MIPFILTER = NONE; };
samplerCUBE TESR_ShadowCubeMapBuffer0 : register(s2) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; ADDRESSW = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
samplerCUBE TESR_ShadowCubeMapBuffer1 : register(s3) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; ADDRESSW = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
samplerCUBE TESR_ShadowCubeMapBuffer2 : register(s4) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; ADDRESSW = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
samplerCUBE TESR_ShadowCubeMapBuffer3 : register(s5) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; ADDRESSW = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
samplerCUBE TESR_ShadowCubeMapBuffer4 : register(s6) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; ADDRESSW = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
samplerCUBE TESR_ShadowCubeMapBuffer5 : register(s7) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; ADDRESSW = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

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
	// float Shadow = 0.0;

	float Shadow = GetPointLightAmount(TESR_ShadowCubeMapBuffer0, world_pos, TESR_ShadowLightPosition0, normal);
	Shadow += GetPointLightAmount(TESR_ShadowCubeMapBuffer1, world_pos, TESR_ShadowLightPosition1, normal);
	Shadow += GetPointLightAmount(TESR_ShadowCubeMapBuffer2, world_pos, TESR_ShadowLightPosition2,normal);
	Shadow += GetPointLightAmount(TESR_ShadowCubeMapBuffer3, world_pos, TESR_ShadowLightPosition3, normal);
	Shadow += GetPointLightAmount(TESR_ShadowCubeMapBuffer4, world_pos, TESR_ShadowLightPosition4, normal);
	Shadow += GetPointLightAmount(TESR_ShadowCubeMapBuffer5, world_pos, TESR_ShadowLightPosition5, normal);
	Shadow += GetPointLightAtten(world_pos, TESR_LightPosition0, normal);
	Shadow += GetPointLightAtten(world_pos, TESR_LightPosition1, normal);
	Shadow += GetPointLightAtten(world_pos, TESR_LightPosition2, normal);
	Shadow += GetPointLightAtten(world_pos, TESR_LightPosition3, normal);
	Shadow += GetPointLightAtten(world_pos, TESR_LightPosition4, normal);
	Shadow += GetPointLightAtten(world_pos, TESR_LightPosition5, normal);
	Shadow += GetPointLightAtten(world_pos, TESR_LightPosition6, normal);
	Shadow += GetPointLightAtten(world_pos, TESR_LightPosition7, normal);
	Shadow += GetPointLightAtten(world_pos, TESR_LightPosition8, normal);
	Shadow += GetPointLightAtten(world_pos, TESR_LightPosition9, normal);
	Shadow += GetPointLightAtten(world_pos, TESR_LightPosition10, normal);
	Shadow += GetPointLightAtten(world_pos, TESR_LightPosition11, normal);

	Shadow = saturate(Shadow);
	
	return float4(Shadow, Shadow, Shadow, 1.0f);
}

technique {
	pass {
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Shadow();
	}
}
