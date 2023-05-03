//***************************************************************************************
// UploadBuffer.h by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************

#pragma once

#include "D3DUtil.h"

template<typename T>
class UploadBuffer
{
public:
	UploadBuffer(ID3D12Device* device, UINT elementCount, bool isConstantBuffer) :
		m_IsConstantBuffer(isConstantBuffer)
	{
		m_ElementByteSize = sizeof(T);

		// 상수 버퍼 인자는 256의 배수 크기여야 한다.
		// 이것은 하드웨어가 256배수만 사용가능하기 때문
		if (isConstantBuffer)
			m_ElementByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(T));

		D3D12_HEAP_PROPERTIES uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(m_ElementByteSize * elementCount);
		ThrowIfFailed(device->CreateCommittedResource(
			&uploadHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_UploadBuffer)));

		ThrowIfFailed(m_UploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_MappedData)));


		// Resource를 upmap을 해줄 필요는 없다. 왜냐하면, 우리는 GPU가 
		// Resource를 사용할 때, 수정해선 안되기 때문이다. (우리는 동기화 요령을 사용해야한다.)
	}

	UploadBuffer(const UploadBuffer& rhs) = delete;
	UploadBuffer& operator=(const UploadBuffer& rhs) = delete;
	~UploadBuffer()
	{
		if (m_UploadBuffer != nullptr)
			m_UploadBuffer->Unmap(0, nullptr);

		m_MappedData = nullptr;
	}

	ID3D12Resource* Resource() const
	{
		return m_UploadBuffer.Get();
	}

	void CopyData(int elementidx, const T& data)
	{
		memcpy(&m_MappedData[elementidx * m_ElementByteSize], &data, sizeof(T));
	}

	void ViewData(int elementidx)
	{
		XMFLOAT4X4 tmp;
		memcpy(&tmp, &m_MappedData[elementidx * m_ElementByteSize], sizeof(T));
	}

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_UploadBuffer;
	BYTE* m_MappedData = nullptr;

	UINT m_ElementByteSize = 0;
	bool m_IsConstantBuffer = false;
};



