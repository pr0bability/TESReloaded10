static const float PI = 3.14159274;
static const float TWO_PI = 6.28318548;
static const float ANGLE_SCALE = 0.499999583;
static const float ANGLE_BIAS = 0.5;
static const float ANGLE_TO_SINCOS = 0.159154937;

void CalculateAnimationRotation(in float4 animationParams, in float index, out float angleSin, out float angleCos) {
    float firstTime = frac((index / 48.0f + animationParams.y) * ANGLE_SCALE + ANGLE_BIAS);
    float firstAngle = firstTime * TWO_PI - PI;

    float secondTime = frac((sin(firstAngle) * animationParams.x * animationParams.z) * ANGLE_TO_SINCOS + ANGLE_BIAS);
    float secondAngle = secondTime * TWO_PI - PI;
    
    sincos(secondAngle, angleSin, angleCos);
}

float4 SpeedTreeTransform(
    float4 billboardUp,
    float4 billboardRight,
    float4 rockParams,
    float4 rustleParams,
    float4 windMatrices[16], // vectors (.w always 0.0f)
    float4 leafBase[48],  // vectors (.xw always 0.0f)
    float4 blendIndices,
    float4 position
) {
    // Rustling animation.
    float rustleSin, rustleCos;
    CalculateAnimationRotation(rustleParams, blendIndices.z, rustleSin, rustleCos);
    
    // Rustle rotation around the Z-axis (first two rows, last is identity).
    float3 rustleRotationX = float3(rustleCos, -rustleSin, 0.0f);
    float3 rustleRotationY = float3(rustleSin, rustleCos, 0.0f);

    // Rocking animation.
    float rockSin, rockCos;
    CalculateAnimationRotation(rockParams, blendIndices.z, rockSin, rockCos);
    
    // Rock rotation around the X-axis (last two rows, first is identity).
    float3 rockRotationY = float3(0.0f, rockCos, -rockSin);
    float3 rockRotationZ = float3(0.0f, rockSin, rockCos);

    // Matrix offsets.
    float2 matrixIndices = blendIndices.zy - frac(blendIndices.zy);
    
    // Leaf positioning.
    float4 leafVector = leafBase[matrixIndices.x] * blendIndices.w;
    
    // Transforming billboardRight with the rustle Z-rotation matrix, ignoring the Z component because it doesn't change.
    float rotatedBillboardRightX = dot(rustleRotationX, billboardRight.xyz);
    float rotatedBillboardRightY = dot(rustleRotationY, billboardRight.xyz);
    
    // Transforming billboardUp with the rustle Z-rotation matrix, ignoring the Z component because it doesn't change.
    float rotatedBillboardUpX = dot(rustleRotationX, billboardUp.xyz);
    float rotatedBillboardUpY = dot(rustleRotationY, billboardUp.xyz);

    // Transforming the leafVector with the rock X-rotation matrix, ignoring the X component because it doesn't change.
    float rotatedLeafVectorY = dot(rockRotationY, leafVector.xyz);
    float rotatedLeafVectorZ = dot(rockRotationZ, leafVector.xyz);
    
    // The first offset is just the rotated billboard right vector times the rotated leaf vector Y component.
    float4 offset1 = float4(rotatedBillboardRightX, rotatedBillboardRightY, billboardRight.zw);
    offset1 *= rotatedLeafVectorY;
    
    // Second offset is rotated billboard up vector times the rotated leaf vector Z component.
    float4 offset2 = float4(rotatedBillboardUpX, rotatedBillboardUpY, billboardUp.zw);
    offset2 *= rotatedLeafVectorZ;

    float4 offsetPosition = position + offset1 + offset2;
    
    // Apply wind transformation
    float4 windPosition;
    windPosition.x = dot(windMatrices[0 + matrixIndices.y], offsetPosition);
    windPosition.y = dot(windMatrices[1 + matrixIndices.y], offsetPosition);
    windPosition.z = dot(windMatrices[2 + matrixIndices.y], offsetPosition);
    windPosition.w = dot(windMatrices[3 + matrixIndices.y], offsetPosition);
    
    // Blend between original and wind-affected position based on blend weight
    return lerp(offsetPosition, windPosition, blendIndices.x);
}
