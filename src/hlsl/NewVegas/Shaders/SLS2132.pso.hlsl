//  Terrain shader with blending of 6 textures
//
// Parameters:
sampler2D BaseMap[7] :register(s0);
sampler2D NormalMap[7] :register(s7);

float4 AmbientColor : register(c1);
float4 PSLightColor[10] : register(c3);
float4 PSLightDir : register(c18);


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   AmbientColor const_1       1
//   PSLightColor[0] const_3       1
//   PSLightDir   const_18      1
//   BaseMap      texture_0       6
//   NormalMap    texture_7       6
//


// Structures:

struct VS_INPUT {
    float2 uv : TEXCOORD0;
    float3 vertex_color : TEXCOORD1_centroid;
    float3 lPosition : TEXCOORD2_centroid;
    float3 tangent : TEXCOORD3_centroid;
    float3 binormal : TEXCOORD4_centroid;
    float3 normal : TEXCOORD5_centroid;
    float4 blend_0 : COLOR0;
    float4 blend_1 : COLOR1;
    float4 viewPosition : TEXCOORD7_centroid;
    float4 sPosition : POSITION1;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

#define tint magenta

#include "includes/Helpers.hlsl"
#include "includes/Terrain.hlsl"
#include "includes/Parallax.hlsl"

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    int texCount = 6;
    float3 tangent = normalize(IN.tangent.xyz);
    float3 binormal = normalize(IN.binormal.xyz);
    float3 normal = normalize(IN.normal.xyz);
    float3x3 tbn = float3x3(tangent, binormal, normal);
    float3 eyeDir = -mul(tbn, normalize(IN.viewPosition.xyz));

    float dist = distance(IN.sPosition.xyz, IN.viewPosition.xyz);
    float2 dx, dy;
    dx = ddx(IN.uv.xy);
    dy = ddy(IN.uv.xy);
    
    float blends[7] = { IN.blend_0.x, IN.blend_0.y, IN.blend_0.z, IN.blend_0.w, IN.blend_1.x, IN.blend_1.y, IN.blend_1.z };
    float2 coords[7];
    [unroll] for (int i = 0; i < texCount; ++i)
    {
        coords[i] = getParallaxCoords(dist, IN.uv.xy, dx, dy, eyeDir, BaseMap[i], blends[i]);
    }

    float4 normal0 = tex2D(NormalMap[0], coords[0]);
    float4 normal1 = tex2D(NormalMap[1], coords[1]);
    float4 normal2 = tex2D(NormalMap[2], coords[2]);
    float4 normal3 = tex2D(NormalMap[3], coords[3]);
    float4 normal4 = tex2D(NormalMap[4], coords[4]);
    float4 normal5 = tex2D(NormalMap[5], coords[5]);

    float3 texture0 = tex2D(BaseMap[0], coords[0]).xyz;
    float3 texture1 = tex2D(BaseMap[1], coords[1]).xyz;
    float3 texture2 = tex2D(BaseMap[2], coords[2]).xyz;
    float3 texture3 = tex2D(BaseMap[3], coords[3]).xyz;
    float3 texture4 = tex2D(BaseMap[4], coords[4]).xyz;
    float3 texture5 = tex2D(BaseMap[5], coords[5]).xyz;

    float3 baseColor = blendTextures(IN.blend_0, IN.blend_1, IN.vertex_color, texture0, texture1, texture2, texture3, texture4, texture5);
    float3 combinedNormal = normalize(expand(normal0.xyz) * IN.blend_0.r + expand(normal1.xyz) * IN.blend_0.g + expand(normal2.xyz) * IN.blend_0.b + expand(normal3.xyz) * IN.blend_0.a + expand(normal4.xyz) * IN.blend_1.r + expand(normal5.xyz) * IN.blend_1.g);
    float roughness = combineRoughness(IN.blend_0, IN.blend_1, normal0.a, normal1.a, normal2.a, normal3.a, normal4.a, normal5.a);

    float3 lightTS = mul(tbn, PSLightDir.xyz);
    float parallaxShadowMultiplier = getParallaxShadowMultipler(dist, dx, dy, lightTS, texCount, blends, coords, BaseMap);
    
    float3 lighting = getSunLighting(tbn, PSLightDir.xyz, PSLightColor[0].rgb, eyeDir, IN.viewPosition.xyz, combinedNormal, AmbientColor.rgb, baseColor, roughness, 1.0, parallaxShadowMultiplier);

    float3 finalColor = getFinalColor(lighting, baseColor);

    OUT.color_0.a = 1;
    OUT.color_0.rgb = finalColor;

    return OUT;
};

// approximately 61 instruction slots used (12 texture, 49 arithmetic)
