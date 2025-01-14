float4 TESR_ReciprocalResolution  : register(c0);
float4 BlurDirection  : register(c1);
sampler2D SourceBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

struct VSOUT
{
	float4 vertPos : POSITION;
	float2 UVCoord : TEXCOORD0;
};

float4 Blur3(float2 uv) {
    float weights[1] = { 0.5f };
    float offsets[1] = { 0.6f };
    
    float2 offset = BlurDirection * offsets[0] * TESR_ReciprocalResolution.x;

    float4 color = tex2Dlod(SourceBuffer, float4(uv.xy + offset, 0.0f, 0.0f)) * weights[0];
    color += tex2Dlod(SourceBuffer, float4(uv.xy - offset, 0.0f, 0.0f)) * weights[0];

    return color;
}

float4 Blur5(float2 uv) {
    float weights[2] = { 0.2941176470f, 0.3529411764f };
    float offsets[2] = { 0.0f, 1.3333333333f };
    
    float4 color = tex2Dlod(SourceBuffer, float4(uv.xy, 0.0f, 0.0f)) * weights[0];
    
    float2 offset;
    for (int i = 1; i < 2; i++) {
        offset = BlurDirection * offsets[i] * TESR_ReciprocalResolution.x;
        
        color += tex2Dlod(SourceBuffer, float4(uv.xy + offset, 0.0f, 0.0f)) * weights[i];
        color += tex2Dlod(SourceBuffer, float4(uv.xy - offset, 0.0f, 0.0f)) * weights[i];
    }
    
    return color;
}


float4 Blur9(float2 uv) {
    float weights[3] = { 0.2270270270f, 0.3162162162f, 0.0702702703f };
    float offsets[3] = { 0.0f, 1.3846153846f, 3.2307692308f };
    
    float4 color = tex2Dlod(SourceBuffer, float4(uv.xy, 0.0f, 0.0f)) * weights[0];
    
    float2 offset;
    for (int i = 1; i < 3; i++) {
        offset = BlurDirection * offsets[i] * TESR_ReciprocalResolution.x;
        
        color += tex2Dlod(SourceBuffer, float4(uv.xy + offset, 0.0f, 0.0f)) * weights[i];
        color += tex2Dlod(SourceBuffer, float4(uv.xy - offset, 0.0f, 0.0f)) * weights[i];
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
        return Blur9(uv);  // Closest cascade.
    else if (uv.y < 0.5)
        return Blur5(uv);  // Second cascade.
    else if (uv.x < 0.5)
        return Blur3(uv);  // Third cascade.
    else
        return Blur3(uv);  // Furthest cascade.
}
