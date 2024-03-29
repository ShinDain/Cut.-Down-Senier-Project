#ifndef COLLIDER_H

#define COLLIDER_H

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
	std::shared_ptr<RigidBody> m_pRigidBody = nullptr;
	std::shared_ptr<BoundingSphere> m_pBoundingSphere = nullptr;

	XMFLOAT3 m_xmf3Position = XMFLOAT3(0, 0, 0);
	XMFLOAT3 m_xmf3Scale = XMFLOAT3(1, 1, 1);

	XMFLOAT4X4 m_xmf4x4World = MathHelper::identity4x4();

	bool m_bIsActive = true;
	bool m_bCharacterActive = true;
	float m_Intersect = 0;

public:
	virtual void UpdateWorldTransform();

	void SetWorld(XMFLOAT4X4 xmf4x4World) { m_xmf4x4World = xmf4x4World; }

	const XMVECTOR GetAxis(int index) const;
	const XMFLOAT4X4& GetWorld() const { return m_xmf4x4World; }

	std::shared_ptr<RigidBody> GetBody() const { return m_pRigidBody; }

	void SetIsActive(bool bIsActive) { m_bIsActive = bIsActive; }
 	const bool GetIsActive() const { return m_bIsActive; }
	void SetCharacterActive(bool bIsActive) { m_bCharacterActive = bIsActive; }
	const bool GetCharacterActive() const { return m_bCharacterActive; }
	void SetIntersect(float intersect) { m_Intersect = intersect; }
	const float GetIntersect() { return m_Intersect; }

	void SetPhysics(bool bPhysics) const { m_pRigidBody->SetPhysics(bPhysics); }
	bool GetPhysics() const { return m_pRigidBody->GetPhysics(); }

	std::shared_ptr<BoundingSphere> GetBoundingSphere() const { return m_pBoundingSphere; }

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

	// 왜인지 오류가 발생
	D3D12_PRIMITIVE_TOPOLOGY m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	//D3D12_PRIMITIVE_TOPOLOGY m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	SubmeshGeometry m_SubmeshGeometry;

	std::unique_ptr<UploadBuffer<ObjConstant>> m_pObjectCB = nullptr;

public:
	virtual void BuildMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {};
	virtual void Render(float ETime, ID3D12GraphicsCommandList* pd3dCommandList);

	// 여기 구현 아직 안됨,=====================================
	// 여기 구현 아직 안됨,=====================================
	// 여기 구현 아직 안됨,=====================================
	// 여기 구현 아직 안됨,=====================================
	void Destroy() {}

private:
	void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
#endif
};

class ColliderPlane : public Collider
{
public:
	ColliderPlane() = delete;
	ColliderPlane(std::shared_ptr<RigidBody>pBody, XMFLOAT3 xmf3Direction, float distance);
	ColliderPlane(const ColliderPlane& rhs) = delete;
	ColliderPlane& operator=(const ColliderPlane& rhs) = delete;
	virtual ~ColliderPlane();
#if defined(_DEBUG) | defined(DEBUG)
	virtual void BuildMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {};
#endif
	virtual void Render(float ETime, ID3D12GraphicsCommandList* pd3dCommandList) {};

	virtual void CalculateRotateInertiaMatrix();

private:
	XMFLOAT3 m_xmf3Direction = XMFLOAT3(0, 0, 0);

	// 원점으로 부터 Direction 방향으로의 거리
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
	ColliderBox(std::shared_ptr<RigidBody>pBody, XMFLOAT3 xmf3Extents);
	ColliderBox(const ColliderBox& rhs) = delete;
	ColliderBox& operator=(const ColliderBox& rhs) = delete;
	virtual ~ColliderBox();
#if defined(_DEBUG) | defined(DEBUG)
	virtual void BuildMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
#endif

	virtual void CalculateRotateInertiaMatrix();
	virtual void UpdateWorldTransform();

private:
	XMFLOAT3 m_xmf3Extents = XMFLOAT3(0, 0, 0);

	std::shared_ptr<BoundingOrientedBox> m_pOBB;

public:
	void SetExtents(const XMFLOAT3& xmf3Extents) { m_xmf3Extents = xmf3Extents; }
	const XMFLOAT3& GetExtents() const { return m_xmf3Extents; }

	std::shared_ptr<BoundingOrientedBox> GetOBB() const { return m_pOBB; }
};

class ColliderSphere : public Collider
{
public:
	ColliderSphere() = delete;
	ColliderSphere(std::shared_ptr<RigidBody>pBody, float radius);
	ColliderSphere(const ColliderSphere& rhs) = delete;
	ColliderSphere& operator=(const ColliderSphere& rhs) = delete;
	virtual ~ColliderSphere();

#if defined(_DEBUG) | defined(DEBUG)
	virtual void BuildMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {};
#endif

	//virtual void CalculateRotateInertiaMatrix();

private:
	float m_Radius = 0;

public:
	void SetRadius(const float Radius) { m_Radius = Radius; }
	const float GetRadius() const { return m_Radius; }
};

#endif // !COLLIDER_H