#pragma once

#include <tchar.h>
#include "MathHelper.h"
#include "UploadBuffer.h"


#define CLIENT_WIDTH 1920
#define CLIENT_HEIGHT 1080

#define SKINNED_ANIMATION_BONES 256

class Shader;

//////////////// Texture ����ü //////////////////////
struct Texture
{
	__wchar_t FileName[64];

	Microsoft::WRL::ComPtr<ID3D12Resource> Resource;
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
};

// ������ �̰͵� map���� ����
extern std::vector<std::shared_ptr<Texture>> g_CachingTexture;

enum RenderLayer : int
{
	Static,
	Skinned,
	Image,
	Collider,
	Count
};

extern std::map<RenderLayer, std::shared_ptr<Shader>> g_Shaders;

void LoadTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const wchar_t* texFileName);
std::shared_ptr<Texture> FindReplicatedTexture(const wchar_t* pstrTextureName);

//////////////// ��� ���� ����ü //////////////////////

struct ObjConstant
{
	DirectX::XMFLOAT4X4 World = MathHelper::identity4x4();
};

struct PassConstant
{
	DirectX::XMFLOAT4X4 ViewProj = MathHelper::identity4x4();
};

struct MatConstant
{
	DirectX::XMFLOAT4 AlbedoColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
};

struct BoneBindPoseOffsetConstant
{
	DirectX::XMFLOAT4X4 BoneOffset[SKINNED_ANIMATION_BONES];
};
struct SkinningBoneTransformConstant
{
	DirectX::XMFLOAT4X4 BoneTransform[SKINNED_ANIMATION_BONES];
};

//////////////// ����, �ε��� ���� ���� �Լ� //////////////////////

void CreateVertexBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	Microsoft::WRL::ComPtr<ID3D12Resource>* pBufferGPU,
	Microsoft::WRL::ComPtr<ID3D12Resource>* pBufferUploader,
	UINT bufferByteSize,
	UINT strideInBytes,
	D3D12_VERTEX_BUFFER_VIEW* pVertexBufferView, void* pData);

void CreateIndexBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	Microsoft::WRL::ComPtr<ID3D12Resource>* pBufferGPU,
	Microsoft::WRL::ComPtr<ID3D12Resource>* pBufferUploader,
	UINT bufferByteSize, DXGI_FORMAT indexbufferFormat,
	D3D12_INDEX_BUFFER_VIEW* pVertexBufferView, void* pData);

//////////////// ���� �б� �Լ� //////////////////////

int ReadintegerFromFile(FILE* pInFile);
float ReadFloatFromFile(FILE* pInFile);
BYTE ReadStringFromFile(FILE* pInFile, char* pstrToken);


