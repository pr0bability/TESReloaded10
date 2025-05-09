float4 TESR_ShadowBlur : register(c0);  // .x reciprocal resolution of shadow atlas, .y whether last cascade was updated
float4 BlurDirection  : register(c1);

sampler2D SourceBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

struct VSOUT
{
	float4 vertPos : POSITION;
	float2 UVCoord : TEXCOORD0;
};

float4 Blur3(float2 uv, float2 upperLeft, float2 lowerRight) {
    float weights[1] = { 0.5f };
    float offsets[1] = { 0.6f };
    
    float2 offset = BlurDirection * offsets[0] * TESR_ShadowBlur.x;

    float2 offsetUV = clamp(uv.xy + offset, upperLeft, lowerRight);
    float4 color = tex2Dlod(SourceBuffer, float4(offsetUV, 0.0f, 0.0f)) * weights[0];
    
    offsetUV = clamp(uv.xy - offset, upperLeft, lowerRight);
    color += tex2Dlod(SourceBuffer, float4(offsetUV, 0.0f, 0.0f)) * weights[0];

    return color;
}

float4 Blur5(float2 uv, float2 upperLeft, float2 lowerRight) {
    float weights[2] = { 0.2941176470f, 0.3529411764f };
    float offsets[2] = { 0.0f, 1.3333333333f };
    
    float4 color = tex2Dlod(SourceBuffer, float4(uv.xy, 0.0f, 0.0f)) * weights[0];
    
    float2 offset, offsetUV;
    for (int i = 1; i < 2; i++) {
        offset = BlurDirection * offsets[i] * TESR_ShadowBlur.x;
        
        offsetUV = clamp(uv.xy + offset, upperLeft, lowerRight);
        color += tex2Dlod(SourceBuffer, float4(offsetUV, 0.0f, 0.0f)) * weights[i];
        
        offsetUV = clamp(uv.xy - offset, upperLeft, lowerRight);
        color += tex2Dlod(SourceBuffer, float4(offsetUV, 0.0f, 0.0f)) * weights[i];
    }
    
    return color;
}


float4 Blur9(float2 uv, float2 upperLeft, float2 lowerRight) {
    float weights[3] = { 0.2270270270f, 0.3162162162f, 0.0702702703f };
    float offsets[3] = { 0.0f, 1.3846153846f, 3.2307692308f };
    
    float4 color = tex2Dlod(SourceBuffer, float4(uv.xy, 0.0f, 0.0f)) * weights[0];
    
    float2 offset, offsetUV;
    for (int i = 1; i < 3; i++) {
        offset = BlurDirection * offsets[i] * TESR_ShadowBlur.x;
        
        offsetUV = clamp(uv.xy + offset, upperLeft, lowerRight);
        color += tex2Dlod(SourceBuffer, float4(offsetUV, 0.0f, 0.0f)) * weights[i];
        
        offsetUV = clamp(uv.xy - offset, upperLeft, lowerRight);
        color += tex2Dlod(SourceBuffer, float4(offsetUV, 0.0f, 0.0f)) * weights[i];
    }
    
    return color;
}

// Gaussian blur using linear texture sampling.
//
// Weights calculated from the Pascal triangle, and recalculated as below.
//
// Sample in between texels.
// weight_l(t1, t2) = weight_d(t1) + weight_d(t2)
// offset_l(t1, t2) = (offset_d(t1) * weight_d(t1) + offset_d(t2) * weight_d(t2)) / weight_l(t1, t2)
//
// https://www.rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
float4 main(VSOUT IN) : COLOR0 {
	// Blur the different cascades non-uniformly. The closer the cascade, the bigger the blur.
    float2 uv = IN.UVCoord.xy;
    
    [branch] if (uv.x < 0.5 && uv.y < 0.5)
        return Blur9(uv, float2(0.0f, 0.0f), float2(0.5f, 0.5f)); // Closest cascade.
    else if (uv.y < 0.5)
        return Blur5(uv, float2(0.5f, 0.0f), float2(1.0f, 0.5f)); // Second cascade.
    else if (uv.x < 0.5)
        return Blur5(uv, float2(0.0f, 0.5f), float2(0.5f, 1.0f)); // Third cascade.
    else if (TESR_ShadowBlur.y)
        return Blur5(uv, float2(0.5f, 0.5f), float2(1.0f, 1.0f)); // Furthest cascade.
    else
        return tex2Dlod(SourceBuffer, float4(uv.xy, 0.0f, 0.0f)); // Furthest cascade was not updated, leave it be.

}
