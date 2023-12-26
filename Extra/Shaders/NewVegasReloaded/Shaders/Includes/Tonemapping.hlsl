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
    val = pows(val, 2.2);

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
// Lottes (2016) adapted by vtastek

/*
float ColToneB(float hdrMax, float contrast, float shoulder, float midIn, float midOut)
{
    return
        -((-pows(midIn, contrast) + (midOut * (pows(hdrMax, contrast * shoulder) * pows(midIn, contrast) -
            pows(hdrMax, contrast) * pows(midIn, contrast * shoulder) * midOut)) /
            (pows(hdrMax, contrast * shoulder) * midOut - pows(midIn, contrast * shoulder) * midOut)) /
            (pows(midIn, contrast * shoulder) * midOut));
}

// General tonemapping operator, build 'c' term.
float ColToneC(float hdrMax, float contrast, float shoulder, float midIn, float midOut)
{
    return (pows(hdrMax, contrast * shoulder) * pows(midIn, contrast) - pows(hdrMax, contrast) * pows(midIn, contrast * shoulder) * midOut) /
           (pows(hdrMax, contrast * shoulder) * midOut - pows(midIn, contrast * shoulder) * midOut);
}

// General tonemapping operator, p := {contrast,shoulder,b,c}.
float ColTone(float3 x, float4 p)
{
    float3 z = pows(x, p.r);
    return z / (pows(z, p.g) * p.b + p.a);
}
#define CMAX 1.6e+6f
#define EPS 1e-6f
float3 VTLottes(float3 color, float contrast, float midOut, float midIn, float hdrMax, float shoulder, float crossTalk)
{
    hdrMax = max(1.0, hdrMax * 100.0);
    contrast = max(0.01, contrast * 1.35);
    shoulder = saturate(shoulder * 0.993); // shoulder should not! exceed 1.0
    midIn = max(0.01, midIn * 0.18);
    midOut = max(0.01, midOut * 0.18);
    //static const float hdrMax = 100.0; // How much HDR range before clipping. HDR modes likely need this pushed up to say 25.0.
   // float contrast = 1.7; // Use as a baseline to tune the amount of contrast the tonemapper has.
    //static const float shoulder = 0.993; // Likely don?t need to mess with this factor, unless matching existing tonemapper is not working well..
    //static const float midIn = 0.18; // most games will have a {0.0 to 1.0} range for LDR so midIn should be 0.18.
   // float midOut = 0.1818; // 0.23/(-0.760 + 2.896*contrast - 1.980*contrast*contrast + 0.673*contrast*contrast*contrast); // Use for LDR. For HDR10 10:10:10:2 use maybe 0.18/25.0 to start. For scRGB, I forget what a good starting point is, need to re-calculate.
	
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

    //float crosstalk = max(1.0,crossTalk * 4.0); // controls amount of channel crosstalk
    float saturation = contrast; // full tonal range saturation control
    float crossSaturation = contrast * 64.0; // crosstalk saturation
    //float crossSaturation = contrast * (64.0 / crosstalk); // crosstalk saturation

    float3 rgb = 1.0;

    // wrap crosstalk in transform
    ratio = pow(abs(ratio + 0.11) * 0.90909, saturation / crossSaturation);
    ratio = lerp(ratio, rgb, pow(peak, float3(4.0, 1.5, 1.5) * 1.0/peak));
    //ratio = lerp(ratio, rgb, pows(peak, crosstalk));
    ratio = pow(min(1.0, ratio), crossSaturation);

    // then apply ratio to peak
    float3 lincol = color;
    color = peak * ratio;
    return color;
}*/

// Code:
float3 AMDLottes(float3 color, float contrast, float b, float c, float shoulder, float crosstalk)
{
    float3 peak = max(color.r, max(color.g, color.b));
    peak = min(1.6e+6f, max(1e-6f, peak));

    float3 ratio = min(1.6e+6f, color / peak);

    float lum = dot(color, float3(0.5, 0.4, 0.33));
    float gray = min(color.r, min(color.g, color.b));
	gray = max(0.0, gray);
    peak += min(peak, gray);
    peak *= 0.5;
    peak *= 1.0 + 1.666 * max(0, (peak - lum) / peak);

    float3 z = pows(peak, contrast);
    peak = z / (pows(z, b) * shoulder + c);

    crosstalk = max(1.0,crosstalk); // controls amount of channel crosstalk
    float saturation = contrast; // crosstalk saturation
    float crossSaturation = contrast * (64.0 / crosstalk); // crosstalk saturation
    // wrap crosstalk in transform
    ratio = pows(abs(ratio + 0.11) * 0.90909, saturation / crossSaturation);
    ratio = lerp(ratio, 1.0, pows(peak, float3(4.0, 1.5, 1.5) * 1.0/peak));
    ratio = pows(min(1.0, ratio), crossSaturation);

    return peak * ratio;
}

// https://gpuopen.com/wp-content/uploads/2016/03/GdcVdrLottes.pdf
// Adjusted NVR Lottes, initial algorithm
float3 Lottes(float3 x, float contrast, float midOut, float midIn, float hdrMax, float shoulder)
{
    hdrMax = max(1.0, hdrMax * 100.0);
    contrast = max(0.01, contrast * 1.35);
    shoulder = saturate(shoulder * 0.993); // shoulder should not! exceed 1.0
    midIn = max(0.01, midIn * 0.18);
    midOut = max(0.01, midOut * 0.18);

    // shape of the curve
    float3 z = pows(x, contrast); // toe (lower part of curve)

    // curve anchor (mid point)
    float2 e = float2(midIn, hdrMax);
    float2 exp = float2(contrast * shoulder, contrast);
    float4 f = pows(e.xyxy, exp.xxyy);

    // clipping/white point
    float b = -((-f.z + (midOut * (f.y * f.z - f.w * f.z * midOut)) / (f.y * midOut - f.x * midOut)) / (f.x * midOut));
    
    // midOut
    float c = (f.y * f.z - f.w * f.x * midOut) / (f.y * midOut - f.x * midOut);

    // test to tonemap color/brighness separately
    float peak = max(z.r, max(z.g, z.b));
    float3 ratio = z / peak;
    return ratio * (peak / (pows(peak, shoulder) * b + c));
    // return z / (pows(z, shoulder) * b + c);
}

// Uchimura GT Tonemapper
float3 Uchimura(float3 x, float contrast, float brightness, float midIn, float hdrMax, float shoulder)
{
    float P = 1.0 * brightness; // brightness
    float a = 1.0 * contrast; // contrast
    float m = 0.05 * midIn; // linear section start
    float l = 0.05 * hdrMax; // linear section length
    float3 c = 1.0 + (0.333 * shoulder); // black
    float b = 0.0; // pedestal
    
    float l0 = ((P - m) * l) / a;
    float L0 = m - m / a;
    float L1 = m + (1.0 - m) / a;
    float S0 = m + l0;
    float S1 = m + a * l0;
    float C2 = (a * P) / (P - S1);
    float CP = (-C2) / P;
    
    float3 w0 = float3(1.0 - smoothstep(0.0, m, x));
    float3 w2 = float3(step(m + l0, x));
    float3 w1 = float3(1.0 - w0 - w2);
    
    float3 T = float3(m * pows(x / m, c + b));
    float3 S = float3(P - (P - S1) * exp(CP * (x - S0)));
    float3 L = float3(m + a * (x - m));
    
    return T * w0 + L * w1 + S * w2;
}


float3 tonemap(float3 color, float contrast, float midOut, float midIn, float shoulder, float hdrMax, float highlightSaturation)
{
    switch(int(abs(TESR_HDRData.x)))
    {
        case 0:
            return color; 
        case 1:
            return max(0.0, AMDLottes(min(1.6e+6f, color), contrast, midIn, midOut, shoulder, highlightSaturation));
        case 2:
            return max(0.0, Lottes(min(1.6e+6f, color), contrast, midOut, midIn, hdrMax, shoulder));
        case 3:
            return ReinhardExtended(color, hdrMax);
        case 4:
            return ReinhardJodie(color);
        case 5:
            return ACESFilm(color);
        case 6:
            return ACESFitted(color);
        case 7:
            return Uncharted2Tonemap(color, midOut, hdrMax);
        case 8:
            return Uchimura(color, contrast, midOut, midIn, hdrMax, shoulder);
        case 9:
            return agx(color);
        default:
            return color; 
    }
}