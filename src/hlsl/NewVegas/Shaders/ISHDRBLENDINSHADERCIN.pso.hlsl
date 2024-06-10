//
//
//#define	ScreenSpace	Src0
// Parameters:

float4 HDRParam : register(c1);
float3 BlurScale : register(c2);
float4 Cinematic : register(c19); // x:saturation, y:avgluma, z:brightness, w: contrast
float4 Tint : register(c20);  // weather Cinematic IS tint ?
float4 Fade : register(c22);  // Night eye color
float4 TESR_DebugVar : register(c24);
float4 TESR_HDRBloomData : register(c25); //
//float4 TESR_SunAmount : register(c26);
float4 TESR_HDRData : register(c27); //
float4 TESR_LotteData : register(c28); //
float4 TESR_ToneMapping : register(c29); //
float4 TESR_ReciprocalResolution : register(c30); //
float4 TESR_BloomData : register(c31); //

sampler2D Src0 : register(s0);
sampler2D DestBlend : register(s1);    // base non tonemapped image
sampler2D TESR_BloomBuffer : register(s8) = sampler_state { ADDRESSU = WRAP; ADDRESSV = WRAP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   HDRParam     const_1       1
//   BlurScale    const_2       1
//   Cinematic    const_19      1
//   Tint         const_20      1
//   Fade         const_22      1
//   ScreenSpace         texture_0       1
//   DestBlend    texture_1       1
//

#include "Includes/Helpers.hlsl"
#include "Includes/Tonemapping.hlsl"

// Structures:

struct VS_INPUT {
    float2 ScreenOffset : TEXCOORD0;
    float2 texcoord_1 : TEXCOORD1;
    float2 pos : VPOS;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

static const float3 offsets[5] = {
	float3(0, 0, 1),
	float3(-1, -1, 1), 
	float3(1, -1, 1), 
	float3(-1, 1, 1), 
	float3(1, 1, 1)
};

static const float cinematicScalar = TESR_HDRData.z;

float4 sampleBox(sampler2D buffer, float2 uv, float offset){
    float4 color = float4(0, 0, 0, 0);
    float total = 0;

    for (int i = 0; i < 5; i ++){
        float2 coords = uv + TESR_ReciprocalResolution.xy * offsets[i].xy * offset;
        float2 safety = TESR_ReciprocalResolution.xy * float2(0.5, 0.5);
        float isValid = (coords.x > safety.x && coords.x < 1 - safety.x && coords.y > safety.y && coords.y < 1 - safety.y) * offsets[i].z;
        color += tex2D(buffer, coords) * isValid;
        total += isValid;
    }
    color /= total;

    return color;
}

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;
    // Whether we linearize before applying most post process effects that were already present in the vanilla game.
    // If we do, it will look less like vanilla; sometimes it will look better, some other times it will look worse (tint and fade especially make a big difference);
    // generally giving a darker and less saturated look in linear space.
    bool gammaSpacePostProcess = (TESR_ToneMapping.w < 0 || TESR_HDRData.w < 0); // apply if gamma or linearization settings are below 0
    //TODO: TESR_ToneMapping.w isn't used anymore (it was in gamma), TESR_HDRData.w isn't used anymore (it was out gamma)

    float4 final = tex2D(DestBlend, IN.texcoord_1.xy);
    float3 tint = Tint.rgb;
    float3 fade = Fade.rgb;

    if (!gammaSpacePostProcess){
        // linearize
        final.rgb = pows(final.rgb, TESR_ToneMapping.w);
        tint = pows(tint, TESR_ToneMapping.w);
        fade = pows(fade, TESR_ToneMapping.w);
    }

    final.rgb = lerp(luma(final.rgb).xxx, final.rgb, Cinematic.x * cinematicScalar); // saturation
    final.rgb = lerp(final.rgb, tint * luma(final.rgb), saturate(Tint.a * TESR_ToneMapping.z)); // apply tint
    
    if (TESR_BloomData.w){
        // NVR bloom
        float4 NVRbloom = sampleBox(TESR_BloomBuffer, IN.texcoord_1.xy, 0.5); // already linear

        if (gammaSpacePostProcess){ // Always do the new bloom in linear space as it's designed for that
            final.rgb = linearize(final.rgb);
        }
        final.rgb = lerp(final.rgb, NVRbloom.rgb * TESR_BloomData.z, TESR_HDRBloomData.x * 0.15); 
    }else{
        // vanilla bloom
        // scale bloom while maintaining color
        float4 bloom = tex2D(Src0, IN.ScreenOffset.xy);
        if (!gammaSpacePostProcess){
            // linearize bloom
            bloom.rgb = pows(bloom.rgb, TESR_ToneMapping.w);
        }
        bloom.rgb = TESR_HDRBloomData.x * pows(bloom.rgb, TESR_HDRBloomData.y);

        float q0 = 1.0 / max(bloom.w, HDRParam.x); // HDRParam.x is brights cutoff (clamp)
        final.rgb = ((q0 * HDRParam.x) * final.rgb) + max(bloom.rgb * (q0 * 0.5), 0.0); // blend image and bloom
        if (gammaSpacePostProcess){
            final.rgb = linearize(final.rgb);
        }
    }
    
    final.rgb = lerp(final.rgb, final.rgb * Cinematic.w, cinematicScalar); // apply brightness from Cinematic
    final.rgb = tonemap(final.rgb * TESR_HDRData.y); // exposure & tonemap using provided tonemapper
    
    if (gammaSpacePostProcess){
        final.rgb = delinearize(final.rgb);
    }
    
    final.rgb = lerp(final.rgb, (Cinematic.z * (final.rgb - Cinematic.y)) + Cinematic.y, cinematicScalar * TESR_ToneMapping.y); // apply contrast from Cinematic, scaled by modifier
    final.rgb = lerp(final.rgb, fade, Fade.a); // apply night eye and fade

    if (!gammaSpacePostProcess){
        final.rgb = pows(final.rgb, 1/TESR_HDRData.w); // delinearize
    }
    
    OUT.color_0 = float4(final.rgb,BlurScale.z); 

    return OUT;
};

// approximately 18 instruction slots used (2 texture, 16 arithmetic)
