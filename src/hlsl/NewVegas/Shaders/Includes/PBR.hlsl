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


float3 BRDF(float roughness, float NdotL, float NdotV, float NdotH, float3 fresnel){
    float3 BRDF = fresnel + (1 - fresnel) * CookTorrance(roughness, fresnel, NdotV, NdotL, NdotH);
    return BRDF * NdotL;
}

float3 modifiedBRDF(float roughness, float NdotL, float NdotV, float NdotH, float3 fresnel){
    // BRDF without the fresnel term to get cleaner specular on water
    float3 BRDF = (1 - fresnel) * CookTorrance(roughness, fresnel, NdotV, NdotL, NdotH);
    return BRDF * NdotL;
}

float3 PBR(float metallicness, float roughness, float3 albedo, float3 normal, float3 eyeDir, float3 lightDir, float3 lightColor){
    float3 reflectance = lerp(float(0.04).rrr, albedo, metallicness);
    float3 halfway = normalize(eyeDir + lightDir);
    float NdotL = shades(normal, lightDir);
    float NdotV = shades(normal, eyeDir);
    float NdotH = shades(normal, halfway);

    float3 Ks = FresnelShlick(reflectance, halfway, lightDir);
    float3 lambertDiffuse = (1 - metallicness) * Ks * albedo/PI * NdotL;
    float3 spec = BRDF(roughness * roughness, NdotL, NdotV, NdotH, Ks);
    // float spec = 0;//BRDF(reflectance, roughness, NdotL, NdotV, NdotH, Ks);

    return (lambertDiffuse + spec) * lerp(lightColor, albedo, metallicness * 0.5);
}