//  Template for terrain shaders for blending up to 7 textures and using up to 8 pointlights.

#if defined(__INTELLISENSE__)
    #define PS
    #define POINTLIGHT
#endif

#define TERRAIN

#include "includes/Helpers.hlsl"
#include "includes/Terrain.hlsl"
#include "includes/Parallax.hlsl"

struct VS_INPUT {
    float4 position : POSITION;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 normal : NORMAL;
    float4 uv : TEXCOORD0;
    float4 vertex_color : COLOR0;
    float4 blend_0 : TEXCOORD1;
    float4 blend_1 : TEXCOORD2;
};

struct VS_OUTPUT {
    float4 blend_0 : COLOR0;
    float4 blend_1 : COLOR1;
    float4 sPosition : POSITION;
    float2 uv : TEXCOORD0;
    float3 vertex_color : TEXCOORD1;
    float3 lPosition : TEXCOORD2;
    float3 tangent : TEXCOORD3;
    float3 binormal : TEXCOORD4;
    float3 normal : TEXCOORD5;
    float4 fog : TEXCOORD6;
    float3 viewPosition : TEXCOORD7;
};

#ifdef VS

float3 FogColor : register(c15);
float4 FogParam : register(c14);
row_major float4x4 ModelViewProj : register(c0);
float4x4 TESR_InvViewProjectionTransform : register(c36);

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float3 mdl0;

    mdl0.xyz = mul(float3x4(ModelViewProj[0].xyzw, ModelViewProj[1].xyzw, ModelViewProj[2].xyzw), IN.position.xyzw);

    OUT.blend_0 = IN.blend_0;
    OUT.blend_1 = IN.blend_1;

    OUT.sPosition.w = dot(ModelViewProj[3].xyzw, IN.position.xyzw);
    OUT.sPosition.xyz = mdl0.xyz;
    OUT.uv.xy = IN.uv.xy;
    OUT.vertex_color.xyz = IN.vertex_color.rgb;
    OUT.lPosition.xyz = IN.position.xyz;
    OUT.tangent.xyz = IN.tangent.xyz;
    OUT.binormal.xyz = IN.binormal.xyz;
    OUT.normal.xyz = IN.normal.xyz;
    
    // Fog.
    float4 fog;
    fog.zw = FogParam.z;
    fog.xy = 1 - saturate((FogParam.x - length(mdl0.xyz)) / FogParam.y);
    
    fog = lit(fog.x, fog.y, fog.w);
    
    OUT.fog.a = fog.z;
    OUT.fog.rgb = FogColor.rgb;
    
    OUT.viewPosition.xyz = mul(TESR_InvViewProjectionTransform, OUT.sPosition).xyz;

    return OUT;
};

#endif  // Vertex shader.

struct PS_INPUT
{
    float2 uv : TEXCOORD0;
    float3 vertex_color : TEXCOORD1_centroid;
    float3 lPosition : TEXCOORD2_centroid;
    float3 tangent : TEXCOORD3_centroid;
    float3 binormal : TEXCOORD4_centroid;
    float3 normal : TEXCOORD5_centroid;
    float4 blend_0 : COLOR0;
    float4 blend_1 : COLOR1;
    float4 fog : TEXCOORD6_centroid;
    float3 viewPosition : TEXCOORD7_centroid;
    float4 sPosition : POSITION1;
};

struct PS_OUTPUT {
    float4 color_0 : COLOR0;
};

#ifdef PS

sampler2D BaseMap[7]:register(s0);
sampler2D NormalMap[7]:register(s7);

float4 AmbientColor : register(c1);
float4 PSLightColor[13] : register(c3);
float4 PSLightDir : register(c18);
float4 PSLightPosition[12] : register(c19);

PS_OUTPUT main(PS_INPUT IN) {
    PS_OUTPUT OUT;
    
    int texCount = TEX_COUNT;  // Macro.
    float3 tangent = normalize(IN.tangent.xyz);
    float3 binormal = normalize(IN.binormal.xyz);
    float3 normal = normalize(IN.normal.xyz);
    float3x3 tbn = float3x3(tangent, binormal, normal);
    float3 eyeDir = -mul(tbn, normalize(IN.viewPosition.xyz));

    float dist = length(IN.viewPosition.xyz);

    float2 dx, dy;
    dx = ddx(IN.uv.xy);
    dy = ddy(IN.uv.xy);
    
    float weights[7] = { 0, 0, 0, 0, 0, 0, 0 };
    float blends[7] = { IN.blend_0.x, IN.blend_0.y, IN.blend_0.z, IN.blend_0.w, IN.blend_1.x, IN.blend_1.y, IN.blend_1.z };
    float2 offsetUV = getParallaxCoords(dist, IN.uv.xy, dx, dy, eyeDir, texCount, BaseMap, blends, weights);

    float roughness = 1.f;
    float3 baseColor = blendDiffuseMaps(IN.vertex_color, offsetUV, texCount, BaseMap, weights);
    float3 combinedNormal = blendNormalMaps(offsetUV, texCount, NormalMap, weights, roughness);

    float3 lightTS = mul(tbn, PSLightDir.xyz);
    float parallaxShadowMultiplier = getParallaxShadowMultipler(dist, offsetUV, dx, dy, lightTS, texCount, blends, BaseMap);
    
    float3 lighting = getSunLighting(lightTS, PSLightColor[0].rgb, eyeDir, combinedNormal, AmbientColor.rgb, baseColor, roughness, 1.0, parallaxShadowMultiplier);

    #if defined(POINTLIGHT)
        int lightCount = 12;
        float3 pointlightDir;
        [unroll] for (int i = 0; i < lightCount; i++) {
            pointlightDir = mul(tbn, PSLightPosition[i].xyz - IN.lPosition.xyz);
            lighting += getPointLightLighting(pointlightDir, PSLightPosition[i].w, PSLightColor[i + 1].rgb, eyeDir, combinedNormal, baseColor, roughness, 1.0);
        }
    #endif
    
    float3 finalColor = lighting;
    finalColor = lerp(finalColor, IN.fog.rgb, IN.fog.a); // Apply fog.

    OUT.color_0.a = 1;
    OUT.color_0.rgb = finalColor;

    return OUT;
};

#endif  // Pixel shader.
