// Volumetric Fog fullscreen shader for Oblivion/Skyrim Reloaded

float4 TESR_FogColor;
float4 TESR_FogData;  // weather values : x: fogNear, y:fogFar, z: sunglare, w:fogpower
float4 TESR_ReciprocalResolution;
float4 TESR_SunDirection;
float4 TESR_SunPosition;
float4 TESR_SunColor;
float4 TESR_SunDiskColor;
float4 TESR_SunAmbient;
float4 TESR_HorizonColor;
float4 TESR_SkyLowColor;
float4 TESR_SkyColor; // top sky color
float4 TESR_SkyData; // x: athmosphere thickness, y: sun influence, z: sun strength w: sky strength
float4 TESR_DebugVar; 
float4 TESR_SunsetColor; // color boost for sun when near the horizon
float4 TESR_SunAmount; // x: isDaytime
float4 TESR_VolumetricFogLow; // Low Fog
float4 TESR_VolumetricFogHigh; // General Fog
float4 TESR_VolumetricFogSimple; // Simple Fog
float4 TESR_VolumetricFogBlend; // Blend factor for each Fog
float4 TESR_VolumetricFogHeight; // Height of each Fog
float4 TESR_VolumetricFogData; // General shader settings

sampler2D TESR_RenderedBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_DepthBuffer : register(s1) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

/*Height-based fog settings*/
static const float FOG_GROUND =	-10000;
static const float SUNINFLUENCE = 1/TESR_SkyData.y;
// static const float SUNINFLUENCE = TESR_SkyData.y;

static const float nearFog = TESR_FogData.x;
static const float farFog = TESR_FogData.y;
static const float SunGlare = TESR_FogData.z;
static const float FogPower = TESR_FogData.w;

// scale settings for easier tuning
static const float LowFogDensity = TESR_VolumetricFogLow.x;
static const float LowFogFalloff = TESR_VolumetricFogLow.y;
static const float LowFogDist = TESR_VolumetricFogLow.z;
static const float SunPower = TESR_VolumetricFogLow.w;

static const float HeightFogDensity = TESR_VolumetricFogHigh.x;
static const float HeightFogFalloff = TESR_VolumetricFogHigh.y;
static const float HeightFogDist = TESR_VolumetricFogHigh.z;
static const float HeightFogSkyColor = 2 * TESR_VolumetricFogHigh.w;

static const float LowFogHeight = TESR_VolumetricFogHeight.x;
static const float HeightFogHeight = TESR_VolumetricFogHeight.y;
static const float SimpleFogHeight = TESR_VolumetricFogHeight.z * 1.5;
static const float isExterior = TESR_VolumetricFogHeight.w; // 0 or 1 to activate/cancel fog in interiors

static const float SimpleFogExtinction = max(0.0, TESR_VolumetricFogSimple.x) * 0.1;
static const float SimpleFogInscattering = max(0.0, TESR_VolumetricFogSimple.y) * 0.1;
static const float SimpleFogNight = 2.5 * TESR_VolumetricFogSimple.z;
static const float SimpleFogSkyColor = 0.22 * TESR_VolumetricFogSimple.w;

static const float LowFogBlend = TESR_VolumetricFogBlend.x;
static const float HeightFogBlend = TESR_VolumetricFogBlend.y;
static const float HeightFogRolloff = TESR_VolumetricFogBlend.z;
static const float SimpleFogBlend = TESR_VolumetricFogBlend.w;
// static const float SimpleFogBlend = 1.5 * TESR_VolumetricFogBlend.w;

//static const float4x4 ditherMat = {{0.0588, 0.5294, 0.1765, 0.6471},
//									{0.7647, 0.2941, 0.8824, 0.4118},
//									{0.2353, 0.7059, 0.1176, 0.5882},
//									{0.9412, 0.4706, 0.8235, 0.3259}};

#include "Includes/Depth.hlsl"
#include "Includes/Helpers.hlsl"
#include "Includes/Sky.hlsl"


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


float3 getFog(float distance, float3 density){
	return 1 - exp(-distance * density);
}

// exponential fog based on https://iquilezles.org/articles/fog/
float3 getHeightFog(float distance, float falloff, float height, float heightOffset){
	float eyepos = TESR_CameraPosition.z + FOG_GROUND + (heightOffset) * 1000;
	float pointHeight = height - eyepos;
	return (1 / falloff) * exp(-eyepos * falloff) * (1.0 - exp(-distance * pointHeight * falloff)) / pointHeight;
}

float3 mixFog(float3 color, float3 fogColor, float3 extinctionColor, float3 inscatteringColor, float distance, float density){
	float3 extColor = getFog(distance, density * extinctionColor);
	float3 insColor = getFog(distance, density * inscatteringColor);
	return color * (1 - extColor) + fogColor * (insColor);
}

float3 mixHeightFog(float3 color, float3 fogColor, float3 extinctionColor, float3 inscatteringColor, float distance, float density, float falloff, float height, float offset){
	float fog = density * getHeightFog(distance, falloff * 0.000000005f, height, offset);
	float3 extColor = fog * extinctionColor;
	float3 insColor = fog * inscatteringColor;
	return color * (1 - extColor) + fogColor * (insColor);
	// return lerp(insColor, color, extColor);
}

float getAverageDepth(float2 uv, float distance){
    return readDepth(uv);

	float2 coeffs = float2(1, -1) * distance;
    float depth1 = readDepth(uv + TESR_ReciprocalResolution.xy * coeffs.xx);
    float depth2 = readDepth(uv + TESR_ReciprocalResolution.xy * coeffs.xy);
    float depth3 = readDepth(uv + TESR_ReciprocalResolution.xy * coeffs.yx);
    float depth4 = readDepth(uv + TESR_ReciprocalResolution.xy * coeffs.yy);

	float4 average = float4(depth1, depth2, depth3, depth4);
	float4 weights = normalize(average);

	return min(average.x, min(average.y, min(average.z, average.w)));
	return dot(average, float(0.25).xxxx);
}

float4 VolumetricFog(VSOUT IN) : COLOR0 
{
	float4 color = tex2D(TESR_RenderedBuffer, IN.UVCoord);
	color = linearize(color);
	float4 pureFogColor = linearize(TESR_FogColor);

	// get the min value of 9 neighbors to reduce dithering
    float depth = readDepth(IN.UVCoord);

    float isDayTime = smoothstep(0, 0.5, TESR_SunAmount.x);

	float3 eyeVector = toWorld(IN.UVCoord);
	float3 worldPos = eyeVector * depth + TESR_CameraPosition.xyz;
    float pointHeight = worldPos.z;
	float3 eyeDirection = normalize(eyeVector);
	// float fogDepth = length(eyeVector * depth);
	float fogDepth = depth;
	float fogPower = FogPower * lerp(0.3, 1.0, isDayTime); // boost fog power at night to allow it to show up better
	float normalizedDepth = pows(fogDepth / farZ, HeightFogRolloff * fogPower); // apply a curve to the distance to simulate pushing back fog
	float normalizedDepthDistance = pows(fogDepth / farZ, LowFogDist * fogPower); // apply a curve to the distance to simulate pushing back fog
	float multiplier = lerp(0.005, 1, isExterior);
	fogDepth = normalizedDepth * farZ / (HeightFogDist * multiplier); // scale distances > shorter distances = further out fog

	// calculate sun & sky color
	float3 up = blue.xyz;
    float sunHeight = shade(TESR_SunPosition.xyz, up);
    float sunDir = dot(eyeDirection, TESR_SunPosition.xyz);
	float sunInfluence = pows(compress(sunDir), SUNINFLUENCE);
    float4 sunColor = float4(GetSunColor(sunHeight, 1, TESR_SunAmount.x, TESR_SunDiskColor.rgb, TESR_SunsetColor.rgb), 1);
	sunColor = lerp(black, sunColor, isDayTime * isExterior); // set nighttime sun color

	float4 skyColor = float4(GetSkyColor(0, 0.5, sunHeight, sunInfluence, TESR_SkyData.z, TESR_SkyColor.rgb, TESR_SkyLowColor.rgb, TESR_HorizonColor.rgb, sunColor.rgb), 1);

	float strength = (saturate(1 - farFog/farZ) + saturate(1 - nearFog/farZ)) * 0.6 / (fogPower * 4.0f * LowFogHeight); // deduce a strength of density from near/far and power values
	// float strength = (saturate(1 - farFog/farZ) + saturate(1 - nearFog/farZ * 6.0f)) / (FogPower * 4.0f); // deduce a strength of density from near/far and power values
	float density = (exp(strength) - 1) * 0.0001; // scale density so that strength 0 = no fog and strength 1 = fully fogged scene
	float heightFade = exp( -pointHeight / (80000 * SimpleFogHeight)); // height fade to ensure we don't shade the sky

	// add sun influence/scattering. Influence is wider with distance to simulate scattering. Influence is stronger when the sun is low
	float sunStrength = 1 / TESR_FogData.z; // scale sun contribution with sunglare param
	sunStrength = lerp(sunStrength, max(sunStrength * 2, 10), normalizedDepth); // modulate sun strength with distance
	float sunScattering = pows(compress(sunDir), 2 + sunStrength) * pow(1 - sunHeight, 1.5);
	float4 fogColor = lerp(skyColor, pureFogColor, saturate(pows(strength, HeightFogSkyColor))) + sunColor * sunScattering * SunPower * TESR_FogData.z;
	fogColor = lerp(fogColor * saturate(1 - SimpleFogNight), fogColor, isDayTime);

	// float4 inScatteringColor = pureFogColor + sunColor * sunScattering * SunPower;
	// float4 extinctionColor =  lerp(black, linearize(TESR_HorizonColor), normalizedDepth);

	// enforce some level of fog on the horizon to hide Z fighting & sky transition
	float athmosphere = pow(1 - shade(eyeDirection.xyz, up), 10 * LowFogFalloff);
	float distantFog = lerp(0, athmosphere, smoothstep(0.6, 1, normalizedDepthDistance)); 
	float4 finalColor = lerp(color, skyColor, saturate(distantFog) * saturate(LowFogBlend));

	// float inScattering = luma(TESR_SunColor) * SimpleFogInscattering;
	float3 simpleFog = mixFog(finalColor.rgb, fogColor.rgb, SimpleFogExtinction, SimpleFogInscattering, fogDepth, density * heightFade);
	finalColor = lerp(finalColor, float4(simpleFog, 1), saturate(SimpleFogBlend));

	float3 heightFog = mixHeightFog(finalColor.rgb, fogColor.rgb, SimpleFogExtinction, SimpleFogInscattering, fogDepth, density * HeightFogDensity, HeightFogFalloff * ( 1.5 / fogPower), pointHeight, HeightFogHeight);
	finalColor = lerp(finalColor, float4(heightFog, 1), saturate(HeightFogBlend));

	finalColor = lerp(color, finalColor, TESR_VolumetricFogData.z);

	return delinearize(finalColor);
}

technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader  = compile ps_3_0 VolumetricFog();
	}
}
