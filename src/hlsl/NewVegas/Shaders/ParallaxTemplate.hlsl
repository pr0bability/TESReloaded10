// Template for PAR parallax shaders.
//
// VS
//
// AD
// PAR2008 - LIGHTS = 2, PARALLAX (AD)
// PAR2009 - LIGHTS = 2, PARALLAX, PROJ_SHADOW (AD)
// PAR2010 - LIGHTS = 3, PARALLAX (AD)
// PAR2011 - LIGHTS = 3, PARALLAX, PROJ_SHADOW (AD)
//
// ADTS
// PAR2000 - PARALLAX
// PAR2001 - PARALLAX, PROJ_SHADOW
// PAR2002 - PARALLAX, LIGHTS = 2
// PAR2003 - PARALLAX, LIGHTS = 2, PROJ_SHADOW
// PAR2004 - PARALLAX, SPECULAR
// PAR2005 - PARALLAX, SPECULAR, PROJ_SHADOW
// PAR2006 - PARALLAX, SPECULAR, LIGHTS = 2
// PAR2007 - PARALLAX, SPECULAR, LIGHTS = 2, PROJ_SHADOW
//
// Diffuse
// PAR2012 - LIGHTS = 2, PARALLAX (NO_FOG, DIFFUSE)
// PAR2013 - LIGHTS = 3, PARALLAX (NO_FOG, DIFFUSE)
//
// Specular
// PAR2015 - PARALLAX (NO_FOG, NO_VERTEX_COLOR, SPECULAR)
// PAR2016 - PROJ_SHADOW, PARALLAX (NO_FOG, NO_VERTEX_COLOR, SPECULAR)
// PAR2017 - POINT, PARALLAX (NO_FOG. NO_VERTEX_COLOR, SPECULAR)
// PAR2018 - POINT, NUM_PT_LIGHTS = 2, PARALLAX (NO_FOG, NO_VERTEX_COLOR, SPECULAR)
// PAR2019 - POINT, NUM_PT_LIGHTS = 3, PARALLAX (NO_FOG, NO_VERTEX_COLOR, SPECULAR)
//
// Texture
// PAR2014 - PARALLAX (NO_LIGHT, NO_FOG)
//
// PS
//
// AD
// PAR2013 - LIGHTS = 2, PARALLAX (AD)
// PAR2014 - LIGHTS = 2, PARALLAX, SI (AD)
// PAR2015 - LIGHTS = 2, PARALLAX, PROJ_SHADOW (AD)
// PAR2016 - LIGHTS = 2, PARALLAX, SI, PROJ_SHADOW (AD)
// PAR2017 - LIGHTS = 3, PARALLAX (AD)
// PAR2018 - LIGHTS = 3, PARALLAX, SI (AD)
// PAR2019 - LIGHTS = 3, PARALLAX, PROJ_SHADOW (AD)
// PAR2020 - LIGHTS = 3, PARALLAX, SI, PROJ_SHADOW (AD)
//
// ADTS
// PAR2000 - PARALLAX
// PAR2001 - PARALLAX, OPT
// PAR2002 - PARALLAX, SI
// PAR2003 - PARALLAX, PROJ_SHADOW
// PAR2004 - PARALLAX, SI, PROJ_SHADOW
// PAR2005 - PARALLAX, LIGHTS = 2
// PAR2006 - PARALLAX, LIGHTS = 2, SI
// PAR2007 - PARALLAX, LIGHTS = 2, PROJ_SHADOW
// PAR2008 - PARALLAX, LIGHTS = 2, SI, PROJ_SHADOW
// PAR2009 - PARALLAX, SPECULAR
// PAR2010 - PARALLAX, SPECULAR, SI
// PAR2011 - PARALLAX, SPECULAR, PROJ_SHADOW
// PAR2012 - PARALLAX, SPECULAR, SI, PROJ_SHADOW
// PAR2029 - PARALLAX, LIGHTS = 2, SPECULAR
// PAR2030 - PARALLAX, LIGHTS = 2, SPECULAR, SI
// PAR2031 - PARALLAX, LIGHTS = 2, SPECULAR, PROJ_SHADOW
// PAR2032 - PARALLAX, LIGHTS = 2, SPECULAR, SI, PROJ_SHADOW
//
// Diffuse
// PAR2021 - LIGHTS = 2, PARALLAX (NO_FOG, NO_VERTEX_COLOR, DIFFUSE, ONLY_LIGHT, OPT)
// PAR2022 - LIGHTS = 3, PARALLAX (NO_FOG, NO_VERTEX_COLOR, DIFFUSE, ONLY_LIGHT, OPT)
//
// Specular
// PAR2024 - PARALLAX (ONLY_SPECULAR)
// PAR2025 - PARALLAX, PROJ_SHADOW (ONLY_SPECULAR)
// PAR2026 - PARALLAX, POINT (ONLY_SPECULAR)
// PAR2027 - PARALLAX, NUM_PT_LIGHTS = 2, POINT (ONLY_SPECULAR)
// PAR2028 - PARALLAX, NUM_PT_LIGHTS = 3, POINT (ONLY_SPECULAR)
//
// Texture
// PAR2023 - PARALLAX (NO_LIGHT)

#if defined(__INTELLISENSE__)
    #define PS
    #define ONLY_SPECULAR
    #define POINT
    #define NUM_PT_LIGHTS 3
#endif

#if defined(AD)
    #define ONLY_LIGHT
    #define OPT
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
#include "includes/Parallax.hlsl"
#include "includes/Object.hlsl"

struct VS_INPUT
{
    float4 position : POSITION;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 normal : NORMAL;
    float4 uv : TEXCOORD0;
#ifndef NO_VERTEX_COLOR
    float4 vertex_color : COLOR0;
#endif
};

struct VS_OUTPUT
{
#ifndef NO_VERTEX_COLOR
    float4 vertexColor : COLOR0;
#endif
#ifndef NO_FOG
    float4 fogColor : COLOR1;
#endif
    float4 sPosition : POSITION;
    float2 uv : TEXCOORD0;
#ifndef NO_LIGHT
    float4 lightDir : TEXCOORD1;
    #ifdef DIFFUSE
        float4 lightAtt : TEXCOORD2;
    #endif
#endif
#if LIGHTS > 1
    float4 light2Dir : TEXCOORD3;
    float4 light2Att : TEXCOORD4;
#endif
#if LIGHTS > 2  // Only used for AD and diffuse, no need for specular check.
    float4 light3Dir : TEXCOORD5;
    float4 light3Att : TEXCOORD6;
#endif
#if NUM_PT_LIGHTS > 1
    float4 light2Dir : TEXCOORD3;
#endif
#if NUM_PT_LIGHTS > 2
    float4 light3Dir : TEXCOORD5;
#endif
#ifdef PROJ_SHADOW
    float4 shadowUVs : TEXCOORD8;
#endif
    float4 viewDir : TEXCOORD7;
};

#ifdef VS

float4 EyePosition : register(c16);
row_major float4x4 ModelViewProj : register(c0);

#ifndef NO_LIGHT
    float4 LightData[10] : register(c25);
#endif

#ifndef NO_FOG
    float3 FogColor : register(c15);
    float4 FogParam : register(c14);
#endif

#ifdef PROJ_SHADOW
    row_major float4x4 ShadowProj : register(c18);
    float4 ShadowProjData : register(c22);
    float4 ShadowProjTransform : register(c23);
#endif

VS_OUTPUT main(VS_INPUT IN)
{
    VS_OUTPUT OUT;
 
    float3x3 tbn = float3x3(IN.tangent.xyz, IN.binormal.xyz, IN.normal.xyz);
    
    OUT.sPosition.xyzw = mul(ModelViewProj, IN.position.xyzw);
    OUT.uv = IN.uv.xy;
    
    float3 eye = EyePosition.xyz - IN.position.xyz;
    OUT.viewDir.xyz = mul(tbn, eye);
    OUT.viewDir.w = length(eye);
    
    #ifndef NO_VERTEX_COLOR
        OUT.vertexColor = IN.vertex_color;
    #endif
    
    #ifndef NO_LIGHT
        #ifndef POINT
            OUT.lightDir.w = LightData[0].w;
            #ifndef DIFFUSE
                OUT.lightDir.xyz = mul(tbn, LightData[0].xyz);
            #else
                float3 light = LightData[0].xyz - IN.position.xyz;
                OUT.lightDir.xyz = mul(tbn, light);
                OUT.lightAtt.w = 0.5;
                OUT.lightAtt.xyz = compress(light / LightData[0].w);
            #endif
            #if LIGHTS > 1
                float3 light2 = LightData[1].xyz - IN.position.xyz;
                OUT.light2Dir.w = LightData[1].w;
                OUT.light2Dir.xyz = mul(tbn, light2);
                OUT.light2Att.w = 0.5;
                OUT.light2Att.xyz = compress(light2 / LightData[1].w);
            #endif
            #if LIGHTS > 2
                float3 light3 = LightData[2].xyz - IN.position.xyz;
                OUT.light3Dir.w = LightData[2].w;
                OUT.light3Dir.xyz = mul(tbn, light3);
                OUT.light3Att.w = 0.5;
                OUT.light3Att.xyz = compress(light3 / LightData[2].w);
            #endif
        #else
            OUT.lightDir.w = LightData[0].w;
            OUT.lightDir.xyz = mul(tbn, LightData[0].xyz - IN.position.xyz);
            #if NUM_PT_LIGHTS > 1
                OUT.light2Dir.w = LightData[1].w;
                OUT.light2Dir.xyz = mul(tbn, LightData[1].xyz - IN.position.xyz);
            #endif
            #if NUM_PT_LIGHTS > 2
                OUT.light3Dir.w = LightData[2].w;
                OUT.light3Dir.xyz = mul(tbn, LightData[2].xyz - IN.position.xyz);
            #endif
        #endif
    #endif
    
    #ifndef NO_FOG
        float fogStrength = 1 - saturate((FogParam.x - length(OUT.sPosition.xyz)) / FogParam.y);
        fogStrength = log2(fogStrength);  // Unclear.
        OUT.fogColor.a = exp2(fogStrength * FogParam.z);
        OUT.fogColor.rgb = FogColor.rgb;
    #endif
    
    #ifdef PROJ_SHADOW
        float shadowParam = dot(ShadowProj[3].xyzw, IN.position.xyzw);
        float2 shadowUV;
        shadowUV.x = dot(ShadowProj[0].xyzw, IN.position.xyzw);
        shadowUV.y = dot(ShadowProj[1].xyzw, IN.position.xyzw);
        OUT.shadowUVs.xy = ((shadowParam * ShadowProjTransform.xy) + shadowUV) / (shadowParam * ShadowProjTransform.w);
        OUT.shadowUVs.zw = ((shadowUV.xy - ShadowProjData.xy) / ShadowProjData.w) * float2(1, -1) + float2(0, 1);
    #endif

    return OUT;
};

#endif // Vertex shader.

struct PS_INPUT
{
#ifndef NO_VERTEX_COLOR
    float3 vertexColor : COLOR0;
#endif
#ifndef NO_FOG
    float4 fogColor : COLOR1;
#endif
    float2 uv : TEXCOORD0;
#ifndef NO_LIGHT
    float4 lightDir : TEXCOORD1_centroid;
#endif
#ifdef DIFFUSE
    float4 lightAtt : TEXCOORD2;
#endif
#if LIGHTS > 1
    float4 light2Dir : TEXCOORD3_centroid;
    float4 light2Att : TEXCOORD4;
#endif
#if LIGHTS > 2
    float4 light3Dir : TEXCOORD5_centroid;
    float4 light3Att : TEXCOORD6;
#endif
#if NUM_PT_LIGHTS > 1
    float4 light2Dir : TEXCOORD3_centroid;
#endif
#if NUM_PT_LIGHTS > 2
    float4 light3Dir : TEXCOORD5_centroid;
#endif
#ifdef PROJ_SHADOW
    float4 shadowUVs : TEXCOORD8;
#endif
    float4 viewDir : TEXCOORD7_centroid;
};

struct PS_OUTPUT {
    float4 color : COLOR0;
};

#ifdef PS

#if !defined(DIFFUSE) && !defined(ONLY_SPECULAR)
    #if !defined(NO_LIGHT)
        float4 AmbientColor : register(c1);
    #endif
    sampler2D BaseMap : register(s0);
#endif
#if !defined(NO_LIGHT)
    #if !defined(DIFFUSE) && !defined(ONLY_SPECULAR)
        sampler2D NormalMap : register(s1);
    #else
        sampler2D NormalMap : register(s0);
    #endif
#endif
#if !defined(ONLY_LIGHT) && !defined(ONLY_SPECULAR) && !defined(NO_LIGHT)
    sampler2D HeightMap : register(s3);
#else
    sampler2D HeightMap : register(s2);
#endif
#ifdef SI
float4 EmittanceColor : register(c2);
    #ifndef ONLY_LIGHT
        sampler2D GlowMap : register(s4);
    #else
        sampler2D GlowMap : register(s3);
    #endif
#endif
#if LIGHTS > 1
    #ifdef DIFFUSE
        sampler2D AttenuationMap : register(s3);
    #elif defined(ONLY_LIGHT)
        sampler2D AttenuationMap : register(s4);
    #else
        sampler2D AttenuationMap : register(s5);
    #endif
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
#if !defined(NO_LIGHT)
    float4 PSLightColor[10] : register(c3);
#endif
#ifndef OPT
    float4 Toggles : register(c27);

    #define useVertexColor Toggles.x
    #define useFog Toggles.y
    #define glossPower Toggles.z
    #define alphaTestRef Toggles.w
#else
    #define glossPower 1  // OPT is never used in combination with specular in PAR.
#endif

#define	uvtile(w)		(((w) * 0.04) - 0.02)

PS_OUTPUT main(PS_INPUT IN)
{
    PS_OUTPUT OUT;
    
    #if !defined(ONLY_LIGHT) && !defined(ONLY_SPECULAR) && !defined(NO_LIGHT)
        float alpha = tex2D(BaseMap, IN.uv.xy).a;
    
        #ifndef OPT
            clip(AmbientColor.a >= 1 ? 0 : (alpha - alphaTestRef));
        #endif
    #endif
    
    // Parallax.
    float3 viewDir = normalize(IN.viewDir.xyz);
    float distance = IN.viewDir.w;
    
    float2 dx, dy;
    dx = ddx(IN.uv.xy);
    dy = ddy(IN.uv.xy);
    
    float2 offsetUV = getParallaxCoords(distance, IN.uv.xy, dx, dy, viewDir.xyz, HeightMap);

    #if !defined(DIFFUSE) && !defined(ONLY_SPECULAR)
        float4 baseColor = tex2D(BaseMap, offsetUV.xy);
    
        #if defined(ONLY_LIGHT)
            baseColor.rgb = 1.f;
        #endif
    #else
        float4 baseColor = 1.f;
    #endif
    
    // Vertex color.
    #ifndef NO_VERTEX_COLOR
        #ifndef OPT
            // Apply vertex color if toggled.
            baseColor.xyz = (useVertexColor <= 0.0 ? baseColor.xyz : (baseColor.xyz * IN.vertexColor.rgb));
        #else
            baseColor.xyz = baseColor.xyz * IN.vertexColor.rgb;
        #endif
    #endif
    
    // Shadows.
    float3 shadowMultiplier = 1.0;
    #ifdef PROJ_SHADOW
        float3 shadow = tex2D(ShadowMap, IN.shadowUVs.xy).xyz;
        float shadowMask = tex2D(ShadowMaskMap, IN.shadowUVs.zw).x;
        shadowMultiplier = lerp(1, shadow, shadowMask);
    #endif
    
    #ifndef NO_LIGHT
        shadowMultiplier *= getParallaxShadowMultipler(distance, offsetUV, dx, dy, normalize(IN.lightDir.xyz), HeightMap);
    #endif
    
    // Lighting.
    float3 lighting;
    float finalAtt;
    
    #ifdef NO_LIGHT
        lighting = baseColor.rgb;
    #else
        float4 normal = tex2D(NormalMap, offsetUV.xy);
        normal.xyz = normalize(expand(normal.xyz));

        float roughness = getRoughness(normal.a);
    
        #if !defined(DIFFUSE) && !defined(POINT)
            if (TESR_ParallaxData.y)
                lighting = getSunLighting(IN.lightDir.xyz, PSLightColor[0].rgb * shadowMultiplier, IN.viewDir.xyz, normal.xyz, baseColor.rgb, roughness);
            else
                lighting = getVanillaLightingAtt(IN.lightDir.xyz, 1.f, PSLightColor[0].rgb * shadowMultiplier, IN.viewDir.xyz, normal.xyz, baseColor.rgb, normal.a, glossPower);
        #elif defined(DIFFUSE)
            // Pointlight vanilla att.
            if (TESR_ParallaxData.y)
                lighting = getPointLightLighting(IN.lightDir.xyz, IN.lightDir.w, PSLightColor[0].rgb, IN.viewDir.xyz, normal.xyz, baseColor.rgb, roughness);
            else {
                finalAtt = saturate(1 - tex2D(AttenuationMap, IN.lightAtt.xy).x - tex2D(AttenuationMap, IN.lightAtt.zw).x);
                lighting = getVanillaLightingAtt(IN.lightDir.xyz, finalAtt, PSLightColor[0].rgb, IN.viewDir.xyz, normal.xyz, baseColor.rgb, normal.a, glossPower);
            }
        #else
            if (TESR_ParallaxData.y)
                lighting = getPointLightLighting(IN.lightDir.xyz, IN.lightDir.w, PSLightColor[0].rgb, IN.viewDir.xyz, normal.xyz, baseColor.rgb, roughness);
            else
                lighting = getVanillaLighting(IN.lightDir.xyz, IN.lightDir.w, PSLightColor[0].rgb, IN.viewDir.xyz, normal.xyz, baseColor.rgb, normal.a, glossPower);
        #endif
    
        // Self emmitance.
        #ifdef SI
            float3 glow = tex2D(GlowMap, IN.uv.xy).rgb;
            lighting += baseColor.rgb * glow.rgb * EmittanceColor.rgb;
        #endif
    
        #if !defined(DIFFUSE) && !defined(ONLY_SPECULAR)
            if (TESR_ParallaxData.y)
                lighting += getAmbientLighting(AmbientColor.rgb, baseColor.rgb);
            else
                lighting += baseColor.rgb * AmbientColor.rgb;
        #endif
    
        // Other light sources.
        #if LIGHTS > 1
            finalAtt = saturate(1 - tex2D(AttenuationMap, IN.light2Att.xy).x - tex2D(AttenuationMap, IN.light2Att.zw).x);
        
            if (TESR_ParallaxData.y)
                lighting += getPointLightLightingAtt(IN.light2Dir.xyz, finalAtt, PSLightColor[1].rgb, IN.viewDir.xyz, normal.xyz, baseColor.rgb, roughness);
            else
                lighting += getVanillaLightingAtt(IN.light2Dir.xyz, finalAtt, PSLightColor[1].rgb, IN.viewDir.xyz, normal.xyz, baseColor.rgb, normal.a, glossPower);
        #endif
    
        #if LIGHTS > 2
            finalAtt = saturate(1 - tex2D(AttenuationMap, IN.light3Att.xy).x - tex2D(AttenuationMap, IN.light3Att.zw).x);
        
            if (TESR_ParallaxData.y)
                lighting += getPointLightLightingAtt(IN.light3Dir.xyz, finalAtt, PSLightColor[2].rgb, IN.viewDir.xyz, normal.xyz, baseColor.rgb, roughness);
            else
                lighting += getVanillaLightingAtt(IN.light3Dir.xyz, finalAtt, PSLightColor[2].rgb, IN.viewDir.xyz, normal.xyz, baseColor.rgb, normal.a, glossPower);
        #endif
    
        #if NUM_PT_LIGHTS > 1
            if (TESR_ParallaxData.y)
                lighting += getPointLightLighting(IN.light2Dir.xyz, IN.light2Dir.w, PSLightColor[1].rgb, IN.viewDir.xyz, normal.xyz, baseColor.rgb, roughness);
            else
                lighting += getVanillaLighting(IN.light2Dir.xyz, IN.light2Dir.w, PSLightColor[1].rgb, IN.viewDir.xyz, normal.xyz, baseColor.rgb, normal.a, glossPower);
        #endif
    
        #if NUM_PT_LIGHTS > 2
            if (TESR_ParallaxData.y)
                lighting += getPointLightLighting(IN.light3Dir.xyz, IN.light3Dir.w, PSLightColor[2].rgb, IN.viewDir.xyz, normal.xyz, baseColor.rgb, roughness);
            else
                lighting += getVanillaLighting(IN.light3Dir.xyz, IN.light3Dir.w, PSLightColor[2].rgb, IN.viewDir.xyz, normal.xyz, baseColor.rgb, normal.a, glossPower);
        #endif
    #endif
    
    // Fog.
    #ifndef NO_FOG
        #ifndef OPT
            lighting.rgb = (useFog <= 0.0 ? lighting.rgb : lerp(lighting.rgb, IN.fogColor.rgb, IN.fogColor.a));
        #else
            lighting.rgb = lerp(lighting.rgb, IN.fogColor.rgb, IN.fogColor.a);
        #endif
    #endif
    
    OUT.color.rgb = lighting.rgb;
    
    #if defined(DIFFUSE) || defined(NO_LIGHT)
        OUT.color.a = 1;
    #elif defined(ONLY_SPECULAR)
        if (!TESR_ParallaxData.y)
            OUT.color.rgb = saturate(OUT.color.rgb);
        OUT.color.a = weight(lighting.rgb);
    #elif defined(ONLY_LIGHT)
        OUT.color.a = baseColor.a;
    #else
        OUT.color.a = alpha * AmbientColor.a;
    #endif

    return OUT;
};

#endif // Pixel shader.