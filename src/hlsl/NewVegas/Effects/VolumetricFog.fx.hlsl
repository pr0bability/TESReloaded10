// Volumetric Fog fullscreen shader for Oblivion/Skyrim Reloaded

float4 TESR_FogColor;
float4 TESR_FogData;
float4 TESR_VolumetricFogLow; // Low Fog
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
float4 TESR_VolumetricFogHigh; // General Fog
float4 TESR_VolumetricFogSimple; // Simple Fog
float4 TESR_VolumetricFogBlend; // Simple Fog
float4 TESR_VolumetricFogHeight; // Simple Fog
float4 TESR_VolumetricFogData; // Simple Fog

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

static const float SUNINFLUENCE = 1/TESR_SkyData.y;

// scale settings for easier tuning
static const float LowFogDensity = 30 * TESR_VolumetricFogLow.x;
static const float LowFogFalloff = 20.0 * TESR_VolumetricFogLow.y;
static const float LowFogDist = 5.5 * TESR_VolumetricFogLow.z;
static const float SunPower = TESR_VolumetricFogLow.w;

static const float HeightFogDensity = 25 * TESR_VolumetricFogHigh.x;
static const float HeightFogFalloff = 4 * TESR_VolumetricFogHigh.y;
static const float HeightFogDist = 10.0 * TESR_VolumetricFogHigh.z;
static const float HeightFogSkyColor = 0.1 * TESR_VolumetricFogHigh.w;

static const float LowFogHeight = 2000.0 * TESR_VolumetricFogHeight.x;
static const float HeightFogHeight = 7000.0 * TESR_VolumetricFogHeight.y;
static const float SimpleFogHeight = 120000.0 * TESR_VolumetricFogHeight.z;
static const float InteriorFogHeight = TESR_VolumetricFogHeight.w; // 0 or 1 to activate/cancel fog in interiors

static const float SimpleFogExtinction = TESR_VolumetricFogSimple.x;
static const float SimpleFogInscattering = TESR_VolumetricFogSimple.y;
static const float SimpleFogNight = 2.5 * TESR_VolumetricFogSimple.z;
static const float SimpleFogSkyColor = 0.22 * TESR_VolumetricFogSimple.w;

static const float LowFogBlend = TESR_VolumetricFogBlend.x;
static const float HeightFogBlend = TESR_VolumetricFogBlend.y;
static const float HeightFogRolloff = 7.0 * TESR_VolumetricFogBlend.z;
static const float SimpleFogBlend = 1.5 * TESR_VolumetricFogBlend.w;

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

float applyHeightFog(float distance, float distanceFactor, float3 cameraToPoint, float densityFactor, float falloffFactor)
{
	// calculate fog amount
	float density = 0.08 * densityFactor * 0.000006; //0.0002
	float falloff = 0.02 * falloffFactor * 0.00225;
    float fogAmount = max(0.0,density/falloff) * exp(-TESR_CameraPosition.z*falloff) * (1.0-exp(-distance*cameraToPoint.z*falloff))/cameraToPoint.z;
	// return final Fog Factor
    return fogAmount * distanceFactor;
}

float3 applySimpleFog(float3 color, float3 fogColor, float distance, float be, float bi)
{
	return color * exp(-distance*be) + fogColor * (1.0-exp(-distance*bi));
}

float3 fogWithSun(float3 fogColor, float fogPower, float3 skyColor, float skyPower, float3 sunColor, float sunAmount, float isDayTime){
	float3 color = lerp(fogColor, skyColor, pows(fogPower, skyPower * InteriorFogHeight) * isDayTime);
	color = lerp(color, sunColor, sunAmount * saturate(SunPower * isDayTime * InteriorFogHeight));
	return color;
}


float4 VolumetricFog(VSOUT IN) : COLOR0 
{
	float4 color = tex2D(TESR_RenderedBuffer, IN.UVCoord);
	float3 linearColor = linearize(color).rgb;
	float3 pureFogColor = linearize(TESR_FogColor).rgb;

    float depth = readDepth(IN.UVCoord);
	float3 eyeVector = toWorld(IN.UVCoord);
    float initialHeight = reconstructWorldPosition(IN.UVCoord).z;
	float3 eyeDirection = normalize(eyeVector);
	float fogDepth = length(eyeVector * depth);

    float isDayTime = smoothstep(0, 0.5, TESR_SunAmount.x);

	// calculate sun & sky color
	float3 up = blue.xyz;
    float sunHeight = shade(TESR_SunPosition.xyz, up);
    float sunDir = dot(eyeDirection, TESR_SunPosition.xyz);
	float sunInfluence = pows(compress(sunDir), SUNINFLUENCE);
    float3 sunColor = GetSunColor(sunHeight, 1, TESR_SunAmount.x, TESR_SunColor.rgb, TESR_SunsetColor.rgb);
	float3 skyColor = GetSkyColor(0, 0.5, sunHeight, sunInfluence, TESR_SkyData.z, TESR_SkyColor.rgb, TESR_SkyLowColor.rgb, TESR_HorizonColor.rgb, sunColor);
	// calculate sun color area
    float sunAmount = pows(dot(max(eyeDirection, TESR_SunDirection.xyz), 0.0),8.0);


	// apply weather impact
	float preDist = min(0.0, nearFogDistance);
	preDist = max(0.0, invlerps(nearFogDistance, max(nearFogDistance, farFogDistance - preDist), fogDepth));
	float height = saturate(exp( -initialHeight/ LowFogHeight));
	float distance = pows(preDist, FogPower * LowFogDist) * height; // fade with height

	// low height fog uses fog color and a stronger sun factor
	float4 lowFog = float4(pureFogColor,applyHeightFog(fogDepth, distance, eyeDirection, LowFogDensity * (1.0 + saturate(1.0 - FogPower)), LowFogFalloff) * LowFogBlend);
	
	// general fog, uses skyColor (includes Sun)
	float3 fogColor = fogWithSun(pureFogColor, fogDepth/farZ, skyColor, HeightFogSkyColor, sunColor, sunAmount, isDayTime);

	height = saturate(exp( -initialHeight / HeightFogHeight)); // fade with height
	distance = pows(preDist, FogPower * HeightFogDist) * height;	
	float4 heightFog = float4(fogColor, applyHeightFog(fogDepth, distance, eyeDirection, HeightFogDensity * (1.0 + saturate(1.0 - FogPower)), HeightFogFalloff) * HeightFogBlend);
	
	// blend the height fog and low fog
	float2 colorScalar = normalize(float2(lowFog.a, heightFog.a));
	fogColor = (lowFog.rgb * colorScalar.x + heightFog.rgb * colorScalar.y) / (colorScalar.x + colorScalar.y);
	heightFog.rgb = min(1.6e+6f, lerp(linearColor, fogColor.rgb, saturate(pows(max(heightFog.a, lowFog.a), 1.0 / HeightFogRolloff))));

	// Simple fog
	fogColor = fogWithSun(pureFogColor, fogDepth/farZ, skyColor, SimpleFogSkyColor, sunColor, sunAmount, isDayTime); 

	// use luminance to color bright parts of the night sky, simulating light within the fog
	//float lumaDiff = 1.0 - saturate(luma(fogColor) / luma(linearColor));
	//fogColor = lerp(fogColor, luma(fogColor) * color.rgb, lumaDiff * (1.0 - isDayTime));

	height = saturate(exp( -initialHeight/ SimpleFogHeight)); // fade with height
	distance = pows(preDist, FogPower * 3.0) * pow(height,2.0);
	float3 simpleFog = applySimpleFog(linearColor, fogColor, distance, SimpleFogExtinction * nightTime(SimpleFogNight, isDayTime), SimpleFogInscattering);
	
    // fogColor = lerp(simpleFog.rgb, heightFog.rgb, saturate(1.5 * TESR_VolumetricFogBlend.w * FogPower) * compress(isDayTime));
    fogColor = lerp(simpleFog.rgb, heightFog.rgb, saturate(SimpleFogBlend * FogPower));


	fogColor = lerp(linearColor, fogColor, TESR_VolumetricFogData.z); // general fog power setting	
	return delinearize(float4(fogColor, 1.0));
}

technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader  = compile ps_3_0 VolumetricFog();
	}
}
