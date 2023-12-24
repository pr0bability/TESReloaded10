
float3 GetSunColor(float sunHeight, float athmosphere, float dayTime, float3 sunColor, float3 sunsetColor ){
    sunColor = pows(sunColor, 2.2);
    sunsetColor = pows(sunsetColor, 2.2);

    float3 color = (1 + sunHeight) * sunColor; // increase suncolor strength with sun height
    float sunSet = saturate(pows(1 - sunHeight, 8.0)) * dayTime;
    color += sunsetColor.rgb * sunSet * athmosphere; // add extra red to the sun at sunsets
    return color;
}

float3 GetSkyColor(float verticality, float athmosphere, float sunHeight, float sunInfluence, float sunStrength, float3 skyColor, float3 skyLowColor, float3 horizonColor, float3 sunColor){
    skyColor = pows(skyColor, 2.2);
    skyLowColor = pows(skyLowColor, 2.2);
    horizonColor = pows(horizonColor, 2.2);

    float3 color = lerp(skyLowColor.rgb, skyColor.rgb, verticality); // fade from low sky to high sky with height
    color = lerp(color, horizonColor.rgb, saturate(athmosphere * (0.5 + sunInfluence * 0.5))); // fade from base color to horizon color in athmosphere (stronger on the sun side)
    color += sunColor * sunInfluence * (1.0 - sunHeight) * (((1.0 - verticality) + athmosphere)/2) * sunStrength;

    return color;
}