// float4 TESR_DebugVar;

#include "includes/PBR.hlsl"

float4 TESR_TerrainData : register(c32);
float4 TESR_TerrainExtraData : register(c33);
float4 TESR_DebugVar : register(c34);

float3 blendTextures(float4 coeffs1, float4 coeffs2, float3 vertexColor, float3 texture0=black.xyz, float3 texture1=black.xyz, float3 texture2=black.xyz, float3 texture3=black.xyz, float3 texture4=black.xyz, float3 texture5=black.xyz, float3 texture6=black.xyz){
    float3 color = linearize(texture0) * coeffs1.r;
    color += linearize(texture1) * coeffs1.g;
    color += linearize(texture2) * coeffs1.b;
    color += linearize(texture3) * coeffs1.a;
    color += linearize(texture4) * coeffs2.r;
    color += linearize(texture5) * coeffs2.g;
    color += linearize(texture6) * coeffs2.b;

    return color * linearize(vertexColor);
}

float combineRoughness(float4 coeffs1, float4 coeffs2, float roughness0=black.x, float roughness1=black.x, float roughness2=black.x, float roughness3=black.x, float roughness4=black.x, float roughness5=black.x, float roughness6=black.x){
    float roughness = roughness0 * coeffs1.r;
    roughness += roughness1 * coeffs1.g;
    roughness += roughness2 * coeffs1.b;
    roughness += roughness3 * coeffs1.a;
    roughness += roughness4 * coeffs2.r;
    roughness += roughness5 * coeffs2.g;
    roughness += roughness6 * coeffs2.b;

    // roughness = invlerp(TESR_DebugVar.x, TESR_DebugVar.y, roughness);
    // roughness = ((roughness + TESR_DebugVar.x) - TESR_DebugVar.w) * TESR_DebugVar.z + TESR_DebugVar.w;
    
    // roughness = invlerps(0, TESR_DebugVar.x, roughness) * 0.5 + invlerps(TESR_DebugVar.x, 1, roughness) * 0.5;
    // roughness = (roughness - TESR_DebugVar.y) * TESR_DebugVar.z + TESR_DebugVar.y;

    return saturate((1 - roughness) * TESR_TerrainData.y); // maps contain glossiness data, not roughness. We invert it
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

float3 getSunLighting(float3x3 tbn, float3 lightDir, float3 sunColor, float3 eyeDir, float3 position, float3 normal, float3 AmbientColor, float3 albedo, float roughness=1.0, float metallicness=1.0, float parallaxMultiplier=1.0){
    float3 sunDir = mul(tbn, lightDir);
    // float3 eyeDir = -mul(tbn, normalize(position));
    float3 lightColor = linearize(sunColor) * TESR_TerrainData.z * parallaxMultiplier;
    float3 ambientColor = linearize(AmbientColor) * TESR_TerrainData.w;
    float ao = luma(albedo);
    float3 color = albedo;
    color = lerp(ao, color, TESR_TerrainExtraData.z);

    // if (TESR_DebugVar.w > 0)
    //     return roughness.rrr;

    if (TESR_TerrainExtraData.x){
        // PBR
        float3 aspect = PBR(saturate(metallicness * TESR_TerrainData.x), roughness, color, normal, eyeDir, sunDir, lightColor);
        return max(0, aspect + ambientColor * color);
    }else{
        // traditional lighting
        float diffuse = shades(sunDir, normal);
        float fresnel = pow(1 - shades(eyeDir, normal), 5) * (1 - shades(sunDir, eyeDir)) * TESR_TerrainData.x;
        float3 halfway = normalize(eyeDir + sunDir);
        float spec = pow(shades(normal, halfway), 10) * TESR_TerrainData.y;

        return max((diffuse * albedo + spec + fresnel) * sunColor + ambientColor * albedo, 0) ;
    }
}


float3 getFinalColor(float3 lighting, float3 color){
    // fog
    // return (IN.texcoord_7.w * (IN.texcoord_7.xyz - (vertexColor * lighting * color))) + (lighting * color * vertexColor);

    // return delinearize(lighting * tint);
    return delinearize(lighting);
}