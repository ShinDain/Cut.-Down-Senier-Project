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

using namespace DirectX;

class Object
{
public:
	Object();
	Object(const Object& rhs) = delete;
	Object& operator=(const Object& rhs) = delete;
	virtual ~Object();

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext);

	virtual void OnResize(float aspectRatio) {}
	virtual void Update(const GameTimer& gt);
	virtual void PrepareRender(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void BuildConstantBuffers(ID3D12Device* pd3dDevice);

	static std::shared_ptr<Object> LoadModelDataFromFile(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, char* pstrFileName);
	static std::shared_ptr<Object> LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile);
	void LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile);
	void LoadAnimationFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile);


protected:
	char m_FrameName[64];

	std::shared_ptr<Object> m_pChild = NULL;
	std::shared_ptr<Object> m_pSibling = NULL;

	std::unique_ptr<UploadBuffer<tmpObjConstant>> m_pObjectCB = nullptr;

	std::shared_ptr<Mesh> m_pMesh = nullptr;

	int m_nMaterial = 0;
	std::vector<std::shared_ptr<Material>> m_ppMaterials;

public:
	void SetChild(std::shared_ptr<Object> pChild);
	void SetMesh(std::shared_ptr<Mesh> pMesh);
	void SetMaterials(std::vector<std::shared_ptr<Material>> ppMaterial);

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

	float m_Pitch = 0.0f; // x
	float m_Yaw = 0.0f;   // y
	float m_Roll = 0.0f;  // z

	float m_MaxVelocityXZ = 0.0f;
	float m_MaxVelocityY = 0.0f;
	float m_Friction = 0.0f;

public:
	void SetWorld(XMFLOAT4X4 World) { m_xmf4x4World = World; }
	void SetParentWorld(XMFLOAT4X4 ParentWorld) { m_xmf4x4ParentWorld = ParentWorld; }
	void SetLocalTransform(XMFLOAT4X4 LocalTransform) { m_xmf4x4LocalTransform = LocalTransform; }

	void SetName(char* pstrName) { strcpy_s(m_FrameName, pstrName); }
	void SetPosition(float x, float y, float z) { m_xmf3Position = XMFLOAT3(x, y, z); }
	void SetPosition(XMFLOAT3 Position) { m_xmf3Position = Position; }
	void SetScale(XMFLOAT3 Scale) { m_xmf3Scale = Scale; }
	void SetFriction(float fFriction) { m_Friction = fFriction; }
	void SetGravity(const XMFLOAT3& Gravity) { m_xmf3Gravity = Gravity; }
	void SetMaxVelocityXZ(float Velocity) { m_MaxVelocityXZ = Velocity; }
	void SetMaxVelocityY(float Velocity) { m_MaxVelocityY = Velocity; }
	void SetVelocity(const XMFLOAT3& Velocity) { m_xmf3Velocity = Velocity; }
	void SetYaw(const float in) { m_Yaw = in; }
	void SetPitch(const float in) { m_Pitch = in; }
	void SetRoll(const float in) { m_Roll = in; }
	void SetQuaternion(const XMFLOAT4& quaternion) { m_xmf4Quaternion = quaternion; }

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

	const XMFLOAT4X4& GetWorld() { return m_xmf4x4World; }
	const XMFLOAT4X4& GetParentWorld() { return m_xmf4x4ParentWorld; }

};





