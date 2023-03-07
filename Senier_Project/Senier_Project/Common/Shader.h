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
	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;

	ComPtr<ID3DBlob> mvsByteCode = nullptr;
	ComPtr<ID3DBlob> mpsByteCode = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	ComPtr<ID3D12PipelineState> mPSO = nullptr;
};



