// Template for PAR parallax shaders.
//
// VS
//
// AD
// PAR2008 - LIGHTS = 2, PARALLAX (NO_FOG, NO_VERTEX_COLOR)
// PAR2009 - LIGHTS = 2, PARALLAX, PROJ_SHADOW (NO_FOG, NO_VERTEX_COLOR)
// PAR2010 - LIGHTS = 3, PARALLAX (NO_FOG, NO_VERTEX_COLOR)
// PAR2011 - LIGHTS = 3, PARALLAX, PROJ_SHADOW (NO_FOG, NO_VERTEX_COLOR)
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
// PAR2013 - LIGHTS = 2, PARALLAX (NO_FOG, NO_VERTEX_COLOR, ONLY_LIGHT, OPT)
// PAR2014 - LIGHTS = 2, PARALLAX, SI (NO_FOG, NO_VERTEX_COLOR, ONLY_LIGHT, OPT)
// PAR2015 - LIGHTS = 2, PARALLAX, PROJ_SHADOW (NO_FOG, NO_VERTEX_COLOR, ONLY_LIGHT, OPT)
// PAR2016 - LIGHTS = 2, PARALLAX, SI, PROJ_SHADOW (NO_FOG, NO_VERTEX_COLOR, ONLY_LIGHT, OPT)
// PAR2017 - LIGHTS = 3, PARALLAX (NO_FOG, NO_VERTEX_COLOR, ONLY_LIGHT, OPT)
// PAR2018 - LIGHTS = 3, PARALLAX, SI (NO_FOG, NO_VERTEX_COLOR, ONLY_LIGHT, OPT)
// PAR2019 - LIGHTS = 3, PARALLAX, PROJ_SHADOW (NO_FOG, NO_VERTEX_COLOR, ONLY_LIGHT, OPT)
// PAR2020 - LIGHTS = 3, PARALLAX, SI, PROJ_SHADOW (NO_FOG, NO_VERTEX_COLOR, ONLY_LIGHT, OPT)
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
// PAR2024 - PARALLAX (NO_FOG, NO_VERTEX_COLOR, ONLY_LIGHT, ONLY_SPECULAR)
// PAR2025 - PARALLAX, PROJ_SHADOW (NO_FOG, NO_VERTEX_COLOR, ONLY_LIGHT, ONLY_SPECULAR)
// PAR2026 - PARALLAX, POINT (NO_FOG, NO_VERTEX_COLOR, ONLY_LIGHT, ONLY_SPECULAR)
// PAR2027 - PARALLAX, NUM_PT_LIGHTS = 2, POINT (NO_FOG, NO_VERTEX_COLOR, ONLY_LIGHT, ONLY_SPECULAR)
// PAR2028 - PARALLAX, NUM_PT_LIGHTS = 3, POINT (NO_FOG, NO_VERTEX_COLOR, ONLY_LIGHT, ONLY_SPECULAR)
//
// Texture
// PAR2023 - PARALLAX (NO_LIGHT)

#include "includes/Helpers.hlsl"
#include "includes/Parallax.hlsl"

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
        float4 lightAttenuation : TEXCOORD2;
    #endif
    #ifdef SPECULAR
        float3 halfwayDir : TEXCOORD2;
    #endif
#endif
#if LIGHTS > 1
    float4 light2Dir : TEXCOORD3;
    float4 light2Attenuation : TEXCOORD4;
    #ifdef SPECULAR
        float3 halfway2Dir : TEXCOORD5;
    #endif
#endif
#if LIGHTS > 2  // Only used for AD and diffuse, no need for specular check.
    float4 light3Dir : TEXCOORD5;
    float4 light3Attenuation : TEXCOORD6;
#endif
#if NUM_PT_LIGHTS > 1
    float4 light2Dir : TEXCOORD3;
    float3 halfway2Dir : TEXCOORD4;
#endif
#if NUM_PT_LIGHTS > 2
    float4 light3Dir : TEXCOORD5;
    float3 halfway3Dir : TEXCOORD6;
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
            OUT.lightDir.w = 1;
            #ifndef DIFFUSE
                OUT.lightDir.xyz = normalize(mul(tbn, LightData[0].xyz));
            #else
                float3 light = LightData[0].xyz - IN.position.xyz;
                OUT.lightDir.xyz = normalize(mul(tbn, light));
                OUT.lightAttenuation.w = 0.5;
                OUT.lightAttenuation.xyz = compress(light / LightData[0].w);
            #endif
            #ifdef SPECULAR
                OUT.halfwayDir.xyz = normalize(mul(tbn, normalize(eye) + LightData[0].xyz));
            #endif
            #if LIGHTS > 1
                float3 light2 = LightData[1].xyz - IN.position.xyz;
                OUT.light2Dir.w = 1;
                OUT.light2Dir.xyz = mul(tbn, normalize(light2));
                OUT.light2Attenuation.w = 0.5;
                OUT.light2Attenuation.xyz = compress(light2 / LightData[1].w);
                #ifdef SPECULAR
                    OUT.halfway2Dir.xyz = normalize(mul(tbn, normalize(eye + normalize(light2))));
                #endif
            #endif
            #if LIGHTS > 2
                float3 light3 = LightData[2].xyz - IN.position.xyz;
                OUT.light3Dir.w = 1;
                OUT.light3Dir.xyz = mul(tbn, normalize(light3));
                OUT.light3Attenuation.w = 0.5;
                OUT.light3Attenuation.xyz = compress(light3 / LightData[2].w);
            #endif
        #else
            OUT.lightDir.w = LightData[0].w;
            OUT.lightDir.xyz = mul(tbn, LightData[0].xyz - IN.position.xyz);
            OUT.halfwayDir.xyz = normalize(mul(tbn, normalize(eye) + OUT.lightDir.xyz));
            #if NUM_PT_LIGHTS > 1
                OUT.light2Dir.w = LightData[1].w;
                OUT.light2Dir.xyz = mul(tbn, LightData[1].xyz - IN.position.xyz);
                OUT.halfway2Dir.xyz = normalize(mul(tbn, normalize(eye + normalize(OUT.light2Dir.xyz))));
            #endif
            #if NUM_PT_LIGHTS > 2
                OUT.light3Dir.w = LightData[2].w;
                OUT.light3Dir.xyz = mul(tbn, LightData[2].xyz - IN.position.xyz);
                OUT.halfway3Dir.xyz = normalize(mul(tbn, normalize(eye + normalize(OUT.light3Dir.xyz))));
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
    float4 lightAttenuation : TEXCOORD2;
#endif
#if defined(SPECULAR) || defined(ONLY_SPECULAR)
    float3 halfwayDir : TEXCOORD2_centroid;
#endif
#if LIGHTS > 1
    float4 light2Dir : TEXCOORD3_centroid;
    float4 light2Attenuation : TEXCOORD4;
    #ifdef SPECULAR
        float3 halfway2Dir : TEXCOORD5_centroid;
    #endif
#endif
#if LIGHTS > 2
    float4 light3Dir : TEXCOORD5_centroid;
    float4 light3Attenuation : TEXCOORD6;
#endif
#if NUM_PT_LIGHTS > 1
    float4 light2Dir : TEXCOORD3_centroid;
    float3 halfway2Dir : TEXCOORD4_centroid;
#endif
#if NUM_PT_LIGHTS > 2
    float4 light3Dir : TEXCOORD5_centroid;
    float3 halfway3Dir : TEXCOORD6_centroid;
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

#define useVertexColor Toggles.x
#define useFog Toggles.y
#define glossPower Toggles.z
#define alphaTestRef Toggles.w

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
#endif

#define	uvtile(w)		(((w) * 0.04) - 0.02)

PS_OUTPUT main(PS_INPUT IN)
{
    PS_OUTPUT OUT;
    
    float4 finalColor;
    
    #if !defined(ONLY_LIGHT) && !defined(ONLY_SPECULAR) && !defined(NO_LIGHT)
        float alpha = tex2D(BaseMap, IN.uv.xy).a;
    
        #ifndef OPT
            clip(AmbientColor.a >= 1 ? 0 : (alpha - alphaTestRef));
        #endif
    
        finalColor.a = alpha * AmbientColor.a;
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
    #else
        float4 baseColor = float4(0, 0, 0, 1);
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
    
    // Lighting.
    float3 lighting;
    
    #ifdef NO_LIGHT
        OUT.color = 1;
        OUT.color.rgb = baseColor.rgb;
        return OUT;
    #else
        float4 normal = tex2D(NormalMap, offsetUV.xy);
        normal.xyz = normalize(expand(normal.xyz));

        float att1, att2, finalAtt;
        float NdotL = shades(normal.xyz, IN.lightDir.xyz);
    #endif
    
    #ifdef DIFFUSE
        att1 = tex2D(AttenuationMap, IN.lightAttenuation.xy).x;
        att2 = tex2D(AttenuationMap, IN.lightAttenuation.zw).x;
        finalAtt = saturate((1 - att1) - att2);
        lighting = finalAtt * NdotL * PSLightColor[0].rgb;
    #elif defined(ONLY_SPECULAR)
        float specStrength = normal.a * pow(abs(shades(normal.xyz, normalize(IN.halfwayDir.xyz))), glossPower);
        lighting = (0.2 >= NdotL ? (specStrength * saturate(NdotL + 0.5)) : specStrength) * PSLightColor[0].rgb;
    
        #ifdef POINT
            float3 falloff = IN.lightDir.xyz / IN.lightDir.w;
            lighting *= (1 - shades(falloff, falloff));
        #endif
    #elif !defined(NO_LIGHT)
        lighting = NdotL * PSLightColor[0].rgb;
    #endif
    
    // Shadows.
    float3 shadowMultiplier = 1;
    #ifdef PROJ_SHADOW
        float3 shadow = tex2D(ShadowMap, IN.shadowUVs.xy).xyz;
        float shadowMask = tex2D(ShadowMaskMap, IN.shadowUVs.zw).x;
        shadowMultiplier = lerp(1, shadow, shadowMask);
    #endif
    
    #ifndef NO_LIGHT
        shadowMultiplier *= getParallaxShadowMultipler(distance, offsetUV, dx, dy, IN.lightDir.xyz, HeightMap);
    #endif
    
    lighting *= shadowMultiplier;
    
    // Other light sources.
    #if LIGHTS > 1
        att1 = tex2D(AttenuationMap, IN.light2Attenuation.xy).x;
        att2 = tex2D(AttenuationMap, IN.light2Attenuation.zw).x;
        finalAtt = saturate(1 - att1 - att2);
        lighting += (finalAtt * (shades(normal.xyz, normalize(IN.light2Dir.xyz)) * PSLightColor[1].rgb));
    #endif
    
    #if LIGHTS > 2
        att1 = tex2D(AttenuationMap, IN.light3Attenuation.xy).x;
        att2 = tex2D(AttenuationMap, IN.light3Attenuation.zw).x;
        finalAtt = saturate((1 - att1) - att2);
        lighting += (finalAtt * (shades(normal.xyz, normalize(IN.light3Dir.xyz)) * PSLightColor[2].rgb));
    #endif
    
    #if NUM_PT_LIGHTS > 1
        NdotL = shades(normal.xyz, IN.light2Dir.xyz);
        specStrength = normal.a * pow(abs(shades(normal.xyz, normalize(IN.halfway2Dir.xyz))), glossPower);
        falloff = IN.light2Dir.xyz / IN.light2Dir.w;
        falloff = 1 - shades(falloff, falloff);
        lighting += (0.2 >= NdotL ? (specStrength * saturate(NdotL + 0.5)) : specStrength) * PSLightColor[1].rgb * falloff;
    #endif
    
    #if NUM_PT_LIGHTS > 2
        NdotL = shades(normal.xyz, IN.light3Dir.xyz);
        specStrength = normal.a * pow(abs(shades(normal.xyz, normalize(IN.halfway3Dir.xyz))), glossPower);
        falloff = IN.light3Dir.xyz / IN.light3Dir.w;
        falloff = 1 - shades(falloff, falloff);
        lighting += (0.2 >= NdotL ? (specStrength * saturate(NdotL + 0.5)) : specStrength) * PSLightColor[2].rgb * falloff;
    #endif
    
    // Self emmitance.
    #ifdef SI
        float3 glow = tex2D(GlowMap, IN.uv.xy).rgb;
        lighting += glow.rgb * EmittanceColor.rgb;
    #endif
    
    #if !defined(DIFFUSE) && !defined(ONLY_SPECULAR) && !defined(NO_LIGHT)
        lighting += AmbientColor.rgb;
    #endif
    
    #ifdef ONLY_LIGHT
        finalColor.a = baseColor.a;
        finalColor.rgb = lighting.rgb;
    #elif defined(ONLY_SPECULAR)
        finalColor.a = weight(lighting.rgb);
        finalColor.rgb = saturate(lighting.rgb);
    #else
        finalColor.rgb = baseColor.rgb * max(lighting.rgb, 0);;
    #endif
    
    // Specular component (for not specular only variants).
    #ifdef SPECULAR
        float specStrength = normal.a * pow(abs(shades(normal.xyz, normalize(IN.halfwayDir.xyz))), glossPower);
        float3 specular = saturate(((0.2 >= NdotL ? (specStrength * saturate(NdotL + 0.5)) : specStrength) * PSLightColor[0].rgb) * IN.lightDir.w);
    
        finalColor.rgb += specular * shadowMultiplier;
    
        #if LIGHTS > 1
            NdotL = shades(normal.xyz, IN.light2Dir.xyz);
            specStrength = normal.a * pow(abs(shades(normal.xyz, normalize(IN.halfway2Dir.xyz))), glossPower);
            specular = saturate(((0.2 >= NdotL ? (specStrength * saturate(NdotL + 0.5)) : specStrength) * PSLightColor[1].rgb) * finalAtt);
            finalColor.rgb += specular;
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
    
    OUT.color.rgba = finalColor.rgba;

    return OUT;
};

#endif // Pixel shader.