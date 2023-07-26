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
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosW = posW.xyz;
	vout.PosH = mul(posW, gViewProj);

	return vout;
}


float4 PS(VertexOut pin) : SV_Target
{
	float4 black = { 0.8f, 0.8f, 0.5f, 1.0f };
	return black * gFadeInValue;
}
