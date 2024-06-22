// D
float GGX(float alpha, float NdotH){
    float num = alpha * alpha;
    float denom = PI * pow(sqr(NdotH) * (num - 1) + 1, 2);
    return num/max(denom, 0.0001);
}

// G1
float ShlickBeckmann(float NdotX, float alpha){
    // float k = alpha / 2;
    float k = alpha * alpha * 0.797884560802865;
    return max(NdotX, 0.0001)/max(NdotX * (1 - k) + k, 0.0001);
}

// Smith
float GeometryShadowing(float alpha, float NdotV, float NdotL){
    return ShlickBeckmann(NdotV, alpha) * ShlickBeckmann(NdotL, alpha);
}

// F
float3 FresnelShlick(float3 reflectance, float3 halfway, float3 lightDir){
    return reflectance + (1 - reflectance) * pow(1 - shades(halfway, lightDir), 5.0);
}

float3 CookTorrance(float alpha, float3 fresnel, float NdotV, float NdotL, float NdotH){
    float3 num = GGX(alpha, NdotH) * GeometryShadowing(alpha, NdotV, NdotL) * fresnel;
    float denom = max(4 * NdotV * NdotL, 0.0001);
    return num/denom;
}

float3 Reflectance(float3 albedo, float metallicness){
    return lerp(float(0.04).rrr, albedo, metallicness);
}

float3 BRDF(float3 reflectance, float roughness, float3 normal, float3 eyeDir, float3 lightDir){
    float3 halfway = normalize(eyeDir + lightDir);
    float alpha = roughness;

    float NdotL = shades(normal, lightDir);
    float NdotV = shades(normal, eyeDir);
    float NdotH = shades(normal, halfway);

    float3 Ks = FresnelShlick(reflectance, halfway, lightDir);
    float3 BRDF = (1 - Ks) * CookTorrance(alpha, Ks, NdotV, NdotL, NdotH);

    return BRDF * NdotL;
}
