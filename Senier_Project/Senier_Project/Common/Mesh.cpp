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

    float w = 10.f / 2;
    float h = 10.f / 2;
    float d = 10.f / 2;

	Vertices =
	{
		Vertex({XMFLOAT3(-w, -h, -d), PackedVector::XMCOLOR(Colors::White)}),
		Vertex({XMFLOAT3(-w, +h, -d), PackedVector::XMCOLOR(Colors::White)}),
		Vertex({XMFLOAT3(+w, +h, -d), PackedVector::XMCOLOR(Colors::White)}),
		Vertex({XMFLOAT3(+w, -h, -d), PackedVector::XMCOLOR(Colors::White)}),

		Vertex({XMFLOAT3(-w, -h, +d), PackedVector::XMCOLOR(Colors::Black)}),
		Vertex({XMFLOAT3(+w, -h, +d), PackedVector::XMCOLOR(Colors::Black)}),
		Vertex({XMFLOAT3(+w, +h, +d), PackedVector::XMCOLOR(Colors::Black)}),
		Vertex({XMFLOAT3(-w, +h, +d), PackedVector::XMCOLOR(Colors::Black)}),

		Vertex({XMFLOAT3(-w, +h, -d), PackedVector::XMCOLOR(Colors::Red)}),
		Vertex({XMFLOAT3(-w, +h, +d), PackedVector::XMCOLOR(Colors::Red)}),
		Vertex({XMFLOAT3(+w, +h, +d), PackedVector::XMCOLOR(Colors::Red)}),
		Vertex({XMFLOAT3(+w, +h, -d), PackedVector::XMCOLOR(Colors::Red)}),

		Vertex({XMFLOAT3(-w, -h, -d), PackedVector::XMCOLOR(Colors::Aquamarine)}),
		Vertex({XMFLOAT3(+w, -h, -d), PackedVector::XMCOLOR(Colors::Aquamarine)}),
		Vertex({XMFLOAT3(+w, -h, +d), PackedVector::XMCOLOR(Colors::Aquamarine)}),
		Vertex({XMFLOAT3(-w, -h, +d), PackedVector::XMCOLOR(Colors::Aquamarine)}),

		Vertex({XMFLOAT3(-w, -h, +d), PackedVector::XMCOLOR(Colors::Indigo)}),
		Vertex({XMFLOAT3(-w, +h, +d), PackedVector::XMCOLOR(Colors::Indigo)}),
		Vertex({XMFLOAT3(-w, +h, -d), PackedVector::XMCOLOR(Colors::Indigo)}),
		Vertex({XMFLOAT3(-w, -h, -d), PackedVector::XMCOLOR(Colors::Indigo)}),

		Vertex({XMFLOAT3(+w, -h, -d), PackedVector::XMCOLOR(Colors::LawnGreen)}),
		Vertex({XMFLOAT3(+w, +h, -d), PackedVector::XMCOLOR(Colors::LawnGreen)}),
		Vertex({XMFLOAT3(+w, +h, +d), PackedVector::XMCOLOR(Colors::LawnGreen)}),
		Vertex({XMFLOAT3(+w, -h, +d), PackedVector::XMCOLOR(Colors::LawnGreen)})
	};

	Indices.resize(36);

	Indices =
	{
		// 앞면
		0,1,2,
		0,2,3,

		// 뒷면
		4,5,6,
		4,6,7,

		// 왼쪽 면
		8,9,10,
		8,10,11,

		// 오른쪽 면
		12,13,14,
		12,14,15,

		// 윗면
		16,17,18,
		16,18,19,

		// 아랫면
		20,21,22,
		20,22,23
	};

	const UINT vbByteSize = (UINT)Vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)Indices.size() * sizeof(std::uint_fast16_t);

	mName = "boxGeo";

	// CPU에 정점 정보 저장, 사용해본적은 아직 없음
	ThrowIfFailed(D3DCreateBlob(vbByteSize, &mVertexBufferCPU));
	CopyMemory(mVertexBufferCPU->GetBufferPointer(), Vertices.data(), vbByteSize);
	ThrowIfFailed(D3DCreateBlob(ibByteSize, &mIndexBufferCPU));
	CopyMemory(mIndexBufferCPU->GetBufferPointer(), Indices.data(), ibByteSize);

	// GPU 리소스에 정점 정보 저장
	mVertexBufferGPU = d3dUtil::CreateDefaultBuffer(
		pd3dDevice, pd3dCommandList,
		Vertices.data(), vbByteSize,
		mVertexBufferUploader);
	mIndexBufferGPU = d3dUtil::CreateDefaultBuffer(
		pd3dDevice, pd3dCommandList,
		Indices.data(), ibByteSize,
		mIndexBufferUploader);

	mVertexByteStride = sizeof(Vertex);
	mVertexBufferByteSize = vbByteSize;
	mIndexFormat = mIndexFormat;
	mIndexBufferByteSize = ibByteSize;

	SubmeshGeometry subMesh;
	subMesh.IndexCount = (UINT)Indices.size();
	subMesh.StartIndexLocation = 0;
	subMesh.BaseVertexLocation = 0;
	// subMesh.Bounds = BoundingBox()

	mDrawArgs.emplace_back(subMesh);
}

void Mesh::OnprepareRender(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList)
{
    D3D12_VERTEX_BUFFER_VIEW GeoVBView = VertexBufferView();
    D3D12_INDEX_BUFFER_VIEW GeoIBView = IndexBufferView();
    pd3dCommandList->IASetVertexBuffers(0, 1, &GeoVBView);
    pd3dCommandList->IASetIndexBuffer(&GeoIBView);
    pd3dCommandList->IASetPrimitiveTopology(mPrimitiveTopology);
}

void Mesh::Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList)
{
    for (auto begin = mDrawArgs.begin(); begin != mDrawArgs.end(); ++begin)
    {
        pd3dCommandList->DrawIndexedInstanced(
            begin->IndexCount, 1, begin->StartIndexLocation, begin->BaseVertexLocation, 0);
    }
}

D3D12_VERTEX_BUFFER_VIEW Mesh::VertexBufferView() const
{
    D3D12_VERTEX_BUFFER_VIEW vbv;
    vbv.BufferLocation = mVertexBufferGPU->GetGPUVirtualAddress();
    vbv.StrideInBytes = mVertexByteStride;
    vbv.SizeInBytes = mVertexBufferByteSize;

    return vbv;
}

D3D12_INDEX_BUFFER_VIEW Mesh::IndexBufferView() const
{
    D3D12_INDEX_BUFFER_VIEW ibv;
    ibv.BufferLocation = mIndexBufferGPU->GetGPUVirtualAddress();
    ibv.Format = mIndexFormat;
    ibv.SizeInBytes = mIndexBufferByteSize;

    return ibv;
}

void Mesh::DisposeUploaders()
{
    mVertexBufferUploader = nullptr;
    mIndexBufferUploader = nullptr;
}
