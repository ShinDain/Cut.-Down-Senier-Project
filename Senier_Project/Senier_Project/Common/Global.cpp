#include "Global.h"

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

ID3D12Resource* CreateBufferResource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType, D3D12_RESOURCE_STATES d3dResourceStates, ID3D12Resource** ppd3dUploadBuffer)
{
	return(CreateTextureResource(pd3dDevice, pd3dCommandList, pData, nBytes, D3D12_RESOURCE_DIMENSION_BUFFER, nBytes, 1, 1, 1, D3D12_RESOURCE_FLAG_NONE, DXGI_FORMAT_UNKNOWN, d3dHeapType, d3dResourceStates, ppd3dUploadBuffer));
}

ID3D12Resource* CreateTextureResource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nBytes, D3D12_RESOURCE_DIMENSION d3dResourceDimension, UINT nWidth, UINT nHeight, UINT nDepthOrArraySize, UINT nMipLevels, D3D12_RESOURCE_FLAGS d3dResourceFlags, DXGI_FORMAT dxgiFormat, D3D12_HEAP_TYPE d3dHeapType, D3D12_RESOURCE_STATES d3dResourceStates, ID3D12Resource** ppd3dUploadBuffer)
{
	ID3D12Resource* pd3dBuffer = NULL;

	D3D12_HEAP_PROPERTIES d3dHeapPropertiesDesc;
	::ZeroMemory(&d3dHeapPropertiesDesc, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapPropertiesDesc.Type = d3dHeapType;
	d3dHeapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapPropertiesDesc.CreationNodeMask = 1;
	d3dHeapPropertiesDesc.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC d3dResourceDesc;
	::ZeroMemory(&d3dResourceDesc, sizeof(D3D12_RESOURCE_DESC));
	d3dResourceDesc.Dimension = d3dResourceDimension; //D3D12_RESOURCE_DIMENSION_BUFFER, D3D12_RESOURCE_DIMENSION_TEXTURE1D, D3D12_RESOURCE_DIMENSION_TEXTURE2D
	d3dResourceDesc.Alignment = (d3dResourceDimension == D3D12_RESOURCE_DIMENSION_BUFFER) ? D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT : 0;
	d3dResourceDesc.Width = (d3dResourceDimension == D3D12_RESOURCE_DIMENSION_BUFFER) ? nBytes : nWidth;
	d3dResourceDesc.Height = (d3dResourceDimension == D3D12_RESOURCE_DIMENSION_BUFFER) ? 1 : nHeight;
	d3dResourceDesc.DepthOrArraySize = (d3dResourceDimension == D3D12_RESOURCE_DIMENSION_BUFFER) ? 1 : nDepthOrArraySize;
	d3dResourceDesc.MipLevels = (d3dResourceDimension == D3D12_RESOURCE_DIMENSION_BUFFER) ? 1 : nMipLevels;
	d3dResourceDesc.Format = (d3dResourceDimension == D3D12_RESOURCE_DIMENSION_BUFFER) ? DXGI_FORMAT_UNKNOWN : dxgiFormat;
	d3dResourceDesc.SampleDesc.Count = 1;
	d3dResourceDesc.SampleDesc.Quality = 0;
	d3dResourceDesc.Layout = (d3dResourceDimension == D3D12_RESOURCE_DIMENSION_BUFFER) ? D3D12_TEXTURE_LAYOUT_ROW_MAJOR : D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = d3dResourceFlags; // D3D12_RESOURCE_FLAG_NONE

	switch (d3dHeapType)
	{
	case D3D12_HEAP_TYPE_DEFAULT:
	{
		D3D12_RESOURCE_STATES d3dResourceInitialStates = (ppd3dUploadBuffer && pData) ? D3D12_RESOURCE_STATE_COPY_DEST : d3dResourceStates;
		HRESULT hr = pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE,
			&d3dResourceDesc, d3dResourceInitialStates, NULL, __uuidof(ID3D12Resource), (void**)&pd3dBuffer);

		if (ppd3dUploadBuffer && pData)
		{
			d3dHeapPropertiesDesc.Type = D3D12_HEAP_TYPE_UPLOAD;

			d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			d3dResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
			d3dResourceDesc.Width = nBytes;
			d3dResourceDesc.Height = 1;
			d3dResourceDesc.DepthOrArraySize = 1;
			d3dResourceDesc.MipLevels = 1;
			d3dResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
			d3dResourceDesc.SampleDesc.Count = 1;
			d3dResourceDesc.SampleDesc.Quality = 0;
			d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
			hr = pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, 
				D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)ppd3dUploadBuffer);

#ifdef _WITH_MAPPING
			D3D12_RANGE d3dReadRange = { 0, 0 };
			UINT8* pBufferDataBegin = NULL;
			(*ppd3dUploadBuffer)->Map(0, &d3dReadRange, (void**)&pBufferDataBegin);
			memcpy(pBufferDataBegin, pData, nBytes);
			(*ppd3dUploadBuffer)->Unmap(0, NULL);

			pd3dCommandList->CopyResource(pd3dBuffer, *ppd3dUploadBuffer);

#else
			D3D12_SUBRESOURCE_DATA d3dSubResourceData;
			::ZeroMemory(&d3dSubResourceData, sizeof(D3D12_SUBRESOURCE_DATA));
			d3dSubResourceData.pData = pData;
			d3dSubResourceData.SlicePitch = d3dSubResourceData.RowPitch = nBytes;
			::UpdateSubresources<1>(pd3dDevice, pd3dCommandList, pd3dBuffer, *ppd3dUploadBuffer, 0, 0, 1,
				&d3dSubResourceData);
#endif

			D3D12_RESOURCE_BARRIER d3dResourceBarrier;
			::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
			d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			d3dResourceBarrier.Transition.pResource = pd3dBuffer;
			d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			d3dResourceBarrier.Transition.StateAfter = d3dResourceStates;
			d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
		}


		break;
	}
	case D3D12_HEAP_TYPE_UPLOAD:
	{
		d3dResourceStates |= D3D12_RESOURCE_STATE_GENERIC_READ;
		HRESULT hr = pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE,
			&d3dResourceDesc, d3dResourceStates, NULL, __uuidof(ID3D12Resource), (void**)&pd3dBuffer);
		if (pData)
		{
			D3D12_RANGE d3dReadRange = { 0, 0 };
			UINT8* pBufferDataBegin = NULL;
			pd3dBuffer->Map(0, &d3dReadRange, (void**)&pBufferDataBegin);
			memcpy(pBufferDataBegin, pData, nBytes);
			pd3dBuffer->Unmap(0, NULL);
		}


		break;
	}
	case D3D12_HEAP_TYPE_READBACK:
	{
		d3dResourceStates |= D3D12_RESOURCE_STATE_COPY_DEST;
		HRESULT hr = pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE, 
			&d3dResourceDesc, d3dResourceStates, NULL, __uuidof(ID3D12Resource), (void**)&pd3dBuffer);
		if (pData)
		{
			D3D12_RANGE d3dReadRange = { 0, 0 };
			UINT8* pBufferDataBegin = NULL;
			pd3dBuffer->Map(0, &d3dReadRange, (void**)&pBufferDataBegin);
			memcpy(pBufferDataBegin, pData, nBytes);
			pd3dBuffer->Unmap(0, NULL);
		}

		break;
	}
	}

	return (pd3dBuffer);
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

