//***************************************************************************************
// Camera.h by Frank Luna (C) 2011 All Rights Reserved.
//   
//***************************************************************************************
#pragma once

#include "../../Common/Header/D3DUtil.h"
#include "Global.h"

class Object;

class Camera
{
public:
	Camera();
	Camera(const Camera& rhs) = delete;
	Camera& operator=(const Camera& rhs) = delete;
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

	float GetPitch() const { return m_Pitch; }
	float GetYaw() const { return m_Yaw; }

	// frustum 설정
	void SetLens(float fovY, float aspect, float zn, float zf);

	// LookAt 파라미터를 통해 카메라 공간 정의
	void LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);
	void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);

	DirectX::XMMATRIX GetView()const;
	DirectX::XMMATRIX GetProj()const;
	DirectX::XMMATRIX GetOrtho()const;

	DirectX::XMFLOAT4X4 GetView4x4f()const;
	DirectX::XMFLOAT4X4 GetProj4x4f()const;
	DirectX::XMFLOAT4X4 GetOrtho4x4f()const;

	void Strafe(float d);
	void Walk(float d);

	virtual void Pitch(float angle);
	virtual void RotateY(float angle);

	virtual void Update(float Etime) { UpdateViewMatrix(); }
	virtual void UpdateViewMatrix();


protected:
	DirectX::XMFLOAT3 m_xmf3Position = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT4 m_xmf4Orientation = { 0,0,0, 1 };

	DirectX::XMFLOAT3 m_xmf3Right = { 1.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 m_xmf3Up = { 0.0f, 1.0f, 0.0f };
	DirectX::XMFLOAT3 m_xmf3Look = { 0.0f, 0.0f, 1.0f };

	float m_NearZ = 0.0f;
	float m_FarZ = 0.0f;
	float m_Aspect = 0.0f;
	float m_FovY = 0.0f;
	float m_NearWindowHeight = 0.0f;
	float m_FarWindowHeight = 0.0f;

	bool m_bViewDirty = true;

	DirectX::XMFLOAT4X4 m_xmf4x4View = MathHelper::identity4x4();
	DirectX::XMFLOAT4X4 m_xmf4x4Proj = MathHelper::identity4x4();
	DirectX::XMFLOAT4X4 m_xmf4x4Ortho = MathHelper::identity4x4();

	float m_Pitch = 0.0f;
	float m_Yaw = 0.0f;
	float m_Roll = 0.0f;


public:
	BoundingFrustum m_CameraFrustum;
	void SetOrientation(XMFLOAT4 xmf4Orientation) { m_xmf4Orientation = xmf4Orientation; }
};

class Third_Person_Camera : public Camera
{
public:
	Third_Person_Camera() = delete;
	Third_Person_Camera(std::shared_ptr<Object> pObject);
	Third_Person_Camera(const Third_Person_Camera& rhs) = delete;
	Third_Person_Camera& operator=(const Third_Person_Camera& rhs) = delete;
	virtual ~Third_Person_Camera();

	virtual void Update(float Etime) override;

	virtual void Pitch(float angle);
	virtual void RotateY(float angle);

protected:
	float m_OffsetLength = 75.0f;
	float m_OffsetHeight = 10.0f;
	//DirectX::XMFLOAT3 m_xmf3Offset = { 0.0f, 30.0f, -100.0f };

	float m_ShoulderOffsetLength = 7.0f;
	float m_ShoulderOffsetHeight = 12.0f;
	float m_ShoulderCameraPitch = -15.0f;
	bool m_bShoulderView = false;

	float m_MaxPitch = 40;
	float m_MinPitch = -10;

	float m_MaxShoulderPitch = 40;
	float m_MinShoulderPitch = -45;

	// 추적할 대상 오브젝트
	std::shared_ptr<Object> m_pObject = nullptr;


public:
	float GetOffsetLength() const { return m_OffsetLength; }
	//DirectX::XMFLOAT3 GetOffset() const { return m_xmf3Offset; }
	std::shared_ptr<Object> GetTrackedObject() const { return m_pObject; }
	bool GetIsShoulderView() { return m_bShoulderView; }
	float GetShoulderCameraPitch() { return m_ShoulderCameraPitch; }

	void SetOffsetLength(float value) { m_OffsetLength = value; }
	//void SetOffset(DirectX::XMFLOAT3 xmf3Offset) {	m_xmf3Offset = xmf3Offset;}
	void SetTrackedObject(const std::shared_ptr<Object> pObject) { m_pObject = pObject; }
	void SetIsShoulderView(bool bShoulderView) { m_bShoulderView = bShoulderView; }
	void SetShoulderCameraPitch(float shoulderPitch) { m_ShoulderCameraPitch = shoulderPitch; }

};

class CinematicCamera : public Camera
{
public:
	CinematicCamera();
	CinematicCamera(const CinematicCamera& rhs) = delete;
	CinematicCamera& operator=(const CinematicCamera& rhs) = delete;
	virtual ~CinematicCamera();

	virtual void Update(float Etime) override;
};
