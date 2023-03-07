//***************************************************************************************
// Camera.h by Frank Luna (C) 2011 All Rights Reserved.
//   
//***************************************************************************************
#pragma once

#include "D3DUtil.h"

class Camera
{
public:
	Camera();
	virtual ~Camera();

	DirectX::XMVECTOR GetPosition()const;
	DirectX::XMFLOAT3 GetPosition3f()const;
	void SetPosition(float x, float y, float z);
	void SetPosition(const DirectX::XMFLOAT3& v);

	DirectX::XMVECTOR GetRight()const;
	DirectX::XMFLOAT3 GetRight3f()const;
	DirectX::XMVECTOR GetUp()const;
	DirectX::XMFLOAT3 GetUp3f()const;
	DirectX::XMVECTOR GetLook()const;
	DirectX::XMFLOAT3 GetLook3f()const;

	float GetNearZ()const;
	float GetFarZ()const;
	float GetAspect()const;
	float GetFovY()const;
	float GetFovX()const;

	float GetNearWindowWidth()const;
	float GetNearWindowHeight()const;
	float GetFarWindowWidth()const;
	float GetFarWindowHeight()const;

	// frustum 설정
	void SetLens(float fovY, float aspect, float zn, float zf);

	// LookAt 파라미터를 통해 카메라 공간 정의
	void LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);
	void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);

	DirectX::XMMATRIX GetView()const;
	DirectX::XMMATRIX GetProj()const;

	DirectX::XMFLOAT4X4 GetView4x4f()const;
	DirectX::XMFLOAT4X4 GetProj4x4f()const;

	void Strafe(float d);
	void Walk(float d);

	void Pitch(float angle);
	void RotateY(float angle);

	virtual void UpdateViewMatrix();


protected:
	DirectX::XMFLOAT3 mPosition = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 mRight = { 1.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 mUp = { 0.0f, 1.0f, 0.0f };
	DirectX::XMFLOAT3 mLook = { 0.0f, 0.0f, 1.0f };

	float mNearZ = 0.0f;
	float mFarZ = 0.0f;
	float mAspect = 0.0f;
	float mFovY = 0.0f;
	float mNearWindowHeight = 0.0f;
	float mFarWindowHeight = 0.0f;

	bool mViewDirty = true;

	DirectX::XMFLOAT4X4 mView = MathHelper::identity4x4();
	DirectX::XMFLOAT4X4 mProj = MathHelper::identity4x4();
};

class Third_Person_Camera : Camera
{
public:
	Third_Person_Camera();
	virtual ~Third_Person_Camera();

	DirectX::XMFLOAT3 GetOffset() const { return mOffset; }
	DirectX::XMFLOAT3 GetPlayerPos()const { return mPlayerPos; }

	void SetOffset(const DirectX::XMFLOAT3& v) { mOffset = v; }
	void SetOffset(float x, float y, float z) { mOffset = DirectX::XMFLOAT3(x, y ,z); }
	void SetPlayerPos(const DirectX::XMFLOAT3& v) {	mPlayerPos = v;	}
	void SetPlayerPos(float x, float y, float z) {	mPlayerPos = DirectX::XMFLOAT3(x, y, z);}

	virtual void UpdateViewMatrix() override;

protected:
	DirectX::XMFLOAT3 mOffset = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 mPlayerPos = { 0.0f, 0.0f, 0.0f };

};

