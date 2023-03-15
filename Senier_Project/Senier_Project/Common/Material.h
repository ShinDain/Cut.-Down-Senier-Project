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
	std::vector<std::unique_ptr<Texture>> mTextures;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDescriptorHeap = nullptr;

	std::unique_ptr<UploadBuffer<tmpMatConstant>> mMatCB = nullptr;

	XMFLOAT4 mAlbedoColor;
	XMFLOAT4 mEmissiveColor;
	XMFLOAT4 mSpecularColor;
	float mGlossiness;
	float mMetallic;
	float mSmoothness;
	float mSpecularHighlight;
	float mGlossyReflection;

	XMFLOAT3 mFresnelIOR = { 0.01f, 0.01f, 0.01f };
	float mRoughness = 0.25f;
	XMFLOAT4X4 mMatTransform = MathHelper::identity4x4();

public:
	XMFLOAT4 GetAlbedoColor() { return mAlbedoColor; }
	XMFLOAT4 GetEmissiveColor() { return mEmissiveColor; }
	XMFLOAT4 GetSpecularColor() { return mSpecularColor; }
	float GetGlossiness() { return mGlossiness; }
	float GetMetallic() { return mMetallic; }
	float GetSmoothness() { return mSmoothness; }
	float GetSpecularHighlight() { return mSpecularHighlight; }
	float GetGlossyReflection() { return mGlossyReflection; }
	XMFLOAT3 GetFresnelIOR() { return mFresnelIOR; }
	float GetRoughness() { return mRoughness; }

	void SetAlbedoColor(XMFLOAT4 in) { mAlbedoColor = in; }
	void SetEmissiveColor(XMFLOAT4 in) { mEmissiveColor = in; }
	void SetSpecularColor(XMFLOAT4 in) { mSpecularColor = in; }
	void SetGlossiness(float in) { mGlossiness = in; }
	void SetMetallic(float in) { mMetallic = in; }
	void SetSmoothness(float in) { mSmoothness = in; }
	void SetSpecularHighlight(float in) { mSpecularHighlight = in; }
	void SetGlossyReflection(float in) { mGlossyReflection = in; }
	void SetFresnelIOR(XMFLOAT3 in) { mFresnelIOR = in; }
	void SetRoughness(float in) { mRoughness = in; }

};




