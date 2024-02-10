#define	expand(v)	        (((v) - 0.5) / 0.5)  // from 0/1 to -1/1
#define	compress(v)         (((v) * 0.5) + 0.5)  // from -1/1 to 0/1 
#define	shade(n, l)         max(dot(n, l), 0)
#define	shades(n, l)        saturate(dot(n, l))
#define invlerp(a, b, t)    ((t-a)/(b-a))
#define invlerps(a, b, t)    saturate((t-a)/(b-a))
#define luma(color)         dot(color.rgb, float3(0.2126f, 0.7152f, 0.0722f)) // extract luma (BT.709)
// #define luma(color)         dot(color, float3(0.3f, 0.59f, 0.11f)) // extract luma (BT.601)
#define mix(colora, colorb) colora * colorb * 2 // mix two colors without darkening
#define	weight(v)           dot(v, 1)
#define	sqr(v)              ((v) * (v))
#define blendnormals(a, b)   float3(a.xy + b.xy, a.z)
#define rand(s)              (frac(sin(dot(s, float2(12.9898, 78.233))) * 43758.5453)) // pseudo random from https://gist.github.com/keijiro/ee7bc388272548396870
#define pows(a, b)           (pow(abs(a), b) * sign(a)) // no more pow/abs warning!
#define linearize(color)     (float4(pows(color.rgb, 2.2), color.a))
#define delinearize(color)   (float4(max(0.0, pows(color.rgb, 1.0/2.2)), color.a))
#define bend(a, b)           (a * (1 + b) / (1 + a * b)) //bends the response curve in a symetric way 
#define scaledReinhard(a, b) ((a * b) / (1 + a * b)) // curve that reaches a maximum of 1 with a speed b

static const float4 white = float4 (1, 1, 1, 1);
static const float4 grey = float4 (0.5, 0.5, 0.5, 1);
static const float4 black = float4 (0, 0, 0, 1);
static const float4 red = float4 (1, 0, 0, 1);
static const float4 green = float4 (0, 1, 0, 1);
static const float4 blue = float4 (0, 0, 1, 1);
static const float4 yellow = float4 (1, 1, 0, 1);
static const float4 cyan = float4 (0, 1, 1, 1);
static const float4 magenta = float4 (1, 0, 1, 1);

float4 selectColor(float selector, float4 color0, float4 color1, float4 color2, float4 color3, float4 color4, float4 color5, float4 color6, float4 color7, float4 color8, float4 color9){
    if (selector == 0.0) return color0;
    if (selector >= 0.1 && selector < 0.2) return color1;
    if (selector >= 0.2 && selector < 0.3) return color2;
    if (selector >= 0.3 && selector < 0.4) return color3;
    if (selector >= 0.4 && selector < 0.5) return color4;
    if (selector >= 0.5 && selector < 0.6) return color5;
    if (selector >= 0.6 && selector < 0.7) return color6;
    if (selector >= 0.7 && selector < 0.8) return color7;
    if (selector >= 0.8 && selector < 0.9) return color8;
    if (selector >= 0.9 && selector < 1.0) return color9;
    return black;
}

float3 selectColor(float selector, float3 color0, float3 color1, float3 color2, float3 color3, float3 color4, float3 color5, float3 color6, float3 color7, float3 color8, float3 color9){
    if (selector == 0.0) return color0;
    if (selector >= 0.1 && selector < 0.2) return color1;
    if (selector >= 0.2 && selector < 0.3) return color2;
    if (selector >= 0.3 && selector < 0.4) return color3;
    if (selector >= 0.4 && selector < 0.5) return color4;
    if (selector >= 0.5 && selector < 0.6) return color5;
    if (selector >= 0.6 && selector < 0.7) return color6;
    if (selector >= 0.7 && selector < 0.8) return color7;
    if (selector >= 0.8 && selector < 0.9) return color8;
    if (selector >= 0.9 && selector < 1.0) return color9;
    return black.rgb;
}