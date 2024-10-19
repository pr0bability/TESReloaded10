// Template for object lighting shaders.
//
// VS
//
// AD
// SLS2028 - LIGHTS = 2 (ONLY_LIGHT)
// SLS2029 - LIGHTS = 2, SKIN (ONLY_LIGHT)
// SLS2030 - LIGHTS = 2, PROJ_SHADOW (ONLY_LIGHT)
// SLS2031 - LIGHTS = 2, PROJ_SHADOW, SKIN (ONLY_LIGHT)
// SLS2032 - LIGHTS = 3 (ONLY_LIGHT)
// SLS2033 - LIGHTS = 3, SKIN (ONLY_LIGHT)
// SLS2034 - LIGHTS = 3, PROJ_SHADOW (ONLY_LIGHT)
// SLS2035 - LIGHTS = 3, PROJ_SHADOW, SKIN (ONLY_LIGHT)
//
// ADTS
// SLS2000 - Base
// SLS2001 - LOD
// SLS2003 - SKIN
// SLS2004 - PROJ_SHADOW
// SLS2006 - PROJ_SHADOW, SKIN
// SLS2007 - STBB
// SLS2008 - LIGHTS = 2
// SLS2009 - LIGHTS = 2, SKIN
// SLS2010 - LIGHTS = 2, PROJ_SHADOW
// SLS2011 - LIGHTS = 2, PROJ_SHADOW, SKIN
// SLS2012 - SPECULAR
// SLS2013 - SPECULAR, SKIN
// SLS2014 - SPECULAR, PROJ_SHADOW
// SLS2015 - SPECULAR, PROJ_SHADOW, SKIN
// SLS2016 - SPECULAR, LIGHTS = 2
// SLS2017 - SPECULAR, LIGHTS = 2, SKIN
// SLS2018 - SPECULAR, LIGHTS = 2, PROJ_SHADOW
// SLS2019 - SPECULAR, LIGHTS = 2, PROJ_SHADOW, SKIN
// (ignored) SLS2102 - LIGHTS = 0
//
// ADTS10
// SLS2020 - LIGHTS = 9
// SLS2021 - LIGHTS = 9, SKIN
// SLS2022 - LIGHTS = 4
// SLS2023 - LIGHTS = 4, OPT
// SLS2024 - LIGHTS = 4, SKIN
// SLS2025 - LIGHTS = 4, SPECULAR
// SLS2026 - LIGHTS = 4, SPECULAR, OPT
// SLS2027 - LIGHTS = 4, SPECULAR, SKIN
//
// DiffusePt
// SLS2036 - LIGHTS = 2 (DIFFUSE)
// SLS2037 - LIGHTS = 2, SKIN (DIFFUSE)
// SLS2038 - LIGHTS = 3 (DIFFUSE)
// SLS2039 - LIGHTS = 3, SKIN (DIFFUSE)
//
// Specular
// SLS2040 - Base (ONLY_SPECULAR)
// SLS2041 - SKIN (ONLY_SPECULAR)
// SLS2042 - PROJ_SHADOW (ONLY_SPECULAR)
// SLS2043 - PROJ_SHADOW, SKIN (ONLY_SPECULAR)
// SLS2044 - POINT (ONLY_SPECULAR)
// SLS2045 - POINT, SKIN (ONLY_SPECULAR)
// SLS2046 - POINT, NUM_PT_LIGHTS = 2 (ONLY_SPECULAR)
// SLS2047 - POINT, NUM_PT_LIGHTS = 2, SKIN (ONLY_SPECULAR)
// SLS2048 - POINT, NUM_PT_LIGHTS = 3 (ONLY_SPECULAR)
// SLS2049 - POINT, NUM_PT_LIGHTS = 3, SKIN (ONLY_SPECULAR)
//
// PS
//
// AD
// SLS2037 - LIGHTS = 2 (ONLY_LIGHT, OPT)
// SLS2038 - LIGHTS = 2, SI (ONLY_LIGHT, OPT)
// SLS2039 - LIGHTS = 2, PROJ_SHADOW (ONLY_LIGHT, OPT)
// SLS2040 - LIGHTS = 2, SI, PROJ_SHADOW (ONLY_LIGHT, OPT)
// SLS2041 - LIGHTS = 3 (ONLY_LIGHT, OPT)
// SLS2042 - LIGHTS = 3, SI (ONLY_LIGHT, OPT)
// SLS2043 - LIGHTS = 3, PROJ_SHADOW (ONLY_LIGHT, OPT)
// SLS2044 - LIGHTS = 3, SI, PROJ_SHADOW (ONLY_LIGHT, OPT)
//
// ADTS
// SLS2000 - Default
// SLS2001 - OPT
// SLS2002 - OPT, LOD
// SLS2004 - SI 
// SLS2005 - PROJ_SHADOW
// SLS2007 - SI, PROJ_SHADOW 
// SLS2008 - STBB
// SLS2009 - HAIR
// SLS2010 - HAIR, PROJ_SHADOW
// SLS2011 - LIGHTS = 2
// SLS2012 - LIGHTS = 2, SI
// SLS2013 - LIGHTS = 2, HAIR
// SLS2014 - LIGHTS = 2, PROJ_SHADOW
// SLS2015 - LIGHTS = 2, SI, PROJ_SHADOW
// SLS2016 - LIGHTS = 2, HAIR, PROJ_SHADOW
// SLS2017 - SPECULAR
// SLS2018 - SPECULAR, SI
// SLS2019 - SPECULAR, HAIR
// SLS2020 - SPECULAR, PROJ_SHADOW
// SLS2021 - SPECULAR, SI, PROJ_SHADOW
// SLS2022 - SPECULAR, HAIR, PROJ_SHADOW
// SLS2023 - SPECULAR, LIGHTS = 2
// SLS2024 - SPECULAR, LIGHTS = 2, SI
// SLS2026 - SPECULAR, LIGHTS = 2, PROJ_SHADOW
// SLS2027 - SPECULAR, LIGHTS = 2, SI, PROJ_SHADOW
// (ignored) SLS2151 - LIGHTS = 0, SILHOUETTE
//
// ADTS10
// SLS2029 - LIGHTS = 9
// SLS2030 - LIGHTS = 9, SI
// SLS2031 - LIGHTS = 4
// SLS2032 - LIGHTS = 4, OPT 
// SLS2033 - LIGHTS = 4, SI
// SLS2034 - LIGHTS = 4, SPECULAR
// SLS2035 - LIGHTS = 4, SPECULAR, OPT
// SLS2036 - LIGHTS = 4, SPECULAR, SI
//
// DiffusePt
// SLS2045 - LIGHTS = 2 (DIFFUSE)
// SLS2046 - LIGHTS = 3 (DIFFUSE)
//
// Specular
// SLS2047 - Base (ONLY_SPECULAR)
// SLS2048 - HAIR (ONLY_SPECULAR)
// SLS2049 - PROJ_SHADOW (ONLY_SPECULAR)
// SLS2050 - PROJ_SHADOW, HAIR (ONLY_SPECULAR)
// SLS2051 - POINT (ONLY_SPECULAR)
// SLS2052 - POINT, HAIR (ONLY_SPECULAR)
// SLS2053 - NUM_PT_LIGHTS = 2, POINT (ONLY_SPECULAR)
// SLS2054 - NUM_PT_LIGHTS = 2, POINT, HAIR (ONLY_SPECULAR)
// SLS2055 - NUM_PT_LIGHTS = 3, POINT (ONLY_SPECULAR)
// SLS2056 - NUM_PT_LIGHTS = 3, POINT, HAIR (ONLY_SPECULAR)

#if defined(__INTELLISENSE__)
    #define VS
    #define LIGHTS 4
    #define OPT
    #define SPECULAR
#endif

#if defined(DIFFUSE)
    #define ONLY_LIGHT
    #define OPT
#endif

#if defined(ONLY_SPECULAR)
    #define ONLY_LIGHT
    #define SPECULAR
#endif

#ifdef ONLY_LIGHT
    #define NO_FOG
    #define NO_VERTEX_COLOR
#endif

#include "includes/Helpers.hlsl"

#ifdef SKIN
    #include "includes/SkinHelpers.hlsl"
#endif

// Toggles.
#define useVertexColor Toggles.x
#define useFog Toggles.y
#define glossPower Toggles.z
#define alphaTestRef Toggles.w

struct VS_INPUT {
    float4 position : POSITION;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 normal : NORMAL;
    float4 uv : TEXCOORD0;
#ifndef NO_VERTEX_COLOR
    float4 vertexColor : COLOR0;
#endif
#ifdef SKIN
    float3 blendWeight : BLENDWEIGHT;
    float4 blendIndices : BLENDINDICES;
#endif
};

#if defined(VS) && LIGHTS < 4

struct VS_OUTPUT {
#ifndef NO_VERTEX_COLOR
    float4 vertexColor : COLOR0;
#endif
#ifndef NO_FOG
    float4 fogColor : COLOR1;
#endif
    float4 sPosition : POSITION;
    float2 uv : TEXCOORD0;
    float4 lightDir : TEXCOORD1;
#if defined(DIFFUSE)
    float4 lightAtt : TEXCOORD4;
#elif defined(SPECULAR)
    float3 halfwayDir : TEXCOORD3;
#endif

#if LIGHTS > 1
    float4 light2Dir : TEXCOORD2;
    
    #if defined(DIFFUSE)
        float4 light2Att : TEXCOORD5;
    #elif defined(SPECULAR)
        float3 halfway2Dir : TEXCOORD4;
        float4 light2Att : TEXCOORD5;
    #else
        float4 light2Att : TEXCOORD4;
    #endif
#endif
    
#if LIGHTS > 2
    float4 light3Dir : TEXCOORD3;
    #if defined(DIFFUSE)
        float4 light3Att : TEXCOORD6;
    #else
        float4 light3Att : TEXCOORD5;
    #endif
#endif
    
#if NUM_PT_LIGHTS > 1
    float4 light2Dir : TEXCOORD2;
    float3 halfway2Dir : TEXCOORD4;
#endif
    
#if NUM_PT_LIGHTS > 2
    float4 light3Dir : TEXCOORD5;
    float3 halfway3Dir : TEXCOORD6;
#endif
    
#ifdef PROJ_SHADOW
    float4 shadowUVs : TEXCOORD7;
#endif
};

#ifndef NO_FOG
    float3 FogColor : register(c15);
    float4 FogParam : register(c14);
#endif

float4 LightData[10] : register(c25);

#ifndef SKIN
    row_major float4x4 ModelViewProj : register(c0);
#else
    row_major float4x4 SkinModelViewProj : register(c1);
    float4 Bones[54] : register(c44);
#endif

#ifdef SPECULAR
    float4 EyePosition : register(c16);
#endif

#ifdef PROJ_SHADOW
    row_major float4x4 ShadowProj : register(c18);
    float4 ShadowProjData : register(c22);
    float4 ShadowProjTransform : register(c23);
#endif

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;
    
    OUT.uv = IN.uv.xy;
    
    float4 position = IN.position.xyzw;
    
    #ifndef SKIN
        float3x3 tbn = float3x3(IN.tangent.xyz, IN.binormal.xyz, IN.normal.xyz);
    
        OUT.sPosition.xyzw = mul(ModelViewProj, position.xyzw);
    #else
        float4 offset = IN.blendIndices.zyxw * 765.01001;
        float4 blend = IN.blendWeight.xyzz;
        blend.w = 1 - weight(IN.blendWeight.xyz);
        float3x3 tbn = BonesTransformTBN(Bones, offset, blend, IN.tangent, IN.binormal, IN.normal);
    
        position.w = 1;
        position.xyz = BonesTransformPosition(Bones, offset, blend, position);
    
        OUT.sPosition.xyzw = mul(SkinModelViewProj, position.xyzw);
    #endif
    
    #if defined(DIFFUSE) || defined(POINT)
        float3 light = LightData[0].xyz - position.xyz;
    #else
        float3 light = LightData[0].xyz;
    #endif
    
    #if defined(ONLY_SPECULAR) && !defined(POINT)
        OUT.lightDir.w = 1;
    #else
        OUT.lightDir.w = LightData[0].w;
    #endif
    
    #if defined(DIFFUSE)
        OUT.lightDir.xyz = mul(tbn, normalize(light));
    #elif defined(POINT)
        OUT.lightDir.xyz = mul(tbn, light);
    #else
        OUT.lightDir.xyz = normalize(mul(tbn, light));
    #endif
    
    #if defined(DIFFUSE)
        OUT.lightAtt.w = 0.5;
        OUT.lightAtt.xyz = compress(light / LightData[0].w);
    #elif defined(SPECULAR)
        float3 viewDir = normalize(EyePosition.xyz - position.xyz);
        OUT.halfwayDir.xyz = normalize(mul(tbn, normalize(viewDir + normalize(light))));
    #endif
    
    #if LIGHTS > 1
        light = LightData[1].xyz - position.xyz;
        OUT.light2Dir.w = 1;
        OUT.light2Dir.xyz = mul(tbn, normalize(light));
        OUT.light2Att.w = 0.5;
        OUT.light2Att.xyz = compress(light / LightData[1].w);
    
        #ifdef SPECULAR
            OUT.halfway2Dir.xyz = mul(tbn, normalize(viewDir + normalize(light)));
        #endif
    #endif
    
    #if LIGHTS > 2
        light = LightData[2].xyz - position.xyz;
        OUT.light3Dir.w = 1;
        OUT.light3Dir.xyz = mul(tbn, normalize(light));
        OUT.light3Att.w = 0.5;
        OUT.light3Att.xyz = compress(light / LightData[2].w);
    #endif
    
    #if NUM_PT_LIGHTS > 1
        light = LightData[1].xyz - position.xyz;
        OUT.light2Dir.w = LightData[1].w;
        OUT.light2Dir.xyz = mul(tbn, light);
        OUT.halfway2Dir.xyz = mul(tbn, normalize(viewDir + normalize(light)));
    #endif
    
    #if NUM_PT_LIGHTS > 2
        light = LightData[2].xyz - position.xyz;
        OUT.light3Dir.w = LightData[2].w;
        OUT.light3Dir.xyz = mul(tbn, light);
        OUT.halfway3Dir.xyz = mul(tbn, normalize(viewDir + normalize(light)));
    #endif
    
    #ifndef NO_VERTEX_COLOR
        OUT.vertexColor = IN.vertexColor;
    #endif

    #ifndef NO_FOG
        float fogStrength = 1 - saturate((FogParam.x - length(OUT.sPosition.xyz)) / FogParam.y);
        fogStrength = log2(fogStrength);
        OUT.fogColor.a = exp2(fogStrength * FogParam.z);
        OUT.fogColor.rgb = FogColor.rgb;
    #endif
    
    #ifdef PROJ_SHADOW
        float shadowParam = dot(ShadowProj[3].xyzw, position.xyzw);
        float2 shadowUV;
        shadowUV.x = dot(ShadowProj[0].xyzw, position.xyzw);
        shadowUV.y = dot(ShadowProj[1].xyzw, position.xyzw);
        OUT.shadowUVs.xy = ((shadowParam * ShadowProjTransform.xy) + shadowUV) / (shadowParam * ShadowProjTransform.w);
        OUT.shadowUVs.zw = ((shadowUV.xy - ShadowProjData.xy) / ShadowProjData.w) * float2(1, -1) + float2(0, 1);
    #endif

    return OUT;
};

#elif defined(VS) && LIGHTS >= 4

#if LIGHTS > 4
    #define MAX_LIGHTS 6
#elif LIGHTS > 3 && !defined(SPECULAR)
    #define MAX_LIGHTS 4
#else
    #define MAX_LIGHTS 3
#endif

struct VS_OUTPUT {
    float4 vertexColor : COLOR0;
    float4 fogColor : COLOR1;
    float4 sPosition : POSITION;
    float2 uv : TEXCOORD0;
    float4 lPosition : TEXCOORD1;
    float4 lightDir : TEXCOORD2;
    float4 light2 : TEXCOORD3;
    float4 light3 : TEXCOORD4;
#ifndef SPECULAR
    #if MAX_LIGHTS > 3
        float4 light4 : TEXCOORD5;
    #endif
    #if MAX_LIGHTS > 4
        float4 light5 : TEXCOORD6;
        float4 light6 : TEXCOORD7;
    #endif
#else
    float3 halfwayDir : TEXCOORD5;
    float3 halfway2Dir : TEXCOORD6;
    float3 halfway3Dir : TEXCOORD7;
#endif
};

float3 FogColor : register(c15);
float4 FogParam : register(c14);
float4 LightData[10] : register(c25);
#ifndef SKIN
row_major float4x4 ModelViewProj : register(c0);
#else
    row_major float4x4 SkinModelViewProj : register(c1);
    float4 Bones[54] : register(c44);
#endif
#ifdef SPECULAR
float4 EyePosition : register(c16);
#endif
#ifndef OPT
    float4 fvars0 : register(c17);

    #define lightOffset 0
#else
    #define lightOffset 1
#endif

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;
    
    OUT.uv = IN.uv.xy;
    
    float4 position = IN.position.xyzw;
    
    #ifndef SKIN
    float3x3 tbn = float3x3(IN.tangent.xyz, IN.binormal.xyz, IN.normal.xyz);
    
    OUT.sPosition.xyzw = mul(ModelViewProj, position.xyzw);
    #else
        float4 offset = IN.blendIndices.zyxw * 765.01001;
        float4 blend = IN.blendWeight.xyzz;
        blend.w = 1 - weight(IN.blendWeight.xyz);
        float3x3 tbn = BonesTransformTBN(Bones, offset, blend, IN.tangent, IN.binormal, IN.normal);
    
        position.w = 1;
        position.xyz = BonesTransformPosition(Bones, offset, blend, position);
        OUT.sPosition.xyzw = mul(SkinModelViewProj, position.xyzw);
    #endif
    
    OUT.lPosition.xyz = position.xyz;
    OUT.lPosition.w = LightData[0].w;
    
    #ifndef OPT
        float lights = min(MAX_LIGHTS, fvars0.z);
    #elif defined(SPECULAR)
    float lights = min(MAX_LIGHTS, EyePosition.w);
    #else
        float lights = min(MAX_LIGHTS, LightData[0].w);
    #endif
    float lightsFrac = frac(lights);
    float lightsThreshold = (lights < 0.0 ? (-lightsFrac < lightsFrac ? 1.0 : 0.0) : 0) + (lights - lightsFrac);
    float lightUsed;
    float3 light;
    
    #ifdef SPECULAR
    float3 viewDir = normalize(EyePosition.xyz - position.xyz);
    #endif
    
    #ifndef OPT
        OUT.lightDir.w = 1;
        OUT.lightDir.xyz = normalize(mul(tbn, LightData[0].xyz));
    
        #ifdef SPECULAR
            OUT.halfwayDir.xyz = lightUsed * mul(tbn, viewDir + LightData[0].xyz);
        #endif
    #else
    light = normalize(LightData[lightOffset + 0].xyz - position.xyz);
    lightUsed = 0 < lightsThreshold ? 1.0 : 0.0;
    OUT.lightDir.xyz = lightUsed * mul(tbn, light);
    OUT.lightDir.w = lightUsed * LightData[lightOffset + 0].w;
    
        #ifdef SPECULAR
    OUT.halfwayDir.xyz = lightUsed * mul(tbn, viewDir + light);
        #endif
    #endif
  
    light = normalize(LightData[lightOffset + 1].xyz - position.xyz);
    lightUsed = 1 < lightsThreshold ? 1.0 : 0.0;
    OUT.light2.xyz = lightUsed * mul(tbn, light);
    OUT.light2.w = lightUsed * LightData[lightOffset + 1].w;
    #ifdef SPECULAR
    OUT.halfway2Dir.xyz = lightUsed * mul(tbn, viewDir + light);
    #endif
    
    light = normalize(LightData[lightOffset + 2].xyz - position.xyz);
    lightUsed = 2 < lightsThreshold ? 1.0 : 0.0;
    OUT.light3.xyz = lightUsed * mul(tbn, light);
    OUT.light3.w = lightUsed * LightData[lightOffset + 2].w;
    #ifdef SPECULAR
    OUT.halfway3Dir.xyz = lightUsed * mul(tbn, viewDir + light);
    #endif
    
    #if MAX_LIGHTS > 3
        lightUsed = 3 < lightsThreshold ? 1.0 : 0.0;
        OUT.light4.xyz = lightUsed * mul(tbn, normalize(LightData[lightOffset + 3].xyz - position.xyz));
        OUT.light4.w = lightUsed * LightData[lightOffset + 3].w;
    #endif
    
    #if MAX_LIGHTS > 4
        lightUsed = 4 < lightsThreshold ? 1.0 : 0.0;
        OUT.light5.xyz = lightUsed * mul(tbn, normalize(LightData[lightOffset + 4].xyz - position.xyz));
        OUT.light5.w = lightUsed * LightData[lightOffset + 4].w;
        
        lightUsed = 5 < lightsThreshold ? 1.0 : 0.0;
        OUT.light6.xyz = lightUsed * mul(tbn, normalize(LightData[lightOffset + 5].xyz - position.xyz));
        OUT.light6.w = lightUsed * LightData[lightOffset + 5].w;
    #endif
    
    OUT.vertexColor = IN.vertexColor;

    float fogStrength = 1 - saturate((FogParam.x - length(OUT.sPosition.xyz)) / FogParam.y);
    fogStrength = log2(fogStrength);
    OUT.fogColor.a = exp2(fogStrength * FogParam.z);
    OUT.fogColor.rgb = FogColor.rgb;

    return OUT;
};
#endif // Vertex shaders.

#if defined(PS) && (!defined(LIGHTS) || LIGHTS < 4)

struct PS_INPUT {
#ifndef NO_VERTEX_COLOR
    float3 vertexColor : COLOR0;
#endif
#ifndef NO_FOG
    float4 fogColor : COLOR1;
#endif
    float2 uv : TEXCOORD0;
    float4 lightDir : TEXCOORD1_centroid;
    #if defined(DIFFUSE)
        float4 lightAtt : TEXCOORD4;
    #elif defined(SPECULAR)
        float3 halfwayDir : TEXCOORD3_centroid;
    #endif
#if LIGHTS > 1
    float3 light2Dir : TEXCOORD2_centroid;
    #if defined(DIFFUSE)
        float4 light2Att : TEXCOORD5;
    #elif defined(SPECULAR)
        float3 halfway2Dir : TEXCOORD4_centroid;
        float4 light2Att : TEXCOORD5;
    #else
        float4 light2Att : TEXCOORD4;
    #endif
#endif
#if LIGHTS > 2
    float3 light3Dir : TEXCOORD3_centroid;
    #if defined(DIFFUSE)
        float4 light3Att : TEXCOORD6;
    #else
        float4 light3Att : TEXCOORD5;
    #endif
#endif
#if NUM_PT_LIGHTS > 1
    float4 light2Dir : TEXCOORD2_centroid;
    float3 halfway2Dir : TEXCOORD4_centroid;
#endif
#if NUM_PT_LIGHTS > 2
    float4 light3Dir : TEXCOORD5_centroid;
    float3 halfway3Dir : TEXCOORD6_centroid;
#endif
#ifdef PROJ_SHADOW
    float4 shadowUVs : TEXCOORD7;
#endif
};

struct PS_OUTPUT {
    float4 color : COLOR0;
};

#if defined(DIFFUSE) || defined(ONLY_SPECULAR)
    sampler2D NormalMap : register(s0);
#else
    sampler2D BaseMap : register(s0);
    sampler2D NormalMap : register(s1);
#endif

#if !defined(DIFFUSE) && !defined(ONLY_SPECULAR)
    float4 AmbientColor : register(c1);
#endif

float4 PSLightColor[10] : register(c3);

#if (defined(SI) || defined(HAIR)) && !defined(ONLY_SPECULAR)
    #ifdef ONLY_LIGHT
        sampler2D GlowMap : register(s3);
    #else
        sampler2D GlowMap : register(s4);
    #endif
    float4 EmittanceColor : register(c2);
#endif

#ifdef PROJ_SHADOW
    #if defined(ONLY_SPECULAR)
        sampler2D ShadowMap : register(s4);
        sampler2D ShadowMaskMap : register(s5);
    #elif defined(ONLY_LIGHT)
        sampler2D ShadowMap : register(s5);
        sampler2D ShadowMaskMap : register(s6);
    #else
        sampler2D ShadowMap : register(s6);
        sampler2D ShadowMaskMap : register(s7);
    #endif
#endif

#if LIGHTS > 1
    #if defined(DIFFUSE)
        sampler2D AttenuationMap : register(s3);
    #elif defined(ONLY_LIGHT)
        sampler2D AttenuationMap : register(s4);
    #else
        sampler2D AttenuationMap : register(s5);
    #endif
#endif

#ifndef OPT
    float4 Toggles : register(c27);
#endif

PS_OUTPUT main(PS_INPUT IN) {
    PS_OUTPUT OUT;
    
    #if !defined(DIFFUSE) && !defined(ONLY_SPECULAR)
        float4 baseColor = tex2D(BaseMap, IN.uv.xy);
    #endif
    
    #if !defined(OPT) && !defined(ONLY_SPECULAR)
        clip(AmbientColor.a >= 1 ? 0 : (baseColor.a - alphaTestRef));
    #endif
    
    float4 normal = tex2D(NormalMap, IN.uv.xy);
    normal.xyz = normalize(expand(normal.xyz));
    
    #ifndef NO_VERTEX_COLOR
        #if defined(HAIR)
            float4 glow = tex2D(GlowMap, IN.uv.xy);
            baseColor.rgb = (2 * ((IN.vertexColor.g * (EmittanceColor.rgb - 0.5)) + 0.5)) * lerp(baseColor.rgb, glow.rgb, glow.a);
        #elif !defined(OPT)
            baseColor.rgb = useVertexColor <= 0 ? baseColor.rgb : (baseColor.rgb * IN.vertexColor.rgb);
        #else
            baseColor.rgb = baseColor.rgb * IN.vertexColor.rgb;
        #endif
    #endif
    
    float att1, att2, finalAtt;
    #if defined(DIFFUSE)
        float3 lighting = shades(normal.xyz, normalize(IN.lightDir.xyz)) * PSLightColor[0].rgb;
    #elif !defined(ONLY_SPECULAR)
        float3 lighting = shades(normal.xyz, IN.lightDir.xyz) * PSLightColor[0].rgb;
    #else
        float3 lighting = 0;
    #endif
    
    #if defined(DIFFUSE)
        att1 = tex2D(AttenuationMap, IN.lightAtt.xy).x;
        att2 = tex2D(AttenuationMap, IN.lightAtt.zw).x;
        finalAtt = saturate(1 - att1 - att2);
        lighting *= finalAtt;
    #endif
    
    // STBB?
    #ifdef STBB
        lighting *= 0.85;
    #endif
    
    // Shadows.
    float3 shadowMultiplier = 1;
    #ifdef PROJ_SHADOW
        float3 shadow = tex2D(ShadowMap, IN.shadowUVs.xy).xyz;
        float shadowMask = tex2D(ShadowMaskMap, IN.shadowUVs.zw).x;
        shadowMultiplier = lerp(1, shadow, shadowMask);
    #endif
    
    lighting *= shadowMultiplier;
    
    // Self emmitance.
    #ifdef SI
        float3 glow = tex2D(GlowMap, IN.uv.xy).rgb;
        lighting += glow.rgb * EmittanceColor.rgb;
    #endif
    
    #if !defined(DIFFUSE) && !defined(ONLY_SPECULAR)
        lighting += AmbientColor.rgb;
    #endif
    
    // Other light sources.
    #if LIGHTS > 1
        att1 = tex2D(AttenuationMap, IN.light2Att.xy).x;
        att2 = tex2D(AttenuationMap, IN.light2Att.zw).x;
        finalAtt = saturate(1 - att1 - att2);
        lighting += (finalAtt * (shades(normal.xyz, normalize(IN.light2Dir.xyz)) * PSLightColor[1].rgb));
    #endif
    
    #if LIGHTS > 2
        att1 = tex2D(AttenuationMap, IN.light3Att.xy).x;
        att2 = tex2D(AttenuationMap, IN.light3Att.zw).x;
        finalAtt = saturate(1 - att1 - att2);
        lighting += (finalAtt * (shades(normal.xyz, normalize(IN.light3Dir.xyz)) * PSLightColor[2].rgb));
    #endif
    
    #ifdef ONLY_LIGHT
        float3 finalColor = lighting.rgb;
    #else
        float3 finalColor = baseColor.rgb * max(lighting.rgb, 0);
    #endif
    
    // Specular component.
    #ifdef SPECULAR
        float NdotL = shades(normal.xyz, IN.lightDir.xyz);
        #ifndef HAIR
            float specStrength = normal.a * pow(abs(shades(normal.xyz, normalize(IN.halfwayDir.xyz))), glossPower);
        #else
            float3 hairVector = normalize(0.5 * normal.xyz) + float3(0, 0, 1);
            float hairFactor = 1 - saturate(abs(dot(hairVector, IN.lightDir.xyz) - dot(hairVector, normalize(IN.halfwayDir.xyz))));
            float specStrength = (normal.a * 0.7) * pow(abs(hairFactor), 30);
        #endif
        
        #if defined(POINT)
            float3 falloff = IN.lightDir.xyz / IN.lightDir.w;
            float att = 1 - shades(falloff, falloff);
        #else
            float att = IN.lightDir.w;
        #endif
    
        float3 specular = ((0.2 >= NdotL ? (specStrength * saturate(NdotL + 0.5)) : specStrength) * PSLightColor[0].rgb) * att;
    
        finalColor.rgb += saturate(specular * shadowMultiplier);
    
        #if LIGHTS > 1
            NdotL = shades(normal.xyz, IN.light2Dir.xyz);
            specStrength = normal.a * pow(abs(shades(normal.xyz, normalize(IN.halfway2Dir.xyz))), glossPower);
            specular = ((0.2 >= NdotL ? (specStrength * saturate(NdotL + 0.5)) : specStrength) * PSLightColor[1].rgb) * finalAtt;
            finalColor.rgb += saturate(specular);
        #endif
    
        #if NUM_PT_LIGHTS > 1
            NdotL = shades(normal.xyz, IN.light2Dir.xyz);
            #ifndef HAIR
                specStrength = normal.a * pow(abs(shades(normal.xyz, normalize(IN.halfway2Dir.xyz))), glossPower);
            #else
                hairFactor = 1 - saturate(abs(dot(hairVector, IN.light2Dir.xyz) - dot(hairVector, normalize(IN.halfway2Dir.xyz))));
                specStrength = (normal.a * 0.7) * pow(abs(hairFactor), 30);
            #endif
            falloff = IN.light2Dir.xyz / IN.light2Dir.w;
            att = 1 - shades(falloff, falloff);
            specular = ((0.2 >= NdotL ? (specStrength * saturate(NdotL + 0.5)) : specStrength) * PSLightColor[1].rgb) * att;
            finalColor.rgb += saturate(specular);
        #endif
    
        #if NUM_PT_LIGHTS > 2
            NdotL = shades(normal.xyz, IN.light3Dir.xyz);
            #ifndef HAIR
                specStrength = normal.a * pow(abs(shades(normal.xyz, normalize(IN.halfway3Dir.xyz))), glossPower);
            #else
                hairFactor = 1 - saturate(abs(dot(hairVector, IN.light2Dir.xyz) - dot(hairVector, normalize(IN.halfway2Dir.xyz))));
                specStrength = (normal.a * 0.7) * pow(abs(hairFactor), 30);
            #endif
            falloff = IN.light3Dir.xyz / IN.light3Dir.w;
            att = 1 - shades(falloff, falloff);
            specular = ((0.2 >= NdotL ? (specStrength * saturate(NdotL + 0.5)) : specStrength) * PSLightColor[2].rgb) * att;
            finalColor.rgb += saturate(specular);
        #endif
    #endif
    
    // Fog.
    #ifndef NO_FOG
        #ifndef OPT
            finalColor.rgb = (useFog <= 0.0 ? finalColor.rgb : lerp(finalColor.rgb, IN.fogColor.rgb, IN.fogColor.a));
        #else
            finalColor.rgb = lerp(finalColor.rgb, IN.fogColor.rgb, IN.fogColor.a);
        #endif
    #endif
    
    OUT.color.rgb = finalColor.rgb;
    
    #if defined(DIFFUSE)
        OUT.color.a = 1;
    #elif defined(ONLY_SPECULAR)
        OUT.color.rgb = saturate(OUT.color.rgb);
        OUT.color.a = weight(finalColor.rgb);
    #elif defined(ONLY_LIGHT)
        OUT.color.a = baseColor.a;
    #else
        OUT.color.a = baseColor.a * AmbientColor.a;
    #endif

    return OUT;
}

#elif defined(PS) && LIGHTS >= 4

#if LIGHTS > 4
    #define MAX_LIGHTS 6
#elif LIGHTS > 3 && !defined(SPECULAR)
    #define MAX_LIGHTS 4
#else
    #define MAX_LIGHTS 3
#endif

struct PS_INPUT {
    float4 vertexColor : COLOR0;
    float4 fogColor : COLOR1;
    float4 sPosition : POSITION;
    float2 uv : TEXCOORD0;
    float4 lPosition : TEXCOORD1;
    float4 lightDir : TEXCOORD2;
    float4 light2 : TEXCOORD3;
    float4 light3 : TEXCOORD4;
#ifndef SPECULAR
    #if MAX_LIGHTS > 3
        float4 light4 : TEXCOORD5;
    #endif
    #if MAX_LIGHTS > 4
        float4 light5 : TEXCOORD6;
        float4 light6 : TEXCOORD7;
    #endif
#else
    float3 halfwayDir : TEXCOORD5;
    float3 halfway2Dir : TEXCOORD6;
    float3 halfway3Dir : TEXCOORD7;
#endif
};

struct PS_OUTPUT {
    float4 color : COLOR0;
};

sampler2D BaseMap : register(s0);
sampler2D NormalMap : register(s1);

float4 AmbientColor : register(c1);

float4 PSLightColor[10] : register(c3);
float4 PSLightPosition[8] : register(c19);

#ifndef OPT
    float4 EmittanceColor : register(c2);

    float4 Toggles : register(c27);

    #define lightsUsed EmittanceColor.a
    #define lightOffset 0
    #define glossPow Toggles.z
#else
    #define lightsUsed PSLightColor[0].a
    #define lightOffset 1
    #define glossPow PSLightColor[1].w
#endif

PS_OUTPUT main(PS_INPUT IN) {
    PS_OUTPUT OUT;

    float4 baseColor = tex2D(BaseMap, IN.uv.xy);
    
    #ifndef OPT
        clip(AmbientColor.a >= 1 ? 0 : (baseColor.a - alphaTestRef));
    #endif
    
    #ifndef OPT
        baseColor.rgb = useVertexColor <= 0 ? baseColor.rgb : (baseColor.rgb * IN.vertexColor.rgb);
    #else
        baseColor.rgb = baseColor.rgb * IN.vertexColor.rgb;
    #endif
    
    float4 normal = tex2D(NormalMap, IN.uv.xy);
    normal.xyz = normalize(expand(normal.xyz));
    
    // Lighting.
    float3 light;
    float att;
    
    float3 diffuse;
    float NdotL;
    
    #ifdef SPECULAR
        float3 specular;
        float specStrength;
    #endif
    
    #ifndef OPT
        NdotL = shades(normal.xyz, IN.lightDir.xyz);
        diffuse = NdotL * PSLightColor[0].rgb;
        
        #ifdef SPECULAR
            specStrength = normal.a * pow(abs(shades(normal.xyz, normalize(IN.halfwayDir.xyz))), glossPower);
            specular = ((0.2 >= NdotL ? (specStrength * saturate(NdotL + 0.5)) : specStrength) * PSLightColor[0].rgb);
        #endif
    #else
        light = (PSLightPosition[0].xyz - IN.lPosition.xyz) / PSLightPosition[0].w;
        att = 1 - shades(light, light);
        NdotL = shades(normal.xyz, IN.lightDir.xyz);
        diffuse = (0 >= lightsUsed ? 0.0 : 1.0) * att * NdotL * PSLightColor[0].rgb;
        
        #ifdef SPECULAR
            specStrength = normal.a * pow(abs(shades(normal.xyz, normalize(IN.halfwayDir.xyz))), glossPow);
            specular = (0 >= lightsUsed ? 0.0 : 1.0) * att * ((0.2 >= NdotL ? (specStrength * saturate(NdotL + 0.5)) : specStrength) * PSLightColor[0].rgb);
        #endif
    #endif
    
    light = (PSLightPosition[lightOffset + 0].xyz - IN.lPosition.xyz) / PSLightPosition[lightOffset + 0].w;
    att = 1 - shades(light, light);
    NdotL = shades(normal.xyz, normalize(IN.light2.xyz));
    diffuse = 1 >= lightsUsed ? diffuse : diffuse + (att * NdotL * PSLightColor[1].rgb);
    #ifdef SPECULAR
        specStrength = normal.a * pow(abs(shades(normal.xyz, normalize(IN.halfway2Dir.xyz))), glossPow);
        specular += (1 >= lightsUsed ? 0.0 : 1.0) * att * ((0.2 >= NdotL ? (specStrength * saturate(NdotL + 0.5)) : specStrength) * PSLightColor[1].rgb);
    #endif
    
    light = (PSLightPosition[lightOffset + 1].xyz - IN.lPosition.xyz) / PSLightPosition[lightOffset + 1].w;
    att = 1 - shades(light, light);
    NdotL = shades(normal.xyz, normalize(IN.light3.xyz));
    diffuse = 2 >= lightsUsed ? diffuse : diffuse + (att * NdotL * PSLightColor[2].rgb);
    #ifdef SPECULAR
        specStrength = normal.a * pow(abs(shades(normal.xyz, normalize(IN.halfway3Dir.xyz))), glossPow);
        specular += (2 >= lightsUsed ? 0.0 : 1.0) * att * ((0.2 >= NdotL ? (specStrength * saturate(NdotL + 0.5)) : specStrength) * PSLightColor[2].rgb);
    #endif
    
    #if MAX_LIGHTS > 3
        light = (PSLightPosition[lightOffset + 2].xyz - IN.lPosition.xyz) / PSLightPosition[lightOffset + 2].w;
        light = (1 - shades(light, light)) * shades(normal.xyz, normalize(IN.light4.xyz)) * PSLightColor[3].rgb;
        diffuse = 3 >= lightsUsed ? diffuse : light + diffuse;
    #endif
    
    #if MAX_LIGHTS > 4
        light = (PSLightPosition[3].xyz - IN.lPosition.xyz) / PSLightPosition[3].w;
        light = (1 - shades(light, light)) * shades(normal.xyz, normalize(IN.light5.xyz)) * PSLightColor[4].rgb;
        diffuse = 4 >= lightsUsed ? diffuse : light + diffuse;
    
        light = (PSLightPosition[4].xyz - IN.lPosition.xyz) / PSLightPosition[4].w;
        light = (1 - shades(light, light)) * shades(normal.xyz, normalize(IN.light6.xyz)) * PSLightColor[5].rgb;
        diffuse = 5 >= lightsUsed ? diffuse : light + diffuse;
    #endif
    
    diffuse += AmbientColor.rgb;
    
    #ifndef SPECULAR
        float3 finalColor = baseColor.rgb * max(diffuse, 0);
    #else
        float3 finalColor = baseColor.rgb * max(diffuse, 0) + specular * IN.lPosition.w;
    #endif
    
    #ifndef OPT
        finalColor.rgb = (useFog <= 0.0 ? finalColor.rgb : lerp(finalColor.rgb, IN.fogColor.rgb, IN.fogColor.a));
    #else
        finalColor.rgb = lerp(finalColor.rgb, IN.fogColor.rgb, IN.fogColor.a);
    #endif
    
    OUT.color.rgb = finalColor.rgb;
    OUT.color.a = baseColor.a * AmbientColor.a;

    return OUT;
}

#endif // Pixel shaders.