#pragma once

#include "D3DUtil.h"
#include "UploadBuffer.h"
#include "Global.h"

using namespace DirectX;

struct Texture
{
	std::wstring FileName;

	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
};

class Material
{
public:
	Material();
	Material(const Material& rhs) = delete;
	Material& operator=(const Material& rhs) = delete;
	virtual ~Material();

public:
	virtual bool BuildDescriptorHeap(ID3D12Device* pd3dDevice);

	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void LoadMaterialFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile);
	virtual void LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile);

	void LoadTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, std::wstring texFileName);

protected:
	std::vector<std::unique_ptr<Texture>> m_pTextures;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap = nullptr;

	std::unique_ptr<UploadBuffer<tmpMatConstant>> m_pMatCB = nullptr;

	XMFLOAT4 m_xmf4AlbedoColor;
	XMFLOAT4 m_xmf4EmissiveColor;
	XMFLOAT4 m_xmf4SpecularColor;
	float m_Glossiness;
	float m_Metallic;
	float m_Smoothness;
	float m_SpecularHighlight;
	float m_GlossyReflection;

	XMFLOAT3 m_xmf3FresnelIOR = { 0.01f, 0.01f, 0.01f };
	float m_Roughness = 0.25f;
	XMFLOAT4X4 m_xmf4x4MatTransform = MathHelper::identity4x4();

public:
	XMFLOAT4 GetAlbedoColor() { return m_xmf4AlbedoColor; }
	XMFLOAT4 GetEmissiveColor() { return m_xmf4EmissiveColor; }
	XMFLOAT4 GetSpecularColor() { return m_xmf4SpecularColor; }
	float GetGlossiness() { return m_Glossiness; }
	float GetMetallic() { return m_Metallic; }
	float GetSmoothness() { return m_Smoothness; }
	float GetSpecularHighlight() { return m_SpecularHighlight; }
	float GetGlossyReflection() { return m_GlossyReflection; }
	XMFLOAT3 GetFresnelIOR() { return m_xmf3FresnelIOR; }
	float GetRoughness() { return m_Roughness; }

	void SetAlbedoColor(XMFLOAT4 in) { m_xmf4AlbedoColor = in; }
	void SetEmissiveColor(XMFLOAT4 in) { m_xmf4EmissiveColor = in; }
	void SetSpecularColor(XMFLOAT4 in) { m_xmf4SpecularColor = in; }
	void SetGlossiness(float in) { m_Glossiness = in; }
	void SetMetallic(float in) { m_Metallic = in; }
	void SetSmoothness(float in) { m_Smoothness = in; }
	void SetSpecularHighlight(float in) { m_SpecularHighlight = in; }
	void SetGlossyReflection(float in) { m_GlossyReflection = in; }
	void SetFresnelIOR(XMFLOAT3 in) { m_xmf3FresnelIOR = in; }
	void SetRoughness(float in) { m_Roughness = in; }

};




