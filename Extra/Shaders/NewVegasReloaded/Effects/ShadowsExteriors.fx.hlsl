// Image space shadows shader for Oblivion Reloaded
# define viewshadows 0

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

	float4 Shadow = tex2D(TESR_PointShadowBuffer, IN.UVCoord);

	Shadow.r = lerp(Shadow.r, 1.0f, TESR_ShadowFade.x); // apply darkness fading when sun is low or moon is not full

	float ambient = lerp(1, luma(TESR_SunAmbient), TESR_ShadowFade.z * TESR_ShadowFade.x); //ShadowFade.z means point Lights are on
	Shadow.r = lerp(0, ambient, Shadow.r); // scale shadows strength to ambient before adding attenuation
	Shadow.r += Shadow.g; // Apply poing light attenuation

	float darkness = lerp(DARKNESS, 1.0f, TESR_ShadowFade.x);	// fade shadows to light when sun is low
	Shadow = lerp(darkness, 1.0f, Shadow); 	// brighten shadow value from 0 to darkness from config value

	// No point for the shadow buffer to be beyond the 0-1 range
	Shadow = saturate(Shadow);

#if viewshadows == 1
	return Shadow;
#endif

	// quick average lum with 4 samples at corner pixels
	float4 emissives = tex2D(TESR_RenderedBuffer, uv + float2(-0.5, -0.5) * TESR_ReciprocalResolution.xy);
	emissives += tex2D(TESR_RenderedBuffer, uv + float2(-0.5, 0.5) * TESR_ReciprocalResolution.xy);
	emissives += tex2D(TESR_RenderedBuffer, uv + float2(0.5, -0.5) * TESR_ReciprocalResolution.xy);
	emissives += tex2D(TESR_RenderedBuffer, uv + float2(0.5, 0.5) * TESR_ReciprocalResolution.xy);
	emissives /= 4;

	float threshold = 0.9 * max(luma(TESR_SunAmbient), luma(TESR_SunColor)); // scaling the luma treshold with sun intensity // 2
	float brightness = luma(color);
	float bloomScale = 0.1; 

	float bloom = bloomScale * sqr(max(0.0, brightness - threshold)) / brightness;
	bloom *= brightness * 100;
	bloom = 1 - saturate(bloom);

	// tint shadowed areas with Sky color before blending
	// float4 colorShadow = luma(color.rgb) * (Shadow.r * bloom) * TESR_SkyColor;
	float4 colorShadow = luma(color.rgb) * (Shadow.r) * TESR_SkyColor;
	return float4(lerp(colorShadow, color * Shadow.r, saturate(Shadow.r + 0.2)).rgb, 1); // bias the transition between the 2 colors to make it less noticeable
	// return color * Shadow.r; // bias the transition between the 2 colors to make it less noticeable
}


technique {
	pass {
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Shadow();
	}
}
