#pragma once

#include "../../Common/Header/D3DUtil.h"
#include "../../DirectXRendering/Header/Global.h"
#include "../../Common/Header/MathHelper.h"
#include "RigidBody.h"

using namespace DirectX;

class Collider
{
public:
	Collider();
	Collider(const Collider& rhs) = delete;
	Collider& operator=(const Collider& rhs) = delete;
	virtual ~Collider();

	virtual void CalculateRotateInertiaMatrix() {}

protected:
	RigidBody* m_pRigidBody = nullptr;

	XMFLOAT3 m_xmf3OffsetPosition = XMFLOAT3(0, 0, 0);
	XMFLOAT3 m_xmf3OffsetRotate = XMFLOAT3(0, 0, 0);

	XMFLOAT4X4 m_xmf4x4World = MathHelper::identity4x4();

public:
	void UpdateWorldTransform();

	void SetOffsetPosition(const XMFLOAT3& xmf3OffsetPosition);
	void SetOffsetRotate(const XMFLOAT3& xmf3OffsetRotate);

	const XMVECTOR GetAxis(int index) const;

	const XMFLOAT3& GetOffsetPosition() const { return m_xmf3OffsetPosition; }
	const XMFLOAT3& GetOffsetRotate() const { return m_xmf3OffsetRotate; }
	const XMFLOAT4X4& GetWorld() const { return m_xmf4x4World; }

	RigidBody* GetBody() const { return m_pRigidBody; }

#if defined(_DEBUG) | defined(DEBUG)
protected:
	Microsoft::WRL::ComPtr<ID3D12Resource>	 m_PositionBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource>	 m_PositionBufferUploader = nullptr;
	D3D12_VERTEX_BUFFER_VIEW				 m_PositionBufferView;

	Microsoft::WRL::ComPtr<ID3D12Resource>   m_NormalBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource>	 m_NormalBufferUploader = nullptr;
	D3D12_VERTEX_BUFFER_VIEW				 m_NormalBufferView;

	Microsoft::WRL::ComPtr<ID3D12Resource>	 m_IndexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource>	 m_IndexBufferUploader = nullptr;
	D3D12_INDEX_BUFFER_VIEW					 m_IndexBufferView;

	DXGI_FORMAT m_IndexFormat = DXGI_FORMAT_R16_UINT;

	// ������ ������ �߻�
	D3D12_PRIMITIVE_TOPOLOGY m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	//D3D12_PRIMITIVE_TOPOLOGY m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	SubmeshGeometry m_SubmeshGeometry;

public:
	virtual void BuildMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {};
	void Render(float ETime, ID3D12GraphicsCommandList* pd3dCommandList);

private:
	void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
#endif
};

class ColliderPlane : public Collider
{
public:
	ColliderPlane() = delete;
	ColliderPlane(XMFLOAT3 xmf3OffsetPosition, XMFLOAT3 xmf3OffsetRotate, XMFLOAT3 xmf3Direction, float distance);
	ColliderPlane(const ColliderPlane& rhs) = delete;
	ColliderPlane& operator=(const ColliderPlane& rhs) = delete;
	virtual ~ColliderPlane();

	virtual void CalculateRotateInertiaMatrix();
	virtual void BuildMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

private:
	XMFLOAT3 m_xmf3Direction = XMFLOAT3(0, 0, 0);

	// �������� ���� Direction ���������� �Ÿ�
	float m_distance = 0.0f;

public:
	void SetDirection(const XMFLOAT3& xmf3Direction) { m_xmf3Direction = xmf3Direction; }
	void SetDistance(const float distance) { m_distance = distance; }
	
	const XMFLOAT3& GetDirection() const { return m_xmf3Direction; }
	const float GetDistance() const { return m_distance; }
};

class ColliderBox : public Collider
{
public:
	ColliderBox() = delete;
	ColliderBox(RigidBody* pBody, XMFLOAT3 xmf3OffsetPosition, XMFLOAT3 xmf3OffsetRotate, XMFLOAT3 xmf3Extents);
	ColliderBox(const ColliderBox& rhs) = delete;
	ColliderBox& operator=(const ColliderBox& rhs) = delete;
	virtual ~ColliderBox();

	virtual void CalculateRotateInertiaMatrix();
	virtual void BuildMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
private:
	XMFLOAT3 m_xmf3Extents = XMFLOAT3(0, 0, 0);

public:
	void SetExtents(const XMFLOAT3& xmf3Extents) { m_xmf3Extents = xmf3Extents; }
	const XMFLOAT3& GetExtents() const { return m_xmf3Extents; }
};

class ColliderSphere : public Collider
{
public:
	ColliderSphere();
	ColliderSphere(const ColliderSphere& rhs) = delete;
	ColliderSphere& operator=(const ColliderSphere& rhs) = delete;
	virtual ~ColliderSphere();

	virtual void CalculateRotateInertiaMatrix() {};
	virtual void BuildMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {};
private:
	float m_Radius = 0;

public:
	void SetRadius(const float Radius) { m_Radius = Radius; }
	const float GetRadius() const { return m_Radius; }
};