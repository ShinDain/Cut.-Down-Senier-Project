#pragma once
// 파이프라인 설정
// 셰이더에 따른 루트 서명 설정
// Descriptor Heap 초기화 및 관리
// 셰이더 코드 컴파일

#include "D3DApp.h"
#include "GameTimer.h"
#include "Camera.h"
#include "Object.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

class Shader
{
public:
	Shader();
	Shader(const Shader& rhs) = delete;
	Shader& operator=(const Shader& rhs) = delete;
	virtual ~Shader();

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext);

	virtual void OnResize(float aspectRatio) {};
	virtual void Update(const GameTimer& gt) {};
	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList);


	virtual bool BuildRootSignature(ID3D12Device* pd3dDevice);
	virtual bool BuildShadersAndInputLayout();

	virtual bool BuildPSO(ID3D12Device* pd3dDevice);

	virtual void OnWinKeyboardInput(WPARAM wParam) {};

	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 2> GetStaticSampler();

protected:
	ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;

	ComPtr<ID3DBlob> m_vsByteCode = nullptr;
	ComPtr<ID3DBlob> m_psByteCode = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> m_vInputLayout;

	ComPtr<ID3D12PipelineState> m_PSO = nullptr;

	int m_nPassBuffer = -1;

public:
	int GetPassBufferNum() { return m_nPassBuffer; }

};

class SkinnedMeshShader : public Shader
{
public:
	SkinnedMeshShader();
	SkinnedMeshShader(const SkinnedMeshShader& rhs) = delete;
	SkinnedMeshShader& operator=(const SkinnedMeshShader& rhs) = delete;
	virtual ~SkinnedMeshShader();

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext);

	virtual void OnResize(float aspectRatio) {};
	virtual void Update(const GameTimer& gt) {};
	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList);


	virtual bool BuildRootSignature(ID3D12Device* pd3dDevice);
	virtual bool BuildShadersAndInputLayout();

	virtual bool BuildPSO(ID3D12Device* pd3dDevice);

	virtual void OnWinKeyboardInput(WPARAM wParam) {};


};

