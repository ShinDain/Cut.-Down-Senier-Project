//***************************************************************************************
// Camera.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "Camera.h"
#include "Object.h"

using namespace DirectX;

Camera::Camera()
{
	SetLens(0.25 * MathHelper::Pi, 1.0f, 1.0f, 1000.f);
}

Camera::~Camera()
{
}

DirectX::XMVECTOR Camera::GetPosition() const
{
	return XMLoadFloat3(&m_xmf3Position);
}

DirectX::XMFLOAT3 Camera::GetPosition3f() const
{
	return m_xmf3Position;
}

void Camera::SetPosition(float x, float y, float z)
{
	m_xmf3Position = XMFLOAT3(x, y, z);
	m_bViewDirty = true;

}

void Camera::SetPosition(const DirectX::XMFLOAT3& v)
{
	m_xmf3Position = v;
	m_bViewDirty = true;
}

DirectX::XMVECTOR Camera::GetRight() const
{
	return XMLoadFloat3(&m_xmf3Right);
}

DirectX::XMFLOAT3 Camera::GetRight3f() const
{
	return m_xmf3Right;
}

DirectX::XMVECTOR Camera::GetUp() const
{
	return XMLoadFloat3(&m_xmf3Up);
}

DirectX::XMFLOAT3 Camera::GetUp3f() const
{
	return m_xmf3Up;
}

DirectX::XMVECTOR Camera::GetLook() const
{
	return XMLoadFloat3(&m_xmf3Look);
}

DirectX::XMFLOAT3 Camera::GetLook3f() const
{
	return m_xmf3Look;
}

float Camera::GetNearZ() const
{
	return m_NearZ;
}

float Camera::GetFarZ() const
{
	return m_FarZ;
}

float Camera::GetAspect() const
{
	return m_Aspect;
}

float Camera::GetFovY() const
{
	return m_FovY;
}

float Camera::GetFovX() const
{
	float halfWidth = 0.5f * GetNearWindowWidth();
	return 2.0f * atan(halfWidth / m_NearZ);
}

float Camera::GetNearWindowWidth() const
{
	return m_Aspect * m_NearWindowHeight;
}

float Camera::GetNearWindowHeight() const
{
	return m_NearWindowHeight;
}

float Camera::GetFarWindowWidth() const
{
	return m_Aspect * m_FarWindowHeight;
}

float Camera::GetFarWindowHeight() const
{
	return m_FarWindowHeight;
}

void Camera::SetLens(float fovY, float aspect, float zn, float zf)
{
	m_FovY = fovY;
	m_Aspect = aspect;
	m_NearZ = zn;
	m_FarZ = zf;
	
	m_NearWindowHeight = 2.0f * m_NearZ * tanf(0.5f * m_FovY);
	m_FarWindowHeight = 2.0f * m_FarZ * tanf(0.5f * m_FovY);

	XMMATRIX P = XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_NearZ, m_FarZ);
	XMStoreFloat4x4(&m_xmf4x4Proj, P);
	XMStoreFloat4x4(&m_xmf4x4Ortho, XMMatrixOrthographicLH(CLIENT_WIDTH, CLIENT_HEIGHT, m_NearZ, m_FarZ));
}

void Camera::LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp)
{
	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
	XMVECTOR U = XMVector3Cross(L, R);

	XMStoreFloat3(&m_xmf3Position, pos);
	XMStoreFloat3(&m_xmf3Look, L);
	XMStoreFloat3(&m_xmf3Right, R);
	XMStoreFloat3(&m_xmf3Up, U);

	m_bViewDirty = true;
}

void Camera::LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up)
{
	XMVECTOR P = XMLoadFloat3(&pos);
	XMVECTOR T = XMLoadFloat3(&target);
	XMVECTOR U = XMLoadFloat3(&up);

	LookAt(P, T, U);

	m_bViewDirty = true;
}

DirectX::XMMATRIX Camera::GetView() const
{
	assert(!m_bViewDirty);
	return XMLoadFloat4x4(&m_xmf4x4View);
}

DirectX::XMMATRIX Camera::GetProj() const
{
	return XMLoadFloat4x4(&m_xmf4x4Proj);
}

DirectX::XMMATRIX Camera::GetOrtho() const
{
	return XMLoadFloat4x4(&m_xmf4x4Ortho);
}

DirectX::XMFLOAT4X4 Camera::GetView4x4f() const
{
	assert(!m_bViewDirty);
	return m_xmf4x4View;
}

DirectX::XMFLOAT4X4 Camera::GetProj4x4f() const
{
	return m_xmf4x4Proj;
}

DirectX::XMFLOAT4X4 Camera::GetOrtho4x4f() const
{
	return m_xmf4x4Ortho;
}

void Camera::Strafe(float d)
{
	// 좌표 변경 방식
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&m_xmf3Right);
	XMVECTOR p = XMLoadFloat3(&m_xmf3Position);
	XMStoreFloat3(&m_xmf3Position, XMVectorMultiplyAdd(s, r, p));

	m_bViewDirty = true;
}

void Camera::Walk(float d)
{
	// 좌표 변경 방식
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&m_xmf3Look);
	XMVECTOR p = XMLoadFloat3(&m_xmf3Position);
	XMStoreFloat3(&m_xmf3Position, XMVectorMultiplyAdd(s, l, p));

	m_bViewDirty = true;
}

void Camera::Pitch(float angle)
{
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), angle);

	XMStoreFloat3(&m_xmf3Up, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Up), R));
	XMStoreFloat3(&m_xmf3Look, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Look), R));

	m_bViewDirty = true;
}

void Camera::RotateY(float angle)
{
	XMMATRIX R = XMMatrixRotationY(angle);

	XMStoreFloat3(&m_xmf3Right, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Right), R));
	XMStoreFloat3(&m_xmf3Up, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Up), R));
	XMStoreFloat3(&m_xmf3Look, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Look), R));

	m_bViewDirty = true;
}

void Camera::UpdateViewMatrix()
{
	if (m_bViewDirty)
	{
		XMVECTOR R = XMLoadFloat3(&m_xmf3Right);
		XMVECTOR U = XMLoadFloat3(&m_xmf3Up);
		XMVECTOR L = XMLoadFloat3(&m_xmf3Look);
		XMVECTOR P = XMLoadFloat3(&m_xmf3Position);

		L = XMVector3Normalize(L);
		U = XMVector3Normalize(XMVector3Cross(L, R));

		R = XMVector3Cross(U, L);

		float x = -XMVectorGetX(XMVector3Dot(P, R));
		float y = -XMVectorGetX(XMVector3Dot(P, U));
		float z = -XMVectorGetX(XMVector3Dot(P, L));

		XMStoreFloat3(&m_xmf3Right, R);
		XMStoreFloat3(&m_xmf3Up, U);
		XMStoreFloat3(&m_xmf3Look, L);

		m_xmf4x4View(0, 0) = m_xmf3Right.x;
		m_xmf4x4View(1, 0) = m_xmf3Right.y;
		m_xmf4x4View(2, 0) = m_xmf3Right.z;
		m_xmf4x4View(3, 0) = x; 

		m_xmf4x4View(0, 1) = m_xmf3Up.x;
		m_xmf4x4View(1, 1) = m_xmf3Up.y;
		m_xmf4x4View(2, 1) = m_xmf3Up.z;
		m_xmf4x4View(3, 1) = y;

		m_xmf4x4View(0, 2) = m_xmf3Look.x;
		m_xmf4x4View(1, 2) = m_xmf3Look.y;
		m_xmf4x4View(2, 2) = m_xmf3Look.z;
		m_xmf4x4View(3, 2) = z;

		m_xmf4x4View(0, 3) = 0.0f;
		m_xmf4x4View(1, 3) = 0.0f;
		m_xmf4x4View(2, 3) = 0.0f;
		m_xmf4x4View(3, 3) = 1.f;

		m_bViewDirty = false;
	}
}


//------------------------------------------------
// 3인칭 카메라
//------------------------------------------------

Third_Person_Camera::Third_Person_Camera(std::shared_ptr<Object> pObject)
{
	SetLens(0.25f * MathHelper::Pi, 1.5f, 1.0f, 10000.f);

	m_pObject = pObject;
}

Third_Person_Camera::~Third_Person_Camera()
{
}

void Third_Person_Camera::Update(float Etime)
{
	XMVECTOR newPos = XMLoadFloat3(&m_pObject->GetPosition());
	XMVECTOR objPos = XMLoadFloat3(&m_pObject->GetPosition());
	XMVECTOR objLook = XMLoadFloat3(&m_pObject->GetLookVector());
	XMVECTOR objUp = XMLoadFloat3(&m_pObject->GetUpVector());

	XMVECTOR zOffset = XMVectorReplicate(m_xmf3Offset.z);
	XMVECTOR yOffset = XMVectorReplicate(m_xmf3Offset.y);
	XMVECTOR length = XMVectorReplicate(m_OffsetLength);

	newPos = XMVectorMultiplyAdd(objLook, zOffset, newPos);
	newPos = XMVectorMultiplyAdd(objUp, yOffset, newPos);
	newPos = XMVector3Normalize(newPos - objPos);
	newPos = XMVectorMultiplyAdd(newPos, length, objPos);

	XMFLOAT3 xmf3NewPos = { 0,0,0 };
	XMStoreFloat3(&xmf3NewPos, newPos);
	SetPosition(xmf3NewPos);

	LookAt(GetPosition3f(), m_pObject->GetPosition(), XMFLOAT3(0, 1, 0));

	UpdateViewMatrix();
}

