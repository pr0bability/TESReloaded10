

float3 getPointLightLighting(float3x3 tbn, float4 lightPosition, float3 lightColor, float3 position, float3 normal){
    float3 lightDir = lightPosition.xyz - position.xyz;
    float3 atten = lightDir / lightPosition.w;
    lightDir = mul(tbn, lightDir);
    return shades(normal, normalize(lightDir)) * (1 - shades(atten, atten)) * lightColor;
}


float3 getSunLighting(float3x3 tbn, float3 lightDir, float3 sunColor, float3 position, float3 normal, float3 AmbientColor){
    float3 sunDir = mul(tbn, lightDir);

    return shades(normal, sunDir) * sunColor + AmbientColor;

    // float3 eyeDir = normalize(TESR_CameraPosition - position);
    // float diffuse = shades(normal, sunDir);
    // float fresnel = pow(1 - shades(eyeDir, normal), 5) * (1 - shades(sunDir, eyeDir));
    // float spec = pow(shades(normal, (eyeDir + sunDir) / 2), 10);

    // return max((diffuse + spec + fresnel) * sunColor + AmbientColor.rgb , 0);
}