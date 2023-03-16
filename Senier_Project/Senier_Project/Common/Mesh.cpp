#include "Mesh.h"

using namespace DirectX;


Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

void Mesh::BuildMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
    std::vector<Vertex> Vertices;
    std::vector<std::uint16_t> Indices;

    Vertices.resize(24);

    float w = 10.0f / 2;
    float h = 10.0f / 2;
    float d = 10.0f / 2;

	Vertices =
	{
		Vertex({XMFLOAT3(-w, -h, -d), XMFLOAT2(0.0f, 1.0f)}),
		Vertex({XMFLOAT3(-w, +h, -d), XMFLOAT2(0.0f, 0.0f)}),
		Vertex({XMFLOAT3(+w, +h, -d), XMFLOAT2(1.0f, 0.0f)}),
		Vertex({XMFLOAT3(+w, -h, -d), XMFLOAT2(1.0f, 1.0f)}),

		Vertex({XMFLOAT3(-w, -h, +d), XMFLOAT2(1.0f, 1.0f)}),
		Vertex({XMFLOAT3(+w, -h, +d), XMFLOAT2(0.0f, 1.0f)}),
		Vertex({XMFLOAT3(+w, +h, +d), XMFLOAT2(0.0f, 0.0f)}),
		Vertex({XMFLOAT3(-w, +h, +d), XMFLOAT2(1.0f, 0.0f)}),

		Vertex({XMFLOAT3(-w, +h, -d), XMFLOAT2(0.0f, 1.0f)}),
		Vertex({XMFLOAT3(-w, +h, +d), XMFLOAT2(0.0f, 0.0f)}),
		Vertex({XMFLOAT3(+w, +h, +d), XMFLOAT2(1.0f, 0.0f)}),
		Vertex({XMFLOAT3(+w, +h, -d), XMFLOAT2(1.0f, 1.0f)}),

		Vertex({XMFLOAT3(-w, -h, -d), XMFLOAT2(1.0f, 1.0f)}),
		Vertex({XMFLOAT3(+w, -h, -d), XMFLOAT2(0.0f, 1.0f)}),
		Vertex({XMFLOAT3(+w, -h, +d), XMFLOAT2(0.0f, 0.0f)}),
		Vertex({XMFLOAT3(-w, -h, +d), XMFLOAT2(1.0f, 0.0f)}),

		Vertex({XMFLOAT3(-w, -h, +d), XMFLOAT2(0.0f, 1.0f)}),
		Vertex({XMFLOAT3(-w, +h, +d), XMFLOAT2(0.0f, 0.0f)}),
		Vertex({XMFLOAT3(-w, +h, -d), XMFLOAT2(1.0f, 0.0f)}),
		Vertex({XMFLOAT3(-w, -h, -d), XMFLOAT2(1.0f, 1.0f)}),

		Vertex({XMFLOAT3(+w, -h, -d), XMFLOAT2(0.0f, 1.0f)}),
		Vertex({XMFLOAT3(+w, +h, -d), XMFLOAT2(0.0f, 0.0f)}),
		Vertex({XMFLOAT3(+w, +h, +d), XMFLOAT2(1.0f, 0.0f)}),
		Vertex({XMFLOAT3(+w, -h, +d), XMFLOAT2(1.0f, 1.0f)})
	};

	Indices.resize(36);

	Indices =
	{
		// �ո�
		0,1,2,
		0,2,3,

		// �޸�
		4,5,6,
		4,6,7,

		// ���� ��
		8,9,10,
		8,10,11,

		// ������ ��
		12,13,14,
		12,14,15,

		// ����
		16,17,18,
		16,18,19,

		// �Ʒ���
		20,21,22,
		20,22,23
	};

	const UINT vbByteSize = (UINT)Vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)Indices.size() * sizeof(std::uint_fast16_t);

	const char* tmpStr = "boxGeo";
	strcpy_s(m_Name, tmpStr);
	//m_Name = "boxGeo";

	// CPU�� ���� ���� ����, ����غ����� ���� ����
	ThrowIfFailed(D3DCreateBlob(vbByteSize, &m_VertexBufferCPU));
	CopyMemory(m_VertexBufferCPU->GetBufferPointer(), Vertices.data(), vbByteSize);
	ThrowIfFailed(D3DCreateBlob(ibByteSize, &m_IndexBufferCPU));
	CopyMemory(m_IndexBufferCPU->GetBufferPointer(), Indices.data(), ibByteSize);

	// GPU ���ҽ��� ���� ���� ����
	m_VertexBufferGPU = d3dUtil::CreateDefaultBuffer(
		pd3dDevice, pd3dCommandList,
		Vertices.data(), vbByteSize,
		m_VertexBufferUploader);
	m_IndexBufferGPU = d3dUtil::CreateDefaultBuffer(
		pd3dDevice, pd3dCommandList,
		Indices.data(), ibByteSize,
		m_IndexBufferUploader);

	m_VertexByteStride = sizeof(Vertex);
	m_VertexBufferByteSize = vbByteSize;
	m_IndexFormat = m_IndexFormat;
	m_IndexBufferByteSize = ibByteSize;

	SubmeshGeometry subMesh;
	subMesh.IndexCount = (UINT)Indices.size();
	subMesh.StartIndexLocation = 0;
	subMesh.BaseVertexLocation = 0;
	// subMesh.Bounds = BoundingBox()

	m_DrawArgs.emplace_back(subMesh);
}

void Mesh::OnprepareRender(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList)
{
    D3D12_VERTEX_BUFFER_VIEW GeoVBView = VertexBufferView();
    D3D12_INDEX_BUFFER_VIEW GeoIBView = IndexBufferView();
    pd3dCommandList->IASetVertexBuffers(0, 1, &GeoVBView);
    pd3dCommandList->IASetIndexBuffer(&GeoIBView);
    pd3dCommandList->IASetPrimitiveTopology(m_PrimitiveTopology);
}

void Mesh::Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList)
{
    for (auto begin = m_DrawArgs.begin(); begin != m_DrawArgs.end(); ++begin)
    {
        pd3dCommandList->DrawIndexedInstanced(
            begin->IndexCount, 1, begin->StartIndexLocation, begin->BaseVertexLocation, 0);
    }
}

D3D12_VERTEX_BUFFER_VIEW Mesh::VertexBufferView() const
{
    D3D12_VERTEX_BUFFER_VIEW vbv;
    vbv.BufferLocation = m_VertexBufferGPU->GetGPUVirtualAddress();
    vbv.StrideInBytes = m_VertexByteStride;
    vbv.SizeInBytes = m_VertexBufferByteSize;

    return vbv;
}

D3D12_INDEX_BUFFER_VIEW Mesh::IndexBufferView() const
{
    D3D12_INDEX_BUFFER_VIEW ibv;
    ibv.BufferLocation = m_IndexBufferGPU->GetGPUVirtualAddress();
    ibv.Format = m_IndexFormat;
    ibv.SizeInBytes = m_IndexBufferByteSize;

    return ibv;
}

void Mesh::DisposeUploaders()
{
    m_VertexBufferUploader = nullptr;
    m_IndexBufferUploader = nullptr;
}
