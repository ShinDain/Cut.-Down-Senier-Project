#pragma once

// 메쉬, 재질 등 오브젝트를 렌더링하는데 필요한
// 데이터들을 가진다.
// 계층 구조로 구성될 수 있다.

#include "D3DUtil.h"
#include "GameTimer.h"
#include "UploadBuffer.h"
#include "Global.h"
#include "Mesh.h"
#include "Material.h"
#include "AnimationController.h"

#define DIR_FORWARD					0x01
#define DIR_BACKWARD				0x02
#define DIR_LEFT					0x04
#define DIR_RIGHT					0x08

using namespace DirectX;

class Object : public std::enable_shared_from_this<Object>
{
public:
	Object();
	Object(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks);

	Object(const Object& rhs) = delete;
	Object& operator=(const Object& rhs) = delete;
	virtual ~Object();

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext);

	virtual void OnResize(float aspectRatio) {}
	virtual void Animate(const GameTimer& gt);
	virtual void Update(const GameTimer& gt);
	virtual void UpdateTransform(XMFLOAT4X4 *pxmf4x4Parent = NULL);
	virtual void PrepareRender(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void BuildConstantBuffers(ID3D12Device* pd3dDevice);
	virtual void BuildTextureDescriptorHeap(ID3D12Device* pd3dDevice);

	static std::shared_ptr<ModelDataInfo> LoadModelDataFromFile(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, char* pstrFileName);
	static std::shared_ptr<Object> LoadFrameHierarchyFromFile
	(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile, int* pnSkinnedMeshes, Object* pRootObject);
	static void LoadAnimationFromFile(FILE* pInFile, std::shared_ptr<ModelDataInfo> pModelData);
	void LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile, Object* pRootObject);


protected:
	char m_FrameName[64];

	std::shared_ptr<Object> m_pChild = NULL;
	std::shared_ptr<Object> m_pSibling = NULL;

	std::unique_ptr<UploadBuffer<ObjConstant>> m_pObjectCB = nullptr;

	std::shared_ptr<Mesh> m_pMesh = nullptr;

	int m_nMaterial = 0;
	std::vector<std::shared_ptr<Material>> m_vpMaterials;

public:
	void SetChild(std::shared_ptr<Object> pChild);
	void SetMesh(std::shared_ptr<Mesh> pMesh);
	void SetMaterials(std::vector<std::shared_ptr<Material>> vpMaterial);

	std::shared_ptr<Object> FindFrame(char* pstrFrameName);
	void FindAndSetSkinnedMesh(std::vector<std::shared_ptr<SkinnedMesh>>* vpSkinnedMeshes);

protected:
	UINT m_ObjCBByteSize = 0;
	XMFLOAT4X4 m_xmf4x4World = MathHelper::identity4x4();
	XMFLOAT4X4 m_xmf4x4ParentWorld = MathHelper::identity4x4();
	XMFLOAT4X4 m_xmf4x4LocalTransform = MathHelper::identity4x4();	// 부모로부터 상대 좌표

	XMFLOAT3 m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	XMFLOAT3 m_xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMFLOAT4 m_xmf4Quaternion = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	XMFLOAT3 m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float m_Speed = 1.0f;

	float m_Pitch = 0.0f; // x
	float m_Yaw = 0.0f;   // y
	float m_Roll = 0.0f;  // z

	float m_MaxVelocityXZ = 0.0f;
	float m_MaxVelocityY = 0.0f;
	float m_Friction = 250.0f;

public:
	void AddForce(XMVECTOR direction, float distance);
	void Move(DWORD dwDirection, float distance);
	void Rotate(float x, float y, float z);
	void Walk(float delta);
	void Strafe(float delta);

	void AddPosition(float x, float y, float z)
	{
		m_xmf4x4LocalTransform._41 += x;
		m_xmf4x4LocalTransform._42 += y;
		m_xmf4x4LocalTransform._43 += z;

		UpdateTransform(NULL);
	}
	void AddPosition(XMFLOAT3 addPos)
	{
		m_xmf4x4LocalTransform._41 += addPos.x;
		m_xmf4x4LocalTransform._42 += addPos.y;
		m_xmf4x4LocalTransform._43 += addPos.z;

		UpdateTransform(NULL);
	}

public:
	std::unique_ptr<AnimationController> m_pAnimationController = nullptr;

	void SetWorld(XMFLOAT4X4 World) { m_xmf4x4World = World; }
	void SetParentWorld(XMFLOAT4X4 ParentWorld) { m_xmf4x4ParentWorld = ParentWorld; }
	void SetLocalTransform(XMFLOAT4X4 LocalTransform) { m_xmf4x4LocalTransform = LocalTransform; }

	void SetName(char* pstrName) { strcpy_s(m_FrameName, pstrName); }
	void SetPosition(float x, float y, float z) 
	{
		m_xmf4x4LocalTransform._41 = x;
		m_xmf4x4LocalTransform._42 = y;
		m_xmf4x4LocalTransform._43 = z;

		UpdateTransform(NULL);
	}
	void SetPosition(XMFLOAT3 Position) { SetPosition(Position.x, Position.y, Position.z); }
	void SetScale(float x, float y, float z) 
	{
		XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
		m_xmf4x4LocalTransform = MathHelper::MatrixMultiply(mtxScale, m_xmf4x4LocalTransform);

		UpdateTransform(NULL);
	}
	void SetRotate(float fPitch, float fYaw, float fRoll)
	{
		XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
		m_xmf4x4LocalTransform = MathHelper::MatrixMultiply(mtxRotate, m_xmf4x4LocalTransform);

		UpdateTransform(NULL);
	}

	void SetFriction(float fFriction) { m_Friction = fFriction; }
	void SetGravity(const XMFLOAT3& Gravity) { m_xmf3Gravity = Gravity; }
	void SetMaxVelocityXZ(float Velocity) { m_MaxVelocityXZ = Velocity; }
	void SetMaxVelocityY(float Velocity) { m_MaxVelocityY = Velocity; }
	void SetVelocity(const XMFLOAT3& Velocity) { m_xmf3Velocity = Velocity; }
	void SetQuaternion(const XMFLOAT4& quaternion) { m_xmf4Quaternion = quaternion; }
	void SetSpeed(const float Speed) { m_Speed = Speed; }

	const XMFLOAT4X4& GetWorld() { return m_xmf4x4World; }
	const XMFLOAT4X4& GetParentWorld() { return m_xmf4x4ParentWorld; }
	const XMFLOAT4X4& GetLocalTransform() { return m_xmf4x4LocalTransform; }

	const char* GetName() { return m_FrameName; }
	const XMFLOAT3& GetPosition() { return(m_xmf3Position); }
	const XMFLOAT3& GetScale() { return(m_xmf3Scale); }
	const XMFLOAT3& GetLookVector() { return(m_xmf3Look); }
	const XMFLOAT3& GetUpVector() { return(m_xmf3Up); }
	const XMFLOAT3& GetRightVector() { return(m_xmf3Right); }
	const XMFLOAT3& GetVelocity() { return(m_xmf3Velocity); }
	const float& GetYaw() { return(m_Yaw); }
	const float& GetPitch() { return(m_Pitch); }
	const float& GetRoll() { return(m_Roll); }
	const XMFLOAT4& SetQuaternion() { return m_xmf4Quaternion; }
	const float& GetSpeed() { return m_Speed; }

};





