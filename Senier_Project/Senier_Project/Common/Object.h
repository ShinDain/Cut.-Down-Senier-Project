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

protected:
	char mFrameName[64];

	std::shared_ptr<Object> mpChild = NULL;
	std::shared_ptr<Object> mpSibling = NULL;

	std::unique_ptr<UploadBuffer<tmpObjConstant>> mObjectCB = nullptr;

	std::shared_ptr<Mesh> mMesh = nullptr;
	std::shared_ptr<Material> mMaterial = nullptr;

public:
	void SetChild(std::shared_ptr<Object> pChild);
	void SetMesh(std::shared_ptr<Mesh> pMesh);
	void SetMaterial(std::shared_ptr<Material> pMesh);


protected:
	UINT mObjCBByteSize = 0;
	XMFLOAT4X4 mWorld = MathHelper::identity4x4();
	XMFLOAT4X4 mParentWorld = MathHelper::identity4x4();

	XMFLOAT3 mPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 mRight = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3 mUp = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 mLook = XMFLOAT3(0.0f, 0.0f, 1.0f);

	XMFLOAT3 mScale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	XMFLOAT3 mVelocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 mGravity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	float mPitch = 0.0f; // x
	float mYaw = 0.0f;   // y
	float mRoll = 0.0f;  // z

	float mMaxVelocityXZ = 0.0f;
	float mMaxVelocityY = 0.0f;
	float mFriction = 0.0f;

public:
	void SetParentWorld(XMFLOAT4X4 ParentWorld) { mParentWorld = ParentWorld; }

	void SetPosition(float x, float y, float z) { mPosition = XMFLOAT3(x, y, z); }
	void SetPosition(XMFLOAT3 Position) { mPosition = Position; }
	void SetScale(XMFLOAT3 Scale) { mScale = Scale; }
	void SetFriction(float fFriction) { mFriction = fFriction; }
	void SetGravity(const XMFLOAT3& Gravity) { mGravity = Gravity; }
	void SetMaxVelocityXZ(float Velocity) { mMaxVelocityXZ = Velocity; }
	void SetMaxVelocityY(float Velocity) { mMaxVelocityY = Velocity; }
	void SetVelocity(const XMFLOAT3& Velocity) { mVelocity = Velocity; }
	void SetYaw(const float in) { mYaw = in; }
	void SetPitch(const float in) { mPitch = in; }
	void SetRoll(const float in) { mRoll = in; }

	const XMFLOAT3& GetPosition() { return(mPosition); }
	const XMFLOAT3& GetScale() { return(mScale); }
	const XMFLOAT3& GetLookVector() { return(mLook); }
	const XMFLOAT3& GetUpVector() { return(mUp); }
	const XMFLOAT3& GetRightVector() { return(mRight); }
	const XMFLOAT3& GetVelocity() const { return(mVelocity); }
	const float& GetYaw() const { return(mYaw); }
	const float& GetPitch() const { return(mPitch); }
	const float& GetRoll() const { return(mRoll); }
	const XMFLOAT4X4& GetWorld() const { return mWorld; }
	const XMFLOAT4X4& GetParentWorld() const { return mParentWorld; }

};





