#include "../Header/Collider.h"

Collider::Collider()
{
}

Collider::~Collider()
{
}

void Collider::UpdateWorldTransform()
{
	m_xmf4x4World = m_pRigidBody->GetWorld();

	XMMATRIX World = XMLoadFloat4x4(&m_xmf4x4World);
	XMMATRIX OffsetTranslation = XMMatrixTranslation(m_xmf3OffsetPosition.x, m_xmf3OffsetPosition.y, m_xmf3OffsetPosition.z);
	XMMATRIX OffsetRotate = XMMatrixRotationRollPitchYaw(m_xmf3OffsetRotate.x, m_xmf3OffsetRotate.y, m_xmf3OffsetRotate.z);
	World = XMMatrixMultiply(XMMatrixMultiply(OffsetRotate, OffsetTranslation), World);

	XMStoreFloat4x4(&m_xmf4x4World, World);
}

void Collider::SetOffsetPosition(const XMFLOAT3& xmf3OffsetPosition)
{
	m_xmf3OffsetPosition = xmf3OffsetPosition;
	UpdateWorldTransform();
}

void Collider::SetOffsetRotate(const XMFLOAT3& xmf3OffsetRotate)
{
	m_xmf3OffsetRotate = xmf3OffsetRotate;
	UpdateWorldTransform();
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
	D3D12_VERTEX_BUFFER_VIEW pVertexBufferView[2] = { m_PositionBufferView, m_NormalBufferView };
	pd3dCommandList->IASetVertexBuffers(0, 2, pVertexBufferView);
	pd3dCommandList->IASetPrimitiveTopology(m_PrimitiveTopology);

	XMFLOAT4X4 tmp = m_pRigidBody->GetWorld();

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

	if (m_pRigidBody)
	{
		CalculateRotateInertiaMatrix();
		UpdateWorldTransform();
	}
}

ColliderPlane::~ColliderPlane()
{
}

void ColliderPlane::CalculateRotateInertiaMatrix()
{
	XMFLOAT4X4 xmf4x4RotateInertia = MathHelper::identity4x4();
	XMFLOAT3 colliderExtents = XMFLOAT3(m_distance, 0.1, m_distance);
	float objectMass = m_pRigidBody->GetMass();

	xmf4x4RotateInertia._11 = objectMass * (colliderExtents.y * colliderExtents.y) + (colliderExtents.z * colliderExtents.z) / 12;
	xmf4x4RotateInertia._22 = objectMass * (colliderExtents.x * colliderExtents.x) + (colliderExtents.z * colliderExtents.z) / 12;
	xmf4x4RotateInertia._33 = objectMass * (colliderExtents.y * colliderExtents.y) + (colliderExtents.x * colliderExtents.x) / 12;

	m_pRigidBody->SetRotateInertia(xmf4x4RotateInertia);
}

void ColliderPlane::BuildMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	std::vector<XMFLOAT3> Positions;
	std::vector<XMFLOAT3> Normal;
	std::vector<std::uint16_t> Indices;

	Positions.resize(24);
	Normal.resize(24);

	float w = 100.0f;
	float h = 0.1f;
	float d = 100.0f;

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
	Normal =
	{
		XMFLOAT3(0.0f, 0.0f, -1.0f),
		XMFLOAT3(0.0f, 0.0f, -1.0f),
		XMFLOAT3(0.0f, 0.0f, -1.0f),
		XMFLOAT3(0.0f, 0.0f, -1.0f),

		XMFLOAT3(0.0f, 0.0f, 1.0f),
		XMFLOAT3(0.0f, 0.0f, 1.0f),
		XMFLOAT3(0.0f, 0.0f, 1.0f),
		XMFLOAT3(0.0f, 0.0f, 1.0f),

		XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT3(0.0f, 1.0f, 0.0f),

		XMFLOAT3(0.0f, -1.0f, 0.0f),
		XMFLOAT3(0.0f, -1.0f, 0.0f),
		XMFLOAT3(0.0f, -1.0f, 0.0f),
		XMFLOAT3(0.0f, -1.0f, 0.0f),

		XMFLOAT3(-1.0f, 0.0f, 0.0f),
		XMFLOAT3(-1.0f, 0.0f, 0.0f),
		XMFLOAT3(-1.0f, 0.0f, 0.0f),
		XMFLOAT3(-1.0f, 0.0f, 0.0f),

		XMFLOAT3(1.0f, 0.0f, 0.0f),
		XMFLOAT3(1.0f, 0.0f, 0.0f),
		XMFLOAT3(1.0f, 0.0f, 0.0f),
		XMFLOAT3(1.0f, 0.0f, 0.0f)
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
	const UINT NormalBufferByteSize = (UINT)Normal.size() * sizeof(XMFLOAT3);
	const UINT indexBufferByteSize = (UINT)Indices.size() * sizeof(std::uint_fast16_t);

	CreateVertexBuffer(pd3dDevice, pd3dCommandList,
		&m_PositionBufferGPU, &m_PositionBufferUploader,
		positionBufferByteSize, sizeof(XMFLOAT3),
		&m_PositionBufferView, Positions.data());

	CreateVertexBuffer(pd3dDevice, pd3dCommandList,
		&m_NormalBufferGPU, &m_NormalBufferUploader,
		NormalBufferByteSize, sizeof(XMFLOAT3),
		&m_NormalBufferView, Normal.data());

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

ColliderBox::ColliderBox(RigidBody* pBody, XMFLOAT3 xmf3OffsetPosition, XMFLOAT3 xmf3OffsetRotate, XMFLOAT3 xmf3Extents)
{
	m_pRigidBody = pBody;
	m_xmf3OffsetPosition = xmf3OffsetPosition;
	m_xmf3OffsetRotate = xmf3OffsetRotate;
	m_xmf3Extents = xmf3Extents;

	CalculateRotateInertiaMatrix();
	UpdateWorldTransform();
}

ColliderBox::~ColliderBox()
{
}

void ColliderBox::CalculateRotateInertiaMatrix()
{
	XMFLOAT4X4 xmf4x4RotateInertia = MathHelper::identity4x4();
	XMFLOAT3 colliderExtents = m_xmf3Extents;
	float objectMass = m_pRigidBody->GetMass();

	xmf4x4RotateInertia._11 = objectMass * (colliderExtents.y * colliderExtents.y) + (colliderExtents.z * colliderExtents.z) / 12;
	xmf4x4RotateInertia._22 = objectMass * (colliderExtents.x * colliderExtents.x) + (colliderExtents.z * colliderExtents.z) / 12;
	xmf4x4RotateInertia._33 = objectMass * (colliderExtents.y * colliderExtents.y) + (colliderExtents.x * colliderExtents.x) / 12;

	m_pRigidBody->SetRotateInertia(xmf4x4RotateInertia);
}

void ColliderBox::BuildMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	std::vector<XMFLOAT3> Positions;
	std::vector<XMFLOAT3> Normal;
	std::vector<std::uint16_t> Indices;

	Positions.resize(24);
	Normal.resize(24);

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
	Normal =
	{
		XMFLOAT3(0.0f, 0.0f, -1.0f),
		XMFLOAT3(0.0f, 0.0f, -1.0f),
		XMFLOAT3(0.0f, 0.0f, -1.0f),
		XMFLOAT3(0.0f, 0.0f, -1.0f),

		XMFLOAT3(0.0f, 0.0f, 1.0f),
		XMFLOAT3(0.0f, 0.0f, 1.0f),
		XMFLOAT3(0.0f, 0.0f, 1.0f),
		XMFLOAT3(0.0f, 0.0f, 1.0f),

		XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT3(0.0f, 1.0f, 0.0f),

		XMFLOAT3(0.0f, -1.0f, 0.0f),
		XMFLOAT3(0.0f, -1.0f, 0.0f),
		XMFLOAT3(0.0f, -1.0f, 0.0f),
		XMFLOAT3(0.0f, -1.0f, 0.0f),

		XMFLOAT3(-1.0f, 0.0f, 0.0f),
		XMFLOAT3(-1.0f, 0.0f, 0.0f),
		XMFLOAT3(-1.0f, 0.0f, 0.0f),
		XMFLOAT3(-1.0f, 0.0f, 0.0f),

		XMFLOAT3(1.0f, 0.0f, 0.0f),
		XMFLOAT3(1.0f, 0.0f, 0.0f),
		XMFLOAT3(1.0f, 0.0f, 0.0f),
		XMFLOAT3(1.0f, 0.0f, 0.0f)
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
	const UINT NormalBufferByteSize = (UINT)Normal.size() * sizeof(XMFLOAT3);
	const UINT indexBufferByteSize = (UINT)Indices.size() * sizeof(std::uint_fast16_t);

	CreateVertexBuffer(pd3dDevice, pd3dCommandList,
		&m_PositionBufferGPU, &m_PositionBufferUploader,
		positionBufferByteSize, sizeof(XMFLOAT3),
		&m_PositionBufferView, Positions.data());

	CreateVertexBuffer(pd3dDevice, pd3dCommandList,
		&m_NormalBufferGPU, &m_NormalBufferUploader,
		NormalBufferByteSize, sizeof(XMFLOAT3),
		&m_NormalBufferView, Normal.data());

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

ColliderSphere::ColliderSphere()
{
}

ColliderSphere::~ColliderSphere()
{
}
