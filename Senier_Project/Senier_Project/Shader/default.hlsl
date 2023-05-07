cbuffer cbPerWorld : register(b0)
{
	float4x4 gWorld;
};

cbuffer cbPerPass : register(b3)
{
	float4x4 gViewProj;
};

cbuffer cbPerMaterial : register(b4)
{
	float4 gAlbedoColor;
}

Texture2D gDiffuseMap : register(t0);
SamplerState gSamLinear : register(s0);

struct VertexIn
{
	float3 PosL : POSITION;
	float2 TexC : TEXCOORD;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float2 TexC : TEXCOORD;
};

VertexOut defaultVS(VertexIn vin)
{
	VertexOut vout;

	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);

	vout.PosH = mul(posW, gViewProj);

	vout.TexC = vin.TexC;

	return vout;
}


float4 defaultPS(VertexOut pin) : SV_Target
{
	float4 diffuseAlbedo = gAlbedoColor;

	return diffuseAlbedo;
}

////////////////////////////////////////////////////////////////////////

VertexOut TextureVS(VertexIn vin)
{
	VertexOut vout;

	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);

	vout.PosH = mul(posW, gViewProj);

	vout.TexC = vin.TexC;

	return vout;
}

float4 TexturePS(VertexOut pin) : SV_Target
{
	float4 diffuseAlbedo = gDiffuseMap.Sample(gSamLinear, pin.TexC);

	return diffuseAlbedo;
}