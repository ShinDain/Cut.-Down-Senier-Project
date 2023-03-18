#pragma once

// 정점 구조체 정의
// 상수 버퍼 구조체 정의

#include "MathHelper.h"
#include "UploadBuffer.h"

struct tmpObjConstant
{
	DirectX::XMFLOAT4X4 World = MathHelper::identity4x4();
};

struct tmpPassConstant
{
	DirectX::XMFLOAT4X4 ViewProj = MathHelper::identity4x4();
};

struct tmpMatConstant
{
	DirectX::XMFLOAT4 AlbedoColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
};

int ReadintegerFromFile(FILE* pInFile);
float ReadFloatFromFile(FILE* pInFile);
BYTE ReadStringFromFile(FILE* pInFile, char* pstrToken);


ID3D12Resource* CreateBufferResource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	void* pData, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType, D3D12_RESOURCE_STATES d3dResourceStates,
	ID3D12Resource** ppd3dUploadBuffer);

ID3D12Resource* CreateTextureResource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	void* pData, UINT nBytes, D3D12_RESOURCE_DIMENSION d3dResourceDimension, UINT nWidth, UINT nHeight,
	UINT nDepthOrArraySize, UINT nMipLevels, D3D12_RESOURCE_FLAGS d3dResourceFlags, DXGI_FORMAT dxgiFormat,
	D3D12_HEAP_TYPE d3dHeapType, D3D12_RESOURCE_STATES d3dResourceStates, ID3D12Resource** ppd3dUploadBuffer);

inline UINT64 UpdateSubresources(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	ID3D12Resource* pd3dResource, ID3D12Resource* pd3dIntermediate, UINT64 nIntermediateOffset,
	UINT nFirstSubresource, UINT nSubresources, D3D12_SUBRESOURCE_DATA* pd3dSrcData);

void SynchronizeResourceTransition(ID3D12GraphicsCommandList* pd3dCommandList, ID3D12Resource* pd3dResource,
	D3D12_RESOURCE_STATES d3dStateBefore, D3D12_RESOURCE_STATES d3dStateAfter);

template <UINT nMaxSubresources>
inline UINT64 UpdateSubresources(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	ID3D12Resource* pd3dResource, ID3D12Resource* pd3dIntermediate, UINT64 nIntermediateOffset,
	UINT nFirstSubresource, UINT nSubresources, D3D12_SUBRESOURCE_DATA* pd3dSrcData)
{
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT pnd3dLayouts[nMaxSubresources];
	UINT pnRows[nMaxSubresources];
	UINT64 pnRowSizesInBytes[nMaxSubresources];

	UINT64 nRequiredSize = 0;
	D3D12_RESOURCE_DESC d3dResourceDesc = pd3dResource->GetDesc();
	pd3dDevice->GetCopyableFootprints(&d3dResourceDesc, nFirstSubresource, nSubresources, nIntermediateOffset,
		pnd3dLayouts, pnRows, pnRowSizesInBytes, &nRequiredSize);


	return (UpdateSubresources(pd3dCommandList, pd3dResource, 
		pd3dIntermediate, nFirstSubresource, nSubresources,
		nRequiredSize, pnd3dLayouts, pnRows, pnRowSizesInBytes, pd3dSrcData));
}

