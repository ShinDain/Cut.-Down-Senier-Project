//***************************************************************************************
// Camera.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "../Header/Camera.h"
#include "../Header/Object.h"

using namespace DirectX;

Camera::Camera()
{
	SetLens(0.25 * MathHelper::Pi, 1.0f, 1.0f, 1000.f);

	//m_CameraFrustum = BoundingFrustum(m_xmf3Position, m_xmf4Orientation, 0.001f, -0.001f, 0.001f, -0.001f, 1, 10000);
	m_pCameraFrustum = std::make_shared<BoundingFrustum>(XMFLOAT3(0,0,0), XMFLOAT4(0,0,0,1), 0.6375f, -0.6375f, 0.425f, -0.425f, 1.0f, 5000.0f);
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
	m_Pitch += angle;

	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(angle));

	XMStoreFloat3(&m_xmf3Up, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Up), R));
	XMStoreFloat3(&m_xmf3Look, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Look), R));

	m_bViewDirty = true;
}

void Camera::RotateY(float angle)
{
	m_Yaw += angle;

	XMMATRIX R = XMMatrixRotationY(XMConvertToRadians(angle));

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

		UpdateViewFrustum();

		m_bViewDirty = false;
	}
}

void Camera::UpdateViewFrustum()
{
	XMVECTOR orientation = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(m_Pitch), XMConvertToRadians(m_Yaw), 0);
	orientation = XMQuaternionNormalize(orientation);
	XMStoreFloat4(&m_xmf4Orientation, orientation);

	m_pCameraFrustum->Origin = m_xmf3Position;
	m_pCameraFrustum->Orientation = m_xmf4Orientation;
}


//------------------------------------------------
// 3인칭 카메라
//------------------------------------------------

Third_Person_Camera::Third_Person_Camera(std::shared_ptr<Object> pObject)
{
	SetLens(0.25f * MathHelper::Pi, 1.5f, 1.0f, 10000.f);
	//m_Pitch = 20;
	m_pObject = pObject;
}

Third_Person_Camera::~Third_Person_Camera()
{
}

void Third_Person_Camera::Update(float Etime)
{
	// 오브젝트의 크기에 따라 가변
	XMFLOAT3 xmf3Scale = m_pObject->GetScale();
	xmf3Scale.x /= 10;
	xmf3Scale.y /= 10;
	xmf3Scale.z /= 10;
	float cameraOffsetLength = m_OffsetLength;

	cameraOffsetLength *= xmf3Scale.x;

	XMVECTOR Look = XMVectorSet(0, 0, 1, 0);
	XMVECTOR Right = XMVectorSet(1, 0, 0, 0);
	XMVECTOR Up = XMVectorSet(0, 1, 0, 0);

	XMMATRIX R = XMMatrixRotationY(XMConvertToRadians(m_Yaw));
	Look = XMVector3TransformNormal(Look, R);
	Right = XMVector3TransformNormal(Right, R);
	Up = XMVector3TransformNormal(Up, R);


	if (m_bShoulderView)
	{
		// Pitch 적용
		R = XMMatrixRotationAxis(Right, XMConvertToRadians(m_ShoulderCameraPitch));

		Look = XMVector3TransformNormal(Look, R);
		Up = XMVector3TransformNormal(Up, R);

		float cameraOffsetHeight = m_ShoulderOffsetHeight;

		cameraOffsetHeight *= xmf3Scale.y;

		XMFLOAT3 xmf3ObjectPos = m_pObject->GetPosition();
		//xmf3ObjectPos.y += cameraOffsetHeight;
		XMVECTOR objectPos = XMLoadFloat3(&xmf3ObjectPos);

		cameraOffsetLength = m_ShoulderOffsetLength;
		cameraOffsetLength *= xmf3Scale.x;

		objectPos = Up * cameraOffsetHeight + objectPos;
		objectPos = Right * 4 + objectPos;
		objectPos = -Look * cameraOffsetLength + objectPos;
		XMStoreFloat3(&xmf3ObjectPos, objectPos);


		XMVECTOR newPos = XMLoadFloat3(&xmf3ObjectPos);
		float targetLength = 100.0f;
		newPos = Look * targetLength + newPos;

		XMFLOAT3 xmf3NewPos;
		XMStoreFloat3(&xmf3NewPos, newPos);
		SetPosition(xmf3NewPos);

		LookAt(xmf3ObjectPos, xmf3NewPos, XMFLOAT3(0, 1, 0));
	}
	else
	{
		// Pitch 적용
		R = XMMatrixRotationAxis(Right, XMConvertToRadians(m_Pitch));

		Look = XMVector3TransformNormal(Look, R);
		Up = XMVector3TransformNormal(Up, R);

		XMFLOAT3 xmf3ObjectPos = m_pObject->GetPosition();
		xmf3ObjectPos.y += m_OffsetHeight * xmf3Scale.y;;
		XMVECTOR objectPos = XMLoadFloat3(&xmf3ObjectPos);

		XMVECTOR newPos = XMLoadFloat3(&xmf3ObjectPos);
		newPos = -Look * cameraOffsetLength + newPos;

		// 카메라와 플레이어 사이에 오브젝트 있는지 확인
		XMVECTOR camDir = newPos - objectPos;
		camDir = XMVector3Normalize(camDir);
		float distance = 9999;
		float best = 9999;
		UINT nClosest = 0;
		for (int i = 1; i < g_vpWorldObjs.size(); ++i)
		{
			if (!g_vpWorldObjs[i]->GetIsAlive()) continue;
			if (g_vpWorldObjs[i]->GetColliderType() != ColliderType::Collider_Box) continue;

			ColliderBox* pColliderBox = (ColliderBox*)g_vpWorldObjs[i]->GetCollider().get();
			if (!pColliderBox->GetIsActive())
				continue;

			BoundingOrientedBox* pOBB = pColliderBox->GetOBB().get();
			if (!pOBB->Intersects(objectPos, camDir, distance))
				continue;
			if (distance < best && distance != 0 && distance > 10)
			{
				best = distance;
				nClosest = i;
			}
		}
		if (best > m_OffsetLength * xmf3Scale.x)
		{
			best = m_OffsetLength * xmf3Scale.x;
		}
		else if (best < 20)
		{
			best = 20;
			g_vpWorldObjs[nClosest]->SetVisible(false);
		}
		cameraOffsetLength = best;

		newPos = XMLoadFloat3(&xmf3ObjectPos);
		newPos = -Look * cameraOffsetLength + newPos;

		XMFLOAT3 xmf3NewPos;
		XMStoreFloat3(&xmf3NewPos, newPos);
		SetPosition(xmf3NewPos);

		LookAt(xmf3NewPos, xmf3ObjectPos, XMFLOAT3(0, 1, 0));
	}

	UpdateViewMatrix();
}

void Third_Person_Camera::UpdateViewFrustum()
{
	if (m_bShoulderView)
	{
		XMVECTOR orientation = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(m_ShoulderCameraPitch), XMConvertToRadians(m_Yaw), 0);
		orientation = XMQuaternionNormalize(orientation);
		XMStoreFloat4(&m_xmf4Orientation, orientation);

		m_pCameraFrustum->Origin = m_xmf3Position;
		m_pCameraFrustum->Orientation = m_xmf4Orientation;
	}
	else
	{
		XMVECTOR orientation = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(m_Pitch), XMConvertToRadians(m_Yaw), 0);
		orientation = XMQuaternionNormalize(orientation);
		XMStoreFloat4(&m_xmf4Orientation, orientation);

		m_pCameraFrustum->Origin = m_xmf3Position;
		m_pCameraFrustum->Orientation = m_xmf4Orientation;
	}
}

void Third_Person_Camera::Pitch(float angle)
{
	m_Pitch += angle;

	if (m_Pitch > m_MaxPitch)
	{ m_Pitch = m_MaxPitch; }
	if (m_Pitch < m_MinPitch) 
	{ m_Pitch = m_MinPitch; }

	m_ShoulderCameraPitch += angle;
	if (m_ShoulderCameraPitch > m_MaxShoulderPitch)
	{
		m_ShoulderCameraPitch = m_MaxShoulderPitch;
	}
	if (m_ShoulderCameraPitch < m_MinShoulderPitch)
	{
		m_ShoulderCameraPitch = m_MinShoulderPitch;
	}
}

void Third_Person_Camera::RotateY(float angle)
{
	m_Yaw += angle;
	if (m_Yaw > 360.0f) m_Yaw -= 360.0f;
	if (m_Yaw < 0.0f) m_Yaw += 360.0f;

}

// 시네마틱 카메라

CinematicCamera::CinematicCamera()
{
	SetLens(0.25 * MathHelper::Pi, 1.0f, 1.0f, 1000.f);

	m_pCameraFrustum = std::make_shared<BoundingFrustum>(XMFLOAT3(0, 0, 0), XMFLOAT4(0, 0, 0, 1), 0.6375f, -0.6375f, 0.425f, -0.425f, 1.0, 5000.0f);
}

CinematicCamera::~CinematicCamera()
{
}

void CinematicCamera::Update(float Etime)
{
	XMFLOAT3 xmf3Right = { 1,0,0 };
	XMFLOAT3 xmf3Up = { 0,1,0 };
	XMFLOAT3 xmf3Look = { 0,0,1 };

	XMMATRIX R = XMMatrixRotationQuaternion(XMLoadFloat4(&m_xmf4Orientation));

	XMStoreFloat3(&m_xmf3Right, XMVector3TransformNormal(XMLoadFloat3(&xmf3Right), R));
	XMStoreFloat3(&m_xmf3Up, XMVector3TransformNormal(XMLoadFloat3(&xmf3Up), R));
	XMStoreFloat3(&m_xmf3Look, XMVector3TransformNormal(XMLoadFloat3(&xmf3Look), R));

	m_bViewDirty = true;

	UpdateViewMatrix();
}

void CinematicCamera::UpdateViewFrustum()
{
	m_pCameraFrustum->Origin = m_xmf3Position;
	m_pCameraFrustum->Orientation = m_xmf4Orientation;
}