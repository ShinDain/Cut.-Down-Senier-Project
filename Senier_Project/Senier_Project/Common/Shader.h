#pragma once
// 파이프라인 설정
// 셰이더에 따른 루트 서명 설정
// Descriptor Heap 초기화 및 관리
// 셰이더 코드 컴파일

#include "DirectXApp.h"
#include "GameTimer.h"
#include "Camera.h"
#include "Object.h"
#include "ImgObject.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

// 일반 텍스쳐 셰이더

class Shader
{
public:
	Shader();
	Shader(const Shader& rhs) = delete;
	Shader& operator=(const Shader& rhs) = delete;
	virtual ~Shader();

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext);

	virtual void OnResize(float aspectRatio) {}
	virtual void Update(const GameTimer& gt) {}
	virtual void ChangeShader(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ProcessInput(UCHAR* pKeybuffer) {}

protected:
	virtual bool BuildShadersAndInputLayout();
	virtual bool BuildRootSignature(ID3D12Device* pd3dDevice);
	virtual bool BuildPSO(ID3D12Device* pd3dDevice);

	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 2> GetStaticSampler();

protected:
	ComPtr<ID3DBlob> m_vsByteCode = nullptr;
	ComPtr<ID3DBlob> m_psByteCode = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> m_vInputLayout;
	ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;

	ComPtr<ID3D12PipelineState> m_PSO = nullptr;

	int m_nPassBuffer = -1;

public:
	int GetPassBufferNum() { return m_nPassBuffer; }

};

// 스키닝 메시 셰이더

class SkinnedMeshShader : public Shader
{
public:
	SkinnedMeshShader();
	SkinnedMeshShader(const SkinnedMeshShader& rhs) = delete;
	SkinnedMeshShader& operator=(const SkinnedMeshShader& rhs) = delete;
	virtual ~SkinnedMeshShader();

protected:
	virtual bool BuildShadersAndInputLayout();
	virtual bool BuildRootSignature(ID3D12Device* pd3dDevice);
};

// 2D 이미지 렌더링 텍스쳐
class ImageObjectShader :public Shader
{
public:
	ImageObjectShader();
	ImageObjectShader(const ImageObjectShader& rhs) = delete;
	ImageObjectShader& operator=(const ImageObjectShader& rhs) = delete;
	virtual ~ImageObjectShader();

	virtual void OnResize(float aspectRatio);

protected:
	virtual bool BuildShadersAndInputLayout();
	virtual bool BuildRootSignature(ID3D12Device* pd3dDevice);
	virtual bool BuildPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);
};

// Collider 렌더링 셰이더
class ColliderShader : public Shader
{
public:
	ColliderShader();
	ColliderShader(const ColliderShader& rhs) = delete;
	ColliderShader& operator=(const ColliderShader& rhs) = delete;
	virtual ~ColliderShader();

protected:
	virtual bool BuildShadersAndInputLayout();
	virtual bool BuildRootSignature(ID3D12Device* pd3dDevice);
	virtual bool BuildPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);
};