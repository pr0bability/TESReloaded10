

float3 getPointLightLighting(float3x3 tbn, float4 lightPosition, float3 lightColor, float3 position, float3 normal){
    float3 lightDir = lightPosition.xyz - position.xyz;
    float3 atten = lightDir / lightPosition.w;
    lightDir = mul(tbn, lightDir);
    return shades(normal, normalize(lightDir)) * (1 - shades(atten, atten)) * lightColor;
}


float3 getSunLighting(float3x3 tbn, float3 lightDir, float3 sunColor, float3 position, float3 normal, float3 AmbientColor){
    float3 sunDir = mul(tbn, lightDir);

    // return shades(normal, sunDir) * sunColor + AmbientColor;

    float3 eyeDir = -mul(tbn, normalize(position));
    float diffuse = shades(sunDir, normal);
    float fresnel = pow(1 - shades(eyeDir, normal), 5) * (1 - shades(sunDir, eyeDir)) * 0.3;
    float spec = pow(shades(normal, normalize(eyeDir + sunDir)), 10) * 0.5;

    return max((diffuse + spec + fresnel) * linearize(sunColor) + linearize(AmbientColor) , 0);
}


float3 getFinalColor(float3 lighting, float3 color, float3 vertexColor){

    return delinearize(lighting * linearize(color) * linearize(vertexColor));
}