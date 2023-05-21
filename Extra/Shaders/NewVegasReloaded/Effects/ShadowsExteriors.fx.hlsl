// Image space shadows shader for Oblivion Reloaded

float4x4 TESR_WorldViewProjectionTransform;
// float4x4 TESR_ShadowCameraToLightTransformNear;
// float4x4 TESR_ShadowCameraToLightTransformMiddle;
// float4x4 TESR_ShadowCameraToLightTransformFar;
// float4x4 TESR_ShadowCameraToLightTransformLod;
float4 TESR_ReciprocalResolution;
float4 TESR_ViewSpaceLightDir;
float4 TESR_WaterSettings; //x: water height in the cell, y: water depth darkness, z: is camera underwater
float4 TESR_ShadowData; // x: quality, y: darkness, z: nearmap resolution, w: farmap resolution
float4 TESR_ShadowScreenSpaceData; // x: Enabled, y: blurRadius, z: renderDistance
float4 TESR_ShadowFade;
float4 TESR_ShadowRadius; // radius of the 4 cascades
float4 TESR_SkyColor;
float4 TESR_SunAmbient;
float4 TESR_SunColor;

sampler2D TESR_RenderedBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_SourceBuffer : register(s1) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_DepthBuffer : register(s2) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = ANISOTROPIC; MIPFILTER = LINEAR; };
//sampler2D TESR_NormalsBuffer : register(s7) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_PointShadowBuffer : register(s3)  = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

#define SSS_STEPNUM 10

static const float DARKNESS = 1-TESR_ShadowData.y;
static const float SSS_DIST = 20;
static const float SSS_THICKNESS = 20;
static const float SSS_MAXDEPTH = TESR_ShadowScreenSpaceData.z * TESR_ShadowScreenSpaceData.x;


struct VSOUT
{
	float4 vertPos : POSITION;
	float4 normal : TEXCOORD1;
	float2 UVCoord : TEXCOORD0;
};

struct VSIN
{
	float4 vertPos : POSITION0;
	float2 UVCoord : TEXCOORD0;
};

#include "Includes/Helpers.hlsl"
#include "Includes/Depth.hlsl"
#include "Includes/BlurDepth.hlsl"
#include "Includes/Shadows.hlsl"


VSOUT FrameVS(VSIN IN)
{
	VSOUT OUT = (VSOUT)0.0f;
	OUT.vertPos = IN.vertPos;
	OUT.UVCoord = IN.UVCoord;
	return OUT;
}

float4 Shadow(VSOUT IN) : COLOR0
{
	float2 uv = IN.UVCoord;

	// returns a shadow value from darkness setting value (full shadow) to 1 (full light)
	float depth = readDepth(uv);
	float3 camera_vector = toWorld(uv) * depth;
	float4 world_pos = float4(TESR_CameraPosition.xyz + camera_vector, 1.0f);

	// early out for underwater surface (if camera is underwater and surface to shade is close to water level with normal pointing downward)
	if (TESR_ShadowFade.y == 0 || (TESR_WaterSettings.z && world_pos.z < TESR_WaterSettings.x + 1.5 && world_pos.z > TESR_WaterSettings.x - 1.5)) return float4 (1.0f, 1.0, 1.0, 1.0);
	// if (TESR_WaterSettings.z && world_pos.z < TESR_WaterSettings.x + 2 && world_pos.z > TESR_WaterSettings.x - 2 && dot(world_normal, float3(0, 0, -1)) > 0.999) return float4 (1.0f, 1.0, 1.0, 1.0);

	float4 Shadow = tex2D(TESR_PointShadowBuffer, IN.UVCoord);

	// fade shadows to light when sun is low

	// fade shadow close to light sources

	// brighten shadow value from 0 to darkness from config value
	Shadow = lerp(DARKNESS, 1.0f, Shadow);
	Shadow = lerp(Shadow, 1.0f, invlerps(TESR_ShadowRadius.z, TESR_ShadowRadius.w, depth));

	// No point for the shadow buffer to be beyond the 0-1 range
	Shadow = saturate(Shadow);

	return Shadow.rrrr;
}


float4 CombineShadow (VSOUT IN) : COLOR0 
{
	// old style multiply blending (for testing)
	float4 color = tex2D(TESR_SourceBuffer, IN.UVCoord);
	float Shadow = tex2D(TESR_RenderedBuffer, IN.UVCoord).r;

	float4 colorShadow = luma(color.rgb) * Shadow * TESR_SkyColor;

	return lerp(colorShadow, color * Shadow, saturate(Shadow + 0.2)); // bias the transition between the 2 colors to make it less noticeable
}

technique {


	pass {
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Shadow();
	}

	pass
	{ 
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 BlurRChannel(float2(1.0f, 0.0f), TESR_ShadowScreenSpaceData.y, 5, SSS_MAXDEPTH);
	}
	
	pass
	{ 
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 BlurRChannel(float2(0.0f, 1.0f), TESR_ShadowScreenSpaceData.y, 5, SSS_MAXDEPTH);
	}

	pass {
		VertexShader = compile vs_3_0 FrameVS();
	 	PixelShader = compile ps_3_0 CombineShadow();
	}

}
