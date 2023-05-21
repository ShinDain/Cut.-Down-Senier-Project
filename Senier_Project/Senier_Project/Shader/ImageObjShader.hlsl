cbuffer cbPerPass : register(b0)
{
	float4x4 gViewProjWorld;
};

Texture2D gTexMap : register(t0);
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

	//VertexOut vout = (VertexOut)0.0f;

	//// Already in homogeneous clip space.
	//vout.PosH = float4(vin.PosL, 1.0f);

	//vout.TexC = vin.TexC;

	//return vout;

}

float4 PS(VertexOut pin) : SV_Target
{
	// shadow map 테스트 중
	//float4 diffuseAlbedo = gTexMap.Sample(gSamLinear, pin.TexC);
	float4 diffuseAlbedo = float4(gTexMap.Sample(gSamLinear, pin.TexC).rrr, 1.0f);
	//float4 diffuseAlbedo = float4(1.0f,1.0f,1.0f,1.0f);

	return diffuseAlbedo;
}
