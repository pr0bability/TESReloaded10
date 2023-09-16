// Volumetric Fog fullscreen shader for Oblivion/Skyrim Reloaded

float4 TESR_FogColor;
float4 TESR_FogData;
float4 TESR_VolumetricFogData;
float4 TESR_ReciprocalResolution;
float4 TESR_SunDirection;
float4 TESR_SunColor;
float4 TESR_SunAmbient;
float4 TESR_HorizonColor;
float4 TESR_SkyLowColor;
float4 TESR_SkyColor;
float4 TESR_SkyData;
float4 TESR_DebugVar;

sampler2D TESR_RenderedBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_DepthBuffer : register(s1) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

/*Height-based fog settings*/
static const float fogHeight = 4096; 
static const float NOISE_SCALE = 4.2;
static const float sunScatter = 8.0; //raise to decrease the intensity of sun fog
static const float FOG_GROUND =	15000;
static const float FOG_HEIGHT = 25000;

static const float nearFogDistance = TESR_FogData.x;
static const float farFogDistance = TESR_FogData.y;
static const float SunGlare = TESR_FogData.z;
static const float FogPower = TESR_FogData.w;

static const float SunExponent = max(TESR_VolumetricFogData.x, 1); // 8
static const float SunGlareCoeff = TESR_VolumetricFogData.y; // 100
static const float FogStrength = TESR_VolumetricFogData.z; // 1
static const float MaxFogHeight = TESR_VolumetricFogData.w; // 80000

#include "Includes/Depth.hlsl"
#include "Includes/Helpers.hlsl"

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


float ExponentialFog(float posHeight, float distance) {
	float fogAmount = exp(-posHeight) * (1.0 - exp(-distance)) * FogStrength;
	return fogAmount;
}


float4 VolumetricFog(VSOUT IN) : COLOR0 
{
	float3 color = tex2D(TESR_RenderedBuffer, IN.UVCoord).rgb;
    float height = reconstructWorldPosition(IN.UVCoord).z;
    float depth = readDepth(IN.UVCoord);
	float3 eyeVector = (toWorld(IN.UVCoord));
	float3 eyeDirection = normalize(eyeVector);
	float fogDepth = length(eyeVector * depth);

	// quadratic fog based on linear distance in fog range with fog power
	float distance = invlerps(nearFogDistance, farFogDistance, fogDepth);
	float fogAmount = saturate(pow(distance, FogPower) * FogStrength);
	fogAmount = fogAmount * saturate(exp( - height/MaxFogHeight)); // fade with height

	// vertical exponential depth fog
	// float distantFogAmount = saturate(pow(distance, FogPower) * FogStrength);
	// color = lerp(color, TESR_HorizonColor, distantFogAmount<0.99999?distantFogAmount:0);
	// float density = 0.08 * TESR_DebugVar.x; //0.0002
	// float falloff = 0.00225 * TESR_DebugVar.y; // tie to darkness setting?
	// float fogAmount = saturate((density/falloff) * exp(-TESR_CameraPosition.z*falloff) * (1.0 - exp( -fogDepth*eyeDirection.z*falloff ))/eyeDirection.z);

	// calculate sky color
	float3 up = blue.xyz;
    float verticality = pows(compress(shade(eyeDirection, up)), 3);
    float sunHeight = shade(TESR_SunDirection.xyz, up);
    float athmosphere = pows(1 - verticality, 8) * TESR_SkyData.x;
    float sunInfluence = shade(eyeDirection, TESR_SunDirection.xyz);
    float3 skyColor = lerp(TESR_SkyLowColor.rgb, TESR_HorizonColor.rgb, athmosphere * 0.2); // tint the base more with horizon color when sun is high

	// blend with fog color
	float3 fogColor = lerp(skyColor, TESR_FogColor, saturate(1/ (1 + fogDepth))).rgb; // fade color between fog to horizon based on depth
	float sunAmount = pows(sunInfluence, SunExponent) * (SunGlare * SunGlareCoeff); //sun influence
	fogColor += TESR_SunColor.rgb * sunAmount; // add sun color to the fog

	float3 originalFogColor = fogColor;
	float originalFogLuma = luma(originalFogColor);
	fogColor = lerp(color, fogColor, fogAmount).rgb; // calculate final color of scene through the fog

    // Blend back in some of the original color based on luma (brightest lights will come through):
    float fogLuma = luma(fogColor);
    float lumaDiff = invlerps(saturate(fogLuma), 1.0f, luma(color));
    color = lerp(fogColor, color, lumaDiff); 

    // Bring back any fog above 1 as additive (there usually isn't any, but it's good for HDR rendering):
    float fogAdditiveLumaRatio = saturate(1.0f / originalFogLuma); // From (background) color luma to fog luma
    float3 additiveFogColor = originalFogColor * (1.0f - fogAdditiveLumaRatio);
    color += additiveFogColor;


	// if (IN.UVCoord.x > 0.8 && IN.UVCoord.x < 0.9){
	// 	if (IN.UVCoord.y > 0.3 && IN.UVCoord.y < 0.4) return TESR_FogColor;
	// 	if (IN.UVCoord.y > 0.4 && IN.UVCoord.y < 0.5) return TESR_HorizonColor;
	// 	if (IN.UVCoord.y > 0.5 && IN.UVCoord.y < 0.6) return TESR_SunColor;
	// 	if (IN.UVCoord.y > 0.6 && IN.UVCoord.y < 0.7) return TESR_SunAmbient;
	// }
	return float4(color, 1.0f);
}

technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader  = compile ps_3_0 VolumetricFog();
	}
}
