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
float4 TESR_SkyData; // x: athmosphere thickness, y: sun influence, z: sun strength w: sky strength
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

static const float FogFalloffFactor = TESR_VolumetricFogData.x; // 8
static const float FogColorFactor = TESR_VolumetricFogData.y; // 100
static const float FogDensityFactor = TESR_VolumetricFogData.z; // 1
static const float FogDistanceFactor = TESR_VolumetricFogData.w; // 80000

static const float SUNINFLUENCE = 1/TESR_SkyData.y;

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

float nightTime (float input, float dayTime) {
	return ((1.0 - dayTime) * input) + dayTime;
}

float3 applyFog(float3 color, float distance, float3 cameraToPoint, float3 sunDirection, float3 fogColor, float3 sunColor, float fogFactor, float colorFactor, float densityFactor, float falloffFactor, float isDayTime, float sunFactor)
{
	// calculate fog amount
	float density = 0.08 * densityFactor * nightTime(10.0, isDayTime) * 0.000006; //0.0002
	float falloff = 0.02 * falloffFactor * nightTime(2.5, isDayTime) * 0.00225;
    float fogAmount = max(0.0,density/falloff) * exp(-TESR_CameraPosition.z*falloff) * (1.0-exp(-distance*cameraToPoint.z*falloff))/cameraToPoint.z;

	// calculate sun color area
    float sunAmount = max( dot(cameraToPoint, sunDirection), 0.0);
    fogColor = lerp(fogColor, sunColor, saturate(pows(sunAmount,2.0 * sunFactor) * isDayTime));

	float nearDist = max(1.0,nearFogDistance);
	// apply weather impact
	float distanceFactor = min(nearDist, invlerps(nearDist, max(nearDist,farFogDistance), distance));
	// color with fog
    return min(1.6e+6f, lerp(color, fogColor * colorFactor, saturate(pows(fogAmount * pows(distanceFactor, max(0.01,1.0 - FogPower)),fogFactor * nightTime(0.4,isDayTime)))));
}

float4 VolumetricFog(VSOUT IN) : COLOR0 
{
	float3 color = tex2D(TESR_RenderedBuffer, IN.UVCoord).rgb;
	color = pows(color,2.2); //linearise

    float depth = readDepth(IN.UVCoord);
	float3 eyeVector = toWorld(IN.UVCoord);
    //float height = (TESR_CameraPosition.xyz + eyeVector * depth).z;
	float3 eyeDirection = normalize(eyeVector);
	float fogDepth = length(eyeVector * depth);

    float isDayTime = smoothstep(0, 0.5, TESR_SunAmount.x);

	// calculate sun & sky color
	float3 up = blue.xyz;
    float sunHeight = shade(TESR_SunPosition.xyz, up);
    float sunDir = dot(eyeDirection, TESR_SunPosition.xyz);
	float sunInfluence = pows(compress(sunDir), SUNINFLUENCE);
    float3 sunColor = GetSunColor(sunHeight, TESR_SunAmount.x, isDayTime, TESR_SunColor.rgb, TESR_SunsetColor.rgb);
	float3 skyColor = GetSkyColor(0, 0.5, sunHeight, sunInfluence, TESR_SkyData.z, TESR_SkyColor.rgb, TESR_SkyLowColor.rgb, TESR_HorizonColor.rgb, sunColor);

	// low height fog uses fog color and a stronger sun factor
	float3 fogColor = pows(TESR_FogColor.rgb,2.2); // fade color between fog to horizon based on depth
	float3 newColor = applyFog(color, fogDepth, eyeDirection, TESR_SunDirection.xyz, fogColor, lerp(fogColor, sunColor, isDayTime), 0.8, 1.5, 30, 10.0, isDayTime, 2.0);
	
	// general fog, uses skyColor (includes Sun)
    fogColor = lerp(pows(TESR_FogColor.rgb,2.2), skyColor, isDayTime);
	newColor = applyFog(newColor, fogDepth, eyeDirection, TESR_SunDirection.xyz, fogColor, sunColor, 1.0 * FogDistanceFactor, 1.35 * FogColorFactor, 50.0 * FogDensityFactor, 5.4 * FogFalloffFactor, isDayTime, 1.0);

	// use luminance to color bright parts of the night sky, simulating light within the fog
	float lumaDiff = 1.0 - saturate(luma(newColor) / luma(color));
	newColor = lerp(newColor, luma(newColor) * color, lumaDiff * (1.0 - isDayTime) * TESR_DebugVar.w);

	newColor = pows(max(0.0,newColor),1.0/2.2); //delinearise
	return float4(newColor, 1.0f);
}

technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader  = compile ps_3_0 VolumetricFog();
	}
}
