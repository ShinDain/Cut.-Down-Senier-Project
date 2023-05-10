#include "../Header/Collider.h"

Collider::Collider()
{
}

Collider::~Collider()
{
	m_PositionBufferGPU = nullptr;
	m_PositionBufferUploader = nullptr;
	m_NormalBufferGPU = nullptr;
	m_NormalBufferUploader = nullptr;
	m_IndexBufferGPU = nullptr;
	m_IndexBufferUploader = nullptr;
}

void Collider::UpdateWorldTransform(XMFLOAT4X4& xmf4x4World)
{
	m_xmf4x4World = xmf4x4World;

	XMMATRIX World = XMLoadFloat4x4(&m_xmf4x4World);
	XMMATRIX OffsetTranslation = XMMatrixTranslation(m_xmf3OffsetPosition.x, m_xmf3OffsetPosition.y, m_xmf3OffsetPosition.z);
	XMMATRIX OffsetRotate = XMMatrixRotationRollPitchYaw(m_xmf3OffsetRotate.x, m_xmf3OffsetRotate.y, m_xmf3OffsetRotate.z);
	World = XMMatrixMultiply(XMMatrixMultiply(OffsetRotate, OffsetTranslation), World);

	XMStoreFloat4x4(&m_xmf4x4World, World);
}

void Collider::SetOffsetPosition(const XMFLOAT3& xmf3OffsetPosition)
{
	m_xmf3OffsetPosition = xmf3OffsetPosition;
}

void Collider::SetOffsetRotate(const XMFLOAT3& xmf3OffsetRotate)
{
	m_xmf3OffsetRotate = xmf3OffsetRotate;
}

const XMVECTOR Collider::GetAxis(int index) const
{
	XMVECTOR result = XMVectorZero();

	if (index == 0)
	{
		XMFLOAT3 xmf3Temp = XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
		result = XMLoadFloat3(&xmf3Temp);
	}
	else if (index == 1)
	{
		XMFLOAT3 xmf3Temp = XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);
		result = XMLoadFloat3(&xmf3Temp);
	}
	else if (index == 2)
	{
		XMFLOAT3 xmf3Temp = XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);
		result = XMLoadFloat3(&xmf3Temp);
	}
	else if (index == 3)
	{
		// Collider World Position (Center)

		XMFLOAT3 xmf3Temp = XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
		result = XMLoadFloat3(&xmf3Temp);
	}

	return result;
}

#if defined(_DEBUG)

void Collider::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	D3D12_VERTEX_BUFFER_VIEW pVertexBufferView[] = { m_PositionBufferView};
	pd3dCommandList->IASetVertexBuffers(0, 1, pVertexBufferView);
	pd3dCommandList->IASetPrimitiveTopology(m_PrimitiveTopology);

	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);

	pd3dCommandList->IASetIndexBuffer(&m_IndexBufferView);
}

void Collider::Render(float ETime, ID3D12GraphicsCommandList* pd3dCommandList)
{
	OnPrepareRender(pd3dCommandList);

	pd3dCommandList->DrawIndexedInstanced(
		m_SubmeshGeometry.IndexCount, 1, m_SubmeshGeometry.StartIndexLocation, m_SubmeshGeometry.BaseVertexLocation, 0);
}

#endif

// =============== Collider Plane =================================

ColliderPlane::ColliderPlane(XMFLOAT3 xmf3OffsetPosition, XMFLOAT3 xmf3OffsetRotate, XMFLOAT3 xmf3Direction, float distance)
{
	m_xmf3OffsetPosition = xmf3OffsetPosition;
	m_xmf3OffsetRotate = xmf3OffsetRotate;
	m_xmf3Direction = xmf3Direction;
	m_distance = distance;
}

ColliderPlane::~ColliderPlane()
{
}

void ColliderPlane::BuildMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	std::vector<XMFLOAT3> Positions;
	std::vector<std::uint16_t> Indices;

	Positions.resize(4);

	float w = 0.5;
	float h = 0.5;
	float d = 0.5;

	if (m_xmf3Direction.x == 1)
	{
		Positions =
		{
			XMFLOAT3(0, -h, -d),
			XMFLOAT3(0, +h, -d),
			XMFLOAT3(0, +h, +d),
			XMFLOAT3(0, -h, +d)
		};
	}
	else if	(m_xmf3Direction.x == -1)
	{
		Positions =
		{
			XMFLOAT3(0, -h, +d),
			XMFLOAT3(0, +h, +d),
			XMFLOAT3(0, +h, -d),
			XMFLOAT3(0, -h, -d)
		};
	}
	else if (m_xmf3Direction.y == 1)
	{
		Positions =
		{
			XMFLOAT3(-w, 0, -d),
			XMFLOAT3(-w, 0, +d),
			XMFLOAT3(+w, 0, +d),
			XMFLOAT3(+w, 0, -d)
		};
	}
	else if (m_xmf3Direction.y == -1)
	{
		Positions =
		{
			XMFLOAT3(-w, 0, +d),
			XMFLOAT3(-w, 0, -d),
			XMFLOAT3(+w, 0, -d),
			XMFLOAT3(+w, 0, +d)
		};
	}
	else if (m_xmf3Direction.z == 1)
	{
		Positions =
		{
			XMFLOAT3(-w, -h, 0),
			XMFLOAT3(-w, +h, 0),
			XMFLOAT3(+w, +h, 0),
			XMFLOAT3(+w, -h, 0)
		};
	}
	else if (m_xmf3Direction.z == -1)
	{
		Positions =
		{
			XMFLOAT3(+w, -h, 0),
			XMFLOAT3(+w, +h, 0),
			XMFLOAT3(-w, +h, 0),
			XMFLOAT3(-w, -h, 0)
		};
	}

	Indices.resize(6);

	Indices =
	{
		// ¾Õ¸é
		0,1,2,
		0,2,3		
	};

	const UINT positionBufferByteSize = (UINT)Positions.size() * sizeof(XMFLOAT3);
	const UINT indexBufferByteSize = (UINT)Indices.size() * sizeof(std::uint_fast16_t);

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

	m_SubmeshGeometry = subMesh;
}

// =============== Collider Box =================================

ColliderBox::ColliderBox(XMFLOAT3 xmf3OffsetPosition, XMFLOAT3 xmf3OffsetRotate, XMFLOAT3 xmf3Extents)
{
	m_xmf3OffsetPosition = xmf3OffsetPosition;
	m_xmf3OffsetRotate = xmf3OffsetRotate;
	m_xmf3Extents = xmf3Extents;
}

ColliderBox::~ColliderBox()
{
}

void ColliderBox::BuildMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	std::vector<XMFLOAT3> Positions;
	std::vector<std::uint16_t> Indices;

	Positions.resize(24);

	float w = m_xmf3Extents.x;
	float h = m_xmf3Extents.y;
	float d = m_xmf3Extents.z;

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
	const UINT indexBufferByteSize = (UINT)Indices.size() * sizeof(std::uint_fast16_t);

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

	m_SubmeshGeometry = subMesh;
}

// =============== Collider Sphere =================================

ColliderSphere::ColliderSphere(XMFLOAT3 xmf3OffsetPosition, float radius)
{
	m_xmf3OffsetPosition = xmf3OffsetPosition;
	m_Radius = radius;
}

ColliderSphere::~ColliderSphere()
{
}
