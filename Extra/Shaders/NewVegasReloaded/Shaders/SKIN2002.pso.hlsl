// Skin with one point light?
//
// Parameters:

float4 AmbientColor : register(c1);
sampler2D AttenuationMap : register(s5);
sampler2D BaseMap : register(s0);
sampler2D FaceGenMap0 : register(s2);
sampler2D FaceGenMap1 : register(s3);
sampler2D GlowMap : register(s4);
sampler2D NormalMap : register(s1);
float4 PSLightColor[10];
float4 Toggles : register(c27);

float4 TESR_ReciprocalResolution;
float4 TESR_SkinData;
float4 TESR_SkinColor;
float4 TESR_DebugVar;


// Registers:
//
//   Name           Reg   Size
//   -------------- ----- ----
//   AmbientColor   const_1       1
//   PSLightColor[0]   const_3       2
//   Toggles        const_27      1
//   BaseMap        texture_0       1
//   NormalMap      texture_1       1
//   FaceGenMap0    texture_2       1
//   FaceGenMap1    texture_3       1
//   GlowMap        texture_4       1
//   AttenuationMap texture_5       1
//

#include "Includes/helpers.hlsl"

// Structures:
struct VS_INPUT {
    float2 BaseUV : TEXCOORD0;                     // uv
    float3 color_0 : COLOR0;                        // vertex color?
    float4 color_1 : COLOR1;                        // fog contribution?
    float3 texcoord_1 : TEXCOORD1_centroid;         // light data in tangent space
    float3 texcoord_2 : TEXCOORD2_centroid;
    float4 texcoord_4 : TEXCOORD4;        
    float3 texcoord_6 : TEXCOORD6_centroid;         // eye data in tangent space
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

#include "Includes/Skin.hlsl"

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float3 sunLightDirection = normalize(IN.texcoord_1);
    float3 eyeDirection = normalize(IN.texcoord_6);
    float3 normal = normalize(expand(tex2D(NormalMap, IN.BaseUV)));	
    float3 glowTexture = tex2D(GlowMap, IN.BaseUV).rgb;
    float4 baseColor = getBaseColor(IN.BaseUV, FaceGenMap0, FaceGenMap1, BaseMap);
    float fresnel = sqr(1 - shades(normal, eyeDirection));
    baseColor.rgb = (Toggles.x <= 0.0 ? baseColor.rgb : (baseColor.rgb * IN.color_0.rgb)); // vertex color

    float4 SSScolor;
    SSScolor.rgb = lerp(PSLightColor[2].rgb, glowTexture, 0.5);
    SSScolor.a = (AmbientColor.a >= 1 ? 0 : (baseColor.a - Toggles.w)); // alpha flag?

    float3 pointLightDirection = normalize(IN.texcoord_2);
    float pointLightDiffuse = dot(normal, pointLightDirection);
    float pointLightDiffuse2 = saturate((pointLightDiffuse + 0.3) * 0.769230783);
    pointLightDiffuse = saturate(pointLightDiffuse);

    float pointLightContribution = saturate(((3 - pointLightDiffuse2 * 2) * sqr(pointLightDiffuse2)) - ((3 - (pointLightDiffuse * 2)) * sqr(pointLightDiffuse))) * glowTexture;
    pointLightContribution += pointLightDiffuse * PSLightColor[2].rgb;
    pointLightContribution += fresnel * shades(eyeDirection, -pointLightDirection) * SSScolor.rgb;
    // clip(normal);

    float atten1 = tex2D(AttenuationMap, IN.texcoord_4.xy);
    float atten2 = tex2D(AttenuationMap, IN.texcoord_4.zw);
    float3 pointLightLighting = saturate((1 - atten1) - atten2) * pointLightContribution;

    float3 sunLighting = shades(normal, sunLightDirection) * PSLightColor[1].rgb;
    float3 rimColor = ((fresnel * shades(eyeDirection, -sunLightDirection)) * PSLightColor[1].rgb) * 0.5;
    float3 lighting = AmbientColor.rgb + sunLighting + rimColor + pointLightLighting;
    lighting = max(lighting, 0);

    float4 finalColor = float4(lighting.rgb * baseColor.rgb, baseColor.a * AmbientColor.a);

    finalColor.rgb = (Toggles.y <= 0.0 ? finalColor.rgb : (IN.color_1.a * (IN.color_1.rgb - finalColor.rgb)) + finalColor.rgb);	// fog
    OUT.color_0.rgba = finalColor;

    return OUT;
};

// approximately 69 instruction slots used (7 texture, 62 arithmetic)