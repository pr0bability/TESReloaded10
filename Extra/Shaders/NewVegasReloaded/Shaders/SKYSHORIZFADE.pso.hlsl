// Night sky
//
// Parameters:

sampler2D TexMap : register(s0);

float4 TESR_GameTime;
float4 TESR_DebugVar;
float4 TESR_SkyData;
float4 TESR_ReciprocalResolution;

// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   TexMap       texture_0       1
//


// Structures:

struct VS_INPUT {
    float2 TexUV : TEXCOORD0;
    float2 position : VPOS;
    float texcoord_2 : TEXCOORD2;
    float3 location : TEXCOORD1;
    float4 color_0 : COLOR0;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};


// hash based 3d value noise
// function taken from https://www.shadertoy.com/view/XslGRr
// Created by inigo quilez - iq/2013
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
// ported from GLSL to HLSL by https://stackoverflow.com/questions/15628039/simplex-noise-shader
float hash( float n )
{
    return frac(sin(n)*43758.5453);
}

float noise( float3 x )
{
    // The noise function returns a value in the range -1.0f -> 1.0f

    float3 p = floor(x);
    float3 f = frac(x);

    f       = f*f*(3.0-2.0*f);
    float n = p.x + p.y*57.0 + 113.0*p.z;

    return lerp(lerp(lerp( hash(n+0.0), hash(n+1.0),f.x),
            lerp( hash(n+57.0), hash(n+58.0),f.x),f.y),
            lerp(lerp( hash(n+113.0), hash(n+114.0),f.x),
            lerp( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
}

#include "Includes/Helpers.hlsl"
#include "Includes/Position.hlsl"

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float4 stars = tex2D(TexMap, IN.TexUV.xy);
    float4 sky = IN.color_0;

    float starFlicker = 0.05;
    float noiseScale = 4;
    float flickerSpeed = 0.005;

    float3 eyeDir = normalize(IN.location);
  
    // create animated noise to modulate star brightness
    float n = noise(noiseScale * eyeDir);
    n *= noise((TESR_GameTime.x * flickerSpeed).xxx + noiseScale * eyeDir) * 2;

    OUT.color_0.a = (stars.a * sky.a) * IN.texcoord_2.x * TESR_SkyData.w * (n + 1);
    OUT.color_0.rgb = stars.rgb * (n * 0.3 + 1) * sky.rgb;

    return OUT;
};
