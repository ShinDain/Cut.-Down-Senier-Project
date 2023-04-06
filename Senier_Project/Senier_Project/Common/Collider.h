#pragma once

#include "Global.h"
#include "D3DUtil.h"
#include "GeometryGenerator.h"

class Ray
{
public:
	Ray() = delete;
	Ray(XMFLOAT3 xmf3Center, XMFLOAT3 xmf3Direction);
	Ray(const Ray& rhs) = delete;
	Ray& operator=(const Ray& rhs) = delete;
	virtual ~Ray();

private:

	XMFLOAT3 m_xmf3Center = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 m_xmf3Direction = {0.0f, 1.0f, 0.0f};

	float m_Length = 5.0f;

public:

#if defined(_DEBUG) | defined(DEBUG)
	Microsoft::WRL::ComPtr<ID3D12Resource>	 m_PositionBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource>	 m_PositionBufferUploader = nullptr;
	D3D12_VERTEX_BUFFER_VIEW				 m_PositionBufferView;
	
	void BuildMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void Render(float ETime, ID3D12GraphicsCommandList* pd3dCommandList);

#endif

};

class Collider
{
	Collider() = delete;
	Collider(XMFLOAT3 xmf3Center, XMFLOAT3 xmf3Extents);
	Collider(const Collider& rhs) = delete;
	Collider& operator=(const Collider& rhs) = delete;
	virtual ~Collider();

public:

	XMFLOAT4X4 m_xmf4x4World = MathHelper::identity4x4();

private:

#if defined(_DEBUG) | defined(DEBUG)
	Microsoft::WRL::ComPtr<ID3D12Resource>	 m_PositionBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource>	 m_PositionBufferUploader = nullptr;
	D3D12_VERTEX_BUFFER_VIEW				 m_PositionBufferView;

	Microsoft::WRL::ComPtr<ID3D12Resource>   m_NormalBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource>	 m_NormalBufferUploader = nullptr;
	D3D12_VERTEX_BUFFER_VIEW				 m_NormalBufferView;

	Microsoft::WRL::ComPtr<ID3D12Resource>   m_TangentBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource>	 m_TangentBufferUploader = nullptr;
	D3D12_VERTEX_BUFFER_VIEW				 m_TangentBufferView;

	Microsoft::WRL::ComPtr<ID3D12Resource>	 m_IndexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource>	 m_IndexBufferUploader = nullptr;
	D3D12_INDEX_BUFFER_VIEW					 m_IndexBufferView;

	DXGI_FORMAT m_IndexFormat = DXGI_FORMAT_R16_UINT;

	SubmeshGeometry m_SubmeshGeometry;

public:
	void BuildMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void Render(float ETime, ID3D12GraphicsCommandList* pd3dCommandList);
#endif

};
