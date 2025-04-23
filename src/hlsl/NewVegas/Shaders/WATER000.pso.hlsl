// autowater surface shader (closeup, no wading displacement)

float4 EyePos : register(c1);
float4 ShallowColor : register(c2);
float4 DeepColor : register(c3);
float4 ReflectionColor : register(c4);
float4 FresnelRI : register(c5); //x: reflectamount, y:fresnel, w: opacity, z:speed
float4 VarAmounts : register(c8); // x: water glossiness y: reflectivity z: refrac, w: lod
float4 FogParam : register(c9);
float4 FogColor : register(c10);
float2 DepthFalloff : register(c11); // start / end depth fog
float4 SunDir : register(c12);
float4 SunColor : register(c13);
float4 TESR_WaveParams : register(c14); // x: choppiness, y:wave width, z: wave speed, w: reflectivity?
float4 TESR_WaterVolume : register(c15); // x: caustic strength, y:shoreFactor, w: turbidity, z: caustic strength S ?
float4 TESR_WaterSettings : register(c16); // x: caustic strength, y:depthDarkness, w: refraction, z: caustic strength S ?
float4 TESR_GameTime : register(c17);
float4 TESR_HorizonColor : register(c18);
float4 TESR_SunDirection : register(c19);
float4 TESR_ReciprocalResolution : register(c20);
float4 TESR_WetWorldData : register(c21);
float4 TESR_WaterShorelineParams : register(c22);
float4 TESR_WaterLODColor : register(c23);
float4 TESR_DebugVar : register(c24);
float4 TESR_SunAmount : register(c25);

sampler2D ReflectionMap : register(s0);
sampler2D RefractionMap : register(s1);
sampler2D NoiseMap : register(s2);
sampler2D DisplacementMap : register(s3); //unused
sampler2D DepthMap : register(s4);
sampler2D TESR_samplerWater : register(s5) < string ResourceName = "Water\water_NRM.dds"; > = sampler_state { ADDRESSU = WRAP; ADDRESSV = WRAP; ADDRESSW = WRAP; MAGFILTER = ANISOTROPIC; MINFILTER = LINEAR; MIPFILTER = LINEAR; } ;
sampler2D TESR_RippleSampler : register(s6) < string ResourceName = "Precipitations\ripples.dds"; > = sampler_state { ADDRESSU = WRAP; ADDRESSV = WRAP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

#include "Includes/Helpers.hlsl"
#include "Includes/Water.hlsl"

PS_OUTPUT main(PS_INPUT IN, float2 PixelPos : VPOS) {
    PS_OUTPUT OUT;

    float4 linSunColor = linearize(SunColor);
    float4 linShallowColor = linearize(ShallowColor);
    float4 linDeepColor = linearize(DeepColor);
    float4 linHorizonColor = linearize(TESR_HorizonColor);

    // float2 UVCoord = (PixelPos+0.5)*TESR_ReciprocalResolution.xy;
    // float4 worldPos = reconstructWorldPosition(UVCoord);
    // float3 floorNormal = normalize(cross(ddx(worldPos), ddy(worldPos)));


    float3 eyeVector = EyePos.xyz - IN.LTEXCOORD_0.xyz; // vector of camera position to point being shaded
    float3 eyeDirection = normalize(eyeVector);         // normalized eye to world vector (for lighting)
    float distance = length(eyeVector.xy);              // surface distance to eye
    float depth = length(eyeVector);                    // depth distance to eye

    // calculate fog coeffs
    float4 screenPos = getScreenpos(IN);                // point coordinates in screen space for water surface

    float2 waterDepth = tex2Dproj(DepthMap, screenPos).xy;  // x= shallowfog, y = deepfog?
    float depthFog = saturate(invlerp(DepthFalloff.x, DepthFalloff.y, waterDepth.y));
    
    float2 fadedDepth = saturate(lerp(waterDepth, 1, invlerp(0, 4096, distance)));

    float LODfade = saturate(smoothstep(4096,4096 * 2, distance));
    float isDayTime = smoothstep(0, 0.5, TESR_SunAmount.x);
    float sunLuma = luma(linSunColor) * isDayTime;
    float exteriorRefractionModifier = TESR_WaterSettings.w;		// reduce refraction because of the way interior depth is encoded
    float exteriorDepthModifier = 1;			// reduce depth value for fog because of the way interior depth is encoded

    float3 surfaceNormal = getWaveTexture(IN, distance, TESR_WaveParams).xyz;
    surfaceNormal = getRipples(IN, TESR_RippleSampler, surfaceNormal, distance, TESR_WetWorldData.x);

    float refractionCoeff = (waterDepth.y * depthFog) * ((saturate(distance * 0.002) * (-4 + VarAmounts.w)) + 4);
    float4 reflectionPos = getReflectionSamplePosition(IN, surfaceNormal, refractionCoeff * exteriorRefractionModifier);
    float4 reflection = linearize(tex2Dproj(ReflectionMap, reflectionPos));
    float4 refractionPos = reflectionPos;
    refractionPos.y = refractionPos.w - reflectionPos.y;
    float3 refractedDepth = tex2Dproj(DepthMap, refractionPos).rgb * exteriorDepthModifier;

    // float water = max(refractedDepth.y, 0.0000000001) * 4096;
    // float water = max(waterDepth.y, 0.0000000001) * 4096;
    // float4 floorNormal = float4(normalize(float4(ddx(water), ddy(water), 1, 1).rgb) + eyeDirection.rgb, 1);

    float4 color = linearize(tex2Dproj(RefractionMap, refractionPos));
    color = getLightTravel(refractedDepth, linShallowColor, linDeepColor, sunLuma, TESR_WaterSettings, color);
    color = lerp(getTurbidityFog(refractedDepth, linShallowColor, TESR_WaterVolume, sunLuma, color), linearize(TESR_WaterLODColor) * sunLuma, LODfade); // fade to full fog to hide LOD seam
    //color = getDiffuse(surfaceNormal, TESR_SunDirection.xyz, eyeDirection, distance, linHorizonColor, color);
    color = lerp(color, getFresnel(surfaceNormal, eyeDirection, reflection, TESR_WaveParams.w, color), smoothstep(0, 0.2, refractedDepth.x)); // reduce fresnel in low depths
    color = getSpecular(surfaceNormal, TESR_SunDirection.xyz, eyeDirection, linSunColor.rgb, color);
    color = lerp(getShoreFade(IN, waterDepth.x, TESR_WaterShorelineParams.x, TESR_WaterVolume.y, color), color, LODfade);

    color = delinearize(color);
    
    // Standard fog.
    float fogStrength = pow(1 - saturate((FogParam.x - depth) / FogParam.y), FresnelRI.y);
    
    OUT.color_0.rgb = fogStrength * (FogColor.rgb - color.rgb) + color.rgb;
    OUT.color_0.a = color.a;
    
    return OUT;
};