
#include "../includes/Speedtree.hlsl"

row_major float4x4 TESR_ShadowWorldTransform : register(c0);
row_major float4x4 TESR_ShadowViewProjTransform : register(c4);
float4 TESR_ShadowData : register(c8);
float4 Bones[54] : register(c9);
float4 BillboardRight : register(c63);
float4 BillboardUp : register(c64);
float4 RockParams : register(c65);
float4 RustleParams : register(c66);
float4 WindMatrices[16] : register(c67);
float4 LeafBase[48] : register(c83);

// Terrain LOD params for hiding it under normal terrain.
row_major float4x4 WorldTranspose : register(c140);
float4 HighDetailRange : register(c144);
float4 LODLandParams : register(c145);

#define	weight(v)		dot(v, 1)

struct VS_INPUT {
    float4 position : POSITION;
	float4 texcoord_0 : TEXCOORD0;
    float4 texcoord_1 : TEXCOORD1;  // Terrain LOD, no idea what it represents.
    float4 blendweight : BLENDWEIGHT;
    float4 blendindexes : BLENDINDICES;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float4 texcoord_0 : TEXCOORD0;
	float4 texcoord_1 : TEXCOORD1;
};

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

	const float4 const_0 = {1.0f, 765.01001f, 0.0f, 0.0f};
	const float4 const_4 = {(1.0 / (PI * 2)), 0.25, 0.5, 0};
	
	float4 offset;
    float4 q0;
	float4 q1;
	float4 q2;
	float4 q3;
    float4 q4;
    float4 q5;
    float4 q6;
    float4 q7;
    float4 q8;
	float4 q9;
	float4 q10;
	float4 q11;
	float4 q28;
	float4 q59;
	float4 r1;
	float4 r2;
	float4 r3;
	float4 r4;
	float4 r5;
	float4 r0 = IN.position;
	
	if (TESR_ShadowData.x == 1.0f) { // Skinned (Actors)
		offset.xyzw = IN.blendindexes.zyxw * const_0.y;
		r0.w = 1;
		q0.xyzw = (IN.position.xyzx * const_0.xxxz) + const_0.zzzx;
		q8.xyz = mul(float3x4(Bones[0 + offset.w].xyzw, Bones[1 + offset.w].xyzw, Bones[2 + offset.w].xyzw), q0.xyzw);
		q6.xyz = mul(float3x4(Bones[0 + offset.z].xyzw, Bones[1 + offset.z].xyzw, Bones[2 + offset.z].xyzw), q0.xyzw);
		q5.xyz = mul(float3x4(Bones[0 + offset.x].xyzw, Bones[1 + offset.x].xyzw, Bones[2 + offset.x].xyzw), q0.xyzw);
		q4.xyz = mul(float3x4(Bones[0 + offset.y].xyzw, Bones[1 + offset.y].xyzw, Bones[2 + offset.y].xyzw), q0.xyzw);
		q7.xyz = (IN.blendweight.z * q6.xyz) + ((IN.blendweight.x * q5.xyz) + (q4.xyz * IN.blendweight.y));
		r0.xyz = ((1 - weight(IN.blendweight.xyz)) * q8.xyz) + q7.xyz;
	}
	else if (TESR_ShadowData.x == 2.0f) { // Leaves (Speedtrees)
        r0.xyzw = SpeedTreeTransform(BillboardUp, BillboardRight, RockParams, RustleParams, WindMatrices, LeafBase, IN.blendindexes, r0);
    }
    else if (TESR_ShadowData.x == 3.0f) { // Terrain LOD.
        float4 r1 = IN.position;
        r1.z = lerp(IN.texcoord_1.x, IN.position.z, LODLandParams.x);

        float q0 = (abs(dot(WorldTranspose[1].xyzw, r1.xyzw) - HighDetailRange.y) < HighDetailRange.w ? 1.0 : 0.0);
        float q1 = (abs(dot(WorldTranspose[0].xyzw, r1.xyzw) - HighDetailRange.x) < HighDetailRange.z ? 1.0 : 0.0);

        r0.z = r1.z - ((q0.x * q1.x) * LODLandParams.y);
    }
    if (TESR_ShadowData.x != 1.0f) r0 = mul(r0, TESR_ShadowWorldTransform);
	r0 = mul(r0, TESR_ShadowViewProjTransform);
	
	// Pancaking to ensure geometry outside of near plane is not culled.
    r0.z = max(r0.z, 0.0f);
	
	OUT.position = r0;
    OUT.texcoord_0 = r0;
	OUT.texcoord_1 = IN.texcoord_0;
    return OUT;
	
};