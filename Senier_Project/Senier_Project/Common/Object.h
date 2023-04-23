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

#include "RigidCollider.h"

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
	virtual void Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList);

	static std::shared_ptr<ModelDataInfo> LoadModelDataFromFile(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, const char* pstrFileName);
	static std::shared_ptr<Object> LoadFrameHierarchyFromFile
	(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile, int* pnSkinnedMeshes, Object* pRootObject);
	static void LoadAnimationFromFile(FILE* pInFile, std::shared_ptr<ModelDataInfo> pModelData);
	void LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile, Object* pRootObject);

	void Impulse(XMFLOAT3 xmf3Impulse, XMFLOAT3 xmf3CollisionNormal, XMFLOAT3 xmf3CollisionPoint);

protected:
	virtual void OnPrepareRender(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void BuildConstantBuffers(ID3D12Device* pd3dDevice);
	virtual void BuildTextureDescriptorHeap(ID3D12Device* pd3dDevice);

	// 속도에 따른 갱신
	void CalculatePositionByVelocity(float Etime);
	void CalculateRotateByAngleVelocity(float Etime);

	// 속도 변화 갱신
	void CalculateDeltaVelocityByImpulse(XMFLOAT3 xmf3Impulse);
	void CalculateDeltaAngleVelocityByImpulse(XMFLOAT3 xmf3Impulse, XMFLOAT3 xmf3CollisionNormal, XMFLOAT3 xmf3CollisionPoint);

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
	// 좌표 관련

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
	float m_Pitch = 0.0f; // x
	float m_Yaw = 0.0f;   // y
	float m_Roll = 0.0f;  // z


	// 속도 및 물리 연산 관련
	// 연산에 활용되어 Obj의 좌표를 업데이트
	XMFLOAT3 m_xmf3ColliderCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3ColliderExtents = XMFLOAT3(0.5f, 0.5f, 0.5f);

	float m_Mass = 1.0f;
	XMFLOAT3 m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3AngleVelocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	XMFLOAT3 m_xmf3Gravity = XMFLOAT3(0.0f, -90.0f, 0.0f);
	float m_Acceleration = 3.0f;

	float m_MaxSpeedXZ = 60.0f;
	float m_MaxSpeedY = 100.f;
	float m_MaxAngleSpeed = 200.0f;
	float m_Friction = 250.0f;
	float m_AngleDamping = 2000.f;
	

	bool m_bIsAlive = true;

public:
	void AddForce(XMVECTOR direction, float distance);
	void Move(DWORD dwDirection, float distance);
	void Rotate(float x, float y, float z);
	void Walk(float delta);
	void Strafe(float delta);

	void AddPosition(float x, float y, float z)
	{
		m_xmf3Position.x += x;
		m_xmf3Position.y += y;
		m_xmf3Position.z += z;


#if defined (_DEBUG)
		// 임시로 0 미만인 경우 position 조정
		if (m_xmf3Position.y < 0)
		{
			m_xmf3Position.y = 0;
		}
#endif
	}
	void AddPosition(XMFLOAT3 addPos)
	{
		AddPosition(addPos.x, addPos.y, addPos.z);
	}
	void AddRotate(float x, float y, float z)
	{
		m_Pitch += x;
		m_Yaw	+= y;
		m_Roll	+= z;
	}
	void AddRotate(XMFLOAT3 addRotate)
	{
		AddRotate(addRotate.x, addRotate.y, addRotate.z);
	}
	void AddVelocity(float x, float y, float z)
	{
		m_xmf3Velocity.x += x;
		m_xmf3Velocity.y += y;
		m_xmf3Velocity.z += z;
	}
	void AddVelocity(XMFLOAT3 addVelocity)
	{
		AddVelocity(addVelocity.x, addVelocity.y, addVelocity.z);
	}
	void AddAngleVelocity(float x, float y, float z)
	{
		m_xmf3AngleVelocity.x += x;
		m_xmf3AngleVelocity.y += y;
		m_xmf3AngleVelocity.z += z;
	}
	void AddAngleVelocity(XMFLOAT3 addAngleVelocity)
	{
		AddAngleVelocity(addAngleVelocity.x, addAngleVelocity.y, addAngleVelocity.z);
	}

public:
	std::unique_ptr<AnimationController> m_pAnimationController = nullptr;

	void SetWorld(const XMFLOAT4X4& World) { m_xmf4x4World = World; }
	void SetParentWorld(const XMFLOAT4X4& ParentWorld) { m_xmf4x4ParentWorld = ParentWorld; }
	void SetLocalTransform(const XMFLOAT4X4& LocalTransform) { m_xmf4x4LocalTransform = LocalTransform; }

	void SetName(char* pstrName) { strcpy_s(m_FrameName, pstrName); }
	void SetPosition(float x, float y, float z) 
	{
		m_xmf3Position = { x, y, z };
	}
	void SetPosition(const XMFLOAT3& Position) { SetPosition(Position.x, Position.y, Position.z); }
	void SetScale(float x, float y, float z) 
	{
		m_xmf3Scale = { x, y, z };
	}
	void SetScale(const XMFLOAT3& Scale) { SetScale(Scale.x, Scale.y, Scale.z); }
	void SetRotate(float fPitch, float fYaw, float fRoll)
	{
		m_Pitch = fPitch;
		m_Yaw = fYaw;
		m_Roll = fRoll;
	}
	void SetRotate(const XMFLOAT3& Rotate) { SetRotate(Rotate.x, Rotate.y, Rotate.z); }
	void SetQuaternion(const XMFLOAT4& quaternion) { m_xmf4Quaternion = quaternion; }


	void SetMass(float Mass) { m_Mass = Mass; }
	void SetAcceleration(float Acceleration) { m_Acceleration = Acceleration; }
	void SetVelocity(const XMFLOAT3& Velocity) { m_xmf3Velocity = Velocity; }
	void SetAngleVelocity(const XMFLOAT3& AngleVelocity) { m_xmf3AngleVelocity = AngleVelocity; }
	void SetGravity(const XMFLOAT3& Gravity) { m_xmf3Gravity = Gravity; }
	void SetMaxSpeedXZ(float MaxSpeedXZ) { m_MaxSpeedXZ = MaxSpeedXZ; }
	void SetMaxSpeedY(float MaxSpeedY) { m_MaxSpeedY = MaxSpeedY; }
	void SetFriction(float Friction) { m_Friction = Friction; }
	void SetMaxAngleSpeed(float MaxAngleSpeed) { m_MaxAngleSpeed = MaxAngleSpeed; }
	void SetAngleDamping(float AngleDamping) { m_AngleDamping = AngleDamping; }
	
	
	void SetIsAlive(bool bIsAlive) { m_bIsAlive = bIsAlive; }


	const XMFLOAT4X4& GetWorld() { return m_xmf4x4World; }
	const XMFLOAT4X4& GetParentWorld() { return m_xmf4x4ParentWorld; }
	const XMFLOAT4X4& GetLocalTransform() { return m_xmf4x4LocalTransform; }

	const char* GetName() { return m_FrameName; }
	const XMFLOAT3& GetPosition() { return(m_xmf3Position); }
	const XMFLOAT3& GetScale() { return(m_xmf3Scale); }
	const XMFLOAT3& GetLookVector() { return(m_xmf3Look); }
	const XMFLOAT3& GetUpVector() { return(m_xmf3Up); }
	const XMFLOAT3& GetRightVector() { return(m_xmf3Right); }
	const float& GetYaw() { return(m_Yaw); }
	const float& GetPitch() { return(m_Pitch); }
	const float& GetRoll() { return(m_Roll); }
	const XMFLOAT4& GetQuaternion() { return m_xmf4Quaternion; }


	const float& GetMass() { return m_Mass; }
	const float& GetAcceleration() { return m_Acceleration; }
	const XMFLOAT3& GetVelocity() { return(m_xmf3Velocity); }
	const XMFLOAT3& GetAngleVelocity() { return m_xmf3AngleVelocity; }
	const XMFLOAT3& GetGravity() { return m_xmf3Gravity; }
	const float& GetMaxSpeedXZ() { return m_MaxSpeedXZ; }
	const float& GetMaxSpeedY() { return m_MaxSpeedY; }
	const float& GetFriction() { return m_Friction; }
	const float& GetMaxAngleSpeed() { return m_MaxAngleSpeed; }
	const float& GetAngleDamping() { return m_AngleDamping; }

	
	
	const bool GetIsAlive() { return m_bIsAlive; }

#if defined(_DEBUG)
public:
	std::shared_ptr<RigidCollider> m_pCollider = nullptr;


#endif
};





