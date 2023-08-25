// Skin Shader for when only the sun light is used
//
// Parameters:

float4 AmbientColor : register(c1);
sampler2D BaseMap : register(s0);
sampler2D FaceGenMap0 : register(s2);
sampler2D FaceGenMap1 : register(s3);
sampler2D NormalMap : register(s1);
float4 PSLightColor[10] : register(c3);
float4 Toggles : register(c27);


#include "Includes/Helpers.hlsl"

// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   AmbientColor const_1       1
//   PSLightColor[0] const_3       1
//   Toggles      const_27      1
//   BaseMap      texture_0       1
//   NormalMap    texture_1       1
//   FaceGenMap0  texture_2       1
//   FaceGenMap1  texture_3       1
//


// Structures:

struct VS_INPUT {
    float2 BaseUV : TEXCOORD0;			            // UV
    float3 texcoord_1 : TEXCOORD1;			// light data in tangent space
    float3 texcoord_6 : TEXCOORD6;			// eye data in tangent space
    float3 color_0 : COLOR0;                // vertex color?
    float4 color_1 : COLOR1;                // fog contribution?
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};


VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    // base geometry information
    float3 lightDirection = normalize(IN.texcoord_1);
    float3 eyeDirection = normalize(IN.texcoord_6);
    float3 normal = normalize(expand(tex2D(NormalMap, IN.BaseUV.xy).xyz));

    // calculate lighting components
    float fresnelCoeff = 1 - shades(normal, eyeDirection);
    float3 fresnel = (shades(lightDirection, -eyeDirection) * sqr(fresnelCoeff) * PSLightColor[0].rgb) * 0.5;
    float3 lighting = ((shades(normal, lightDirection) * PSLightColor[0].rgb) + fresnel) + AmbientColor.rgb;

    // sample color textures
    float3 faceGenMap0 = tex2D(FaceGenMap0, IN.BaseUV.xy).rgb;
    float3 faceGenMap1 = tex2D(FaceGenMap1, IN.BaseUV.xy).rgb;
    float4 baseTexture = tex2D(BaseMap, IN.BaseUV.xy);
    float3 baseColor = 2 * ((expand(faceGenMap0) + baseTexture.rgb) * (2 * faceGenMap1));

    baseColor = (Toggles.x <= 0.0 ? baseColor : (baseColor * IN.color_0.rgb)); // apply vertex color
    float4 color = (AmbientColor.a >= 1 ? 0 : (baseTexture.a - Toggles.w));

    float3 finalColor = max(lighting, 0) * baseColor;

    color.rgb = Toggles.y <= 0.0 ? finalColor : ((IN.color_1.a * (IN.color_1.rgb - finalColor)) + finalColor); // apply fog color contribution
    color.a = baseTexture.a * AmbientColor.a;

    // OUT.color_0 = color;
    OUT.color_0 = float4(IN.BaseUV, 0, 1);

    return OUT;
};

// approximately 42 instruction slots used (4 texture, 38 arithmetic)
