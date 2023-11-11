// ACES tonemapping https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
float3 ACESFilm(float3 x) {
	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;
	return (x*(a*x+b))/(x*(c*x+d)+e);
}

// -----------------------------------------------------------------------------------------------
// The code in this file was originally written by Stephen Hill (@self_shadow), who deserves all
// credit for coming up with this fit and implementing it. Buy him a beer next time you see him. :)

// sRGB => XYZ => D65_2_D60 => AP1 => RRT_SAT
static const float3x3 ACESInputMat =
{
    {0.59719, 0.35458, 0.04823},
    {0.07600, 0.90834, 0.01566},
    {0.02840, 0.13383, 0.83777}
};

// ODT_SAT => XYZ => D60_2_D65 => sRGB
static const float3x3 ACESOutputMat =
{
    { 1.60475, -0.53108, -0.07367},
    {-0.10208,  1.10813, -0.00605},
    {-0.00327, -0.07276,  1.07602}
};

float3 RRTAndODTFit(float3 v)
{
    float3 a = v * (v + 0.0245786f) - 0.000090537f;
    float3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

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


// https://64.github.io/tonemapping/
float3 Reinhard(float3 x, float whitepoint) {
    float luminance = luma(x);
    float tonemappedLuma = luminance * (1 + luminance / (1 + float(whitepoint * whitepoint)))/(1 + luminance);
    return x * (tonemappedLuma / luminance).rrr;
}

float3 Uncharted2Tonemap(float3 x) {
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    float W = 11.2;
    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

// https://gpuopen.com/wp-content/uploads/2016/03/GdcVdrLottes.pdf
float3 Lottes(float3 x, float contrast, float brightness, float midIn, float hdrMax, float shoulder){

    // scale values for easier tuning
    float3 params = float3(contrast, brightness, shoulder) * 0.1 + 1.0;

    // shape of the curve
    float3 z = pows(x, params.x); // toe (lower part of curve)

    // curve anchor (mid point)
    float2 e = float2(midIn, hdrMax);
    float2 exp = float2(params.x * params.z, params.x);
    float4 f = pows(e.xyxy, exp.xxyy);

    // clipping/white point
    float b = -((-f.z + (params.y*(f.y*f.z - f.w * f.z * params.y)) / (f.y*params.y - f.x*params.y)) / (f.x*params.y));
    
    // midOut
    float c = (f.y*f.z - f.w*f.x*params.y)/(f.y*params.y - f.x*params.y);

    // test to tonemap color/brighness separately
    // float peak = max(z.r, max(z.g, z.b));
    // float3 ratio = z/peak;
    // return ratio * (peak / (pows(peak, params.z) * b + c));
    return z / (pows(z, params.z) * b + c);
}

float3 tonemap(float3 color){
    if (TESR_HDRData.x == 1){
        return Cinematic.z * (Cinematic.w * color - Cinematic.y) + Cinematic.y;
    }else if (TESR_HDRData.x == 2){
        return ACESFilm(color);
    }else if (TESR_HDRData.x == 3){
        return Reinhard(color, TESR_HDRBloomData.w);
    }else if (TESR_HDRData.x == 4){
        return Lottes(color, TESR_LotteData.x, TESR_LotteData.y,  TESR_LotteData.z, TESR_HDRBloomData.w, TESR_LotteData.w);
    }else if (TESR_HDRData.x == 5){
        return ACESFitted(color);
    }else if (TESR_HDRData.x == 6){
        return Uncharted2Tonemap(color);
    }else{
        return color;
    }
}