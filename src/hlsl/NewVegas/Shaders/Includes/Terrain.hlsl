// float4 TESR_DebugVar;

#include "includes/PBR.hlsl"

float4 TESR_TerrainData : register(c32);
float4 TESR_TerrainExtraData : register(c33);
float4 TESR_DebugVar : register(c34);

float3 blendDiffuseMaps(float3 vertexColor, float2 uv, int texCount, sampler2D tex[7], float blends[7]) {
    float3 color = linearize(tex2D(tex[0], uv).xyz) * blends[0];
    
    [unroll] for (int i = 1; i < texCount; i++) {
        color += linearize(tex2D(tex[i], uv).xyz) * blends[i];
    }

    return color * linearize(vertexColor);
}

float3 blendNormalMaps(float2 uv, int texCount, sampler2D tex[7], float blends[7], out float roughness) {
    roughness = 1.f;
    
    float4 normal = tex2D(tex[0], uv);
    float3 combinedNormal = expand(normal.xyz) * blends[0];
    roughness = normal.w * blends[0];
    
    [unroll] for (int i = 1; i < texCount; i++){
        normal = tex2D(tex[i], uv);
        combinedNormal += expand(normal.xyz) * blends[i];
        roughness += normal.w * blends[i];
    }

    roughness = saturate((1 - roughness) * TESR_TerrainData.y);
    return normalize(combinedNormal);
}

float3 getPointLightLighting(float3x3 tbn, float4 lightPosition, float3 lightColor, float3 eyeDir, float3 position, float3 normal, float3 albedo, float roughness = 1.0, float metallicness = 1.0, float parallaxMultiplier = 1.0) {
    float3 lightDir = lightPosition.xyz - position.xyz;
    float3 pointlightColor = linearize(lightColor) * TESR_TerrainData.z * parallaxMultiplier;

    if (TESR_TerrainExtraData.x){
        float atten = length(lightDir / lightPosition.w);
        float s = saturate(sqr(atten)); 
        atten = saturate(((1 - s) * (1 - s)) / (1 + 5.0 * s));

        lightDir = mul(tbn, lightDir);
        lightDir = normalize(lightDir);

        float3 aspect = PBR(saturate(metallicness * TESR_TerrainData.x), roughness, albedo, normal, eyeDir, lightDir, pointlightColor * atten);
        return max(0, aspect);
    } else{
        float3 atten = lightDir / lightPosition.w;
        atten = 1 - shades(atten, atten);
        return shades(normal, normalize(lightDir)) * atten * pointlightColor * albedo;
    }
}

float3 getSunLighting(float3x3 tbn, float3 lightDir, float3 sunColor, float3 eyeDir, float3 position, float3 normal, float3 AmbientColor, float3 albedo, float roughness = 1.0, float metallicness = 1.0, float parallaxMultiplier = 1.0) {
    float3 sunDir = mul(tbn, lightDir);
    
    // float3 eyeDir = -mul(tbn, normalize(position));
    float3 lightColor = linearize(sunColor) * TESR_TerrainData.z * parallaxMultiplier;
    float3 ambientColor = linearize(AmbientColor) * TESR_TerrainData.w;
    float3 color = albedo;
    color = lerp(luma(albedo), color, TESR_TerrainExtraData.z);

    // if (TESR_DebugVar.x > 0)
    //     return roughness.rrr;

    if (TESR_TerrainExtraData.x) {
        // PBR
        float3 aspect = PBR(saturate(metallicness * TESR_TerrainData.x), roughness, color, normal, eyeDir, sunDir, lightColor, TESR_DebugVar.y);
        return max(0, ambientColor * color + aspect);
    } else {
        // traditional lighting
        float diffuse = shades(sunDir, normal);
        float fresnel = pow(1 - shades(eyeDir, normal), 5) * (1 - shades(sunDir, eyeDir)) * TESR_TerrainData.x;
        float3 halfway = normalize(eyeDir + sunDir);
        float spec = pow(shades(normal, halfway), 10) * TESR_TerrainData.y;

        return max((diffuse * albedo + spec + fresnel) * sunColor + ambientColor * albedo, 0) ;
    }
}


float3 getFinalColor(float3 lighting, float3 color){
    return delinearize(lighting);
}