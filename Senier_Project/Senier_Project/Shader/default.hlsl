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

VertexOut defaultVS(VertexIn vin)
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


float4 defaultPS(VertexOut pin) : SV_Target
{
	pin.NormalW = normalize(pin.NormalW);

	float3 toEyeW = normalize(gEyePosW - pin.PosW);

	// 간접 조명을 흉내 내는 주변광 항.
	float4 ambient = gAmbientLight * gAlbedoColor;

	// 직접 조명
	const float shininess = 1.0f - gRoughness;
	Material mat = { gAlbedoColor, gFresnelR0, shininess };
	float3 shadowFactor = 1.0f;
	float4 directLight = ComputeLighting(gLights, mat, pin.PosW, pin.NormalW, toEyeW, shadowFactor);

	float4 litColor = ambient + directLight;

	// 흔히 하는 방식대로, 분산 재질에서 알파를 가져온다.
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
	vout.TangentW = mul(vin.Tangent, (float3x3)gWorld).xyz;
	vout.BiTangent = mul(vin.BiTangent, (float3x3)gWorld).xyz;

	vout.TexC = vin.TexC;

	return vout;
}

float4 TexturePS(VertexOut pin) : SV_Target
{
	float4 diffuseAlbedo = gDiffuseMap.Sample(gSamLinear, pin.TexC);

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

	//diffuseAlbedo = gShadowMap.Sample(gSamLinear, pin.TexC);

	//return diffuseAlbedo;
}