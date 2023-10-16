// Volumetric Fog fullscreen shader for Oblivion/Skyrim Reloaded

float4 TESR_FogColor;
float4 TESR_FogData;
float4 TESR_VolumetricFogData;
float4 TESR_ReciprocalResolution;
float4 TESR_SunDirection;
float4 TESR_SunPosition;
float4 TESR_SunColor;
float4 TESR_SunAmbient;
float4 TESR_HorizonColor;
float4 TESR_SkyLowColor;
float4 TESR_SkyColor; // top sky color
float4 TESR_SkyData; //
float4 TESR_DebugVar; 
float4 TESR_SunsetColor; // color boost for sun when near the horizon
float4 TESR_SunAmount; // x: isDaytime

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

static const float SUNINFLUENCE = 1/TESR_SkyData.y;

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

// ACES tonemapping https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
float3 ACESFilm(float3 x)
{
	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;
	return saturate((x*(a*x+b))/(x*(c*x+d)+e));
}


float4 VolumetricFog(VSOUT IN) : COLOR0 
{
	float3 color = tex2D(TESR_RenderedBuffer, IN.UVCoord).rgb;
    float height = reconstructWorldPosition(IN.UVCoord).z;
    float depth = readDepth(IN.UVCoord);
	float3 eyeVector = toWorld(IN.UVCoord);
	float3 eyeDirection = normalize(eyeVector);
	float fogDepth = length(eyeVector * depth);

	// quadratic fog based on linear distance in fog range with fog power
	float distance = invlerps(nearFogDistance, farFogDistance, fogDepth);
	float fogAmount = saturate(pow(distance, FogPower) * FogStrength);
	fogAmount = fogAmount * saturate(exp( - height/MaxFogHeight)); // fade with height

	// vertical exponential depth fog
	float density = 0.08 * 0.000006 * TESR_DebugVar.x; //0.0002
	float falloff = 0.00225 * 0.02 * TESR_DebugVar.y; // tie to darkness setting?
	// float fogAmount = saturate((density/falloff) * exp(-TESR_CameraPosition.z*falloff) * (1.0 - exp( -fogDepth*eyeDirection.z*falloff ))/eyeDirection.z);

	// calculate sky color
	float3 up = blue.xyz;
    float sunHeight = shade(TESR_SunPosition.xyz, up);
    float sunDir = dot(eyeDirection, TESR_SunPosition.xyz);
	float sunInfluence = pows(compress(sunDir), SUNINFLUENCE);

    float3 skyColor = lerp(TESR_SkyLowColor.rgb, TESR_HorizonColor.rgb, 0.5 + 0.5 * sunHeight); // tint the base more with horizon color when sun is high

	// add extra red to the sun at sunsets
    float sunSet = saturate(pows(1 - sunHeight, 8) * TESR_SkyData.x);
    float3 sunColor = lerp(TESR_SunColor.rgb, TESR_SunColor.rgb + TESR_SunsetColor.rgb, sunSet * invlerp(0, 0.5, TESR_SunAmount.x)); 
	float sunAmount = pows(saturate(sunDir), SunExponent) * (SunGlare * SunGlareCoeff); //sun influence
    skyColor += sunInfluence * SunGlareCoeff * sunColor;

	// blend with fog color
	float3 fogColor = lerp(skyColor, TESR_FogColor.rgb, saturate(1/ (1 + fogDepth))).rgb; // fade color between fog to horizon based on depth
    fogColor = pow(ACESFilm(fogColor), 2.2); // tonemap final color
	float3 originalFogColor = fogColor;
	float originalFogLuma = luma(originalFogColor);
	fogColor = lerp(color, fogColor, fogAmount).rgb; // calculate final color of scene through the fog

    // Blend back in some of the original color based on luma (brightest lights will come through):
    float fogLuma = luma(fogColor);
    float lumaDiff = invlerps(saturate(fogLuma), 1.0f, luma(color));
    color = lerp(fogColor, color, lumaDiff); 

    // // Bring back any fog above 1 as additive (there usually isn't any, but it's good for HDR rendering):
    float fogAdditiveLumaRatio = saturate(1.0f / originalFogLuma); // From (background) color luma to fog luma
    float3 additiveFogColor = originalFogColor * (1.0f - fogAdditiveLumaRatio);
    color += additiveFogColor;

	// return float4(selectColor(TESR_DebugVar.w, color, float(fogDepth / farZ).xxx, fogAmount.xxx, originalFogColor, sunInfluence.xxx, skyColor, TESR_FogColor, black, black, black), 1);

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
