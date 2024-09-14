
#include "includes/PBR.hlsl"

float getRoughness(float glossmap, float meshgloss){
    // return pow(glossmap, log(meshgloss));    
    // no gloss = 1
    // full gloss = 0

    return 1 - log(meshgloss)/4 * glossmap;
    // return 1 - saturate(log(meshgloss)/4 + glossmap);
    // return pow(1 - glossmap, meshgloss);
}

float3 getSunLighting(float3 albedo, float roughness, float3 normal, float3 eyeDirection, float3 lightDirection, float3 lightColor, float3 ambient){
    // return roughness.rrr;
    return PBR(0, roughness, albedo, normal.xyz, eyeDirection, normalize(lightDirection), linearize(lightColor)) + albedo * linearize(ambient);
}

float3 getPointLightLighting(float3 albedo, float roughness, float3 normal, float3 eyeDirection, float3 lightDirection, float distance, float3 lightColor){
    float atten = length(lightDirection / distance);
    float s = saturate(sqr(atten)); 
    atten = saturate(((1 - s) * (1 - s)) / (1 + 5.0 * s));
    // return float(0).rrr;
    return PBR(0, roughness, albedo, normal.xyz, eyeDirection, normalize(lightDirection), linearize(lightColor) * atten);
}

float3 getFinalColor(float3 finalColor){
    // return finalColor;
    return delinearize(finalColor);
    // return delinearize(finalColor) * tint;
}