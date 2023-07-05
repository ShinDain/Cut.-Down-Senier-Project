//***************************************************************************************
// Default.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Default shader, currently supports lighting.
//***************************************************************************************

// Include common HLSL code.
#include "Common.hlsl"

struct VertexIn
{
    float3 PosL    : POSITION;
    float2 TexC    : TEXCOORD;
};

struct VertexOut
{
    float4 PosH    : SV_POSITION;
    float4 PosW    : POSITION;
    float2 TexC    : TEXCOORD;
};

cbuffer cbPerCut : register(b6)
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
    VertexOut vout = (VertexOut)0.0f;

   // MaterialData matData = gMaterialData[gMaterialIndex];

    // Transform to world space.
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);

    // Transform to homogeneous clip space.
    vout.PosH = mul(posW, gViewProj);

    // Output vertex attributes for interpolation across triangle.
    // float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);

    vout.TexC = vin.TexC;

    return vout;
}

void PS(VertexOut pin)
{
    //if (dot(pin.PosW, PlaneNormal_1 * (PlaneDirection_1)) < PlaneDistance_1 * (PlaneDirection_1))
    //{
    //    // Alpha Test
    //    clip(-1);
    //}
    //if (dot(pin.PosW, PlaneNormal_2 * (PlaneDirection_2)) < PlaneDistance_2 * (PlaneDirection_2))
    //{
    //    // Alpha Test
    //    clip(-1);
    //}
    //if (dot(pin.PosW, PlaneNormal_3 * (PlaneDirection_3)) < PlaneDistance_3 * (PlaneDirection_3))
    //{
    //    // Alpha Test
    //    clip(-1);
    //}
}


