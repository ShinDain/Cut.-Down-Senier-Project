cbuffer cbPerPass : register(b0)
{
	float4x4 gViewProjWorld;
};

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

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.PosH = mul(float4(vin.PosL, 1.0f), gViewProjWorld);

	vout.TexC = vin.TexC;

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float4 diffuseAlbedo = gDiffuseMap.Sample(gSamLinear, pin.TexC);

	return diffuseAlbedo;
}
