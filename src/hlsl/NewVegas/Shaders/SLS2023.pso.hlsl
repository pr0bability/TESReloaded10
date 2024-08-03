// general object shader with one direct light and one point light
//
// Parameters:

float4 AmbientColor : register(c1);
float4 PSLightColor[10] : register (c3);
float4 Toggles : register(c27);

sampler2D AttenuationMap : register(s5);
sampler2D BaseMap : register(s0);
sampler2D NormalMap : register(s1);


// Registers:
//
//   Name           Reg   Size
//   -------------- ----- ----
//   AmbientColor   const_1       1
//   PSLightColor[0]   const_3       2
//   Toggles        const_27      1
//   BaseMap        texture_0       1
//   NormalMap      texture_1       1
//   AttenuationMap texture_5       1
//


// Structures:

struct VS_INPUT {
    float2 BaseUV : TEXCOORD0;
    float3 color_0 : COLOR0;
    float4 color_1 : COLOR1;
    float4 texcoord_1 : TEXCOORD1_centroid;
    float4 texcoord_2 : TEXCOORD2_centroid;
    float3 texcoord_3 : TEXCOORD3_centroid;
    float3 texcoord_4 : TEXCOORD4_centroid;
    float4 texcoord_5 : TEXCOORD5;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:
#define useVertexColor Toggles.x
#define useFog Toggles.y
#define glossPower Toggles.z
#define alphaTestRef Toggles.w

#include "Includes/Helpers.hlsl"
#include "Includes/PBR.hlsl"

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float4 normal = tex2D(NormalMap, IN.BaseUV.xy);
    normal.xyz = normalize(expand(tex2D(NormalMap, IN.BaseUV.xy).xyz));

    float4 texture0 = linearize(tex2D(BaseMap, IN.BaseUV.xy));

    // clip faded objects and no alpha pixels
    float4 r1 = (AmbientColor.a >= 1 ? 0 : (texture0.w - alphaTestRef));
    clip(r1.xyzw);

    float3 color = useVertexColor > 0 ? texture0.rgb * linearize(IN.color_0.rgb) : texture0.xyz;

    float3 eyeDir = normalize(IN.texcoord_3.xyz);
    float3 final = PBR(0, 1 - normal.a, color, normal.xyz, eyeDir, normalize(IN.texcoord_1.xyz), linearize(PSLightColor[0].rgb) * IN.texcoord_1.w) + color * linearize(AmbientColor.rgb);

    float atten = length(IN.texcoord_2.xyz / IN.texcoord_2.w);
    float s = saturate(sqr(atten)); 
    atten = saturate(((1 - s) * (1 - s)) / (1 + 5.0 * s));
    final += PBR(0, 1 - normal.a, color, normal.xyz, eyeDir, normalize(IN.texcoord_2.xyz), linearize(PSLightColor[1].rgb) * atten);

    OUT.color_0.rgb = delinearize(final);
    OUT.color_0.a = texture0.a * AmbientColor.a;

    return OUT;
};

// approximately 67 instruction slots used (4 texture, 63 arithmetic)
