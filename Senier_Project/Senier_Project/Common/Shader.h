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

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dRootSignature, void* pContext);

	virtual void OnResize(float aspectRatio) {};
	virtual void Update(const GameTimer& gt) {};
	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual bool BuildShadersAndInputLayout();

	virtual bool BuildPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);

	virtual void ProcessInput(UCHAR* pKeybuffer);

protected:
	ComPtr<ID3DBlob> m_vsByteCode = nullptr;
	ComPtr<ID3DBlob> m_psByteCode = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> m_vInputLayout;

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

	virtual bool BuildShadersAndInputLayout();
};

// 2D 이미지 렌더링 텍스쳐
class ImageObjectShader :public Shader
{
public:
	ImageObjectShader();
	ImageObjectShader(const ImageObjectShader& rhs) = delete;
	ImageObjectShader& operator=(const ImageObjectShader& rhs) = delete;
	virtual ~ImageObjectShader();

	virtual bool BuildShadersAndInputLayout();
	virtual bool BuildPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);

	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void OnResize(float aspectRatio);
	virtual void Update(const GameTimer& gt);
	virtual void Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList);

private:
	std::vector<std::shared_ptr<ImgObject>> m_vpImgObjects;

public:
	bool CreateImgObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nScreenWidth, int nScreenHeight,
		const wchar_t* pstrTextureFileName, int nBitmapWidth, int nBitmapHeight);
};

// Collider 렌더링 셰이더
class ColliderShader : public Shader
{
public:
	ColliderShader();
	ColliderShader(const ColliderShader& rhs) = delete;
	ColliderShader& operator=(const ColliderShader& rhs) = delete;
	virtual ~ColliderShader();

	virtual bool BuildShadersAndInputLayout();
	virtual bool BuildPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);
};