#pragma once
// ���������� ����
// ���̴��� ���� ��Ʈ ���� ����
// Descriptor Heap �ʱ�ȭ �� ����
// ���̴� �ڵ� ������

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

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dRootSignature, void* pContext);

	virtual void OnResize(float aspectRatio) {};
	virtual void Update(const GameTimer& gt) {};
	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual bool BuildShadersAndInputLayout();

	virtual bool BuildPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);

	virtual void OnWinKeyboardInput(WPARAM wParam) {};


protected:
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

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dRootSignature, void* pContext);

	virtual void OnResize(float aspectRatio) {};
	virtual void Update(const GameTimer& gt) {};
	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual bool BuildShadersAndInputLayout();

	virtual bool BuildPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);

	virtual void OnWinKeyboardInput(WPARAM wParam) {};


};

