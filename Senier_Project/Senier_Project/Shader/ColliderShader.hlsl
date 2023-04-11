cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld : packoffset(c0);
};

cbuffer cbPerPass : register(b3)
{
	float4x4 gViewProj;
};

Texture2D gDiffuseMap : register(t0);
SamplerState gSamLinear : register(s0);

struct VertexIn
{
	float3 PosL : POSITION;
	float3 Normal : NORMAL;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 Normal : NORMAL;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);

	vout.PosH = mul(posW, gViewProj);

	vout.Normal = vin.Normal;

	return vout;
}


float4 PS(VertexOut pin) : SV_Target
{
	float4 diffuseAlbedo = float4(1.0f, 0.0f, 0.0f, 1.0f);

	return diffuseAlbedo;
}

////////////////////////////////////////////////////////////////////////




