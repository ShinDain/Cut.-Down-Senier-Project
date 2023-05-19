#ifndef GLOBAL_H

#define GLOBAL_H

#include <tchar.h>
#include "../../Common/Header/MathHelper.h"
#include "../../Common/Header/D3DUtil.h"
#include "../../Common/Header/UploadBuffer.h"
#include "../../Common/Header/Struct.h"

#define GRAVITY_SIZE -9.81f

#define CHARACTER_MODEL_NAME "Vampire_A_Lusth"
#define ZOMBIE_MODEL_NAME "Zombie1"
#define WEAPON_MODEL_NAME "Katana"
#define CUBE_MODEL_NAME "Cube"
#define GROUND_MODEL_NAME "Epoxy_Ground"
#define CARPET_MODEL_NAME "Carpet_5"

class Shader;
class ColliderPlane;
class ColliderBox;
class ColliderSphere;
class ModelDataInfo;
class Object;

extern std::vector<std::shared_ptr<ColliderPlane>> g_ppColliderPlanes;
extern std::vector<std::shared_ptr<ColliderBox>> g_ppColliderBoxs;
extern std::vector<std::shared_ptr<ColliderSphere>> g_ppColliderSpheres;

extern std::map<const char*, std::shared_ptr<ModelDataInfo>> g_LoadedModelData;

extern std::vector<std::shared_ptr<Texture>> g_CachingTexture;
extern std::map<ShaderType, std::shared_ptr<Shader>> g_Shaders;

extern UINT g_curShader;

extern std::map<const char*, ObjectDefaultData> g_DefaultObjectData;
extern std::vector<const char*> g_DefaultObjectNames;

void LoadTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const wchar_t* texFileName);
std::shared_ptr<Texture> FindReplicatedTexture(const wchar_t* pstrTextureName);

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


#endif