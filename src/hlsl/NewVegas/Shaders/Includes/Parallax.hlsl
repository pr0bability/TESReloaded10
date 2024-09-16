float4 TESR_TerrainParallaxData : register(c35);
float4 TESR_TerrainParallaxExtraData : register(c36);

// Complex Parallax Materials for Community Shaders
// https://www.artstation.com/blogs/andreariccardi/3VPo/a-new-approach-for-parallax-mapping-presenting-the-contact-refinement-parallax-mapping-technique
float2 getParallaxCoords(float distance, float2 coords, float2 dx, float2 dy, float3 viewDirTS, sampler2D tex, float blend) {
    // Check if parallax is active first.
    if (!TESR_TerrainParallaxData.x)
        return coords;
    
    // Variables.
    float v_maxDistance = TESR_TerrainParallaxExtraData.x;
    float v_range = TESR_TerrainParallaxExtraData.y;
    float v_blendRange = TESR_TerrainParallaxExtraData.z;
    float v_height = TESR_TerrainParallaxData.w;
    bool v_highQuality = TESR_TerrainParallaxData.y;

    viewDirTS = normalize(viewDirTS);
    distance /= v_maxDistance;

    viewDirTS.z = ((viewDirTS.z * 0.5) + 0.5);
    viewDirTS.xy /= viewDirTS.z;

    float nearQuality = smoothstep(0.0, v_range, distance);
    float nearBlendToMid = smoothstep(v_range - v_blendRange, v_range, distance);
    float midBlendToFar = smoothstep(1.0 - v_blendRange, 1.0, distance);

    float maxHeight = v_height * 0.1;
    float minHeight = maxHeight * 0.5;

    float2 output;
    if (nearBlendToMid < 1.0) {
        int numSteps;

        float quality = 1.0 - nearQuality;
        if (v_highQuality) {
            numSteps = max(1, round(64 * quality));
            numSteps = clamp(((numSteps + 3) / 4) * 4, 4, 64);
        } else {
            numSteps = max(1, round(32 * quality));
            numSteps = clamp(((numSteps + 3) / 4) * 4, 4, 32);
        }

        float stepSize = 1.0 / ((float)numSteps + 1.0);

        float2 offsetPerStep = viewDirTS.xy * float2(maxHeight, maxHeight) * stepSize.xx;
        float2 prevOffset = viewDirTS.xy * float2(minHeight, minHeight) + coords.xy;

        float prevBound = 1.0;
        float prevHeight = 1.0;

        int numStepsTemp = numSteps;

        float2 pt1 = 0;
        float2 pt2 = 0;

        bool contactRefinement = false;

        // Need fastopt otherwise compile times are crazy.
        [fastopt] while (numSteps > 0) {
            float4 currentOffset[2];
            currentOffset[0] = prevOffset.xyxy - float4(1, 1, 2, 2) * offsetPerStep.xyxy;
            currentOffset[1] = prevOffset.xyxy - float4(3, 3, 4, 4) * offsetPerStep.xyxy;
            float4 currentBound = prevBound.xxxx - float4(1, 2, 3, 4) * stepSize;

            float4 currHeight;
            currHeight.x = tex2Dgrad(tex, currentOffset[0].xy, dx, dy).a;
            currHeight.y = tex2Dgrad(tex, currentOffset[0].zw, dx, dy).a;
            currHeight.z = tex2Dgrad(tex, currentOffset[1].xy, dx, dy).a;
            currHeight.w = tex2Dgrad(tex, currentOffset[1].zw, dx, dy).a;

            bool4 testResult = currHeight >= currentBound;
            [branch] if (any(testResult))
            {
                float2 outOffset = 0;
                [flatten] if (testResult.w)
                {
                    outOffset = currentOffset[1].xy;
                    pt1 = float2(currentBound.w, currHeight.w);
                    pt2 = float2(currentBound.z, currHeight.z);
                }
                [flatten] if (testResult.z)
                {
                    outOffset = currentOffset[0].zw;
                    pt1 = float2(currentBound.z, currHeight.z);
                    pt2 = float2(currentBound.y, currHeight.y);
                }
                [flatten] if (testResult.y)
                {
                    outOffset = currentOffset[0].xy;
                    pt1 = float2(currentBound.y, currHeight.y);
                    pt2 = float2(currentBound.x, currHeight.x);
                }
                [flatten] if (testResult.x)
                {
                    outOffset = prevOffset;

                    pt1 = float2(currentBound.x, currHeight.x);
                    pt2 = float2(prevBound, prevHeight);
                }

                if (contactRefinement) {
                    break;
                } else {
                    contactRefinement = true;
                    prevOffset = outOffset;
                    prevBound = pt2.x;
                    numSteps = numStepsTemp;
                    stepSize /= (float)numSteps;
                    offsetPerStep /= (float)numSteps;
                    continue;
                }
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
        if (denominator == 0.0) {
            parallaxAmount = 0.0;
        } else {
            parallaxAmount = (pt1.x * delta2 - pt2.x * delta1) / denominator;
        }

        if (v_highQuality) {
            float delta2 = pt2.x - pt2.y;
            float delta1 = pt1.x - pt1.y;

            float denominator = delta2 - delta1;

            float parallaxAmount = 0.0;
            if (denominator == 0.0) {
                parallaxAmount = 0.0;
            } else {
                parallaxAmount = (pt1.x * delta2 - pt2.x * delta1) / denominator;
            }

            float offset = (1.0 - parallaxAmount) * -maxHeight + minHeight;
            output = viewDirTS.xy * offset + coords.xy;
        } else {
            float offset = (1.0 - pt1.x) * -maxHeight + minHeight;
            output = viewDirTS.xy * offset + coords.xy;
        }

        if (nearBlendToMid > 0.0) {
            float height = tex2D(tex, coords.xy).a;
            height = height * maxHeight - minHeight;
            output = lerp(output, viewDirTS.xy * height.xx + coords.xy, nearBlendToMid);
        }
    } else if (midBlendToFar < 1.0) {
        float height = tex2D(tex, coords.xy).a;
        if (midBlendToFar > 0.0) {
            maxHeight *= (1 - midBlendToFar);
            minHeight *= (1 - midBlendToFar);
        }
        height = height * maxHeight - minHeight;
        output = viewDirTS.xy * height.xx + coords.xy;
    } else {
        output = coords;
    }

    return output;
}

float getParallaxShadowMultipler(float distance, float2 dx, float2 dy, float3 lightTS, int tex_count, float blends[7], float2 coords[7], sampler2D tex[7]) {
    if (!TESR_TerrainParallaxData.z)
        return 1.0;
    
    float maxDistance = TESR_TerrainParallaxExtraData.w;
    
    float quality = 1.0 - smoothstep(maxDistance / 2, maxDistance, distance);
    
    float multiplier = 1.0;
    if (quality > 0.0) {
        lightTS = normalize(lightTS);
        lightTS.z = ((lightTS.z * 0.5) + 0.5);
        lightTS.xy /= lightTS.z;

        float sh0 = 0;
        [unroll] for (int i = 0; i < tex_count; ++i)
        {
            sh0 += tex2Dgrad(tex[i], coords[i], dx, dy).a * blends[i];
        }
        sh0 = saturate(sh0);

        const float height = 0.025;
        const float2 rayDir = lightTS.xy * height;

        const float h0 = 1.0 - sh0;

        float sh = 0;
        [unroll] for (i = 0; i < tex_count; ++i)
        {
            sh += tex2Dgrad(tex[i], coords[i] + rayDir, dx, dy).a * blends[i];
        }
        sh = saturate(sh);
        
        const float h = 1.0 - sh;

        // Compare the difference between the two heights to see if the height blocks it.
        multiplier = 1.0 - saturate((h0 - h) * 7.0);
        
        // Fade out with distance.
        multiplier = lerp(1.0, multiplier, quality);
    }
    
    return multiplier;
}
