// Sampling functions.

#if defined(__INTELLISENSE__)
    #include "Helpers.hlsl"
#else
    #include "includes/Helpers.hlsl"
#endif

// Downsample with blur by averaging 13 samples around the texel by weighting their values.
//
// Take 13 samples around current texel (basically forming 5 separate boxes):
// a - b - c
// - j - k -
// d - e - f
// - l - m -
// g - h - i
// === ('e' is the current texel) ===
//
// Apply weighted distribution:
// 0.5 + 0.125 + 0.125 + 0.125 + 0.125 = 1
// a,b,d,e * 0.125
// b,c,e,f * 0.125
// d,e,g,h * 0.125
// e,f,h,i * 0.125
// j,k,l,m * 0.5
float4 DownsampleBox13(uniform sampler2D buffer, float2 uv, float2 texelSize) {
    const float x = texelSize.x;
    const float y = texelSize.y;

    float4 a = tex2D(buffer, float2(uv.x - 2 * x, uv.y + 2 * y));
    float4 b = tex2D(buffer, float2(uv.x, uv.y + 2 * y));
    float4 c = tex2D(buffer, float2(uv.x + 2 * x, uv.y + 2 * y));

    float4 d = tex2D(buffer, float2(uv.x - 2 * x, uv.y));
    float4 e = tex2D(buffer, float2(uv.x, uv.y));
    float4 f = tex2D(buffer, float2(uv.x + 2 * x, uv.y));

    float4 g = tex2D(buffer, float2(uv.x - 2 * x, uv.y - 2 * y));
    float4 h = tex2D(buffer, float2(uv.x, uv.y - 2 * y));
    float4 i = tex2D(buffer, float2(uv.x + 2 * x, uv.y - 2 * y));

    float4 j = tex2D(buffer, float2(uv.x - x, uv.y + y));
    float4 k = tex2D(buffer, float2(uv.x + x, uv.y + y));
    float4 l = tex2D(buffer, float2(uv.x - x, uv.y - y));
    float4 m = tex2D(buffer, float2(uv.x + x, uv.y - y));
    
    float2 weights = float2(0.125, 0.5);
    
    float4 boxes[5];
    
    boxes[0] = (a + b + d + e) * 0.25;
    boxes[1] = (b + c + e + f) * 0.25;
    boxes[2] = (d + e + g + h) * 0.25;
    boxes[3] = (e + f + h + i) * 0.25;
    boxes[4] = (j + k + l + m) * 0.25;
	
    return boxes[0] * weights[0] + boxes[1] * weights[0] + boxes[2] * weights[0] + boxes[3] * weights[0] + boxes[4] * weights[1];
}

// Upsample with tent filter.
//
// Take 9 samples around current texel:
// a - b - c
// d - e - f
// g - h - i
// === ('e' is the current texel) ===
//
// Apply weighted distribution, by using a 3x3 tent filter:
//  1   | 1 2 1 |
// -- * | 2 4 2 |
// 16   | 1 2 1 |
float4 UpsampleTent9(uniform sampler2D buffer, float2 uv, float2 filterRadius) {
	// The filter kernel is applied with a radius, specified in texture
    // coordinates, so that the radius will vary across mip resolutions.
    float x = filterRadius.x;
    float y = filterRadius.y;

    float4 a = tex2D(buffer, float2(uv.x - x, uv.y + y));
    float4 b = tex2D(buffer, float2(uv.x, uv.y + y));
    float4 c = tex2D(buffer, float2(uv.x + x, uv.y + y));

    float4 d = tex2D(buffer, float2(uv.x - x, uv.y));
    float4 e = tex2D(buffer, float2(uv.x, uv.y));
    float4 f = tex2D(buffer, float2(uv.x + x, uv.y));

    float4 g = tex2D(buffer, float2(uv.x - x, uv.y - y));
    float4 h = tex2D(buffer, float2(uv.x, uv.y - y));
    float4 i = tex2D(buffer, float2(uv.x + x, uv.y - y));

    float4 upsample = e * 4.0;
    upsample += (b + d + f + h) * 2.0;
    upsample += (a + c + g + i);
    upsample *= 1.0 / 16.0;
    
    return upsample;
}
