#ifndef STRUCT_H

#define STRUCT_H

#define CLIENT_WIDTH 1920
#define CLIENT_HEIGHT 1080

#define SKINNED_ANIMATION_BONES 256
#define MaxLights 16

//////////////// Texture 구조체 //////////////////////
struct Texture
{
	__wchar_t FileName[64];

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
	Shader_Count
};

enum RenderLayer : UINT
{
	Render_Static,
	Render_TextureMesh,
	Render_Skinned,
	Render_Image,
	Render_WireFrame,
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
	Object_Platform,
	Object_Player,
	Object_Monster,
	Object_Weapon,
	Object_UI,
	Object_None
};

//////////////// 상수 버퍼 구조체 //////////////////////

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
	XMFLOAT3 xmf3Extents = XMFLOAT3(0, 0, 0);
};

// 메시의 하위 범위 인덱스를 정의
struct SubmeshGeometry
{
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	INT BaseVertexLocation = 0;
};

struct Light
{
	DirectX::XMFLOAT3 Strength = { 0.5f, 0.5f, 0.5f };
	float FalloffStart = 1.0f;
	DirectX::XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f };
	float FalloffEnd = 10.0f;
	DirectX::XMFLOAT3 Position = { 0.5f, 0.5f, 0.5f };
	float SpotPower = 64.0f;
};








#endif