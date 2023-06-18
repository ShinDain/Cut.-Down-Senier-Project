#ifndef NUM_DIR_LIGHTS
#define NUM_DIR_LIGHTS 1
#endif

#ifndef NUM_POINT_LIGHTS
#define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
#define NUM_SPOT_LIGHTS 0
#endif

#include "LightingUtil.hlsl"

Texture2D gShadowMap : register(t0);
Texture2D gDiffuseMap : register(t1);
Texture2D gMaskMap : register(t2);
Texture2D gNormalMap : register(t3);

SamplerState gsamAnisotropicWrap  : register(s0);
SamplerState gsamAnisotropicClamp : register(s1);
SamplerComparisonState gsamShadow : register(s2);


cbuffer cbPerWorld : register(b0)
{
	float4x4 gWorld;
	float4x4 gInverseTransWorld;
};

cbuffer cbPerPass : register(b3)
{
	float4x4 gViewProj;
	float4x4 gShadowTransform;
	float3 gEyePosW;
	float cbPerObjectPad1;
	float2 gRenderTargetSize;
	float2 gInvRenderTargetSize;
	float gNearZ;
	float gFarZ;
	float gTotalTime;
	float gDeltaTime;
	float4 gAmbientLight;

	// �ִ� MaxLights���� ��ü�� ���� �߿���
	// [0, NUM_DIR_LIGHTS) ������ ���ε��� ���Ɽ
	// [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS + NUM_POINT_LIGHTS)�� ����
	// [NUM_DIR_LIGHTS + NUM_POINT_LIGHTS, NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS) ������

	Light gLights[MaxLights];
};

cbuffer cbPerMaterial : register(b4)
{
	float4 gAlbedoColor;
	float3 gFresnelR0;
	float gRoughness;
	float4x4 gMatTransform;
}

// ���� �� ǥ���� ���� �������� ��ȯ�Ѵ�.
float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 unitNormalW, float3 tangentW)
{
    // �� ������ [0, 1]���� [-1, 1]�� ���
    float3 normalT = 2.0f * normalMapSample - 1.0f;

    // �������� ������ �����Ѵ�.
    float3 N = unitNormalW;
    float3 T = normalize(tangentW - dot(tangentW, N) * N);
    float3 B = cross(N, T);

    float3x3 TBN = float3x3(T, B, N);

    // ���������� ���� �������� ��ȯ
    float3 bumpedNormalW = mul(normalT, TBN);

    return bumpedNormalW;
}

float CalcShadowFactor(float4 shadowPosH)
{
    shadowPosH.xyz /= shadowPosH.w;

    // NDC ���� ����
    float depth = shadowPosH.z;

    uint width, height, numMips;
    gShadowMap.GetDimensions(0, width, height, numMips);

    // �ؼ� ũ��
    float dx = 1.0f / (float)width;

    float percentLit = 0.0f;
    const float2 offsets[9] =
    {
        float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
        float2(-dx, dx), float2(0.0f, dx), float2(dx, dx),
    };

    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        percentLit += gShadowMap.SampleCmpLevelZero(
            gsamShadow, shadowPosH.xy + offsets[i], depth).r;
    }

    //return 1.0f;
    return percentLit / 9.0f;
}