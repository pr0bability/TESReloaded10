// float4 TESR_DebugVar;

#include "includes/PBR.hlsl"

float4 TESR_TerrainData : register(c16);
float4 TESR_TerrainExtraData : register(c17);

float3 blendTextures(float4 coeffs1, float4 coeffs2, float3 vertexColor, float3 texture0=black.xyz, float3 texture1=black.xyz, float3 texture2=black.xyz, float3 texture3=black.xyz, float3 texture4=black.xyz, float3 texture5=black.xyz, float3 texture6=black.xyz){
    float3 color = texture0 * coeffs1.r;
    color += texture1 * coeffs1.g;
    color += texture2 * coeffs1.b;
    color += texture3 * coeffs1.a;
    color += texture4 * coeffs2.r;
    color += texture5 * coeffs2.g;
    color += texture6 * coeffs2.b;

    return linearize(color) * linearize(vertexColor);
}


float3 getPointLightLighting(float3x3 tbn, float4 lightPosition, float3 lightColor, float3 position, float3 normal){
    float3 lightDir = lightPosition.xyz - position.xyz;
    float3 atten = lightDir / lightPosition.w;
    lightDir = mul(tbn, lightDir);
    return shades(normal, normalize(lightDir)) * (1 - shades(atten, atten)) * lightColor;
}

float3 getSunLighting(float3x3 tbn, float3 lightDir, float3 sunColor, float3 position, float3 normal, float3 AmbientColor, float3 albedo){
    float3 sunDir = mul(tbn, lightDir);
    float3 eyeDir = -mul(tbn, normalize(position));
    float3 lightColor = linearize(sunColor) * TESR_TerrainData.z;
    float3 ambientColor = linearize(AmbientColor) * TESR_TerrainData.w;

    if (TESR_TerrainExtraData.x){
        // PBR
        lightColor *= 1; // boosting the sun to get the same behavior as vanilla in terms of overall brightness
        ambientColor *= 1;
        albedo = lerp(luma(albedo), albedo, TESR_TerrainExtraData.z);

        float3 aspect = PBR(saturate(TESR_TerrainData.x), saturate(TESR_TerrainData.y), albedo, normal, eyeDir, sunDir, lightColor);
        return max(0, aspect + ambientColor * albedo);
    }else{
        // traditional lighting
        float diffuse = shades(sunDir, normal);
        float fresnel = pow(1 - shades(eyeDir, normal), 5) * (1 - shades(sunDir, eyeDir)) * TESR_TerrainData.x;
        float3 halfway = normalize(eyeDir + sunDir);
        float spec = pow(shades(normal, halfway), 10) * TESR_TerrainData.y;

        return max((diffuse + spec + fresnel) * sunColor + ambientColor, 0) * albedo;
    }
}


float3 getFinalColor(float3 lighting, float3 color){
    // fog
    // return (IN.texcoord_7.w * (IN.texcoord_7.xyz - (vertexColor * lighting * color))) + (lighting * color * vertexColor);
    // float3 lambert = (linearize(color) * linearize(vertexColor)); // apply conservation of energy
    // float3 lambert = (linearize(color) * linearize(vertexColor)) * lerp(1, 1/PI, TESR_TerrainExtraData.x); // apply conservation of energy

    return delinearize(lighting);
    // return delinearize(color);
    // return delinearize(color);
}