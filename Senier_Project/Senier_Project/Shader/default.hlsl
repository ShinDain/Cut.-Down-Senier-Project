cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
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

	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);

	vout.PosH = mul(posW, gViewProj);

	vout.TexC = vin.TexC;

	return vout;
}


float4 PS(VertexOut pin) : SV_Target
{
	float4 diffuseAlbedo = gDiffuseMap.Sample(gSamLinear, pin.TexC);

	return diffuseAlbedo;
}

////////////////////////////////////////////////////////////////////////

#define MAX_VERTEX_INFLUENCES			4
#define SKINNED_ANIMATION_BONES			256

cbuffer cbBoneOffsets : register(b1)
{
	float4x4 gmtxBoneOffsets[SKINNED_ANIMATION_BONES];
};

cbuffer cbBoneTransforms : register(b2)
{
	float4x4 gmtxBoneTransforms[SKINNED_ANIMATION_BONES];
};

struct SkinnedMeshVertexIn
{
	float3 PosL : POSITION;
	float2 TexC : TEXCOORD;
	float3 Normal :NORMAL;
	float3 Tangent : TANGENT;
	float3 BiTangent : BITANGENT;
	int4 Indices : BONEINDEX;
	float4 Weight : BONEWEIGHT;
};

struct SkinnedMeshVertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 BiTangent : BITANGENT;
	float2 TexC : TEXCOORD;
};

SkinnedMeshVertexOut VSSkinnedMesh(SkinnedMeshVertexIn vin)
{
	SkinnedMeshVertexOut vout;

	float4x4 mtxVertexToBoneWorld = (float4x4)0.0f;
	
	for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
	{
		mtxVertexToBoneWorld += vin.Weight[i] * mul(gmtxBoneOffsets[vin.Indices[i]], gmtxBoneTransforms[vin.Indices[i]]);
		//mtxVertexToBoneWorld += vin.Weight[i] * gmtxBoneTransforms[vin.Indices[i]];
		//mtxVertexToBoneWorld += gmtxBoneOffsets[vin.Indices[i]];
	}
	//vout.PosW = vin.PosL;
	//vout.PosW = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	vout.PosW = mul(float4(vin.PosL, 1.0f), mtxVertexToBoneWorld).xyz;
	vout.Normal = mul(vin.Normal, (float3x3)mtxVertexToBoneWorld).xyz;
	vout.Tangent = mul(vin.Tangent, (float3x3)mtxVertexToBoneWorld).xyz;
	vout.BiTangent = mul(vin.BiTangent, (float3x3)mtxVertexToBoneWorld).xyz;

	vout.PosH = mul(float4(vout.PosW, 1.0f), gViewProj);
	vout.TexC = vin.TexC;

	return(vout);
}


float4 PSSkinnedMesh(SkinnedMeshVertexOut pin) : SV_Target
{
	float4 diffuseAlbedo = gDiffuseMap.Sample(gSamLinear, pin.TexC);
	/*float a = gmtxBoneOffsets[0][0];
	float b = gmtxBoneOffsets[0][1];
	float c = gmtxBoneOffsets[0][2];
	
	float4 diffuseAlbedo = float4(a, b, c, 1.0f);*/

	return diffuseAlbedo;
}




