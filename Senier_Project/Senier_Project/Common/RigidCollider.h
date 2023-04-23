#pragma once

#include "Global.h"
#include "D3DUtil.h"
#include "GeometryGenerator.h"

enum ColliderType
{
	Collider_Type_Box,
	Collider_Type_Sphere,
	Collider_Type_Capsule,
	Collider_Type_Count
};

class Ray
{
public:
	Ray() = delete;
	Ray(XMFLOAT3 xmf3Center, XMFLOAT3 xmf3Direction, float Length, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	Ray(const Ray& rhs) = delete;
	Ray& operator=(const Ray& rhs) = delete;
	virtual ~Ray();

#if defined(_DEBUG) | defined(DEBUG)
private:

	Microsoft::WRL::ComPtr<ID3D12Resource>	 m_PositionBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource>	 m_PositionBufferUploader = nullptr;
	D3D12_VERTEX_BUFFER_VIEW				 m_PositionBufferView;

	D3D12_PRIMITIVE_TOPOLOGY m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	
public:
	void BuildMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void Render(float ETime, ID3D12GraphicsCommandList* pd3dCommandList);

private:
	void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
#endif



private:
	XMFLOAT4X4 m_xmf4x4World = MathHelper::identity4x4();
	XMFLOAT3 m_xmf3Center = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 m_xmf3Direction = { 0.0f, 1.0f, 0.0f };

	float m_Length = 5.0f;
	bool m_bIsOverlapped = false;

public:

	void SetWorld(XMFLOAT4X4 World) { m_xmf4x4World = World; }
	void SetCenter(XMFLOAT3 Center) { m_xmf3Center = Center; }
	void SetDirection(XMFLOAT3 Direction) { m_xmf3Direction = Direction; }
	void SetLength(bool Length) { m_Length = Length; }

	XMFLOAT4X4 GetWorld() { return m_xmf4x4World; }
	XMFLOAT3 GetCenter() { return m_xmf3Center; }
	XMFLOAT3 GetDirection() { return m_xmf3Direction; }
	float GetLength() { return m_Length; }

};

class RigidCollider
{
public:
	RigidCollider() = delete;
	RigidCollider(XMFLOAT3 xmf3Center, XMFLOAT3 xmf3Extents, ColliderType colliderType, float mass,
		ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	RigidCollider(const RigidCollider& rhs) = delete;
	RigidCollider& operator=(const RigidCollider& rhs) = delete;
	virtual ~RigidCollider();

	void Update(float ETime);
private:

#if defined(_DEBUG) | defined(DEBUG)
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

public:
	void BuildMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void Render(float ETime, ID3D12GraphicsCommandList* pd3dCommandList);

private:
	void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
#endif
private:
	void CalculateRotateInertiaMatrix();

private:
	float m_Mass = 1.0f;
	XMFLOAT4X4 m_xmf4x4World = MathHelper::identity4x4();
	XMFLOAT3 m_xmf3Center = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 m_xmf3Extents = { 0.5f, 0.5f, 0.5f };

	XMMATRIX m_xmmatRotateInertia = XMMatrixIdentity();
	float m_Restitution = 0.5f;

	ColliderType m_ColliderType = Collider_Type_Box;
	bool m_bIsOverlapped = false;

public:
	void SetMass(float Mass) { m_Mass = Mass; }
	void SetWorld(const XMFLOAT4X4& World) { m_xmf4x4World = World; }
	void SetCenter(const XMFLOAT3& Center) { m_xmf3Center = Center; }	
	void SetExtents(const XMFLOAT3& Extents) { m_xmf3Extents = Extents; }
	void SetColliderType(ColliderType nType) { m_ColliderType = nType; }


	const float& GetMass() { return m_Mass; }
	const XMFLOAT4X4& GetWorld() { return m_xmf4x4World; }
	const XMFLOAT3& GetCenter() { return m_xmf3Center; }
	const XMFLOAT3& GetExtents() { return m_xmf3Extents; }
	const ColliderType& GetColliderType() { return m_ColliderType; }
	const bool& GetIsOverlapped() { return m_bIsOverlapped; }
	const XMMATRIX& GetRotateInertiaMatrix() {	return m_xmmatRotateInertia;}


};
