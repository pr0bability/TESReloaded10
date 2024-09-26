float4 TESR_TerrainParallaxData : register(c35);
float4 TESR_TerrainParallaxExtraData : register(c36);

// Complex Parallax Materials for Community Shaders
// https://www.artstation.com/blogs/andreariccardi/3VPo/a-new-approach-for-parallax-mapping-presenting-the-contact-refinement-parallax-mapping-technique
float getTerrainHeight(float2 coords, float2 dx, float2 dy, float blendFactor, int texCount, sampler2D tex[7], float blends[7], out float weights[7]) {
    weights = blends;

    float blendPower = blendFactor * 4;
    float total = 0;
    [unroll] for (int i = 0; i < texCount; i++){
        weights[i] = pow(abs(blends[i]), 1 + 1 * blendFactor);
        if (weights[i] > 0.0) {
            weights[i] *= 0.001 + pow(abs(tex2Dgrad(tex[i], coords, dx, dy).a), blendPower);
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

float2 getParallaxCoords(float distance, float2 coords, float2 dx, float2 dy, float3 viewDirTS, int texCount, sampler2D tex[7], float blends[7], out float weights[7]) {
    // Check if parallax is active first.
    if (!TESR_TerrainParallaxData.x) {
        weights = blends;
        return coords;
    }
    
    // Variables.
    bool highQuality = TESR_TerrainParallaxData.w;
    float maxDistance = TESR_TerrainParallaxExtraData.x;
    float height = TESR_TerrainParallaxExtraData.y;
    
    float distanceBlend = saturate(distance / maxDistance);
    float quality = saturate(1.0 - distanceBlend);
    float blendFactor = TESR_TerrainParallaxData.z ? quality : 0.25;

    viewDirTS = normalize(viewDirTS);

    // Fix for angles.
    viewDirTS.z = ((viewDirTS.z * 0.7) + 0.3);
    viewDirTS.xy /= viewDirTS.z;

    float maxHeight = height;
    float minHeight = maxHeight * 0.5;

    float2 output;
    if (distanceBlend < 1.0)
    {
        int numSteps;
        
        if (highQuality) {
            numSteps = lerp(4, 32, quality);
            numSteps = clamp((numSteps / 4) * 4, 4, 32);
        }
        else {
            numSteps = lerp(4, 16, quality);
            numSteps = clamp((numSteps / 4) * 4, 4, 16);
        }

        float stepSize = rcp((float) numSteps);

        float2 offsetPerStep = viewDirTS.xy * float2(maxHeight, maxHeight) * stepSize.xx;
        float2 prevOffset = viewDirTS.xy * float2(minHeight, minHeight) + coords.xy;

        float prevBound = 1.0;
        float prevHeight = 1.0;

        float2 pt1 = 0;
        float2 pt2 = 0;

        // Need fastopt otherwise compile times are crazy.
        [fastopt] while (numSteps > 0) {
            float4 currentOffset[2];
            currentOffset[0] = prevOffset.xyxy - float4(1, 1, 2, 2) * offsetPerStep.xyxy;
            currentOffset[1] = prevOffset.xyxy - float4(3, 3, 4, 4) * offsetPerStep.xyxy;
            float4 currentBound = prevBound.xxxx - float4(1, 2, 3, 4) * stepSize;

            float4 currHeight;
            
            currHeight.x = getTerrainHeight(currentOffset[0].xy, dx, dy, blendFactor, texCount, tex, blends, weights);
            currHeight.y = getTerrainHeight(currentOffset[0].zw, dx, dy, blendFactor, texCount, tex, blends, weights);
            currHeight.z = getTerrainHeight(currentOffset[1].xy, dx, dy, blendFactor, texCount, tex, blends, weights);
            currHeight.w = getTerrainHeight(currentOffset[1].zw, dx, dy, blendFactor, texCount, tex, blends, weights);

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
    
    weights = blends;
    return coords;
}

float getParallaxShadowMultipler(float distance, float2 coords, float2 dx, float2 dy, float3 lightTS, int texCount, float blends[7], sampler2D tex[7])
{
    if (!TESR_TerrainParallaxData.y)
        return 1.0;
    
    float maxDistance = TESR_TerrainParallaxExtraData.x;
    float shadowsIntensity = TESR_TerrainParallaxExtraData.z;
    
    float quality = 1.0 - distance / maxDistance;
    
    if (quality > 0.0)
    {
        float weights[7] = { 0, 0, 0, 0, 0, 0, 0 };
        float sh0 = getTerrainHeight(coords, dx, dy, quality, texCount, tex, blends, weights);

        const float2 rayDir = lightTS.xy * 0.025;
        float4 multipliers = rcp((float4(1, 2, 3, 4)));

        float4 sh = getTerrainHeight(coords + rayDir * multipliers.x, dx, dy, quality, texCount, tex, blends, weights);
        if (quality > 0.25)
            sh.y = getTerrainHeight(coords + rayDir * multipliers.y, dx, dy, quality, texCount, tex, blends, weights);
        if (quality > 0.5)
            sh.z = getTerrainHeight(coords + rayDir * multipliers.z, dx, dy, quality, texCount, tex, blends, weights);
        if (quality > 0.75)
            sh.w = getTerrainHeight(coords + rayDir * multipliers.w, dx, dy, quality, texCount, tex, blends, weights);
        
        return 1.0 - saturate(dot(max(0, sh - sh0), 1.0) * shadowsIntensity) * quality;
    }
    
    return 1.0;
}

