// Pointlight helper functions.

#if defined(__INTELLISENSE__)
    #include "Helpers.hlsl"
#endif

// Vanilla attenuation, lightVector not normalized.
float vanillaAtt(float3 lightVector, float radius) {
    const float3 att = lightVector / radius;
    return 1 - shades(att, att);
}

// Modified Frostbite attenuation (UE4), lightVector not normalized.
float frostbiteAtt(float3 lightVector, float radius) {
    const float squaredDistance = dot(lightVector, lightVector);
    const float invSqrRadius = 1.f / (radius * radius);
    const float factor = squaredDistance * invSqrRadius;
    const float smoothFactor = saturate(1.f - factor * factor);
    const float smoothDistanceAtt = smoothFactor * smoothFactor;
    
    return smoothDistanceAtt / (squaredDistance + 1);
}
