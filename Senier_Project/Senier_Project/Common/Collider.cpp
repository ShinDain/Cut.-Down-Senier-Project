#include "Collider.h"

///////////////// Ray (±¤¼±) /////////////////

Ray::Ray(XMFLOAT3 xmf3Center, XMFLOAT3 xmf3Direction)
{
}

Ray::~Ray()
{
}

void Ray::BuildMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void Ray::Render(float ETime, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

///////////////// Collider /////////////////

Collider::Collider(XMFLOAT3 xmf3Center, XMFLOAT3 xmf3Extents)
{
}

Collider::~Collider()
{
}

void Collider::BuildMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	std::vector<XMFLOAT3> Positions;
	std::vector<XMFLOAT2> TexC0;
	std::vector<std::uint32_t> Indices;

	Positions.resize(24);
	TexC0.resize(24);

	float w = 10.0f / 2;
	float h = 10.0f / 2;
	float d = 10.0f / 2;

	Positions =
	{
		XMFLOAT3(-w, -h, -d),
		XMFLOAT3(-w, +h, -d),
		XMFLOAT3(+w, +h, -d),
		XMFLOAT3(+w, -h, -d),

		XMFLOAT3(-w, -h, +d),
		XMFLOAT3(+w, -h, +d),
		XMFLOAT3(+w, +h, +d),
		XMFLOAT3(-w, +h, +d),

		XMFLOAT3(-w, +h, -d),
		XMFLOAT3(-w, +h, +d),
		XMFLOAT3(+w, +h, +d),
		XMFLOAT3(+w, +h, -d),

		XMFLOAT3(-w, -h, -d),
		XMFLOAT3(+w, -h, -d),
		XMFLOAT3(+w, -h, +d),
		XMFLOAT3(-w, -h, +d),

		XMFLOAT3(-w, -h, +d),
		XMFLOAT3(-w, +h, +d),
		XMFLOAT3(-w, +h, -d),
		XMFLOAT3(-w, -h, -d),

		XMFLOAT3(+w, -h, -d),
		XMFLOAT3(+w, +h, -d),
		XMFLOAT3(+w, +h, +d),
		XMFLOAT3(+w, -h, +d)
	};
	TexC0 =
	{
		XMFLOAT2(0.0f, 1.0f),
		XMFLOAT2(0.0f, 0.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT2(1.0f, 1.0f),

		XMFLOAT2(1.0f, 1.0f),
		XMFLOAT2(0.0f, 1.0f),
		XMFLOAT2(0.0f, 0.0f),
		XMFLOAT2(1.0f, 0.0f),

		XMFLOAT2(0.0f, 1.0f),
		XMFLOAT2(0.0f, 0.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT2(1.0f, 1.0f),

		XMFLOAT2(1.0f, 1.0f),
		XMFLOAT2(0.0f, 1.0f),
		XMFLOAT2(0.0f, 0.0f),
		XMFLOAT2(1.0f, 0.0f),

		XMFLOAT2(0.0f, 1.0f),
		XMFLOAT2(0.0f, 0.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT2(1.0f, 1.0f),

		XMFLOAT2(0.0f, 1.0f),
		XMFLOAT2(0.0f, 0.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT2(1.0f, 1.0f)
	};

	Indices.resize(36);

	Indices =
	{
		// ¾Õ¸é
		0,1,2,
		0,2,3,

		// µÞ¸é
		4,5,6,
		4,6,7,

		// ¿ÞÂÊ ¸é
		8,9,10,
		8,10,11,

		// ¿À¸¥ÂÊ ¸é
		12,13,14,
		12,14,15,

		// À­¸é
		16,17,18,
		16,18,19,

		// ¾Æ·§¸é
		20,21,22,
		20,22,23
	};

	const UINT positionBufferByteSize = (UINT)Positions.size() * sizeof(XMFLOAT3);
	const UINT texC0BufferByteSize = (UINT)TexC0.size() * sizeof(XMFLOAT2);
	const UINT indexBufferByteSize = (UINT)Indices.size() * sizeof(std::uint_fast32_t);


	CreateVertexBuffer(pd3dDevice, pd3dCommandList,
		&m_PositionBufferGPU, &m_PositionBufferUploader,
		positionBufferByteSize, sizeof(XMFLOAT3),
		&m_PositionBufferView, Positions.data());

	CreateIndexBuffer(pd3dDevice, pd3dCommandList,
		&m_IndexBufferGPU, &m_IndexBufferUploader,
		indexBufferByteSize, m_IndexFormat,
		&m_IndexBufferView, Indices.data());

	SubmeshGeometry subMesh;
	subMesh.IndexCount = (UINT)Indices.size();
	subMesh.StartIndexLocation = 0;
	subMesh.BaseVertexLocation = 0;
	// subMesh.Bounds = BoundingBox()

	m_SubmeshGeometry = subMesh;
}

void Collider::Render(float ETime, ID3D12GraphicsCommandList* pd3dCommandList)
{
}
