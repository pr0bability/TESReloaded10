//  Terrain shader with 1 texture
//
// Parameters:
sampler2D BaseMap[7] : register(s0);
sampler2D NormalMap[7] : register(s7);

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
//   BaseMap      texture_0       1
//   NormalMap    texture_7       1
//


// Structures:

struct VS_INPUT {
    float2 uv : TEXCOORD0_centroid;
    float3 vertex_color : TEXCOORD1_centroid;
    float3 tangent : TEXCOORD3_centroid;
    float3 binormal : TEXCOORD4_centroid;
    float3 normal : TEXCOORD5_centroid;
    float blend_0 : COLOR0;
    float4 viewPosition : TEXCOORD7_centroid;
    float4 sPosition : POSITION1;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

#define tint yellow

#include "includes/Helpers.hlsl"
#include "includes/Terrain.hlsl"
#include "includes/Parallax.hlsl"

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;
    
    float3 tangent = normalize(IN.tangent.xyz);
    float3 binormal = normalize(IN.binormal.xyz);
    float3 normal = normalize(IN.normal.xyz);
    float3x3 tbn = float3x3(tangent, binormal, normal);
    float3 eyeDir = -mul(tbn, normalize(IN.viewPosition.xyz));
    
    float dist = distance(IN.sPosition.xyz, IN.viewPosition.xyz);
    float2 dx, dy;
    dx = ddx(IN.uv.xy);
    dy = ddy(IN.uv.xy);
    
    float2 coords0 = getParallaxCoords(dist, IN.uv.xy, dx, dy, eyeDir, BaseMap[0], IN.blend_0.r);;

    float4 normal0 = tex2D(NormalMap[0], coords0);    
    float3 texture0 = tex2D(BaseMap[0], coords0).rgb;

    float3 baseColor = linearize(texture0 * IN.blend_0.r) * linearize(IN.vertex_color);
    float3 combinedNormal = normalize(expand(normal0.xyz)) * IN.blend_0.r;
    float roughness = combineRoughness(IN.blend_0, black, normal0.a);
    
    float blends[7];
    blends[0] = IN.blend_0.r;
    float2 coords[7];
    coords[0] = coords0;
    float3 lightTS = mul(tbn, PSLightDir.xyz);
    float parallaxShadowMultiplier = getParallaxShadowMultipler(dist, dx, dy, lightTS, 1, blends, coords, BaseMap);

    float3 lighting = getSunLighting(tbn, PSLightDir.xyz, PSLightColor[0].rgb, eyeDir, IN.viewPosition.xyz, combinedNormal, AmbientColor.rgb, baseColor, roughness, 1.0, parallaxShadowMultiplier);

    float3 finalColor = getFinalColor(lighting, baseColor);

    OUT.color_0.rgb = finalColor;
    OUT.color_0.a = 1;

    return OUT;
};


// approximately 31 instruction slots used (2 texture, 29 arithmetic)
