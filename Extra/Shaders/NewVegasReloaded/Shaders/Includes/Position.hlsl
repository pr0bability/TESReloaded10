float4x4 TESR_ProjectionTransform;
float4x4 TESR_ViewTransform;

float3 toWorld(float2 tex)
{
    // float4 screenpos = float4(expand(tex), 1.0, 1.0f);
    // screenpos.y = -screenpos.y;
    // float4 viewpos = mul(screenpos, TESR_InvWorldViewProjectionTransform);
    // viewpos.xyz /= viewpos.w;
    // return viewpos.xyz;
    float2 uv = expand(tex);

	float3 v = float3(TESR_ViewTransform[2][0], TESR_ViewTransform[2][1], TESR_ViewTransform[2][2]);
	v += (1 / TESR_ProjectionTransform[0][0] * uv.x) * float3(TESR_ViewTransform[0][0], TESR_ViewTransform[0][1], TESR_ViewTransform[0][2]);
	v += (-1 / TESR_ProjectionTransform[1][1] * uv.y) * float3(TESR_ViewTransform[1][0], TESR_ViewTransform[1][1], TESR_ViewTransform[1][2]);
	return v;
}