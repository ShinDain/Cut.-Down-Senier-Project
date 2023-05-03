#pragma once

// �޽�, ���� �� ������Ʈ�� �������ϴµ� �ʿ���
// �����͵��� ������.
// ���� ������ ������ �� �ִ�.

#include "../../Common/Header/D3DUtil.h"
#include "../../Common/Header/UploadBuffer.h"
#include "Global.h"
#include "Mesh.h"
#include "Material.h"
#include "AnimationController.h"
#include "../../Physics/Header/RigidBody.h"
#include "../../Physics/Header/Collider.h"

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
	virtual void Animate(float elapsedTime);
	virtual void Update(float elapsedTime);
	virtual void UpdateTransform(XMFLOAT4X4 *pxmf4x4Parent = NULL);
	virtual void Render(float elapsedTime, ID3D12GraphicsCommandList* pd3dCommandList);

	static std::shared_ptr<ModelDataInfo> LoadModelDataFromFile(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, const char* pstrFileName);
	static std::shared_ptr<Object> LoadFrameHierarchyFromFile
	(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile, int* pnSkinnedMeshes, Object* pRootObject);
	static void LoadAnimationFromFile(FILE* pInFile, std::shared_ptr<ModelDataInfo> pModelData);
	void LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile, Object* pRootObject);

protected:
	virtual void OnPrepareRender(float elapsedTime, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void BuildConstantBuffers(ID3D12Device* pd3dDevice);
	virtual void BuildTextureDescriptorHeap(ID3D12Device* pd3dDevice);

protected:
	char m_FrameName[64];

	std::shared_ptr<Object> m_pChild = NULL;
	std::shared_ptr<Object> m_pSibling = NULL;

	std::unique_ptr<UploadBuffer<ObjConstant>> m_pObjectCB = nullptr;

	std::shared_ptr<Mesh> m_pMesh = nullptr;
	int m_nMaterial = 0;
	std::vector<std::shared_ptr<Material>> m_vpMaterials;

public:
	std::unique_ptr<AnimationController> m_pAnimationController = nullptr;

public:
	void SetChild(std::shared_ptr<Object> pChild);
	void SetMesh(std::shared_ptr<Mesh> pMesh);
	void SetMaterials(std::vector<std::shared_ptr<Material>> vpMaterial);

	std::shared_ptr<Object> FindFrame(char* pstrFrameName);
	void FindAndSetSkinnedMesh(std::vector<std::shared_ptr<SkinnedMesh>>* vpSkinnedMeshes);


protected:
	// ��ǥ ����

	UINT m_ObjCBByteSize = 0;
	XMFLOAT4X4 m_xmf4x4World = MathHelper::identity4x4();
	XMFLOAT4X4 m_xmf4x4ParentWorld = MathHelper::identity4x4();
	XMFLOAT4X4 m_xmf4x4LocalTransform = MathHelper::identity4x4();	// �θ�κ��� ��� ��ǥ

	XMFLOAT3 m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	XMFLOAT3 m_xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMFLOAT4 m_xmf4Quaternion = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT3 m_xmf3Rotate = XMFLOAT3(0, 0, 0);

	// ���� ���� ����
	std::shared_ptr<RigidBody> m_pBody = nullptr;
	std::shared_ptr<ColliderBox> m_pCollider = nullptr;

	bool m_bPhysics = true;
	bool m_bIsAlive = true;
	
public:
	void Move(DWORD dwDirection, float distance);
	void Rotate(float x, float y, float z);
	void Walk(float delta);
	void Strafe(float delta);

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
		m_xmf3Rotate.x += x;
		m_xmf3Rotate.y += y;
		m_xmf3Rotate.z += z;
	}
	void AddRotate(XMFLOAT3 addRotate)
	{
		AddRotate(addRotate.x, addRotate.y, addRotate.z);
	}

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
		m_xmf3Rotate.x = fPitch;
		m_xmf3Rotate.y = fYaw;
		m_xmf3Rotate.z = fRoll;
	}
	void SetRotate(const XMFLOAT3& Rotate) { SetRotate(Rotate.x, Rotate.y, Rotate.z); }
	void SetQuaternion(const XMFLOAT4& quaternion) { m_xmf4Quaternion = quaternion; }
	void SetIsAlive(bool IsAlive) { m_bIsAlive = IsAlive; }


	const XMFLOAT4X4& GetWorld() { return m_xmf4x4World; }
	const XMFLOAT4X4& GetParentWorld() { return m_xmf4x4ParentWorld; }
	const XMFLOAT4X4& GetLocalTransform() { return m_xmf4x4LocalTransform; }

	const char* GetName() { return m_FrameName; }
	const XMFLOAT3& GetPosition() { return(m_xmf3Position); }
	const XMFLOAT3& GetScale() { return(m_xmf3Scale); }
	const XMFLOAT3& GetLookVector() { return(m_xmf3Look); }
	const XMFLOAT3& GetUpVector() { return(m_xmf3Up); }
	const XMFLOAT3& GetRightVector() { return(m_xmf3Right); }
	const float& GetYaw() { return(m_xmf3Rotate.y); }
	const float& GetPitch() { return(m_xmf3Rotate.x); }
	const float& GetRoll() { return(m_xmf3Rotate.z); }
	const XMFLOAT4& GetQuaternion() { return m_xmf4Quaternion; }

	RigidBody* GetBody() { return m_pBody.get(); }
	ColliderBox* GetCollider() { return m_pCollider.get(); }

	bool GetIsAlive() { return m_bIsAlive; }
};




