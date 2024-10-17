#if defined(__INTELLISENSE__)
    #include "Helpers.hlsl"
    #include "Pointlights.hlsl"
    #include "PBR.hlsl"
#else
    #include "includes/Pointlights.hlsl"
    #include "includes/PBR.hlsl"
#endif

float4 TESR_TerrainData : register(c32);
float4 TESR_TerrainExtraData : register(c33);
float4 TESR_DebugVar : register(c34);

float3 blendDiffuseMaps(float3 vertexColor, float2 uv, int texCount, sampler2D tex[7], float blends[7]) {
    float3 color = tex2D(tex[0], uv).xyz * blends[0];
    
    [unroll] for (int i = 1; i < texCount; i++) {
        color += tex2D(tex[i], uv).xyz * blends[i];
    }

    return color * vertexColor;
}

float3 blendNormalMaps(float2 uv, int texCount, sampler2D tex[7], float blends[7], out float roughness) {
    roughness = 1.f;
    
    float4 normal = tex2D(tex[0], uv);
    float3 combinedNormal = normal.xyz * blends[0];
    roughness = normal.w * blends[0];
    
    [unroll] for (int i = 1; i < texCount; i++){
        normal = tex2D(tex[i], uv);
        combinedNormal += normal.xyz * blends[i];
        roughness += normal.w * blends[i];
    }

    roughness = saturate((1 - roughness) * TESR_TerrainData.y);
    return normalize(expand(combinedNormal));
}

float3 getPointLightLighting(float3 lightDir, float radius, float3 lightColor, float3 eyeDir, float3 normal, float3 albedo, float roughness = 1.0, float metallicness = 1.0) {
    float3 pointlightColor = lightColor * TESR_TerrainData.z;

    if (TESR_TerrainExtraData.x){
        // PBR.
        float att = vanillaAtt(lightDir, radius);
        
        float3 lighting = PBR(saturate(metallicness * TESR_TerrainData.x), roughness, albedo, normal, eyeDir, lightDir, pointlightColor);
        
        return max(0, lighting * att);
    } else {
        // Vanilla.
        float att = vanillaAtt(lightDir, radius);
        
        lightDir = normalize(lightDir);
        
        float3 lighting = LambertianDiffuse(albedo, (0).xxx) * shades(normal, lightDir) * pointlightColor * PI;
        
        return max(0, lighting * att);
    }
}

float3 getSunLighting(float3 lightDir, float3 sunColor, float3 eyeDir, float3 normal, float3 AmbientColor, float3 albedo, float roughness = 1.0, float metallicness = 1.0, float parallaxMultiplier = 1.0) {
    float3 lightColor = sunColor * TESR_TerrainData.z * parallaxMultiplier;
    float3 ambientColor = AmbientColor * TESR_TerrainData.w;
    float3 color = albedo;
    color = lerp(luma(albedo), color, TESR_TerrainExtraData.z);

    if (TESR_TerrainExtraData.x) {
        // PBR.
        float3 lighting = PBRSun(saturate(TESR_TerrainData.x), roughness, color, normal, eyeDir, lightDir, lightColor);
        return max(0, lighting + ambientColor * color);
    } else {
        // Vanilla, no specular.
        float3 lighting = LambertianDiffuse(color, (0).xxx) * shades(normal, lightDir) * lightColor * PI;
        
        return max(0, lighting + ambientColor * color);
    }
}
