#pragma once

#include "Global.h"
#include "D3DUtil.h"
#include "GeometryGenerator.h"

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

class Collider
{
public:
	Collider() = delete;
	Collider(XMFLOAT3 xmf3Center, XMFLOAT3 xmf3Extents, bool IsBox, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	Collider(const Collider& rhs) = delete;
	Collider& operator=(const Collider& rhs) = delete;
	virtual ~Collider();


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
	D3D12_PRIMITIVE_TOPOLOGY m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;

	SubmeshGeometry m_SubmeshGeometry;

public:
	void BuildMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	void Render(float ETime, ID3D12GraphicsCommandList* pd3dCommandList);

private:
	void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
#endif
	void Update(float ETime);

private:
	XMFLOAT4X4 m_xmf4x4World = MathHelper::identity4x4();
	XMFLOAT3 m_xmf3Center = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 m_xmf3Extents = { 0.0f, 0.0f, 0.0f };
	BoundingOrientedBox m_BoundingOrientedBox;


	bool m_bIsBox = true;
	bool m_bIsOverlapped = false;

public:
	void SetWorld(XMFLOAT4X4 World) { m_xmf4x4World = World; }
	void SetCenter(XMFLOAT3 Center) { m_xmf3Center = Center; }	
	void SetExtents(XMFLOAT3 Extents) { m_xmf3Extents = Extents; }
	void SetIsBox(bool IsBox) { m_bIsBox = IsBox; }

	XMFLOAT4X4 GetWorld() { return m_xmf4x4World; }
	XMFLOAT3 GetCenter() { return m_xmf3Center; }
	XMFLOAT3 GetExtents() { return m_xmf3Extents; }
	bool GetIsBox() { return m_bIsBox; }
	BoundingOrientedBox& GetOrientedBox() { return m_BoundingOrientedBox; }
	bool GetIsOverlapped() { return m_bIsOverlapped; }

};
