float HalfLambert(float3 Vector1, float3 Vector2) {
	
	float product = dot(Vector1, Vector2);
	product *= 0.5;
	product += 0.5;
	return product;
	
}

float BlinnPhongSpecular(float3 Normal, float3 LightDir) {
	
	float3 halfAngle = Normal + LightDir;
	return pow(saturate(dot(Normal, halfAngle)), TESR_SkinData.y);
	
}

float3 Skin(float3 SkinColor, float3 LightColor, float3 CameraDir, float3 LightDir, float3 Normal) {

	float4 dotLN = HalfLambert(LightDir, Normal) * TESR_SkinData.x;
	float3 indirectLightComponent  = TESR_SkinData.z * max(0, dot(-Normal, LightDir));
	
	indirectLightComponent += TESR_SkinData.z * HalfLambert(-CameraDir, LightDir);
	indirectLightComponent *= TESR_SkinData.x;
	indirectLightComponent *= pow(SkinColor, 2);

	float3 rim = (float3)(1.0f - max(0.0f, dot(Normal, CameraDir)));
	rim = pow(rim, 3);
	rim *= max(0.0f, dot(Normal, LightDir)) * LightColor;
	rim *= TESR_SkinData.w;

	float4 finalCol = dotLN * 0.5 + float4(indirectLightComponent, 1.0f);

	finalCol.rgb += finalCol.a * TESR_SkinData.x * rim;
	finalCol.rgb += finalCol.a * TESR_SkinData.x * BlinnPhongSpecular(Normal, LightDir) * TESR_SkinColor.rgb * 0.05f;
	finalCol.rgb *= LightColor;
	
	return finalCol.rgb;
	
};


float3 GetLighting(float3 lightDir, float3 eyeDir, float3 normal, float3 lightColor){
    float fresnelCoeff = pows(1 - shades(normal, eyeDir), 5);
    float diffuse = shades(normal, lightDir);
    float3 fresnel = fresnelCoeff * lightColor * 0.5 * pow(diffuse, TESR_DebugVar.w);
    float3 lighting = diffuse * lightColor + fresnel + AmbientColor.rgb;
    return max(lighting, 0);
}

float GetSpecular(float3 lightDirection, float3 eyeDirection, float3 normal, float3 lightColor){
    return  pow(shades(normal, normalize(lightDirection + eyeDirection)), TESR_DebugVar.z) * luma(lightColor) * TESR_DebugVar.y;
}

float GetSSS(float3 lightDirection, float3 normal){
    return (1 - shades(normal, lightDirection)) * float3(0.5, 0.2, 0.3) * TESR_DebugVar.x;
}

float3 getNormal(float2 uv){
    return (expand(tex2D(NormalMap, uv).xyz));
}

// float2 getScreenpos(VS_INPUT IN){
//     return IN.position.xy * TESR_ReciprocalResolution.xy;
// };


float3 ApplyVertexColor(float3 baseColor, float3 vertexColor, float4 toggles){
   return toggles.x <= 0.0 ? baseColor : (baseColor.rgb * vertexColor); // apply vertex color
}

float3 ApplyFog(float3 baseColor, float4 fogColor, float4 toggles){
    return Toggles.y <= 0.0 ? baseColor : ((fogColor.a * (fogColor.rgb - baseColor)) + baseColor);
}

float4 getBaseColor(float2 uv, sampler2D FaceGenMap0Buffer, sampler2D FaceGenMap1Buffer, sampler2D BaseColorBuffer){
    // sample color textures
    float3 faceGenMap0 = tex2D(FaceGenMap0Buffer, uv).rgb;
    float3 faceGenMap1 = tex2D(FaceGenMap1Buffer, uv).rgb;
    float4 baseTexture = tex2D(BaseColorBuffer, uv);
    float4 baseColor = float4(0, 0, 0, 1);
	baseColor.rgb = 2 * ((expand(faceGenMap0) + baseTexture.rgb) * (2 * faceGenMap1));
    baseColor.a = baseTexture.a;
    
    return baseColor;
}
