// Image space shadows shader for Oblivion Reloaded
# define viewshadows 0

float4 TESR_ReciprocalResolution;
float4 TESR_WaterSettings; //x: water height in the cell, y: water depth darkness, z: is camera underwater
float4 TESR_ShadowData; // x: quality, y: darkness, z: nearmap resolution, w: farmap resolution
float4 TESR_ShadowFade; // x: fading at sunrise/sunset, y:disabled shadows, z: pointlights shadows
float4 TESR_ShadowRadius; // radius of the 4 cascades
float4 TESR_SkyColor;
float4 TESR_SunAmbient;
float4 TESR_SunColor;
float4 TESR_ShadowScreenSpaceData;

sampler2D TESR_RenderedBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_DepthBuffer : register(s1) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = ANISOTROPIC; MIPFILTER = LINEAR; };
sampler2D TESR_PointShadowBuffer : register(s2)  = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_NormalsBuffer : register(s3) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };


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
 * returns a shadow value from darkness setting value (full shadow) to 1 (full light)
*/
float4 Shadow(VSOUT IN) : COLOR0
{
	float4 color = tex2D(TESR_RenderedBuffer, IN.UVCoord);
	float2 uv = IN.UVCoord;

	float depth = readDepth(uv);
	float3 camera_vector = toWorld(uv) * depth;
	float uniformDepth = length(camera_vector);
	float4 world_pos = float4(TESR_CameraPosition.xyz + camera_vector, 1.0f);
	float3 world_normal = GetWorldNormal(IN.UVCoord);

	// early out for underwater surface (if camera is underwater and surface to shade is close to water level with normal pointing downward)
	if (TESR_WaterSettings.z == 1 && world_pos.z < (TESR_WaterSettings.x + 2) && world_pos.z > (TESR_WaterSettings.x - 2) && dot(world_normal, float3(0, 0, -1)) > 0.999) return color;

	float2 Shadow = tex2D(TESR_PointShadowBuffer, IN.UVCoord).rg;
	Shadow.r = lerp(TESR_ShadowFade.x, 1.0f, Shadow.r); // fade shadows to light when sun is low

	// scale shadows strength to ambient before adding attenuation for pointlights (ShadowFade.z means point Lights are on)
	float ambient = lerp(1, luma(TESR_SunAmbient), DARKNESS * TESR_ShadowFade.z);
	Shadow.r = lerp(0, ambient, Shadow.r); //scale brightest areas to the ambient so it can be lit further with attenuation
	Shadow.r += Shadow.g; // Apply poing light attenuation (includes point light shadows)

	Shadow.r = lerp(DARKNESS, 1.0, Shadow.r); 	// brighten shadow value from 0 to darkness from config value

	// No point for the shadow buffer to be beyond the 0-1 range
	Shadow.r = saturate(Shadow.r);

#if viewshadows == 1
	return Shadow;
#endif

	// tint shadowed areas with Sky color before blending
	float4 colorShadow = luma(color.rgb) * (Shadow.r) * TESR_SkyColor;
	return float4(lerp(colorShadow, color * Shadow.r, saturate(Shadow.r + 0.2)).rgb, 1); // bias the transition between the 2 colors to make it less noticeable
}


technique {
	pass {
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Shadow();
	}
}
