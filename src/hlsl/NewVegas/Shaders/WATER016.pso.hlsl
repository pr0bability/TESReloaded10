// autowater seen from below

// Parameters:
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
float4 TESR_WaterSettings : register(c16); // x: caustic strength, y:depthDarkness, w: turbidity, z: caustic strength S ?
float4 TESR_GameTime : register(c17);
float4 TESR_HorizonColor : register(c18);
float4 TESR_SkyLowColor : register(c19);
float4 TESR_SunDirection : register(c20);
float4 TESR_ReciprocalResolution : register(c21);
float4 TESR_WetWorldData : register(c22);
float4 TESR_WaterShorelineParams : register(c23);
float4 TESR_SunColor : register(c24);
float4 TESR_CameraPosition : register(c25);
float4 TESR_DebugVar : register(c26);

sampler2D ReflectionMap : register(s0);
sampler2D RefractionMap : register(s1);
sampler2D NoiseMap : register(s2);
sampler2D DisplacementMap : register(s3); //unused
sampler2D DepthMap : register(s4);
sampler2D TESR_samplerWater : register(s5) < string ResourceName = "Water\water_NRM.dds"; > = sampler_state { ADDRESSU = WRAP; ADDRESSV = WRAP; ADDRESSW = WRAP; MAGFILTER = ANISOTROPIC; MINFILTER = ANISOTROPIC; MIPFILTER = ANISOTROPIC; } ;
sampler2D TESR_RippleSampler : register(s6) < string ResourceName = "Precipitations\ripples.dds"; > = sampler_state { ADDRESSU = WRAP; ADDRESSV = WRAP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };


#include "Includes/Helpers.hlsl"
#include "Includes/Water.hlsl"


float4 getFresnelBelowWater(float3 surfaceNormal, float3 eyeDirection, float4 reflectionColor, float4 color){
    float fresnelCoeff = 1 - pow(dot(eyeDirection, surfaceNormal), 5);  // texture based fresnel
    fresnelCoeff += 0.5 * pows(1 - dot(eyeDirection.xyz, float3(0, 0, -1)), 1.5);  // modulate with distance
    return lerp(color, reflectionColor, pow(saturate(fresnelCoeff), 5));
}

float4 skyColor(float3 eyeDirection, float4 sunColor){
    float3 skyColor = lerp(linearize(TESR_HorizonColor), linearize(TESR_SkyLowColor), pow(dot(eyeDirection, float3(0, 0, 1)), 0.5)).rgb; //linearise
    skyColor += sunColor.rgb * 5 * pow(shades(eyeDirection, -TESR_SunDirection.xyz), 20);
    return float4(skyColor, 1);
}

PS_OUTPUT main(PS_INPUT IN) {
    PS_OUTPUT OUT;

    float4 linSunColor = linearize(TESR_SunColor);
    float4 linShallowColor = linearize(ShallowColor);

    float3 eyeVector = EyePos.xyz - IN.LTEXCOORD_0.xyz; // vector of camera position to point being shaded
    float3 eyeDirection = normalize(eyeVector);         // normalized eye to world vector (for lighting)
    float distance = length(eyeVector.xy);              // surface distance to eye

    // calculate fog coeffs
    //float4 screenPos = getScreenpos(IN);                // point coordinates in screen space for water surface

    float sunLuma = luma(linSunColor.rgb);
    float exteriorRefractionModifier = TESR_WaterSettings.w;	// reduce refraction because of the way interior depth is encoded

    float3 surfaceNormal = getWaveTexture(IN, distance, TESR_WaveParams).xyz;
    surfaceNormal = getRipples(IN, TESR_RippleSampler, surfaceNormal, distance, TESR_WetWorldData.x) * -1;

    float4 refractionPos = getReflectionSamplePosition(IN, surfaceNormal, exteriorRefractionModifier);
    refractionPos.y = refractionPos.w - refractionPos.y;

    float4 sky = skyColor(eyeDirection, linSunColor);
    float depth = TESR_WaterSettings.x - TESR_CameraPosition.z;
    float4 refractions = linearize(tex2Dproj(RefractionMap, refractionPos)) * smoothstep(200, 0, depth) + sky; // mix & fade refraction with depth because vanilla refractions suck

    float4 color = sky;
    color = getFresnelBelowWater(surfaceNormal, eyeDirection, linShallowColor * sunLuma, color);
    color +=  2 * pow(dot(surfaceNormal, eyeDirection), 2) * (refractions); // highlight

    color = delinearize(color); //delinearise
    
    // Standard fog.
    float fogStrength = pow(1 - saturate((FogParam.x - depth) / FogParam.y), FresnelRI.y);
    
    OUT.color_0.rgb = fogStrength * (FogColor.rgb - color.rgb) + color.rgb;
    OUT.color_0.a = 1;
    
    return OUT;
};

// approximately 48 instruction slots used (2 texture, 46 arithmetic)
