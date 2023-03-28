#include "ImgObject.h"

ImgObject::ImgObject()
{
}

ImgObject::~ImgObject()
{
}

bool ImgObject::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
	int nScreenWidth, int nScreenHeight, const wchar_t* pstrTextureFileName, int nBitmapWidth, int nBitmapHeight) 
{
	m_nScreenWidth = nScreenWidth;
	m_nScreenHeight = nScreenHeight;
	m_nBitmapWidth = nBitmapWidth;
	m_nBitmapHeight = nBitmapHeight;

	bool result = BuildBufferResource(pd3dDevice, pd3dCommandList);
	if (!result)
		return false;
	m_strTextureName = pstrTextureFileName;
	LoadTexture(pd3dDevice, pd3dCommandList, pstrTextureFileName);
	m_pTexture = FindReplicatedTexture(m_strTextureName.c_str());

	UpdateBuffer(0, 0);

	return true;
}

void ImgObject::Update(const GameTimer& gt)
{
}

void ImgObject::OnResize()
{
	// Screen 변수값 조정
	// 가지고 있던 bitmap들의 크기 변경된 비율에 따라 조절
	// 버퍼 업데이트

}

void ImgObject::PrePareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	D3D12_VERTEX_BUFFER_VIEW pVertexBufferView[2] =
	{ m_DynamicPositionBufferView, m_DynamicTexC0BufferView };
	pd3dCommandList->IASetVertexBuffers(0, 2, pVertexBufferView);
	pd3dCommandList->IASetPrimitiveTopology(m_PrimitiveTopology);

	pd3dCommandList->IASetIndexBuffer(&m_IndexBufferView);
}

void ImgObject::Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList)
{
	PrePareRender(pd3dCommandList);
	pd3dCommandList->DrawIndexedInstanced(m_nIndexCnt, 1, 0, 0, 0);
}

bool ImgObject::BuildBufferResource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_nVertexCnt = 6;
	m_nIndexCnt = 6;

	std::vector<XMFLOAT3> vPositions;
	std::vector<XMFLOAT2> vTexC0;
	std::vector<UINT16> vIndices;

	vIndices.resize(m_nVertexCnt);
	for (int i = 0; i < m_nVertexCnt; ++i)
	{
		vIndices[i] = i;
	}

	UINT nPositionBufferByteSize = m_nVertexCnt * sizeof(XMFLOAT3);
	UINT nTexC0BufferByteSize = m_nVertexCnt * sizeof(XMFLOAT2);
	UINT nIndexBufferByteSize = m_nIndexCnt * sizeof(uint_fast16_t);

	m_DynamicPositionBuffer = std::make_unique<UploadBuffer<XMFLOAT3>>(pd3dDevice, m_nVertexCnt, false);
	m_DynamicPositionBufferView.BufferLocation = m_DynamicPositionBuffer->Resource()->GetGPUVirtualAddress();
	m_DynamicPositionBufferView.SizeInBytes = nPositionBufferByteSize;
	m_DynamicPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);

	m_DynamicTexC0Buffer = std::make_unique<UploadBuffer<XMFLOAT2>>(pd3dDevice, m_nVertexCnt, false);
	m_DynamicTexC0BufferView.BufferLocation = m_DynamicTexC0Buffer->Resource()->GetGPUVirtualAddress();
	m_DynamicTexC0BufferView.SizeInBytes = nTexC0BufferByteSize;
	m_DynamicTexC0BufferView.StrideInBytes = sizeof(XMFLOAT2);

	CreateIndexBuffer(pd3dDevice, pd3dCommandList,
		&m_IndexBufferGPU, &m_IndexBufferUploader,
		nIndexBufferByteSize, m_IndexFormat,
		&m_IndexBufferView, vIndices.data());


	return true;
}

bool ImgObject::UpdateBuffer(int nPosX, int nPosY)
{
	float left, right, top, bottom;

	if ((nPosX == m_nPreviousPosX) && (nPosY == m_nPreviousPosY))
		return true;

	m_nPreviousPosX = nPosX;
	m_nPreviousPosY = nPosY;

	left = (float)((m_nScreenWidth / 2) * -1) + (float)nPosX;
	right = left + m_nBitmapWidth;
	top = (float)((m_nScreenHeight / 2)) - (float)nPosY;
	bottom = top - m_nBitmapHeight;

	std::vector<XMFLOAT3> vPositions;
	std::vector<XMFLOAT2> vTexC0;
	vPositions.resize(m_nVertexCnt);
	vTexC0.resize(m_nVertexCnt);

	vPositions[0] = XMFLOAT3(left, top, 0.0f);
	vTexC0[0] = XMFLOAT2(0.0f, 0.0f);

	vPositions[1] = XMFLOAT3(right, bottom, 0.0f);
	vTexC0[1] = XMFLOAT2(1.0f, 1.0f);

	vPositions[2] = XMFLOAT3(left, bottom, 0.0f);
	vTexC0[2] = XMFLOAT2(0.0f, 1.0f);

	vPositions[3] = XMFLOAT3(left, top, 0.0f);
	vTexC0[3] = XMFLOAT2(0.0f, 0.0f);

	vPositions[4] = XMFLOAT3(right, top, 0.0f);
	vTexC0[4] = XMFLOAT2(1.0f, 0.0f);

	vPositions[5] = XMFLOAT3(right, bottom, 0.0f);
	vTexC0[5] = XMFLOAT2(1.0f, 1.0f);

	for (int i = 0; i < m_nVertexCnt; ++i)
	{
		m_DynamicPositionBuffer->CopyData(i, vPositions[i]);
		m_DynamicTexC0Buffer->CopyData(i, vTexC0[i]);
	}
}
