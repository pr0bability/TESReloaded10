// float4 TESR_DebugVar

// Specular D (normal distribution function)
float GGX(float NdotH, float roughness) {
    float alpha = roughness * roughness;
    float a2 = pow(roughness, 4);
    float d = max((NdotH * a2 - NdotH) * NdotH + 1, 1e-5);
    return a2 / (PI * d * d);
}

float GGXTest(float NdotH, float alpha) {
    float a2 = alpha * alpha;
    float d = max((NdotH * a2 - NdotH) * NdotH + 1, 1e-5);
    return a2 / (PI * d * d);
}

// G1
float ShlickBeckmann(float NdotX, float roughness) {
    float k = pow(roughness + 1, 2) / 8.0;
    return NdotX/max(NdotX * (1 - k) + k, 0.00000001);
}

float ShlickBeckmannTest(float NdotX, float k) {
    return NdotX / max(NdotX * (1 - k) + k, 0.00000001);
}

// Smith
float GeometryShadowing(float roughness, float NdotV, float NdotL) {
    return ShlickBeckmann(NdotV, roughness) * ShlickBeckmann(NdotL, roughness);
}

float GeometryShadowingTest(float k, float NdotV, float NdotL) {
    return ShlickBeckmannTest(NdotV, k) * ShlickBeckmannTest(NdotL, k);
}

// F
float3 FresnelShlick(float3 reflectance, float3 halfway, float3 eyeDir){
    return reflectance + (1 - reflectance) * pow(1 - shades(halfway, eyeDir), 5.0);
}

float3 FresnelShlickRough(float3 reflectance, float roughness, float3 halfway, float3 eyeDir) {
    return reflectance + (max(1 - roughness, reflectance) - reflectance) * pow(1 - shades(halfway, eyeDir), 5.0);
}

// BRDF
float3 BRDF(float roughness, float3 fresnel, float NdotV, float NdotL, float NdotH){
    float3 num = GGX(NdotH, roughness) * GeometryShadowing(roughness, NdotV, NdotL) * fresnel;
    float denom = 4.0 * NdotV * NdotL;
    return num/denom;
}

float3 BRDFTest(float alpha, float k, float3 fresnel, float NdotV, float NdotL, float NdotH) {
    float3 num = GGXTest(NdotH, alpha) * GeometryShadowingTest(k, NdotV, NdotL) * fresnel;
    float denom = 4.0 * NdotV * NdotL;
    return num / denom;
}

float3 PBR(float metallicness, float roughness, float3 albedo, float3 normal, float3 eyeDir, float3 lightDir, float3 lightColor, float test=0.0) {
    float3 reflectance = lerp(float(0.04).rrr, albedo, metallicness);
    
    normal = normalize(normal);
    eyeDir = normalize(eyeDir);
    lightDir = normalize(lightDir);
    
    float3 halfway = normalize(eyeDir + lightDir);
    float NdotL = max(shades(normal, lightDir), 0.00001);
    float NdotV = max(shades(normal, eyeDir), 0.00001);
    float NdotH = shades(normal, halfway);

    float3 fresnel = FresnelShlick(reflectance, halfway, eyeDir);
    float3 lambertDiffuse = (1 - metallicness) * (1 - fresnel) * albedo/PI;
    
    float3 spec;
    if (test > 0) {
        float alpha = roughness;
        float k = pow(alpha + 1, 2) / 8.0;
        spec = BRDFTest(alpha, k, fresnel, NdotV, NdotL, NdotH);
    }
    else
        spec = BRDF(roughness, fresnel, NdotV, NdotL, NdotH);

    return (lambertDiffuse + spec) * NdotL * lightColor * PI;
}