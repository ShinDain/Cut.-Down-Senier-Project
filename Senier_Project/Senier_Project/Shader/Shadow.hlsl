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
    float2 TexC    : TEXCOORD;
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
#ifdef ALPHA_TEST
    clip(diffuseAlbedo.a - 0.1f);
#endif
}


