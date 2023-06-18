cbuffer cbPerWorld : register(b0)
{
	float4x4 gWorld;
	float4x4 gInverseTransWorld;
};

cbuffer cbPerPass : register(b3)
{
	float4x4 gViewProj;
};

cbuffer cbPerCollider : register(b4)
{
	float gIsActive;
}

Texture2D gDiffuseMap : register(t0);
SamplerState gsamAnisotropicWrap : register(s0);

struct VertexIn
{
	float3 PosL : POSITION;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);

	vout.PosH = mul(posW, gViewProj);

	return vout;
}


float4 PS(VertexOut pin) : SV_Target
{
	float4 diffuseAlbedo = float4(1.0f, 0.0f, 0.0f, 1.0f);
	if(gIsActive)
		diffuseAlbedo = float4(1.0f, gIsActive, 0.0f, 1.0f);

	return diffuseAlbedo;
}

////////////////////////////////////////////////////////////////////////




