
float4 TESR_ShadowData : register(c0);
sampler2D DiffuseMap : register(s0) = sampler_state { ADDRESSU = WRAP; ADDRESSV = WRAP; MAGFILTER = POINT; MINFILTER = POINT; MIPFILTER = POINT; };
sampler2D LeafDiffuseMap : register(s1) = sampler_state { ADDRESSU = WRAP; ADDRESSV = WRAP; MAGFILTER = POINT; MINFILTER = POINT; MIPFILTER = POINT; };

struct VS_OUTPUT {
    float4 texcoord_0 : TEXCOORD0;
	float4 texcoord_1 : TEXCOORD1;
};

struct PS_OUTPUT {
    float4 color_0 : COLOR0;
};


PS_OUTPUT main(VS_OUTPUT IN) {
    PS_OUTPUT OUT;
	
	float4 r0;
	float r1;
	float depth;

	if (TESR_ShadowData.y == 1.0f) { // Leaves (Speedtrees) or alpha is required
		r0.rgba = (TESR_ShadowData.x == 2.0f)?tex2D(LeafDiffuseMap, IN.texcoord_1.xy):tex2D(DiffuseMap, IN.texcoord_1.xy);
		if (r0.a < 0.5f) discard;
	}
	depth = IN.texcoord_0.z / IN.texcoord_0.w;
	
	//OUT.color_0 = float4(0.0f, 0.0f, 0.0f, 1.0f);

	// TESR_ShadowData.w : shadow technique
	// 0: disabled
	// 1: VSM
	// 2: simple ESM
	// 3: filtered ESM
	// 4: PCF or Orthomap
	if (TESR_ShadowData.w == 1.0f && !TESR_ShadowData.z){
	// 	shadowMode = (1 - TESR_ShadowData.z) * shadowMode + float4(0, 0, 0, 1) * TESR_ShadowData.z; //will cancel out shadowData.w if ortho map and replace by PCF mode
	// }
	// float4 shadowMode = {TESR_ShadowData.w == 1.0f, TESR_ShadowData.w == 2.0f, TESR_ShadowData.w == 3.0f, (TESR_ShadowData.w == 4.0f)};
	// shadowMode = (1 - TESR_ShadowData.z) * shadowMode + float4(0, 0, 0, 1) * TESR_ShadowData.z; //will cancel out shadowData.w if ortho map and replace by PCF mode

		// VSM
		//cheat to reduce shadow acne in variance maps
		float dx = ddx(depth);
		float dy = ddy(depth);
		float moment2 = depth * depth + 0.25 * (dx * dx + dy * dy);
		OUT.color_0 = float4(depth, moment2, 0.0f, 1.0f); 
	
	} else if (TESR_ShadowData.w == 3.0 && !TESR_ShadowData.z){
		// ESSM
		float k = 80;
		float esm = exp( 80 * depth);
		OUT.color_0 = float4(esm, esm - depth, 0.0f, 1.0f);
	
	}else { // if ESM or ortho map
		// PCF/simple ESM
		OUT.color_0 = float4(depth, 0.0f, 0.0f, 1.0f);
	}

	return OUT;	
};