#pragma once
// 파이프라인 설정
// 셰이더에 따른 루트 서명 설정
// Descriptor Heap 초기화 및 관리
// 셰이더 코드 컴파일

#include "DirectXApp.h"
#include "Camera.h"
#include "Object.h"
#include "ImgObject.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

// 스태틱 메시 셰이더

class Shader
{
public:
	Shader();
	Shader(const Shader& rhs) = delete;
	Shader& operator=(const Shader& rhs) = delete;
	virtual ~Shader();

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext);

	virtual void OnResize(float aspectRatio) {}
	virtual void Update(float elapsedTime) {}
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

	ShaderType m_Type = ShaderType::Shader_Static;
	int m_nPassBuffer = -1;

public:
	int GetPassBufferNum() { return m_nPassBuffer; }

};

// 텍스쳐 스태틱 메시 셰이더

class TextureMeshShader : public Shader
{
public:
	TextureMeshShader();
	TextureMeshShader(const TextureMeshShader& rhs) = delete;
	TextureMeshShader& operator=(const TextureMeshShader& rhs) = delete;
	virtual ~TextureMeshShader();

	virtual void ChangeShader(ID3D12GraphicsCommandList* pd3dCommandList)
	{
		Shader::ChangeShader(pd3dCommandList);
		g_curShader = m_Type;
	}
protected:
	virtual bool BuildShadersAndInputLayout();
	virtual bool BuildRootSignature(ID3D12Device* pd3dDevice);
};

// 스키닝 메시 셰이더

class SkinnedMeshShader : public Shader
{
public:
	SkinnedMeshShader();
	SkinnedMeshShader(const SkinnedMeshShader& rhs) = delete;
	SkinnedMeshShader& operator=(const SkinnedMeshShader& rhs) = delete;
	virtual ~SkinnedMeshShader();

	virtual void ChangeShader(ID3D12GraphicsCommandList* pd3dCommandList) 
	{ 
		Shader::ChangeShader(pd3dCommandList); 
		g_curShader = m_Type;
	}

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

	virtual void ChangeShader(ID3D12GraphicsCommandList* pd3dCommandList) 
	{
		Shader::ChangeShader(pd3dCommandList);
		g_curShader = m_Type;
	}
	virtual void OnResize(float aspectRatio);

protected:
	virtual bool BuildShadersAndInputLayout();
	virtual bool BuildRootSignature(ID3D12Device* pd3dDevice);
	virtual bool BuildPSO(ID3D12Device* pd3dDevice);
};

// Collider 렌더링 셰이더
class WireFrameShader : public Shader
{
public:
	WireFrameShader();
	WireFrameShader(const WireFrameShader& rhs) = delete;
	WireFrameShader& operator=(const WireFrameShader& rhs) = delete;
	virtual ~WireFrameShader();

	virtual void ChangeShader(ID3D12GraphicsCommandList* pd3dCommandList) 
	{ 
		Shader::ChangeShader(pd3dCommandList);
		g_curShader = m_Type;
	}
protected:
	virtual bool BuildShadersAndInputLayout();
	virtual bool BuildRootSignature(ID3D12Device* pd3dDevice);
	virtual bool BuildPSO(ID3D12Device* pd3dDevice);
};

// DepthMap 렌더링 셰이더
class DepthMapShader : public Shader
{
public:
	DepthMapShader();
	DepthMapShader(const DepthMapShader& rhs) = delete;
	DepthMapShader& operator=(const DepthMapShader& rhs) = delete;
	virtual ~DepthMapShader();

	virtual void ChangeShader(ID3D12GraphicsCommandList* pd3dCommandList)
	{
		Shader::ChangeShader(pd3dCommandList);
		g_curShader = m_Type;
	}

protected:
	virtual bool BuildShadersAndInputLayout();
	virtual bool BuildRootSignature(ID3D12Device* pd3dDevice);
	virtual bool BuildPSO(ID3D12Device* pd3dDevice);
};

// 잘린 오브젝트
// 
// 스태틱 메시 셰이더

class CuttedStaticMeshShader : public Shader
{
public:
	CuttedStaticMeshShader();
	CuttedStaticMeshShader(const CuttedStaticMeshShader& rhs) = delete;
	CuttedStaticMeshShader& operator=(const CuttedStaticMeshShader& rhs) = delete;
	virtual ~CuttedStaticMeshShader();

	virtual void ChangeShader(ID3D12GraphicsCommandList* pd3dCommandList)
	{
		Shader::ChangeShader(pd3dCommandList);
		g_curShader = m_Type;
	}
protected:
	virtual bool BuildShadersAndInputLayout();
	virtual bool BuildRootSignature(ID3D12Device* pd3dDevice);
	virtual bool BuildPSO(ID3D12Device* pd3dDevice);
};
// 텍스쳐 스태틱 메시 셰이더

class CuttedTextureMeshShader : public CuttedStaticMeshShader
{
public:
	CuttedTextureMeshShader();
	CuttedTextureMeshShader(const CuttedTextureMeshShader& rhs) = delete;
	CuttedTextureMeshShader& operator=(const CuttedTextureMeshShader& rhs) = delete;
	virtual ~CuttedTextureMeshShader();

	virtual void ChangeShader(ID3D12GraphicsCommandList* pd3dCommandList)
	{
		Shader::ChangeShader(pd3dCommandList);
		g_curShader = m_Type;
	}
protected:
	virtual bool BuildShadersAndInputLayout();
	virtual bool BuildRootSignature(ID3D12Device* pd3dDevice);
};

// 스키닝 메시 셰이더

class CuttedSkinnedMeshShader : public CuttedStaticMeshShader
{
public:
	CuttedSkinnedMeshShader();
	CuttedSkinnedMeshShader(const CuttedSkinnedMeshShader& rhs) = delete;
	CuttedSkinnedMeshShader& operator=(const CuttedSkinnedMeshShader& rhs) = delete;
	virtual ~CuttedSkinnedMeshShader();

	virtual void ChangeShader(ID3D12GraphicsCommandList* pd3dCommandList)
	{
		Shader::ChangeShader(pd3dCommandList);
		g_curShader = m_Type;
	}

protected:
	virtual bool BuildShadersAndInputLayout();
	virtual bool BuildRootSignature(ID3D12Device* pd3dDevice);
};
