// Terrain shader with blending of 3 textures and 3 lights
//
// Parameters:

sampler2D BaseMap[7]:register(s0);
sampler2D NormalMap[7]:register(s7);

float4 AmbientColor : register(c1);
float4 PSLightColor[10] : register(c3);
float4 PSLightDir : register(c18);
float4 PSLightPosition[8] : register(c19);;


// Registers:
//
//   Name            Reg   Size
//   --------------- ----- ----
//   AmbientColor    const_1       1
//   PSLightColor[0]    const_3       4
//   PSLightDir      const_18      1
//   PSLightPosition[0] const_19      3
//   BaseMap         texture_0       3
//   NormalMap       texture_7       3
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
    float4 viewPosition : TEXCOORD7_centroid;
    float4 sPosition : POSITION1;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

 #define tint cyan

#include "includes/Helpers.hlsl"
#include "includes/Terrain.hlsl"
#include "includes/Parallax.hlsl"

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;
    
    int texCount = 3;
    float3 tangent = normalize(IN.tangent.xyz);
    float3 binormal = normalize(IN.binormal.xyz);
    float3 normal = normalize(IN.normal.xyz);
    float3x3 tbn = float3x3(tangent, binormal, normal);
    float3 eyeDir = -mul(tbn, normalize(IN.viewPosition.xyz));

    float dist = distance(IN.sPosition.xyz, IN.viewPosition.xyz);
    float2 dx, dy;
    dx = ddx(IN.uv.xy);
    dy = ddy(IN.uv.xy);
    
    float blends[7] = { IN.blend_0.x, IN.blend_0.y, IN.blend_0.z, IN.blend_0.w, 0, 0, 0 };
    float2 coords[7];
    [unroll] for (int i = 0; i < texCount; ++i)
    {
        coords[i] = getParallaxCoords(dist, IN.uv.xy, dx, dy, eyeDir, BaseMap[i], blends[i]);
    }

    float4 normal0 = tex2D(NormalMap[0], coords[0]);
    float4 normal1 = tex2D(NormalMap[1], coords[1]);
    float4 normal2 = tex2D(NormalMap[2], coords[2]);
    
    float3 texture0 = tex2D(BaseMap[0], coords[0]).xyz;
    float3 texture1 = tex2D(BaseMap[1], coords[1]).xyz;
    float3 texture2 = tex2D(BaseMap[2], coords[2]).xyz;

    float3 baseColor = blendTextures(IN.blend_0, black, IN.vertex_color, texture0, texture1, texture2);
    float3 combinedNormal = normalize(expand(normal0.xyz) * IN.blend_0.r + expand(normal1.xyz) * IN.blend_0.g + expand(normal2.xyz) * IN.blend_0.b);
    float roughness = combineRoughness(IN.blend_0, black, normal0.a, normal1.a, normal2.a);

    float3 lightTS = mul(tbn, PSLightDir.xyz);
    float parallaxShadowMultiplier = getParallaxShadowMultipler(dist, dx, dy, lightTS, texCount, blends, coords, BaseMap);
    
    float3 lighting = getSunLighting(tbn, PSLightDir.xyz, PSLightColor[0].rgb, eyeDir, IN.viewPosition.xyz, combinedNormal, AmbientColor.rgb, baseColor, roughness, 1.0, parallaxShadowMultiplier);
 
    int lightCount = 8;
    [unroll] for (i = 0; i < lightCount; i++)
    {
        lighting += getPointLightLighting(tbn, PSLightPosition[i], PSLightColor[i + 1].rgb, eyeDir, IN.lPosition.xyz, combinedNormal, baseColor, roughness);
    }
   
    float3 finalColor = getFinalColor(lighting, baseColor);

    OUT.color_0.a = 1;
    OUT.color_0.rgb = finalColor;

    return OUT;

};

// approximately 85 instruction slots used (6 texture, 79 arithmetic)
