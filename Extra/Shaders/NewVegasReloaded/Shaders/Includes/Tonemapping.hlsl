// ACES tonemapping https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
float3 ACESFilm(float3 x) {
	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;
	return saturate((x*(a*x+b))/(x*(c*x+d)+e));
}

// https://64.github.io/tonemapping/
float3 Reinhard(float3 x, float whitepoint) {
    return x * (1 + x / whitepoint.xxx)/(1 + x);
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
    contrast = 1.0 + contrast * 0.1;
    brightness = 1.0 + brightness * 0.1; // midOut
    shoulder = 1.0 + shoulder * 0.1; // compression speed for shoulder

    // shape of the curve
    float3 z = pow(x, contrast); // toe (lower part of curve)

    // curve anchor (mid point)
    float2 e = float2(midIn, hdrMax);
    float2 exp = float2(contrast * shoulder, contrast);
    float4 f = pow(e.xyxy, exp.xxyy);

    // clipping/white point
    float b = -((-f.z + (brightness*(f.y*f.z - f.w * f.z * brightness)) / (f.y*brightness - f.x*brightness)) / (f.x*brightness));
    
    // compression speed for shoulder (linear)
    float c = (f.y*f.z - f.w*f.x*brightness)/(f.y*brightness - f.x*brightness);

    return z / (pow(z, shoulder) * b + c);
}