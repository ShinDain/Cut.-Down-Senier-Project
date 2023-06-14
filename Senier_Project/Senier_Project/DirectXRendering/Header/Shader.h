#pragma once
// ���������� ����
// ���̴��� ���� ��Ʈ ���� ����
// Descriptor Heap �ʱ�ȭ �� ����
// ���̴� �ڵ� ������

#include "DirectXApp.h"
#include "Camera.h"
#include "Object.h"
#include "ImgObject.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

// ����ƽ �޽� ���̴�

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

// �ؽ��� ����ƽ �޽� ���̴�

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

// ��Ű�� �޽� ���̴�

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

// 2D �̹��� ������ �ؽ���
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

// Collider ������ ���̴�
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

// DepthMap ������ ���̴�
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

// �߸� ������Ʈ
// 
// ����ƽ �޽� ���̴�

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
// �ؽ��� ����ƽ �޽� ���̴�

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

// ��Ű�� �޽� ���̴�

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
