// LOD autowater surface shader

float4 EyePos : register(c1);
float4 ShallowColor : register(c2);
float4 DeepColor : register(c3);
float4 ReflectionColor : register(c4);
float4 FresnelRI : register(c5);
float4 VarAmounts : register(c8);
float4 FogParam : register(c9);
float4 FogColor : register(c10);
float4 SunDir : register(c12);
float4 SunColor : register(c13);

float4 TESR_WaveParams : register(c14);
float4 TESR_WaterVolume : register(c15); // x: caustic strength, y:shoreFactor, w: turbidity, z: caustic strength S ?
float4 TESR_WaterSettings : register(c16); // x: caustic strength, y:depthDarkness, w: turbidity, z: caustic strength S ?
float4 TESR_GameTime : register(c17);
float4 TESR_HorizonColor : register(c18);
float4 TESR_SunDirection : register(c19);
float4 TESR_WaterShorelineParams : register(c20);
float4 TESR_DebugVar : register(c21);
float4 TESR_WaterShallowColor : register(c22);
float4 TESR_SunAmount : register(c23);

sampler2D ReflectionMap : register(s0);
sampler2D RefractionMap : register(s1); //unused
sampler2D NoiseMap : register(s2); //unused
sampler2D DisplacementMap : register(s3); //unused
sampler2D DepthMap : register(s4); //unused
sampler2D TESR_samplerWater : register(s5) < string ResourceName = "Water\water_NRM.dds"; > = sampler_state { ADDRESSU = WRAP; ADDRESSV = WRAP; ADDRESSW = WRAP; MAGFILTER = ANISOTROPIC; MINFILTER = ANISOTROPIC; MIPFILTER = ANISOTROPIC; } ;


// Registers:
//
//   Name            Reg   Size
//   --------------- ----- ----
//   EyePos          const_1       1
//   ShallowColor    const_2       1
//   DeepColor       const_3       1
//   ReflectionColor const_4       1
//   FresnelRI       const_5       1
//   VarAmounts      const_8       1
//   FogParam        const_9       1
//   FogColor        const_10      1
//   SunDir          const_12      1
//   SunColor        const_13      1
//   ReflectionMap   texture_0       1
//

#include "Includes/Helpers.hlsl"
#include "Includes/Water.hlsl"

PS_OUTPUT main(PS_INPUT IN) {
    PS_OUTPUT OUT;

    float4 linSunColor = linearize(SunColor); //linearise
    float4 linShallowColor = linearize(ShallowColor); //linearise
    //float4 linShallowColor = linearize(TESR_WaterShallowColor); //linearise
    float4 linHorizonColor = linearize(TESR_HorizonColor); //linearise

    float3 eyeVector = EyePos.xyz - IN.LTEXCOORD_0.xyz; // vector of camera position to point being shaded
    float3 eyeDirection = normalize(eyeVector);         // normalized eye to world vector (for lighting)
    float distance = length(eyeVector.xy);              // surface distance to eye
    float depth = length(eyeVector);                    // depth distance to eye

    // calculate fog coeffs
    float4 screenPos = getScreenpos(IN);                // point coordinates in screen space for water surface
    float isDayTime = smoothstep(0, 0.5, TESR_SunAmount.x);
    float sunLuma = luma(linSunColor) * isDayTime;

    float3 surfaceNormal = getWaveTexture(IN, distance, TESR_WaveParams).xyz;
    float refractionCoeff = ((saturate(distance * 0.002) * (-4 + VarAmounts.w)) + 4);
    float4 reflectionPos = getReflectionSamplePosition(IN, surfaceNormal, refractionCoeff);
    float4 reflection = linearize(tex2Dproj(ReflectionMap, reflectionPos));

    float4 color = linShallowColor * sunLuma;
    // color = getDiffuse(surfaceNormal, TESR_SunDirection.xyz, eyeDirection, distance, linHorizonColor, color);
    color = getFresnel(surfaceNormal, eyeDirection, reflection, TESR_WaveParams.w, color);
    color = getSpecular(surfaceNormal, TESR_SunDirection.xyz, eyeDirection, linSunColor.rgb, color);
    color.a = 1;

    color = delinearize(color); //delinearise
    
    // Standard fog.
    float fogStrength = pow(1 - saturate((FogParam.x - depth) / FogParam.y), FresnelRI.y);
    
    OUT.color_0.rgb = fogStrength * (FogColor.rgb - color.rgb) + color.rgb;
    OUT.color_0.a = color.a;

    return OUT;	
};