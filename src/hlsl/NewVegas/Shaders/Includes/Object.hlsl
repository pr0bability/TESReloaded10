
#include "includes/PBR.hlsl"

float getRoughness(float glossmap, float meshgloss){
    // return pow(glossmap, log(meshgloss));
    return 1 - saturate(lerp(log(meshgloss)/6, 1, glossmap));
    // return pow(1 - glossmap, meshgloss);
}

float3 getSunLighting(float3 albedo, float roughness, float3 normal, float3 eyeDirection, float3 lightDirection, float3 lightColor, float3 ambient){
    return PBR(0, roughness, albedo, normal.xyz, eyeDirection, normalize(lightDirection), linearize(lightColor)) + albedo * linearize(ambient);
}

float3 getPointLightLighting(float3 albedo, float roughness, float3 normal, float3 eyeDirection, float3 lightDirection, float distance, float3 lightColor){
    float atten = length(lightDirection / distance);
    float s = saturate(sqr(atten)); 
    atten = saturate(((1 - s) * (1 - s)) / (1 + 5.0 * s));
    return PBR(0, roughness, albedo, normal.xyz, eyeDirection, normalize(lightDirection), linearize(lightColor) * atten);
}

float3 getFinalColor(float3 finalColor){
    return delinearize(finalColor);
    // return delinearize(finalColor) * tint;
}