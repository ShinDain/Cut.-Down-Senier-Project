#ifndef NUM_DIR_LIGHTS 
#define NUM_DIR_LIGHTS 0
#endif

#ifndef NUM_POINT_LIGHTS 
#define NUM_POINT_LIGHTS 1
#endif

#ifndef NUM_SPOT_LIGHTS 
#define NUM_SPOT_LIGHTS 0
#endif

# define MaxLights 16

#include "LightingUtil.hlsl"

cbuffer cbPerWorld : register(b0)
{
	float4x4 gWorld;
	float4x4 gInverseTransWorld;
};

cbuffer cbPerPass : register(b3)
{
	float4x4 gViewProj;
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

Texture2D gDiffuseMap : register(t0);
SamplerState gSamLinear : register(s0);

struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 TexC : TEXCOORD;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 TexC : TEXCOORD;
};

VertexOut defaultVS(VertexIn vin)
{
	VertexOut vout;

	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosW = posW;
	vout.PosH = mul(posW, gViewProj);

	vout.NormalW = mul(float4(vin.NormalL, 1.0f), gInverseTransWorld);

	vout.TexC = vin.TexC;

	return vout;
}


float4 defaultPS(VertexOut pin) : SV_Target
{
	pin.NormalW = normalize(pin.NormalW);

	float3 toEyeW = normalize(gEyePosW - pin.PosW);

	// ���� ������ �䳻 ���� �ֺ��� ��.
	float4 ambient = gAmbientLight * gAlbedoColor;

	// ���� ����
	const float shininess = 1.0f - gRoughness;
	Material mat = { gAlbedoColor, gFresnelR0, shininess };
	float3 shadowFactor = 1.0f;
	float4 directLight = ComputeLighting(gLights, mat, pin.PosW, pin.NormalW, toEyeW, shadowFactor);

	float4 litColor = ambient + directLight;

	// ���� �ϴ� ��Ĵ��, �л� �������� ���ĸ� �����´�.
	litColor.a = gAlbedoColor.a;

	return litColor;

	//float4 diffuseAlbedo = gAlbedoColor;

	//return diffuseAlbedo;
}

////////////////////////////////////////////////////////////////////////

VertexOut TextureVS(VertexIn vin)
{
	VertexOut vout;

	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosW = posW;
	vout.PosH = mul(posW, gViewProj);

	vout.NormalW = mul(float4(vin.NormalL, 1.0f), gInverseTransWorld);

	vout.TexC = vin.TexC;

	return vout;
}

float4 TexturePS(VertexOut pin) : SV_Target
{
	float4 diffuseAlbedo = gDiffuseMap.Sample(gSamLinear, pin.TexC);

	//return diffuseAlbedo;

	pin.NormalW = normalize(pin.NormalW);

	float3 toEyeW = normalize(gEyePosW - pin.PosW);

	// ���� ������ �䳻 ���� �ֺ��� ��.
	float4 ambient = gAmbientLight * diffuseAlbedo;

	// ���� ����
	const float shininess = 1.0f - gRoughness;
	Material mat = { gAlbedoColor, gFresnelR0, shininess };
	float3 shadowFactor = 1.0f;
	float4 directLight = ComputeLighting(gLights, mat, pin.PosW, pin.NormalW, toEyeW, shadowFactor);

	float4 litColor = ambient + directLight;

	// ���� �ϴ� ��Ĵ��, �л� �������� ���ĸ� �����´�.
	litColor.a = diffuseAlbedo.a;

	return litColor;
}