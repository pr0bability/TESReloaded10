// ACES tonemapping https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
float3 ACESFilm(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return (x * (a * x + b)) / (x * (c * x + d) + e);
}

// -----------------------------------------------------------------------------------------------
// The code in this file was originally written by Stephen Hill (@self_shadow), who deserves all
// credit for coming up with this fit and implementing it. Buy him a beer next time you see him. :)

// sRGB => XYZ => D65_2_D60 => AP1 => RRT_SAT
static const float3x3 ACESInputMat =
{
    { 0.59719, 0.35458, 0.04823 },
    { 0.07600, 0.90834, 0.01566 },
    { 0.02840, 0.13383, 0.83777 }
};

// ODT_SAT => XYZ => D60_2_D65 => sRGB
static const float3x3 ACESOutputMat =
{
    { 1.60475, -0.53108, -0.07367 },
    { -0.10208, 1.10813, -0.00605 },
    { -0.00327, -0.07276, 1.07602 }
};

float3 RRTAndODTFit(float3 v)
{
    float3 a = v * (v + 0.0245786f) - 0.000090537f;
    float3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

// ACES Filmic (Fitted)
float3 ACESFitted(float3 color)
{
    color = mul(ACESInputMat, color);

    // Apply RRT and ODT
    color = RRTAndODTFit(color);

    color = mul(ACESOutputMat, color);

    // Clamp to [0, 1]
    color = saturate(color);

    return color;
}
// -----------------------------------------------------------------------------------------------

// Reinhard-Jodie (color+luminance)
float3 ReinhardJodie(float3 v)
{
    float l = luma(v);
    float3 tv = v / (1.0f + v);
    return lerp(v / (1.0f + l), tv, tv);
}

// Reinhard (luminance tonemap)
float3 change_luminance(float3 c_in, float l_out)
{
    float l_in = luma(c_in);
    return c_in * (l_out / l_in);
}
float3 ReinhardExtended(float3 v, float whitepoint)
{
    whitepoint = max(1.0, whitepoint * 3.0);
    float l_old = luma(v);
    float numerator = l_old * (1.0f + (l_old / (whitepoint * whitepoint)));
    float l_new = numerator / (1.0f + l_old);
    return change_luminance(v, l_new);
}

// Uncharted 2 tonemapper
float3 uncharted2_tonemap_partial(float3 x)
{
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.01;
    float F = 0.30;
    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}
float3 Uncharted2Tonemap(float3 v, float exposure_bias, float whitepoint)
{
    exposure_bias = max(0.1, exposure_bias * 1.4);
    float3 curr = uncharted2_tonemap_partial(v * exposure_bias);
    
    whitepoint = max(1.0, whitepoint * 3.0);
    float3 white_scale = float3(1.0f, 1.0f, 1.0f) / uncharted2_tonemap_partial(whitepoint);
    return curr * white_scale;
}

//AGX
//https://iolite-engine.com/blog_posts/minimal_agx_implementation
static const float3x3 agx_mat =
{
    { 0.842479062253094, 0.0423282422610123, 0.0423756549057051 },
    { 0.0784335999999992, 0.878468636469772, 0.0784336 },
    { 0.0792237451477643, 0.0791661274605434, 0.879142973793104 }
};
static const float3x3 agx_mat_inv =
{
    { 1.19687900512017, -0.0528968517574562, -0.0529716355144438 },
    { -0.0980208811401368, 1.15190312990417, -0.0980434501171241 },
    { -0.0990297440797205, -0.0989611768448433, 1.15107367264116 }
};

// Mean error^2: 3.6705141e-06
float3 agxDefaultContrastApprox(float3 x)
{
    float3 x2 = x * x;
    float3 x4 = x2 * x2;
  
    return +15.5 * x4 * x2
         - 40.14 * x4 * x
         + 31.96 * x4
         - 6.868 * x2 * x
         + 0.4298 * x2
         + 0.1191 * x
         - 0.00232;
}

float3 agxEotf(float3 val)
{
    
  // Inverse input transform (outset)
    val = mul(agx_mat_inv, val);
  
  // sRGB IEC 61966-2-1 2.2 Exponent Reference EOTF Display
  // NOTE: We're linearizing the output here. Comment/adjust when
  // *not* using a sRGB render target
    val = pows(val, 2.2); //linearise

    return val;
}

// 0: Default, 1: Golden, 2: Punchy
//#define AGX_LOOK 0
float3 agxLook(float3 val)
{
    float3 luma_val = luma(val);
  
  // Default
    float3 offset = 0.0;
    float3 slope = 1.0;
    float power = 1.0;
    float sat = 1.0;
 
//#if AGX_LOOK == 1
//  // Golden
//  slope = float3(1.0, 0.9, 0.5);
//  power = 0.8;
//  sat = 0.8;
//#elif AGX_LOOK == 2
//  // Punchy
//  slope = 1.0;
//  power = 1.35;
//  sat = 1.4;
//#endif
  
  // ASC CDL
    val = pows(val * slope + offset, power);
    return luma_val + sat * (val - luma_val);
}

float3 agx(float3 val)
{
    //val = pows(val, 1.0 / 2.2);
    
    float min_ev = -12.47393f;
    float max_ev = 4.026069f;

  // Input transform (inset)
    val = mul(agx_mat, val);
  
  // Log2 space encoding
    val = clamp(log2(val), min_ev, max_ev);
    val = (val - min_ev) / (max_ev - min_ev);
  
  // Apply sigmoid function approximation
    val = agxDefaultContrastApprox(val);
    
    val = agxLook(val);
    val = agxEotf(val);

    return val;
}

// https://gist.github.com/KelSolaar/1213139203911a72fef531c32c3d4ec2
// https://gist.github.com/vtastek/935be12fb8d87adda751b5276fd88f0c
// Lottes (2016) adapted by vtastek, optimised as the default tonemapper

#define CMAX 1.6e+6f
#define EPS 1e-6f
// Code:
float3 VTLottes(float3 color, float contrast, float b, float c, float shoulder, float crosstalk)
{
    float3 peak = max(color.r, max(color.g, color.b));
    peak = min(CMAX, max(EPS, peak));

    float3 ratio = min(CMAX, color / peak);

    float lum = dot(color, float3(0.5, 0.4, 0.33));
    float gray = min(color.r, min(color.g, color.b));
	gray = max(0.0, gray);
    peak += min(peak, gray);
    peak *= 0.5;
    peak *= 1.0 + 1.666 * max(0, (peak - lum) / peak);

    float3 z = pows(peak, contrast);
    peak = z / (pows(z, shoulder) * b + c);

    crosstalk = max(1.0,crosstalk); // controls amount of channel crosstalk
    float saturation = contrast; // crosstalk saturation
    float crossSaturation = contrast * (64.0 / crosstalk); // crosstalk saturation
    // wrap crosstalk in transform
    ratio = pows(abs(ratio + 0.11) * 0.90909, saturation / crossSaturation);
    ratio = lerp(ratio, 1.0, pows(peak, max(float3(1.0, 1.0, 1.0), float3(4.0, 3.0, 3.0) * crosstalk) * 1.0 / peak));
    ratio = pows(min(1.0, ratio), crossSaturation);

    return peak * ratio;
}

float ColToneB(float hdrMax, float contrast, float shoulder, float midIn, float midOut) 
{
    return
        -((-pow(midIn, contrast) + (midOut*(pow(hdrMax, contrast*shoulder)*pow(midIn, contrast) -
            pow(hdrMax, contrast)*pow(midIn, contrast*shoulder)*midOut)) /
            (pow(hdrMax, contrast*shoulder)*midOut - pow(midIn, contrast*shoulder)*midOut)) /
            (pow(midIn, contrast*shoulder)*midOut));
}

// General tonemapping operator, build 'c' term.
float ColToneC(float hdrMax, float contrast, float shoulder, float midIn, float midOut) 
{
    return (pow(hdrMax, contrast*shoulder)*pow(midIn, contrast) - pow(hdrMax, contrast)*pow(midIn, contrast*shoulder)*midOut) /
        (pow(hdrMax, contrast*shoulder)*midOut - pow(midIn, contrast*shoulder)*midOut);
}
float ColTone(float x, float4 p) 
{ 
    float z = pow(x, p.x); 
    return z / (pow(z, p.y)*p.z + p.w); 
}

// https://gpuopen.com/wp-content/uploads/2016/03/GdcVdrLottes.pdf
float3 Lottes(float3 color, float contrast, float midOut, float midIn, float hdrMax, float shoulder)
{
    hdrMax = max(1.0, hdrMax * 50.0);
    contrast = max(0.01, contrast * 1.25);
    shoulder = saturate(shoulder * 0.993); // shoulder should not! exceed 1.0
    midIn = max(0.01, midIn * 0.18);
    midOut = max(0.01, midOut * 0.1818);
    
    float b = ColToneB(hdrMax, contrast, shoulder, midIn, midOut);
    float c = ColToneC(hdrMax, contrast, shoulder, midIn, midOut);

    float3 peak = max(color.r, max(color.g, color.b));
    peak = min(CMAX, max(EPS, peak));

    float3 ratio = min(CMAX, color / peak);

    float lum = dot(color, float3(0.5, 0.4, 0.33));
    float gray = min(color.r, min(color.g, color.b));
	gray = max(0.0, gray);
    peak += min(peak, gray);
    peak *= 0.5;
    peak *= 1.0 + 1.666 * max(0, (peak - lum) / peak);

    peak = ColTone(peak, float4(contrast, shoulder, b, c) );

    float saturation = contrast; // crosstalk saturation
    float crossSaturation = contrast * 64.0; // crosstalk saturation

    float3 rgb = 1.0;

    // wrap crosstalk in transform
    ratio = pows(abs(ratio + 0.11) * 0.90909, saturation / crossSaturation);
    ratio = lerp(ratio, rgb, pow(peak, float3(4.0, 1.5, 1.5) * 1.0/peak));
    ratio = pows(min(1.0, ratio), crossSaturation);

    return peak * ratio;
}

float UchimuraChannel(float x, float P, float a, float m, float l, float c, float b)
{
    float l0 = ((P - m) * l) / a;
    float S0 = m + l0;
    float S1 = m + a * l0;
    float C2 = (a * P) / (P - S1);
    
    float L = m + a * (x - m);
    float T = m * pow(x / m, c);
    float S = P - (P - S1) * exp(-C2 * (x - S0) / P);
    float w0 = 1 - smoothstep(0.0f, m, x);
    float w2 = (x < m + l0) ? 0 : 1;
    float w1 = 1 - w0 - w2;
    return (float) (T * w0 + L * w1 + S * w2);

}

// Uchimura GT Tonemapper
float3 Uchimura(float3 x, float contrast, float brightness, float midIn, float hdrMax, float shoulder)
{
    float P = 1.0 * brightness; // brightness
    float a = 1.0 * contrast; // contrast
    float m = 0.22 * midIn; // linear section start
    float l = 0.4 * hdrMax; // linear section length
    float c = 1.0 + (0.333 * shoulder); // black
    float b = 0.0; // pedestal
    
    return float3(UchimuraChannel(x.r, P, a, m, l, c, b), UchimuraChannel(x.g, P, a, m, l, c, b), UchimuraChannel(x.b, P, a, m, l, c, b));
}

float rangeCompressPow(float x, float fPow /*= 1.0f*/)
{
    return 1.0 - pow(exp(-x), fPow);
}

float lumaCompress(float val, float fMaxValue, float fShoulderStart, float fPow /*= 1.0f*/)
{
    float v2 = fShoulderStart + (fMaxValue - fShoulderStart) * rangeCompressPow((val - fShoulderStart) / (fMaxValue - fShoulderStart), fPow);
    return val <= fShoulderStart ? val : v2;
}

// DICE Tonemapper
float3 DICE(float3 color, float contrast, float brightness, float hdrMax, float shoulder)
{
    float HIGHLIGHTS_SHOULDER_START_ALPHA = shoulder * 0.01;
    float HIGHLIGHTS_SHOULDER_POW = contrast;
    float sRGB_max_nits = brightness;

    float HDRLuminance = luma(color);
    float maxOutputLuminance = hdrMax / sRGB_max_nits;
    float highlightsShoulderStart = HIGHLIGHTS_SHOULDER_START_ALPHA * maxOutputLuminance;
    float compressedHDRLuminance = lumaCompress(HDRLuminance, maxOutputLuminance, highlightsShoulderStart, HIGHLIGHTS_SHOULDER_POW);
    
    return color * (compressedHDRLuminance / HDRLuminance);
}


float3 tonemap(float3 color)
{
    color = max(0.0, color);
    if (TESR_HDRData.x == 0)
    {
        return color;
    }
    else if (TESR_HDRData.x == 1)
    {
        return max(0.0, VTLottes(min(CMAX, color), TESR_LotteData.x, TESR_LotteData.z, TESR_LotteData.y, TESR_LotteData.w, TESR_ToneMapping.x));
    }
    else if (TESR_HDRData.x == 2)
    {
        return max(0.0, Lottes(min(CMAX, color), TESR_LotteData.x, TESR_LotteData.y, TESR_LotteData.z, TESR_HDRBloomData.w, TESR_LotteData.w));
    }
    else if (TESR_HDRData.x == 3)
    {
        return ReinhardExtended(color, TESR_HDRBloomData.w);
    }
    else if (TESR_HDRData.x == 4)
    {
        return ReinhardJodie(color);
    }
    else if (TESR_HDRData.x == 5)
    {
        return ACESFilm(color);
    }
    else if (TESR_HDRData.x == 6)
    {
        return ACESFitted(color);
    }
    else if (TESR_HDRData.x == 7)
    {
        return Uncharted2Tonemap(color, TESR_LotteData.y, TESR_HDRBloomData.w);
    }
    else if (TESR_HDRData.x == 8)
    {
        return max(0.0, Uchimura(min(CMAX, color), TESR_LotteData.x, TESR_LotteData.y, TESR_LotteData.z, TESR_HDRBloomData.w, TESR_LotteData.w));
    }
    else if (TESR_HDRData.x == 9)
    {
        return agx(color);
    }
    else if (TESR_HDRData.x == 10)
    {
        return max(0.0, DICE(min(CMAX, color), TESR_LotteData.x, TESR_LotteData.y, TESR_HDRBloomData.w, TESR_LotteData.w));
    }
    else
    {
        return color;
    }
}