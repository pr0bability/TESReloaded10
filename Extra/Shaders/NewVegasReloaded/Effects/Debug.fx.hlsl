float4 TESR_SunAmbient;
float4 TESR_SunColor;
float4 TESR_FogColor;
float4 TESR_WaterShallowColor; // Shallow color used by the game for water
float4 TESR_WaterDeepColor; // Deep color used by the game for water
float4 TESR_HorizonColor;
float4 TESR_SkyColor;
float4 TESR_ViewSpaceLightDir;
float4 TESR_SunDirection;
float4 TESR_DebugVar;

float4 TESR_LightPosition0;
float4 TESR_LightPosition1;
float4 TESR_LightPosition2;
float4 TESR_LightPosition3;
float4 TESR_LightPosition4;
float4 TESR_LightPosition5;
float4 TESR_LightPosition6;
float4 TESR_LightPosition7;
float4 TESR_LightPosition8;
float4 TESR_LightPosition9;
float4 TESR_LightPosition10;
float4 TESR_LightPosition11;
float4 TESR_ShadowLightPosition0;
float4 TESR_ShadowLightPosition1;
float4 TESR_ShadowLightPosition2;
float4 TESR_ShadowLightPosition3;
float4 TESR_ShadowLightPosition4;
float4 TESR_ShadowLightPosition5;
float4 TESR_ShadowLightPosition6;
float4 TESR_ShadowLightPosition7;
float4 TESR_ShadowLightPosition8;
float4 TESR_ShadowLightPosition9;
float4 TESR_ShadowLightPosition10;
float4 TESR_ShadowLightPosition11;


sampler2D TESR_RenderedBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_SourceBuffer : register(s1) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_DepthBuffer : register(s2) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_NormalsBuffer : register(s3) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = NONE; MINFILTER = NONE; MIPFILTER = NONE; };
sampler2D TESR_AvgLumaBuffer : register(s4) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = NONE; MINFILTER = NONE; MIPFILTER = NONE; };
sampler2D TESR_PointShadowBuffer : register(s5) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = NONE; MINFILTER = NONE; MIPFILTER = NONE; };


struct VSOUT {
	float4 vertPos : POSITION;
	float2 UVCoord : TEXCOORD0;
};

struct VSIN {
	float4 vertPos : POSITION0;
	float2 UVCoord : TEXCOORD0;
};

VSOUT FrameVS(VSIN IN) {
	VSOUT OUT = (VSOUT)0.0f;
	OUT.vertPos = IN.vertPos;
	OUT.UVCoord = IN.UVCoord;
	return OUT;
}

#include "Includes/Helpers.hlsl"
#include "Includes/Depth.hlsl"
#include "Includes/Normals.hlsl"


float4 displayBuffer(float4 color, float2 uv, float2 bufferPosition, float2 bufferSize, sampler2D buffer){
	float2 lowerCorner = bufferPosition + bufferSize;
	if ((uv.x < bufferPosition.x || uv.y < bufferPosition.y) || (uv.x > lowerCorner.x || uv.y > lowerCorner.y )) return color;
	return tex2D(buffer, float2(invlerp(bufferPosition, lowerCorner, uv)));
}


float4 showLightInfluence(float4 color, float2 uv, float3 position, float4 lightPos, float4 tint){
	float3 eyeDir = normalize(toWorld(uv));

	float3 lightDir = lightPos.xyz - TESR_CameraPosition.xyz;
	float lightDist = length(lightDir);
	lightDir = normalize(lightDir);

	// points for light positions
	float distance = saturate(1 - lightDist / 5000);
	if (shades(eyeDir, lightDir) > (1 - distance * 0.0001)) return tint;

	// shade area of influence
	distance = length(position - lightPos.xyz) / lightPos.w;
	distance *= distance;
	color += saturate((1 - distance) / (1 + distance)) * tint/5;
	//if (lightPos.w > 0 && lightDist < lightPos.w) color = saturate(color + float4(1, 0, 0, 1) * shades(eyeDir, lightDir));

	return color;
}

float4 DebugShader( VSOUT IN) : COLOR0 {
    
	float depth = readDepth(IN.UVCoord);
	float3 eyeDir = toWorld(IN.UVCoord);
	float3 position = eyeDir * depth + TESR_CameraPosition.xyz;

	float4 color = tex2D(TESR_RenderedBuffer, IN.UVCoord);
	color = showLightInfluence(color, IN.UVCoord, position, TESR_ShadowLightPosition0, float4(0, 1, 0, 1));
	color = showLightInfluence(color, IN.UVCoord, position, TESR_ShadowLightPosition1, float4(0, 1, 0, 1));
	color = showLightInfluence(color, IN.UVCoord, position, TESR_ShadowLightPosition2, float4(0, 1, 0, 1));
	color = showLightInfluence(color, IN.UVCoord, position, TESR_ShadowLightPosition3, float4(0, 1, 0, 1));
	color = showLightInfluence(color, IN.UVCoord, position, TESR_ShadowLightPosition4, float4(0, 1, 0, 1));
	color = showLightInfluence(color, IN.UVCoord, position, TESR_ShadowLightPosition5, float4(0, 1, 0, 1));
	color = showLightInfluence(color, IN.UVCoord, position, TESR_ShadowLightPosition6, float4(0, 0, 1, 1));
	color = showLightInfluence(color, IN.UVCoord, position, TESR_ShadowLightPosition7, float4(0, 0, 1, 1));
	color = showLightInfluence(color, IN.UVCoord, position, TESR_ShadowLightPosition8, float4(0, 0, 1, 1));
	color = showLightInfluence(color, IN.UVCoord, position, TESR_ShadowLightPosition9, float4(0, 0, 1, 1));
	color = showLightInfluence(color, IN.UVCoord, position, TESR_ShadowLightPosition10, float4(0, 0, 1, 1));
	color = showLightInfluence(color, IN.UVCoord, position, TESR_ShadowLightPosition11, float4(0, 0, 1, 1));
	color = showLightInfluence(color, IN.UVCoord, position, TESR_LightPosition0, float4(1, 0, 0, 1));
	color = showLightInfluence(color, IN.UVCoord, position, TESR_LightPosition1, float4(1, 0, 0, 1));
	color = showLightInfluence(color, IN.UVCoord, position, TESR_LightPosition2, float4(1, 0, 0, 1));
	color = showLightInfluence(color, IN.UVCoord, position, TESR_LightPosition3, float4(1, 0, 0, 1));
	color = showLightInfluence(color, IN.UVCoord, position, TESR_LightPosition4, float4(1, 0, 0, 1));
	color = showLightInfluence(color, IN.UVCoord, position, TESR_LightPosition6, float4(1, 0, 0, 1));
	color = showLightInfluence(color, IN.UVCoord, position, TESR_LightPosition7, float4(1, 0, 0, 1));
	color = showLightInfluence(color, IN.UVCoord, position, TESR_LightPosition8, float4(1, 0, 0, 1));
	color = showLightInfluence(color, IN.UVCoord, position, TESR_LightPosition9, float4(1, 0, 0, 1));
	color = showLightInfluence(color, IN.UVCoord, position, TESR_LightPosition10, float4(1, 0, 0, 1));
	color = showLightInfluence(color, IN.UVCoord, position, TESR_LightPosition11, float4(1, 0, 0, 1));

    if (IN.UVCoord.x > 0.9 && IN.UVCoord.x < 0.95){
        if (IN.UVCoord.y > 0.15 && IN.UVCoord.y < 0.2) return TESR_SunColor;
        if (IN.UVCoord.y > 0.2 && IN.UVCoord.y < 0.25) return TESR_SunAmbient;
        if (IN.UVCoord.y > 0.25 && IN.UVCoord.y < 0.3) return TESR_FogColor;
        if (IN.UVCoord.y > 0.3 && IN.UVCoord.y < 0.35) return TESR_HorizonColor;
        if (IN.UVCoord.y > 0.35 && IN.UVCoord.y < 0.4) return TESR_SkyColor;
        if (IN.UVCoord.y > 0.4 && IN.UVCoord.y < 0.45) return TESR_WaterShallowColor;
        if (IN.UVCoord.y > 0.45 && IN.UVCoord.y < 0.5) return TESR_WaterDeepColor;
        if (IN.UVCoord.y > 0.55 && IN.UVCoord.y < 0.6) return tex2D(TESR_AvgLumaBuffer, float2(0.5, 0.5));
        if (IN.UVCoord.y > 0.65 && IN.UVCoord.y < 0.7) return TESR_ViewSpaceLightDir;
        if (IN.UVCoord.y > 0.7 && IN.UVCoord.y < 0.75) return TESR_SunDirection;
    }

	color = displayBuffer(color, IN.UVCoord, float2(0.1, 0.05), float2(0.15, 0.15), TESR_NormalsBuffer);
	color = displayBuffer(color, IN.UVCoord, float2(0.3, 0.05), float2(0.15, 0.15), TESR_DepthBuffer);
	color = displayBuffer(color, IN.UVCoord, float2(0.5, 0.05), float2(0.15, 0.15), TESR_PointShadowBuffer);

    return color;
}

technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 DebugShader();
	}
}