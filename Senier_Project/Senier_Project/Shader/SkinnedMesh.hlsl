#include "Common.hlsl"

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
	float3 NormalL :NORMAL;
	float3 Tangent : TANGENT;
	float3 BiTangent : BITANGENT;
	int4 Indices : BONEINDEX;
	float4 Weight : BONEWEIGHT;
};

struct SkinnedMeshVertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float3 TangentW : TANGENT;
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
	}

	vout.PosW = mul(float4(vin.PosL, 1.0f), mtxVertexToBoneWorld).xyz;
	vout.NormalW = mul(vin.NormalL, (float3x3)mtxVertexToBoneWorld).xyz;
	vout.TangentW = mul(vin.Tangent, (float3x3)mtxVertexToBoneWorld).xyz;
	vout.BiTangent = mul(vin.BiTangent, (float3x3)mtxVertexToBoneWorld).xyz;

	vout.PosH = mul(float4(vout.PosW, 1.0f), gViewProj);
	vout.TexC = vin.TexC;

	return(vout);
}


float4 PSSkinnedMesh(SkinnedMeshVertexOut pin) : SV_Target
{
	// Dissolve ȿ�� ����
	float dissolveValue = gDissolveMap.Sample(gsamAnisotropicWrap, pin.TexC).r - gDissolveValue;
	clip(dissolveValue);

	float4 diffuseAlbedo = gDiffuseMap.Sample(gsamAnisotropicWrap, pin.TexC);

	pin.NormalW = normalize(pin.NormalW);
	float4 normalMapSample = gNormalMap.Sample(gsamAnisotropicWrap, pin.TexC);
	float3 bumpedNormalW = NormalSampleToWorldSpace(normalMapSample.rgb, pin.NormalW, pin.TangentW);

	float3 toEyeW = normalize(gEyePosW - pin.PosW);

	// ���� ������ �䳻 ���� �ֺ��� ��.
	float4 ambient = gAmbientLight * diffuseAlbedo;

	// ���� ����
	const float shininess = 1.0f - gRoughness;
	Material mat = { gAlbedoColor, gFresnelR0, shininess };
	float3 shadowFactor = { 1.0f, 1.0f, 1.0f };
	float4 directLight = ComputeLighting(gLights, mat, pin.PosW, bumpedNormalW, toEyeW, shadowFactor);

	float4 litColor = ambient + directLight;

	// ���� �ϴ� ��Ĵ��, �л� �������� ���ĸ� �����´�.
	litColor.a = diffuseAlbedo.a;

	return litColor * gFadeInValue;
	//return tmp;
}




