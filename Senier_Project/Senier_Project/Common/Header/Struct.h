#ifndef STRUCT_H

#include "D3DUtil.h"
#include "MathHelper.h"

#define STRUCT_H

#define CLIENT_WIDTH 1920
#define CLIENT_HEIGHT 1080

#define SKINNED_ANIMATION_BONES 256
#define MaxLights 16

struct Light
{
	DirectX::XMFLOAT3 Strength = { 0.5f, 0.5f, 0.5f };
	float FalloffStart = 1.0f;
	DirectX::XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f };
	float FalloffEnd = 100.0f;
	DirectX::XMFLOAT3 Position = { 0.5f, 0.5f, 0.5f };
	float SpotPower = 64.0f;
};

//////////////// Texture 구조체 //////////////////////
struct Texture
{
	__wchar_t FileName[128];

	Microsoft::WRL::ComPtr<ID3D12Resource> Resource;
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
};

enum ShaderType : UINT
{
	Shader_Static,
	Shader_TextureMesh,
	Shader_Skinned,
	Shader_Image,
	Shader_WireFrame,
	Shader_DepthMap,
	Shader_CuttedDepthMap,
	Shader_CuttedStatic,
	Shader_CuttedTextureMesh,
	Shader_CuttedSkinned,
	Shader_Count
};

enum RenderLayer : UINT
{
	Render_Static,
	Render_TextureMesh,
	Render_Skinned,
	Render_Effect,
	Render_Image,
	Render_WireFrame,
	Render_CuttedStatic,
	Render_CuttedTexture,
	Render_CuttedSkinned,
	Render_Count
};

enum ColliderType : UINT
{
	Collider_Plane,
	Collider_Box,
	Collider_Sphere,
	Collider_None
};

enum ObjectType : UINT
{
	Object_World,
	Object_Player,
	Object_Monster,
	Object_Weapon,
	Object_Movable,
	Object_PlayerProjectile,
	Object_EnemyProjectile,
	Object_Item,
	Object_Event,
	Object_Effect,
	Object_UI,
	Object_None
};

enum SoundType : UINT
{
	Sound_Wood_Heavy,
	Sound_Wood_Light,
	Sound_Stone,
	Sound_Steel_Heavy,
	Sound_Steel_Light,
	Sound_Fabric,
	Sound_Character,
	Sound_None
};

//////////////// 상수 버퍼 구조체 //////////////////////

struct ObjConstant
{
	float DissolveValue = 1.0f;

};

struct PassConstant
{
	DirectX::XMFLOAT4X4 ViewProj = MathHelper::identity4x4();
	DirectX::XMFLOAT4X4 ShadowTransform = MathHelper::identity4x4();
	DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
	float cbPerObjectPad1 = 0.0f;
	DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
	DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
	float NearZ = 0.0f;
	float FarZ = 0.0f;
	float TotalTime = 0.0f;
	float DeltaTime = 0.0f;

	DirectX::XMFLOAT4 AmbientLight;

	Light Lights[MaxLights];

	float FadeInValue = 1.0f;
	XMFLOAT3 xmf3Pad;
};

struct MatConstant
{
	DirectX::XMFLOAT4 AlbedoColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
	float Roughness = 0.5f;

	DirectX::XMFLOAT4X4 MatTransform = MathHelper::identity4x4();

	//UINT DiffuseMapIndex = 0;
	//UINT NormalMapIndex = 0;
	//UINT MaterialPad1;
	//UINT MaterialPad2;
};

struct CuttedConstant
{
	UINT PlaneCnt = 0;
	float PlaneDirection_1 = 1;
	float PlaneDirection_2 = 1;
	float PlaneDirection_3 = 1;
	
	XMFLOAT3 xmf3PlaneNormal_1 = XMFLOAT3(0, 0, 0);
	float PlaneDistance_1 = 0;
	XMFLOAT3 xmf3PlaneNormal_2 = XMFLOAT3(0, 0, 0);
	float PlaneDistance_2 = 0;
	XMFLOAT3 xmf3PlaneNormal_3 = XMFLOAT3(0, 0, 0);
	float PlaneDistance_3 = 0;
};

struct BoneBindPoseOffsetConstant
{
	DirectX::XMFLOAT4X4 BoneOffset[SKINNED_ANIMATION_BONES];
};
struct SkinningBoneTransformConstant
{
	DirectX::XMFLOAT4X4 BoneTransform[SKINNED_ANIMATION_BONES];
};

struct ObjectDefaultData
{
	const char* pstrObjectPath;
	const char* pstrTexPath;
	XMFLOAT3 xmf3Extents;
	ObjectType objectType;
	ColliderType colliderType;
	SoundType soundType;
	int nMass;
	XMFLOAT3 xmf3OffsetScale;
	RenderLayer renderLayer;
	XMFLOAT3 xmf3MeshOffsetPosition;
	XMFLOAT3 xmf3MeshOffsetRotation;
	bool bShadowed;
};

// 오브젝트 초기화 데이터 구조체
struct ObjectInitData
{
	XMFLOAT3 xmf3Position = XMFLOAT3(0, 0, 0);
	XMFLOAT3 xmf3Rotation = XMFLOAT3(0, 0, 0);
	XMFLOAT4 xmf4Orientation = XMFLOAT4(0, 0, 0, 1);
	XMFLOAT3 xmf3Scale = XMFLOAT3(1, 1, 1);
	ObjectType objectType = Object_None;
	unsigned int nMass = 1;
	ColliderType colliderType = Collider_None;
	SoundType soundType = SoundType::Sound_Wood_Light;
	XMFLOAT3 xmf3Extents = XMFLOAT3(0, 0, 0);
	XMFLOAT3 xmf3MeshOffsetPosition = XMFLOAT3(0, 0, 0);
	XMFLOAT3 xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	bool bShadow = true;
};

// 메시의 하위 범위 인덱스를 정의
struct SubmeshGeometry
{
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	INT BaseVertexLocation = 0;
};









#endif