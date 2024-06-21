// D
float GGX(float alpha, float3 normal, float3 halfway){
    float num = alpha * alpha;
    float NdotH = shades(normal, halfway);
    float denom = PI * pow(sqr(NdotH) * (num - 1) + 1, 2);
    return num/max(denom, 0.001);
}

// G1
float ShlickBeckmann(float alpha, float3 normal, float3 v){
    float NdotL = shades(normal, v);
    float k = alpha / 2;
    // float k = alpha * alpha * 0.797884560802865;
    return max(NdotL/(NdotL * (1 - k) + k), 0.005);
}

// Smith
float GeometryShadowing(float alpha, float3 normal, float3 eyeDir, float3 lightDir){
    return saturate(ShlickBeckmann(alpha, normal, eyeDir) * ShlickBeckmann(alpha, normal, lightDir));
}


// F
float3 FresnelShlick(float3 F0, float3 halfway, float3 lightDir){
    return F0 + (1 - F0) * pow(1 - shades(halfway, lightDir), 5.0);
}

float3 CookTorrance(float alpha, float3 fresnel, float3 normal, float3 halfway, float3 eyeDir, float3 lightDir){
    float3 num = GGX(alpha, normal, halfway) * GeometryShadowing(alpha, normal, eyeDir, lightDir) * fresnel;
    float denom = max(4 * shades(eyeDir, normal) * shades(lightDir, normal), 0.001);
    return num/denom;   
}

float3 PBR(float3 F0, float alpha, float3 normal, float3 halfway, float3 eyeDir, float3 lightDir){
    float3 Ks = FresnelShlick(F0, halfway, lightDir);
    float3 Kd = 1 - Ks;
    float3 BRDF = Kd * CookTorrance(alpha, Ks, normal, halfway, eyeDir, lightDir);

    return BRDF * shades(normal, lightDir);
}
