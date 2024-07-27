// Basic lighting shader with one light
//
// Parameters:

float4 AmbientColor : register(c1);
sampler2D BaseMap : register(s0);
sampler2D NormalMap : register(s1);
float4 PSLightColor[10] : register(c3);
float4 Toggles : register(c27);


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   AmbientColor const_1       1
//   PSLightColor[0] const_3       1
//   Toggles      const_27      1
//   BaseMap      texture_0       1
//   NormalMap    texture_1       1
//


// Structures:

struct VS_INPUT {
    float2 BaseUV : TEXCOORD0;
    float3 color_0 : COLOR0;
    float4 color_1 : COLOR1;
    float4 texcoord_1 : TEXCOORD1_centroid;
    float3 texcoord_3 : TEXCOORD3_centroid;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

#include "Includes/Helpers.hlsl"
#include "Includes/PBR.hlsl"

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float4 texture0 = tex2D(BaseMap, IN.BaseUV.xy);
    
    float4 r1 = AmbientColor.rgba;
    float4 r2 = (AmbientColor.a >= 1 ? 0 : (texture0.a - Toggles.w));
    clip(r2.xyzw);

    float3 normal = normalize(expand(tex2D(NormalMap, IN.BaseUV.xy)));

    float NdotH = pow(abs(shades(normal, normalize(IN.texcoord_3.xyz))), Toggles.z) * r2.w; //specular
    float NdotL = dot(normal, IN.texcoord_1.xyz);

    float3 specular = saturate(((NdotL < 0.2 ? (NdotH * saturate(NdotL + 0.5)) : NdotH) * PSLightColor[0].rgb) * IN.texcoord_1.w); // strengthen specular when facing away
    float3 lighting = max((saturate(NdotL) * PSLightColor[0].rgb) + AmbientColor.xyz, 0);

    // final color
    float3 color = ((Toggles.x ? (texture0.rgb * IN.color_0.rgb) : texture0.rgb) * lighting.xyz) + specular;

    // apply fog
    float3 final = Toggles.y ? lerp(color, IN.color_1.rgb, IN.color_1.a) : color;

    OUT.color_0.rgb = final * blue.rgb;
    OUT.color_0.a = texture0.a * AmbientColor.a;

    return OUT;
};

// approximately 38 instruction slots used (2 texture, 36 arithmetic)
