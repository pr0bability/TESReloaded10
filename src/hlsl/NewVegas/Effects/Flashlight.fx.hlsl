float4x4 TESR_FlashLightViewProjTransform;
float4 TESR_CameraForward;
float4 TESR_SpotLightPosition;
float4 TESR_SpotLightColor;
float4 TESR_SpotLightDirection;
float4 TESR_SunColor;
float4 TESR_SunDirection;
float4 TESR_DebugVar;
float4 TESR_ReciprocalResolution;

sampler2D TESR_SourceBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_RenderedBuffer : register(s1) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_DepthBuffer : register(s2) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = ANISOTROPIC; MIPFILTER = LINEAR; };
sampler2D TESR_PointShadowBuffer : register(s3)  = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_NormalsBuffer : register(s4) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_ShadowSpotlightBuffer0 : register(s5) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_SpotLightTexture : register(s6) < string ResourceName = "Effects\Flashlight.png"; > = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };


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

#include "Includes/Depth.hlsl"
#include "Includes/Helpers.hlsl"
#include "Includes/Normals.hlsl"
#include "Includes/BlurDepth.hlsl"

float4 displayBuffer(float4 color, float2 uv, float2 bufferPosition, float2 bufferSize, sampler2D buffer){
	float2 lowerCorner = bufferPosition + bufferSize;
	if ((uv.x < bufferPosition.x || uv.y < bufferPosition.y) || (uv.x > lowerCorner.x || uv.y > lowerCorner.y )) return color;
	return tex2D(buffer, float2(invlerp(bufferPosition, lowerCorner, uv)))/TESR_DebugVar.y;
}


float4 ScreenCoordToTexCoord(float4 coord){
	// apply perspective (perspective division) and convert from -1/1 to range to 0/1 (shadowMap range);
	coord.xyz /= coord.w;
	coord.x = coord.x * 0.5f + 0.5f;
	coord.y = coord.y * -0.5f + 0.5f;

	return coord;
}


float4 Shadows(VSOUT IN) : COLOR0
{
	float depth = readDepth(IN.UVCoord);
    float3 eyeVector = toWorld(IN.UVCoord);
    float4 worldPos = float4(TESR_CameraPosition.xyz + eyeVector * depth, 1);
    float3 lightToWorld = TESR_SpotLightPosition.xyz - worldPos.xyz;

    float radius = TESR_SpotLightPosition.w;
    float Distance = length(lightToWorld)/radius;

	float4 lightSpaceCoord = ScreenCoordToTexCoord(mul(worldPos, TESR_FlashLightViewProjTransform));
	float shadowDepth =	tex2D(TESR_ShadowSpotlightBuffer0, lightSpaceCoord.xy).r;
	float isShadow = shadowDepth + 0.05 * Distance > Distance; // BIAS to reduce shadowing artefacts

	if (Distance < 1 && lightSpaceCoord.x > 0.0 && lightSpaceCoord.x < 1.0 && lightSpaceCoord.y > 0.0 && lightSpaceCoord.y < 1.0) return float4(isShadow.rrr, 1);
	return float4(1, 1, 1, 1);
}

float4 NoShadow(VSOUT IN) : COLOR0
{
	return float4(1, 1, 1, 1);
}

float4 Flashlight(VSOUT IN) : COLOR0
{
	float depth = readDepth(IN.UVCoord);
    float3 eyeVector = toWorld(IN.UVCoord);
	float3 eyeDirection = normalize(eyeVector);
    float3 worldPos = TESR_CameraPosition.xyz + eyeVector * depth;
	float3 eyePos = TESR_CameraPosition.xyz;
	float fogDepth = length(eyeVector * depth); // a depth measure that's at the same distance from the camera at all angles
	float3 normal = GetWorldNormal(IN.UVCoord);

    float3 lightpos = TESR_SpotLightPosition.xyz;
    float3 lightDir = TESR_SpotLightDirection.xyz;
    float3 lightToWorld = lightpos - worldPos;
    float3 lightVector = normalize(lightToWorld);

    float diffuse = shade(lightVector, normal);
    float specular = pows(shades(normalize(eyeDirection + lightVector * -1), normal), 5);

	// radius based attenuation based on https://lisyarus.github.io/blog/graphics/2022/07/30/point-light-attenuation.html
    float radius = TESR_SpotLightPosition.w;
    float Distance = length(lightToWorld)/radius;
	float s = saturate(Distance * Distance); 
	float atten = saturate(((1 - s) * (1 - s)) / (1 + 5.0 * s));

	float4 lightSpaceCoord = ScreenCoordToTexCoord(mul(float4(worldPos, 1), TESR_FlashLightViewProjTransform));
	float isShadow = tex2D(TESR_RenderedBuffer, IN.UVCoord);

	float lightTexture = tex2D(TESR_SpotLightTexture, lightSpaceCoord.xy).r;

	float sunLuma = 1 / max(0.05, luma(TESR_SunColor));
    float3 lightColor = TESR_SpotLightColor.rgb * TESR_SpotLightColor.w * sunLuma;
    float4 color = linearize(tex2D(TESR_SourceBuffer, IN.UVCoord));
	
	float angleCosMax = cos(radians(TESR_SpotLightDirection.w));
	float angleCosMin = cos(radians(TESR_SpotLightDirection.w * 0.5));
	float cone = pow(invlerps(angleCosMax, angleCosMin, shades(lightDir, lightVector * -1)), 2.0);

    float3 light = (diffuse + specular) * lightColor * cone * atten * lightTexture * isShadow;

	color.rgb += color.rgb * max(0.0, luma(exp(-color.rgb * 3.5)) * light); // modulate light with base color brightness to compensate for the post process aspect

	// if (lightSpaceCoord.x > 0.0 && lightSpaceCoord.x < 1.0 && lightSpaceCoord.y > 0.0 && lightSpaceCoord.y < 1.0) return float4(light.xxx, 1);
	// color = displayBuffer(color, IN.UVCoord, float2(0.7, 0.15), float2(0.2, 0.2), TESR_ShadowSpotlightBuffer0);

    return delinearize(color);
}

technique {

	pass {
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 NoShadow();
	}

	pass {
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Flashlight();
	}

}


technique {

	pass {
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Shadows();
	}
	
	pass {
		VertexShader = compile vs_3_0 FrameVS();
	 	PixelShader = compile ps_3_0 DepthBlur(TESR_RenderedBuffer, OffsetMaskH, 1.0, 3500, TESR_SpotLightPosition.w * 2);
	}

	pass {
		VertexShader = compile vs_3_0 FrameVS();
	 	PixelShader = compile ps_3_0 DepthBlur(TESR_RenderedBuffer, OffsetMaskV, 1.0, 3500, TESR_SpotLightPosition.w * 2);
	}
	
	pass {
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Flashlight();
	}

}
 