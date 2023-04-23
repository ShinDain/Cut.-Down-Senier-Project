#include "RigidCollider.h"

///////////////// Ray (±¤¼±) /////////////////

Ray::Ray(XMFLOAT3 xmf3Center, XMFLOAT3 xmf3Direction, float Length, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_xmf3Center = xmf3Center;
	m_xmf3Direction = xmf3Direction;
	m_Length = Length;

#if defined(_DEBUG) | defined(DEBUG)
	BuildMesh(pd3dDevice, pd3dCommandList);

#endif

}

Ray::~Ray()
{
}

void Ray::BuildMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	std::vector<XMFLOAT3> Positions;

	Positions.resize(24);

	XMFLOAT3 endPoint = m_xmf3Direction;
	endPoint.x *= m_Length;
	endPoint.y *= m_Length;
	endPoint.z *= m_Length;

	Positions =
	{
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(endPoint.x, endPoint.y, endPoint.z)
	};

	const UINT positionBufferByteSize = (UINT)Positions.size() * sizeof(XMFLOAT3);

	CreateVertexBuffer(pd3dDevice, pd3dCommandList,
		&m_PositionBufferGPU, &m_PositionBufferUploader,
		positionBufferByteSize, sizeof(XMFLOAT3),
		&m_PositionBufferView, Positions.data());
}

void Ray::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	D3D12_VERTEX_BUFFER_VIEW pVertexBufferView[] = { m_PositionBufferView };
	pd3dCommandList->IASetVertexBuffers(0, 1, pVertexBufferView);
	pd3dCommandList->IASetPrimitiveTopology(m_PrimitiveTopology);
}

void Ray::Render(float ETime, ID3D12GraphicsCommandList* pd3dCommandList)
{
	OnPrepareRender(pd3dCommandList);

	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);

	pd3dCommandList->DrawInstanced(2, 1, 0, 0);
}

///////////////// RigidCollider /////////////////

RigidCollider::RigidCollider(XMFLOAT3 xmf3Center, XMFLOAT3 xmf3Extents, ColliderType colliderType, float mass, 
	ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_xmf3Center = xmf3Center;
	m_xmf3Extents = xmf3Extents;
	m_ColliderType = colliderType;
	m_Mass = mass;

	m_PositionBufferView.BufferLocation = NULL;
	m_NormalBufferView.BufferLocation = NULL;
	m_IndexBufferView.BufferLocation = NULL;

	if (colliderType == Collider_Type_Box)
	{
#if defined(_DEBUG) | defined(DEBUG)

		BuildMesh(pd3dDevice, pd3dCommandList);

#endif
	}
	else
	{

	}

	CalculateRotateInertiaMatrix();
}

RigidCollider::~RigidCollider()
{
}

void RigidCollider::BuildMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
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
			   
		XMFLOAT3( 0.0f, 1.0f, 0.0f),
		XMFLOAT3( 0.0f, 1.0f, 0.0f),
		XMFLOAT3( 0.0f, 1.0f, 0.0f),
		XMFLOAT3( 0.0f, 1.0f, 0.0f),
			   
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

void RigidCollider::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	D3D12_VERTEX_BUFFER_VIEW pVertexBufferView[2] = { m_PositionBufferView, m_NormalBufferView };
	pd3dCommandList->IASetVertexBuffers(0, 2, pVertexBufferView);
	pd3dCommandList->IASetPrimitiveTopology(m_PrimitiveTopology);

	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);

	pd3dCommandList->IASetIndexBuffer(&m_IndexBufferView);
}

void RigidCollider::Update(float ETime)
{
	BoundingOrientedBox tmp =  BoundingOrientedBox(m_xmf3Center, m_xmf3Extents, XMFLOAT4(0,0,0,1));

	tmp.Transform(tmp, XMLoadFloat4x4(&m_xmf4x4World));

	m_bIsOverlapped = false;
}

void RigidCollider::Render(float ETime, ID3D12GraphicsCommandList* pd3dCommandList)
{
	OnPrepareRender(pd3dCommandList);

	pd3dCommandList->DrawIndexedInstanced(
		m_SubmeshGeometry.IndexCount, 1, m_SubmeshGeometry.StartIndexLocation, m_SubmeshGeometry.BaseVertexLocation, 0);
}

void RigidCollider::CalculateRotateInertiaMatrix()
{
	if (m_ColliderType == Collider_Type_Box)
	{
		XMFLOAT4X4 xmf4x4RotateInertia = MathHelper::identity4x4();
		XMFLOAT3 colliderExtents = m_xmf3Extents;
		float objectMass = m_Mass;

		xmf4x4RotateInertia._11 = objectMass * (colliderExtents.y * colliderExtents.y) + (colliderExtents.z * colliderExtents.z) / 12;
		xmf4x4RotateInertia._22 = objectMass * (colliderExtents.x * colliderExtents.x) + (colliderExtents.z * colliderExtents.z) / 12;
		xmf4x4RotateInertia._33 = objectMass * (colliderExtents.y * colliderExtents.y) + (colliderExtents.x * colliderExtents.x) / 12;

		m_xmmatRotateInertia = XMLoadFloat4x4(&xmf4x4RotateInertia);
	}
}
