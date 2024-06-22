// float4 TESR_DebugVar;

#include "includes/PBR.hlsl"

float4 TESR_TerrainData : register(c16);
float4 TESR_TerrainExtraData : register(c17);

float3 getPointLightLighting(float3x3 tbn, float4 lightPosition, float3 lightColor, float3 position, float3 normal){
    float3 lightDir = lightPosition.xyz - position.xyz;
    float3 atten = lightDir / lightPosition.w;
    lightDir = mul(tbn, lightDir);
    return shades(normal, normalize(lightDir)) * (1 - shades(atten, atten)) * lightColor;
}

float3 getSunLighting(float3x3 tbn, float3 lightDir, float3 sunColor, float3 position, float3 normal, float3 AmbientColor, float3 albedo){
    float3 sunDir = mul(tbn, lightDir);
    float3 eyeDir = -mul(tbn, normalize(position));

    if (TESR_TerrainExtraData.x){
        // PBR
        float3 reflectance = Reflectance(albedo, saturate(TESR_TerrainData.x));
        float roughness = saturate(TESR_TerrainData.y);
        // float3 lightColor = linearize(sunColor) * TESR_TerrainData.z * 6.0 * TESR_DebugVar.z;
        // float3 ambient = linearize(AmbientColor) * TESR_TerrainData.w * 4.0 * TESR_DebugVar.w;
        float3 lightColor = linearize(sunColor) * TESR_TerrainData.z * 500; // boosting the sun to get the same behavior as vanilla in terms of overall brightness
        float3 ambient = linearize(AmbientColor) * TESR_TerrainData.w * 3.0;
        float3 spec = BRDF(reflectance, roughness, normal, eyeDir, lightDir);

        return max(0, spec * shades(normal, sunDir)) * lightColor + ambient;
    }else{
        // traditional lighting
        float diffuse = shades(sunDir, normal);
        float fresnel = pow(1 - shades(eyeDir, normal), 5) * (1 - shades(sunDir, eyeDir)) * TESR_TerrainData.x;
        float3 halfway = normalize(eyeDir + sunDir);
        float spec = pow(shades(normal, halfway), 10) * TESR_TerrainData.y;

        return max((diffuse + spec + fresnel) * linearize(sunColor) * TESR_TerrainData.z + linearize(AmbientColor) * TESR_TerrainData.w, 0);
    }
}


float3 getFinalColor(float3 lighting, float3 color, float3 vertexColor){
    // fog
    // return (IN.texcoord_7.w * (IN.texcoord_7.xyz - (vertexColor * lighting * color))) + (lighting * color * vertexColor);
    // float3 lambert = (linearize(color) * linearize(vertexColor)) / PI;
    float3 lambert = (linearize(color) * linearize(vertexColor)) * lerp(1, 1/PI, TESR_TerrainExtraData.x); // apply conservation of energy

    return delinearize(lighting * lambert);
}