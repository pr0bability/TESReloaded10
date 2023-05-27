// Image space shadows shader for Oblivion Reloaded

float4 TESR_ReciprocalResolution;
float4 TESR_WaterSettings; //x: water height in the cell, y: water depth darkness, z: is camera underwater
float4 TESR_ShadowData; // x: quality, y: darkness, z: nearmap resolution, w: farmap resolution
float4 TESR_ShadowFade;
float4 TESR_ShadowRadius; // radius of the 4 cascades
float4 TESR_SkyColor;
float4 TESR_SunAmbient;
float4 TESR_SunColor;
float4 TESR_ShadowScreenSpaceData;

sampler2D TESR_RenderedBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_SourceBuffer : register(s1) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_DepthBuffer : register(s2) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = ANISOTROPIC; MIPFILTER = LINEAR; };
sampler2D TESR_PointShadowBuffer : register(s3)  = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_NormalsBuffer : register(s4) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

static const float DARKNESS = 1-TESR_ShadowData.y;

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
#include "Includes/Normals.hlsl"
#include "Includes/BlurDepth.hlsl"
#include "Includes/Shadows.hlsl"


VSOUT FrameVS(VSIN IN)
{
	VSOUT OUT = (VSOUT)0.0f;
	OUT.vertPos = IN.vertPos;
	OUT.UVCoord = IN.UVCoord;
	return OUT;
}

/*
 * Load Shadows Buffer and filter water surfaces 
*/
float4 Shadow(VSOUT IN) : COLOR0
{
	float2 uv = IN.UVCoord;

	// returns a shadow value from darkness setting value (full shadow) to 1 (full light)
	float depth = readDepth(uv);
	float3 camera_vector = toWorld(uv) * depth;
	float4 world_pos = float4(TESR_CameraPosition.xyz + camera_vector, 1.0f);
	float3 world_normal = GetWorldNormal(IN.UVCoord);

	// early out for underwater surface (if camera is underwater and surface to shade is close to water level with normal pointing downward)
	// if (TESR_ShadowFade.y == 0 || (TESR_WaterSettings.z && world_pos.z < TESR_WaterSettings.x + 1.5 && world_pos.z > TESR_WaterSettings.x - 1.5)) return float4 (1.0f, 1.0, 1.0, 1.0);
	if (TESR_WaterSettings.z && world_pos.z < TESR_WaterSettings.x + 2 && world_pos.z > TESR_WaterSettings.x - 2 && dot(world_normal, float3(0, 0, -1)) > 0.999) return float4 (1.0f, 1.0, 1.0, 1.0);

	float4 Shadow = tex2D(TESR_PointShadowBuffer, IN.UVCoord);

	// fade shadows to light when sun is low

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
		PixelShader = compile ps_3_0 BlurRChannel(float2(1.0f, 0.0f), TESR_ShadowScreenSpaceData.y, 5, TESR_ShadowRadius.w);
	}
	
	pass
	{ 
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 BlurRChannel(float2(0.0f, 1.0f), TESR_ShadowScreenSpaceData.y, 5, TESR_ShadowRadius.w);
	}

	pass {
		VertexShader = compile vs_3_0 FrameVS();
	 	PixelShader = compile ps_3_0 CombineShadow();
	}

}
