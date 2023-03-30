//***************************************************************************************
// Camera.h by Frank Luna (C) 2011 All Rights Reserved.
//   
//***************************************************************************************
#pragma once

#include "D3DUtil.h"
#include "Global.h"

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
	DirectX::XMMATRIX GetOrtho()const;

	DirectX::XMFLOAT4X4 GetView4x4f()const;
	DirectX::XMFLOAT4X4 GetProj4x4f()const;
	DirectX::XMFLOAT4X4 GetOrtho4x4f()const;

	void Strafe(float d);
	void Walk(float d);

	void Pitch(float angle);
	void RotateY(float angle);

	virtual void UpdateViewMatrix();


protected:
	DirectX::XMFLOAT3 m_xmf3Position = { 0.0f, 0.0f, 0.0f };
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
};

class Third_Person_Camera : Camera
{
public:
	Third_Person_Camera();
	virtual ~Third_Person_Camera();

	DirectX::XMFLOAT3 GetOffset() const { return m_xmf3Offset; }
	DirectX::XMFLOAT3 GetPlayerPos()const { return m_xmf3PlayerPos; }

	void SetOffset(const DirectX::XMFLOAT3& v) { m_xmf3Offset = v; }
	void SetOffset(float x, float y, float z) { m_xmf3Offset = DirectX::XMFLOAT3(x, y ,z); }
	void SetPlayerPos(const DirectX::XMFLOAT3& v) {	m_xmf3PlayerPos = v;	}
	void SetPlayerPos(float x, float y, float z) {	m_xmf3PlayerPos = DirectX::XMFLOAT3(x, y, z);}

	virtual void UpdateViewMatrix() override;

protected:
	DirectX::XMFLOAT3 m_xmf3Offset = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 m_xmf3PlayerPos = { 0.0f, 0.0f, 0.0f };

};

