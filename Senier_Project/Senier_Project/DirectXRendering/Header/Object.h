#ifndef OBJECT_H

#define OBJECT_H

// 메쉬, 재질 등 오브젝트를 렌더링하는데 필요한
// 데이터들을 가진다.
// 계층 구조로 구성될 수 있다.

#include "../../Common/Header/D3DUtil.h"
#include "../../Common/Header/UploadBuffer.h"
#include "../../Physics/Header/Collider.h"
#include "../../Physics/Header/RigidBody.h"
#include "Global.h"
#include "Mesh.h"
#include "Material.h"
#include "AnimationController.h"

//#define COLLIDER_RENDER

#define DIR_FORWARD					0x01
#define DIR_BACKWARD				0x02
#define DIR_LEFT					0x04
#define DIR_RIGHT					0x08

using namespace DirectX;

class Scene;

class Object : public std::enable_shared_from_this<Object>
{
public:
	Object();
	Object(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		   ObjectInitData objData,
		   std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	//Object(const Object& rhs);
	Object& operator=(const Object& rhs) = delete;
	virtual ~Object();

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
							ObjectInitData objData,
							std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	virtual void OnResize(float aspectRatio) {}
	virtual void Animate(float elapsedTime);
	virtual void Update(float elapsedTime);
	virtual void UpdateToRigidBody(float elapsedTime);
	virtual void UpdateObjectCB();

	virtual void ProcessInput(UCHAR* pKeybuffer) {}
	virtual void KeyDownEvent(WPARAM wParam) {}
	virtual void KeyUpEvent(WPARAM wParam) {}

	virtual void UpdateTransform(XMFLOAT4X4 *pxmf4x4Parent = NULL);
	virtual void Render(float elapsedTime, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void DepthRender(float elapsedTime, ID3D12GraphicsCommandList* pd3dCommandList);

	static std::shared_ptr<ModelDataInfo> LoadModelDataFromFile(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, const char* pstrFileName, const char* pstrTexPath);
	static std::shared_ptr<Object> LoadFrameHierarchyFromFile
	(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile, int* pnSkinnedMeshes, Object* pRootObject, const char* pstrFileName, const char* pstrTexPath);
	static void LoadAnimationFromFile(FILE* pInFile, std::shared_ptr<ModelDataInfo> pModelData);
	void LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile, Object* pRootObject, const char* pstrFileName, const char* pstrTexPath);

	virtual void Cutting(XMFLOAT3 xmf3PlaneNormal);
	virtual void Destroy();
	virtual void DestroyRunTime();

protected:
	virtual void OnPrepareRender(float elapsedTime, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void BuildConstantBuffers(ID3D12Device* pd3dDevice);
	virtual void BuildTextureDescriptorHeap(ID3D12Device* pd3dDevice);

protected:
	char m_FrameName[64];

	std::shared_ptr<Object> m_pChild = NULL;
	std::shared_ptr<Object> m_pSibling = NULL;

	std::unique_ptr<UploadBuffer<ObjConstant>> m_pObjectCB = nullptr;
	UINT m_ObjCBByteSize = 0;

	std::shared_ptr<Mesh> m_pMesh = nullptr;
	int m_nMaterial = 0;
	std::vector<std::shared_ptr<Material>> m_vpMaterials;

	bool m_bShadow = true;

public:
	char m_pstrFileName[64];
	std::unique_ptr<AnimationController> m_pAnimationController = nullptr;
	UINT m_nObjectConstantsParameterIdx = 0;
	UINT m_nObjectCBParameterIdx = 3;

public:
	void SetChild(std::shared_ptr<Object> pChild);
	void SetMesh(std::shared_ptr<Mesh> pMesh);
	void SetMaterials(std::vector<std::shared_ptr<Material>> vpMaterial);

	std::shared_ptr<Object> FindFrame(char* pstrFrameName);
	void FindAndSetSkinnedMesh(std::vector<std::shared_ptr<SkinnedMesh>>* vpSkinnedMeshes);


protected:
	// 좌표 관련
	XMFLOAT4X4 m_xmf4x4World = MathHelper::identity4x4();
	XMFLOAT4X4 m_xmf4x4ParentWorld = MathHelper::identity4x4();
	XMFLOAT4X4 m_xmf4x4LocalTransform = MathHelper::identity4x4();	// 부모로부터 상대 좌표

	XMFLOAT3 m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3RenderPosition = XMFLOAT3(0, 0, 0);
	XMFLOAT3 m_xmf3RenderOffsetPosition = XMFLOAT3(0, 0, 0);
	XMFLOAT3 m_xmf3RenderOffsetRotation = XMFLOAT3(0, 0, 0);

	XMFLOAT3 m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	XMFLOAT3 m_xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMFLOAT4 m_xmf4Orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT3 m_xmf3Rotation = XMFLOAT3(0, 0, 0);

	// 물리 연산 관련
	std::shared_ptr<RigidBody> m_pBody = nullptr;
	std::shared_ptr<Collider> m_pCollider = nullptr;
	ColliderType m_nColliderType = Collider_None;
	XMFLOAT3 m_xmf3ColliderExtents = XMFLOAT3(0, 0, 0);

	ObjectType m_nObjectType = Object_None;

	float m_Mass = 1.0f;

	float m_Acceleration = 100.0f;

	bool m_bIsFalling = false;
	bool m_bIsAlive = true;

	// ===================================
	float m_HP = 20.0f;

	float m_DestroyTime = 0.0f;
	float m_ElapsedDestroyTime = 0.0f;
	bool m_bDestroying = false;

	float m_DissolveTime = 0.0f;
	float m_ElapsedDissolveTime = 0.0f;
	float m_DissolveValue = 0.0f;
	bool m_bDissolveStart = false;

	float m_InvincibleTime = 0.5f;
	float m_ElapsedInvincibleTime = 0.0f;
	bool m_bInvincible = false;

	
public:
	virtual void Move(DWORD dwDirection) {}
	virtual void Rotate(float x, float y, float z) {}
	virtual void Jump() {}
	virtual void IsFalling();

	virtual void ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection);

	void AddPosition(float x, float y, float z)
	{
		m_xmf3Position.x += x;
		m_xmf3Position.y += y;
		m_xmf3Position.z += z;

	}
	void AddPosition(XMFLOAT3 addPos)
	{
		AddPosition(addPos.x, addPos.y, addPos.z);
	}
	void AddRotate(float x, float y, float z)
	{
		m_xmf3Rotation.x += x;
		m_xmf3Rotation.y += y;
		m_xmf3Rotation.z += z;
	}
	void AddRotate(XMFLOAT3 addRotate)
	{
		AddRotate(addRotate.x, addRotate.y, addRotate.z);
	}

	//void AddVelocity(float x, float y, float z) {
	//	m_xmf3Velocity.x += x;
	//	m_xmf3Velocity.y += y;
	//	m_xmf3Velocity.z += z;
	//}
	//void AddVelocity(XMFLOAT3 addVelocity) {
	//	AddVelocity(addVelocity.x, addVelocity.y, addVelocity.z);
	//}

public:
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
		m_xmf3Rotation.x = fPitch;
		m_xmf3Rotation.y = fYaw;
		m_xmf3Rotation.z = fRoll;
	}
	void SetRotate(const XMFLOAT3& Rotate);
	void SetOrientation(const XMFLOAT4& Orientation) { m_xmf4Orientation = Orientation;
													   XMStoreFloat4(&m_xmf4Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmf4Orientation)));}
	

	void SetIsFalling(bool IsFalling) { m_bIsFalling = IsFalling; }
	void SetIsAlive(bool IsAlive) { m_bIsAlive = IsAlive; }

	void SetHP(float hp) { m_HP = hp; }
	void SetInvincible(bool bInvincible) { m_bInvincible = bInvincible; }


	const XMFLOAT4X4& GetWorld() { return m_xmf4x4World; }
	const XMFLOAT4X4& GetParentWorld() { return m_xmf4x4ParentWorld; }
	const XMFLOAT4X4& GetLocalTransform() { return m_xmf4x4LocalTransform; }

	const char* GetName() { return m_FrameName; }
	const XMFLOAT3& GetPosition() { return(m_xmf3Position); }
	const XMFLOAT3& GetScale() { return(m_xmf3Scale); }
	const XMFLOAT3& GetLookVector() { return(m_xmf3Look); }
	const XMFLOAT3& GetUpVector() { return(m_xmf3Up); }
	const XMFLOAT3& GetRightVector() { return(m_xmf3Right); }
	const XMFLOAT3& GetRotation() { return m_xmf3Rotation; }
	const float& GetYaw() { return(m_xmf3Rotation.y); }
	const float& GetPitch() { return(m_xmf3Rotation.x); }
	const float& GetRoll() { return(m_xmf3Rotation.z); }
	const XMFLOAT4& GetOrientation() { return m_xmf4Orientation; }

	std::shared_ptr<RigidBody> GetBody() { return m_pBody; }
	std::shared_ptr<Collider> GetCollider() { return m_pCollider; }
	ColliderType GetColliderType() { return m_nColliderType; }

	bool GetIsFalling() { return m_bIsFalling; }
	bool GetIsAlive() { return m_bIsAlive; }

	const float& GetHP() { return m_HP; }
	bool GetInvincible() { return m_bInvincible; }
	bool GetShadowed() { return m_bShadow; }

	UINT GetObjectType() {return m_nObjectType;}
};




#endif // !OBJECT_H
