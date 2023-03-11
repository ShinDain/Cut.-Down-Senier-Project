#pragma once

// �޽�, ���� �� ������Ʈ�� �������ϴµ� �ʿ���
// �����͵��� ������.
// ���� ������ ������ �� �ִ�.

#include "D3DUtil.h"
#include "GameTimer.h"
#include "UploadBuffer.h"
#include "Global.h"
#include "Mesh.h"
// #include "Material.h"

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
	virtual void Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void BuildConstantBuffers(ID3D12Device* pd3dDevice);

protected:

	std::unique_ptr<UploadBuffer<tmpObjConstant>> mObjectCB = nullptr;

	std::unique_ptr<Mesh> mMesh = nullptr;
	// std::unique_ptr<Material> mMaterial = nullptr;

	UINT mObjCBByteSize = 0;
	XMFLOAT4X4 mWorld = MathHelper::identity4x4();

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

};





