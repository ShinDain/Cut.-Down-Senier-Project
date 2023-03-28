#include "Global.h"

void LoadTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const wchar_t* texFileName)
{
	auto texMap = std::make_shared<Texture>();
	wcscpy_s(texMap->FileName, texFileName);
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(
		pd3dDevice, pd3dCommandList,
		texMap->FileName,
		texMap->Resource, texMap->UploadHeap));

	g_CachingTexture.emplace_back(std::move(texMap));
}

std::shared_ptr<Texture> FindReplicatedTexture(const wchar_t* pstrTextureName)
{
	for (int i = 0; i < g_CachingTexture.size(); ++i)
	{
		if (!wcscmp(g_CachingTexture[i]->FileName, pstrTextureName))
			return g_CachingTexture[i];
	}

	return NULL;
}

int ReadintegerFromFile(FILE* pInFile)
{
	int nValue = 0;
	UINT nReads = (UINT)::fread(&nValue, sizeof(int), 1, pInFile);
	return nValue;
}

float ReadFloatFromFile(FILE* pInFile)
{
	float fValue = 0;
	UINT nReads = (UINT)::fread(&fValue, sizeof(float), 1, pInFile);
	return fValue;
}

BYTE ReadStringFromFile(FILE* pInFile, char* pstrToken)
{
	BYTE nStrLength = 0;
	UINT nReads = 0;
	nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
	nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
	pstrToken[nStrLength] = '\0';

	return nStrLength;
}

void CreateVertexBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	Microsoft::WRL::ComPtr<ID3D12Resource>* pBufferGPU,
	Microsoft::WRL::ComPtr<ID3D12Resource>* pBufferUploader,
	UINT bufferByteSize,
	UINT strideInBytes,
	D3D12_VERTEX_BUFFER_VIEW* pVertexBufferView, void* pData)
{
	*pBufferGPU = d3dUtil::CreateDefaultBuffer(
		pd3dDevice, pd3dCommandList,
		pData, bufferByteSize,
		*pBufferUploader);

	pVertexBufferView->BufferLocation = (*pBufferGPU)->GetGPUVirtualAddress();
	pVertexBufferView->StrideInBytes = strideInBytes;
	pVertexBufferView->SizeInBytes = bufferByteSize;
}

void CreateIndexBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	Microsoft::WRL::ComPtr<ID3D12Resource>* pBufferGPU,
	Microsoft::WRL::ComPtr<ID3D12Resource>* pBufferUploader,
	UINT bufferByteSize,DXGI_FORMAT indexbufferFormat,
	D3D12_INDEX_BUFFER_VIEW* pIndexBufferView, void* pData)
{
	*pBufferGPU = d3dUtil::CreateDefaultBuffer(
		pd3dDevice, pd3dCommandList,
		pData, bufferByteSize,
		*pBufferUploader);

	pIndexBufferView->BufferLocation = (*pBufferGPU)->GetGPUVirtualAddress();
	pIndexBufferView->Format = indexbufferFormat;
	pIndexBufferView->SizeInBytes = bufferByteSize;
}

void SynchronizeResourceTransition(ID3D12GraphicsCommandList* pd3dCommandList, ID3D12Resource* pd3dResource, D3D12_RESOURCE_STATES d3dStateBefore, D3D12_RESOURCE_STATES d3dStateAfter)
{
	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource = pd3dResource;
	d3dResourceBarrier.Transition.StateBefore = d3dStateBefore;
	d3dResourceBarrier.Transition.StateAfter = d3dStateAfter;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
}

