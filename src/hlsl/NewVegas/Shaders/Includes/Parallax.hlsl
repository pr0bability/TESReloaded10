#ifdef TERRAIN
float4 TESR_TerrainParallaxData : register(c91);
float4 TESR_TerrainParallaxExtraData : register(c92);
#else
float4 TESR_ParallaxData : register(c35);
#endif

#ifdef TERRAIN
// Complex Parallax Materials for Community Shaders
// https://www.artstation.com/blogs/andreariccardi/3VPo/a-new-approach-for-parallax-mapping-presenting-the-contact-refinement-parallax-mapping-technique
float getTerrainHeight(float2 coords, float2 dx, float2 dy, float blendFactor, int texCount, sampler2D tex[7], float blends[7], float status[7], out float weights[7]) {
    weights = blends;

    float blendPower = blendFactor * 4;
    float total = 0;
    [unroll] for (int i = 0; i < texCount; i++){
        weights[i] = pow(abs(blends[i]), 1 + 1 * blendFactor);
        if (weights[i] > 0.0) {
            weights[i] *= 0.001 + pow(abs(status[i] ? tex2Dgrad(tex[i], coords, dx, dy).a : 0.5f), blendPower);
        }
        total += weights[i];
    }
    
    float invtotal = rcp(total);
	
    [unroll] for (i = 0; i < texCount; i++)
    {
        weights[i] *= invtotal;
    }
    
    return pow(total, rcp(blendPower));
}
#endif

#ifdef TERRAIN
float2 getParallaxCoords(
    float distance, 
    float2 coords, 
    float2 dx, 
    float2 dy, 
    float3 viewDirTS, 
    int texCount, 
    sampler2D tex[7], 
    float blends[7], 
    float status[7], 
    out float weights[7]
) {
#else
float2 getParallaxCoords(float distance, float2 coords, float2 dx, float2 dy, float3 viewDirTS, sampler2D heightMap) {
#endif
    #ifdef TERRAIN
        // Check if parallax is active first.
        if (!TESR_TerrainParallaxData.x) {
            weights = blends;
            return coords;
        }
    
        // Variables.
        bool highQuality = TESR_TerrainParallaxData.w;
        float maxDistance = TESR_TerrainParallaxExtraData.x;
        float height = TESR_TerrainParallaxExtraData.y;
    #else
        bool highQuality = false;
        float maxDistance = 2048;
        float height = 0.1 * TESR_ParallaxData.x;
    #endif
    
    float distanceBlend = saturate(distance / maxDistance);
    float quality = saturate(1.0 - distanceBlend);
    
    #ifdef TERRAIN
        float blendFactor = TESR_TerrainParallaxData.z ? quality : 0.25;
    #endif

    viewDirTS = normalize(viewDirTS);

    #ifdef TERRAIN
        // Fix for angles.
        viewDirTS.z = ((viewDirTS.z * 0.7) + 0.3);
        viewDirTS.xy /= viewDirTS.z;
    #endif

    float maxHeight = height;
    float minHeight = maxHeight * 0.5;

    float2 output;
    if (distanceBlend < 1.0)
    {
        int numSteps;
        
        if (highQuality) {
            numSteps = lerp(4, 64, quality);
            numSteps = clamp((numSteps / 4) * 4, 4, 64);
        }
        else {
            numSteps = lerp(4, 32, quality);
            numSteps = clamp((numSteps / 4) * 4, 4, 32);
        }

        float stepSize = rcp((float) numSteps);

        float2 offsetPerStep = viewDirTS.xy * float2(maxHeight, maxHeight) * stepSize.xx;
        float2 prevOffset = viewDirTS.xy * float2(minHeight, minHeight) + coords.xy;

        float prevBound = 1.0;
        float prevHeight = 1.0;

        float2 pt1 = 0;
        float2 pt2 = 0;

        // Need fastopt otherwise compile times are crazy.
        [loop][fastopt] while (numSteps > 0) {
            float4 currentOffset[2];
            currentOffset[0] = prevOffset.xyxy - float4(1, 1, 2, 2) * offsetPerStep.xyxy;
            currentOffset[1] = prevOffset.xyxy - float4(3, 3, 4, 4) * offsetPerStep.xyxy;
            float4 currentBound = prevBound.xxxx - float4(1, 2, 3, 4) * stepSize;

            float4 currHeight;
            
            #ifdef TERRAIN
                currHeight.x = getTerrainHeight(currentOffset[0].xy, dx, dy, blendFactor, texCount, tex, blends, status, weights);
                currHeight.y = getTerrainHeight(currentOffset[0].zw, dx, dy, blendFactor, texCount, tex, blends, status, weights);
                currHeight.z = getTerrainHeight(currentOffset[1].xy, dx, dy, blendFactor, texCount, tex, blends, status, weights);
                currHeight.w = getTerrainHeight(currentOffset[1].zw, dx, dy, blendFactor, texCount, tex, blends, status, weights);
            #else
                currHeight.x = tex2Dgrad(heightMap, currentOffset[0].xy, dx, dy).r;
                currHeight.y = tex2Dgrad(heightMap, currentOffset[0].zw, dx, dy).r;
                currHeight.z = tex2Dgrad(heightMap, currentOffset[1].xy, dx, dy).r;
                currHeight.w = tex2Dgrad(heightMap, currentOffset[1].zw, dx, dy).r;
            #endif

            bool4 testResult = currHeight >= currentBound;

            [branch] if (any(testResult))
            {
                [flatten] if (testResult.w)
                {
                    pt1 = float2(currentBound.w, currHeight.w);
                    pt2 = float2(currentBound.z, currHeight.z);
                }
                [flatten] if (testResult.z)
                {
                    pt1 = float2(currentBound.z, currHeight.z);
                    pt2 = float2(currentBound.y, currHeight.y);
                }
                [flatten] if (testResult.y)
                {
                    pt1 = float2(currentBound.y, currHeight.y);
                    pt2 = float2(currentBound.x, currHeight.x);
                }
                [flatten] if (testResult.x)
                {
                    pt1 = float2(currentBound.x, currHeight.x);
                    pt2 = float2(prevBound, prevHeight);
                }
                break;
            }

            prevOffset = currentOffset[1].zw;
            prevBound = currentBound.w;
            prevHeight = currHeight.w;
            numSteps -= 4;
        }
        
        float delta2 = pt2.x - pt2.y;
        float delta1 = pt1.x - pt1.y;

        float denominator = delta2 - delta1;

        float parallaxAmount = 0.0;
        if (denominator == 0.0)
        {
            parallaxAmount = 0.0;
        }
        else
        {
            parallaxAmount = (pt1.x * delta2 - pt2.x * delta1) / denominator;
        }
        
        distanceBlend *= distanceBlend;
        
        float offset = (1.0 - parallaxAmount) * -maxHeight + minHeight;
        return lerp(viewDirTS.xy * offset + coords.xy, coords, distanceBlend);
    }
    
    #ifdef TERRAIN
        weights = blends;
    #endif
    return coords;
}

#ifdef TERRAIN
float getParallaxShadowMultipler(float distance, float2 coords, float2 dx, float2 dy, float3 lightTS, int texCount, float blends[7], float status[7], sampler2D tex[7]) {
    if (!TESR_TerrainParallaxData.y)
        return 1.0;
    
    float maxDistance = TESR_TerrainParallaxExtraData.x;
    float shadowsIntensity = TESR_TerrainParallaxExtraData.z;
    
    float quality = 1.0 - distance / maxDistance;
    
    if (quality > 0.0)
    {
        float weights[7] = { 0, 0, 0, 0, 0, 0, 0 };
        float sh0 = getTerrainHeight(coords, dx, dy, quality, texCount, tex, blends, status, weights);

        const float2 rayDir = lightTS.xy * 0.1;
        float4 multipliers = rcp((float4(1, 2, 3, 4)));

        float4 sh = getTerrainHeight(coords + rayDir * multipliers.x, dx, dy, quality, texCount, tex, blends, status, weights);
        if (quality > 0.25)
            sh.y = getTerrainHeight(coords + rayDir * multipliers.y, dx, dy, quality, texCount, tex, blends, status, weights);
        if (quality > 0.5)
            sh.z = getTerrainHeight(coords + rayDir * multipliers.z, dx, dy, quality, texCount, tex, blends, status, weights);
        if (quality > 0.75)
            sh.w = getTerrainHeight(coords + rayDir * multipliers.w, dx, dy, quality, texCount, tex, blends, status, weights);
        
        return 1.0 - saturate(dot(max(0, sh - sh0), 1.0) * shadowsIntensity) * quality;
    }
    
    return 1.0;
}
#else
float getParallaxShadowMultipler(float distance, float2 coords, float2 dx, float2 dy, float3 lightTS, sampler2D heightMap) {
    float maxDistance = 2048;
    float shadowsIntensity = 2;
    
    float quality = 1.0 - distance / maxDistance;
    
    if (quality > 0.0)
    {
        float sh0 = tex2Dgrad(heightMap, coords, dx, dy).r;

        const float2 rayDir = lightTS.xy * 0.04;
        float4 multipliers = rcp((float4(1, 2, 3, 4)));

        float4 sh = tex2Dgrad(heightMap, coords + rayDir * multipliers.x, dx, dy).r;
        if (quality > 0.25)
            sh.y = tex2Dgrad(heightMap, coords + rayDir * multipliers.y, dx, dy).r;
        if (quality > 0.5)
            sh.z = tex2Dgrad(heightMap, coords + rayDir * multipliers.z, dx, dy).r;
        if (quality > 0.75)
            sh.w = tex2Dgrad(heightMap, coords + rayDir * multipliers.w, dx, dy).r;
        
        return 1.0 - saturate(dot(max(0, sh - sh0), 1.0) * shadowsIntensity) * quality;
    }
    
    return 1.0;
}
#endif
