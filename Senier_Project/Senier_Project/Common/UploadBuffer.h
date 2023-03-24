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

		// ��� ���� ���ڴ� 256�� ��� ũ�⿩�� �Ѵ�.
		// �̰��� �ϵ��� 256����� ��밡���ϱ� ����
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


		// Resource�� upmap�� ���� �ʿ�� ����. �ֳ��ϸ�, �츮�� GPU�� 
		// Resource�� ����� ��, �����ؼ� �ȵǱ� �����̴�. (�츮�� ����ȭ ����� ����ؾ��Ѵ�.)
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



