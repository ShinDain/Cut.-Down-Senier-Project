#include "Common.hlsl"

struct VertexIn
{
	float3 PosL : POSITION;
	float2 TexC : TEXCOORD;
	float3 NormalL : NORMAL;
	float3 Tangent : TANGENT;
	float3 BiTangent : BITANGENT;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float3 TangentW : TANGENT;
	float3 BiTangent : BITANGENT;
	float2 TexC : TEXCOORD;
};

cbuffer cbPerCut : register(b5)
{
	int PlaneCnt;
	float PlaneDirection_1;
	float PlaneDirection_2;
	float PlaneDirection_3;

	float3 PlaneNormal_1;
	float PlaneDistance_1;
	float3 PlaneNormal_2;
	float PlaneDistance_2;
	float3 PlaneNormal_3;
	float PlaneDistance_3;
};


VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosW = posW;
	vout.PosH = mul(posW, gViewProj);

	vout.NormalW = mul(float4(vin.NormalL, 1.0f), gInverseTransWorld);
	vout.TangentW = mul(vin.Tangent, (float3x3)gWorld).xyz;
	vout.BiTangent = mul(vin.BiTangent, (float3x3)gWorld).xyz;


	vout.TexC = vin.TexC;

	return vout;
}


float4 PS(VertexOut pin) : SV_Target
{
	float4 diffuseAlbedo = gAlbedoColor;

	if (dot(pin.PosW, PlaneNormal_1 * (PlaneDirection_1)) < PlaneDistance_1 * (PlaneDirection_1))
	{
		diffuseAlbedo = float4(0, 0, 0, 0);
		// Alpha Test
		clip(diffuseAlbedo.a - 0.1f);
	}
	if (dot(pin.PosW, PlaneNormal_2 * (PlaneDirection_2)) < PlaneDistance_2 * (PlaneDirection_2))
	{
		diffuseAlbedo = float4(0, 0, 0, 0);
		// Alpha Test
		clip(diffuseAlbedo.a - 0.1f);
	}
	if (dot(pin.PosW, PlaneNormal_3 * (PlaneDirection_3)) < PlaneDistance_3 * (PlaneDirection_3))
	{
		diffuseAlbedo = float4(0, 0, 0, 0);
		// Alpha Test
		clip(diffuseAlbedo.a - 0.1f);
	}

	// Alpha Test
	clip(diffuseAlbedo.a - 0.1f);

	pin.NormalW = normalize(pin.NormalW);

	float3 toEyeW = normalize(gEyePosW - pin.PosW);

	// 간접 조명을 흉내 내는 주변광 항.
	float4 ambient = gAmbientLight * diffuseAlbedo;

	// 직접 조명
	const float shininess = 1.0f - gRoughness;
	Material mat = { gAlbedoColor, gFresnelR0, shininess };
	float3 shadowFactor = 1.0f;
	float4 directLight = ComputeLighting(gLights, mat, pin.PosW, pin.NormalW, toEyeW, shadowFactor);

	float4 litColor = ambient + directLight;

	// 흔히 하는 방식대로, 분산 재질에서 알파를 가져온다.
	litColor.a = diffuseAlbedo.a;

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
	vout.TangentW = mul(vin.Tangent, (float3x3)gWorld).xyz;
	vout.BiTangent = mul(vin.BiTangent, (float3x3)gWorld).xyz;

	vout.TexC = vin.TexC;

	return vout;
}

float4 TexturePS(VertexOut pin) : SV_Target
{
	float4 diffuseAlbedo = float4(0, 0, 0, 1);

	if (dot(pin.PosW, PlaneNormal_1 * (PlaneDirection_1)) < PlaneDistance_1 * (PlaneDirection_1))
	{
		diffuseAlbedo = float4(0, 0, 0, 0);
		// Alpha Test
		clip(diffuseAlbedo.a - 0.1f);
	}
	if (dot(pin.PosW, PlaneNormal_2 * (PlaneDirection_2)) < PlaneDistance_2 * (PlaneDirection_2))
	{
		diffuseAlbedo = float4(0, 0, 0, 0);
		// Alpha Test
		clip(diffuseAlbedo.a - 0.1f);
	}
	if (dot(pin.PosW, PlaneNormal_3  * (PlaneDirection_3)) < PlaneDistance_3 * (PlaneDirection_3))
	{
		diffuseAlbedo = float4(0, 0, 0, 0);
		// Alpha Test
		clip(diffuseAlbedo.a - 0.1f);
	}

	diffuseAlbedo = gDiffuseMap.Sample(gSamLinear, pin.TexC);
	// Alpha Test
	clip(diffuseAlbedo.a - 0.1f);

	pin.NormalW = normalize(pin.NormalW);
	float4 normalMapSample = gNormalMap.Sample(gSamLinear, pin.TexC);
	float3 bumpedNormalW = NormalSampleToWorldSpace(normalMapSample.rgb, pin.NormalW, pin.TangentW);

	float3 toEyeW = normalize(gEyePosW - pin.PosW);

	// 간접 조명을 흉내 내는 주변광 항.
	float4 ambient = gAmbientLight * diffuseAlbedo;

	// 직접 조명
	//const float shininess = 1.0f - gRoughness;
	const float shininess = 0.0f;
	Material mat = { gAlbedoColor, gFresnelR0, shininess };
	float3 shadowFactor = 1.0f;
	float4 directLight = ComputeLighting(gLights, mat, pin.PosW, bumpedNormalW, toEyeW, shadowFactor);

	float4 litColor = ambient + directLight;

	// 흔히 하는 방식대로, 분산 재질에서 알파를 가져온다.
	litColor.a = diffuseAlbedo.a;

	return litColor;

	//float4 diffuseAlbedo = gShadowMap.Sample(gSamLinear, pin.TexC);

	//return diffuseAlbedo;
}