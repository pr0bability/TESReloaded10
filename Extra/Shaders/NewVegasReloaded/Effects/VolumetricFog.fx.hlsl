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

static const float SunExponent = max(TESR_VolumetricFogData.x, 1); // 8
static const float SunGlareCoeff = TESR_VolumetricFogData.y; // 100
static const float FogStrength = TESR_VolumetricFogData.z; // 1
static const float MaxFogHeight = TESR_VolumetricFogData.w; // 80000

static const float SUNINFLUENCE = 1/TESR_SkyData.y;

static const float4x4 ditherMat = {{0.0588, 0.5294, 0.1765, 0.6471},
									{0.7647, 0.2941, 0.8824, 0.4118},
									{0.2353, 0.7059, 0.1176, 0.5882},
									{0.9412, 0.4706, 0.8235, 0.3259}};

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


float ExponentialFog(float posHeight, float distance) {
	float fogAmount = exp(-posHeight) * (1.0 - exp(-distance)) * FogStrength;
	return fogAmount;
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

    //float3 skyColor = lerp(TESR_SkyLowColor.rgb, TESR_HorizonColor.rgb, 0.5 + 0.5 * sunHeight); // tint the base more with horizon color when sun is high
	float3 sunColor = GetSunColor(sunHeight, 1, TESR_SunAmount.x, TESR_SunColor.rgb, TESR_SunsetColor.rgb);
	float3 skyColor = GetSkyColor(0, 0.5, sunHeight, sunInfluence, TESR_SkyData.z, TESR_SkyColor.rgb, TESR_SkyLowColor.rgb, TESR_HorizonColor.rgb, sunColor);

	// blend with fog color
	float3 fogColor = lerp(TESR_FogColor.rgb, skyColor, pow(depth/farZ, 0.3)).rgb; // fade color between fog to horizon based on depth
	fogColor = lerp(color, fogColor, fogAmount).rgb; // calculate final color of scene through the fog

	float lumaDiff = max(luma (color) - luma(fogColor), 0);
	fogColor += lumaDiff * (color - luma(fogColor)) * lerp(1, 0.5, fogAmount); // bring back the light above the fog color

	//color += lerp(0, ditherMat[ (IN.UVCoord.x)%4 ][ (IN.UVCoord.y)%4 ] / 255, TESR_SunAmount.x);

	return float4(fogColor, 1.0f);
}

technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader  = compile ps_3_0 VolumetricFog();
	}
}
