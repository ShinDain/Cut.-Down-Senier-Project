#ifndef GLOBAL_H

#define GLOBAL_H

#include <tchar.h>
#include "../../Common/Header/MathHelper.h"
#include "../../Common/Header/D3DUtil.h"
#include "../../Common/Header/UploadBuffer.h"
#include "../../Common/Header/Struct.h"


class Shader;
class ColliderPlane;
class ColliderBox;
class ColliderSphere;
class ModelDataInfo;

extern std::vector<std::shared_ptr<ColliderPlane>> g_ppColliderPlanes;
extern std::vector<std::shared_ptr<ColliderBox>> g_ppColliderBoxs;
extern std::vector<std::shared_ptr<ColliderSphere>> g_ppColliderSpheres;

extern std::map<const char*, std::shared_ptr<ModelDataInfo>> g_LoadedModelData;

extern std::vector<std::shared_ptr<Texture>> g_CachingTexture;
extern std::map<RenderLayer, std::shared_ptr<Shader>> g_Shaders;

void LoadTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const wchar_t* texFileName);
std::shared_ptr<Texture> FindReplicatedTexture(const wchar_t* pstrTextureName);

//////////////// 정점, 인덱스 버퍼 생성 함수 //////////////////////

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

//////////////// 파일 읽기 함수 //////////////////////

int ReadintegerFromFile(FILE* pInFile);
float ReadFloatFromFile(FILE* pInFile);
BYTE ReadStringFromFile(FILE* pInFile, char* pstrToken);


#endif