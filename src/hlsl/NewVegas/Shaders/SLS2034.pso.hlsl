// basic object shader with 1 direct light and 2 point lights
//
// Parameters:

float4 AmbientColor : register(c1);
float4 EmittanceColor : register(c2);
float4 PSLightColor[10] : register(c3);
float4 PSLightPosition[8] : register(c19);
float4 Toggles : register(c27);

sampler2D BaseMap : register(s0);
sampler2D NormalMap : register(s1);


// Registers:
//
//   Name            Reg   Size
//   --------------- ----- ----
//   AmbientColor    const_1       1
//   EmittanceColor  const_2       1
//   PSLightColor[0]    const_3       3
//   PSLightPosition[0] const_19      2
//   Toggles         const_27      1
//   BaseMap         texture_0       1
//   NormalMap       texture_1       1
//


// Structures:

struct VS_INPUT {
    float4 texcoord_2 : TEXCOORD2;
    float3 texcoord_3 : TEXCOORD3;
    float4 texcoord_4 : TEXCOORD4;
    float3 texcoord_5 : TEXCOORD5;
    float3 texcoord_6 : TEXCOORD6;
    float3 texcoord_7 : TEXCOORD7;
    float2 BaseUV : TEXCOORD0;
    float4 texcoord_1 : TEXCOORD1;
    float3 color_0 : COLOR0;
    float4 color_1 : COLOR1;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:
#define useVertexColor Toggles.x
#define useFog Toggles.y
#define glossPower Toggles.z
#define alphaTestRef Toggles.w

#include "Includes/helpers.hlsl"
#include "Includes/PBR.hlsl"

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float4 texture0 = linearize(tex2D(BaseMap, IN.BaseUV.xy));
    int uiLightCount = int(EmittanceColor.a);

    float4 r2 = (AmbientColor.a < 1 ? (texture0.a - alphaTestRef) : 0); // ??
    clip(r2.xyzw);

    float4 normal = tex2D(NormalMap, IN.BaseUV.xy);
    normal.xyz = normalize(expand(normal.xyz));

    float3 color = useVertexColor > 0 ? texture0.rgb * linearize(IN.color_0.rgb) : texture0.rgb;
    float3 eyeDir = normalize(IN.texcoord_3.xyz);

    float3 final = PBR(0, 1 - normal.a, color, normal.xyz, eyeDir, normalize(IN.texcoord_1.xyz), linearize(PSLightColor[0].rgb) * IN.texcoord_1.w) + color * linearize(AmbientColor.rgb);

    if (uiLightCount >= 1){
        float atten = length(IN.texcoord_2.xyz / IN.texcoord_2.w);
        float s = saturate(sqr(atten)); 
        atten = saturate(((1 - s) * (1 - s)) / (1 + 5.0 * s));
        final += PBR(0, 1 - normal.a, color, normal.xyz, eyeDir, normalize(IN.texcoord_2.xyz), linearize(PSLightColor[1].rgb) * atten);
    }

    if (uiLightCount >= 2){
        float atten1 = length(IN.texcoord_4.xyz / IN.texcoord_4.w);
        float s = saturate(sqr(atten1)); 
        atten1 = saturate(((1 - s) * (1 - s)) / (1 + 5.0 * s));
        final += PBR(0, 1 - normal.a, color, normal.xyz, eyeDir, normalize(IN.texcoord_4.xyz), linearize(PSLightColor[2].rgb) * atten1);
    }

    OUT.color_0.rgb = delinearize(final);
    OUT.color_0.a = texture0.a * AmbientColor.a;

    return OUT;
};

// approximately 85 instruction slots used (2 texture, 83 arithmetic)
