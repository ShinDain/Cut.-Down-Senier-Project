#include "../Header/RigidBody.h"

RigidBody::RigidBody()
{
}

RigidBody::RigidBody(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Orientation, XMFLOAT3 xmf3Rotate, XMFLOAT3 xmf3Scale, float mass,
	XMFLOAT3 xmf3ColliderOffsetPosition, XMFLOAT3 xmf3ColliderOffsetRotation)
{
	m_xmf3Position = xmf3Position;
	m_xmf3Rotate = xmf3Rotate;
	m_xmf4Orientation = xmf4Orientation;
	m_xmf3Scale = xmf3Scale;
	m_Mass = mass;
	m_xmf3ColliderOffsetPosition = xmf3ColliderOffsetPosition;
	m_xmf3ColliderOffsetRotation = xmf3ColliderOffsetRotation;


	XMMATRIX ColliderWorld = XMLoadFloat4x4(&m_xmf4x4World);
	XMMATRIX OffsetTranslation = XMMatrixTranslation(m_xmf3ColliderOffsetPosition.x, m_xmf3ColliderOffsetPosition.y, m_xmf3ColliderOffsetPosition.z);
	XMMATRIX OffsetRotate = XMMatrixRotationRollPitchYaw(m_xmf3ColliderOffsetRotation.x, m_xmf3ColliderOffsetRotation.y, m_xmf3ColliderOffsetRotation.z);
	ColliderWorld = XMMatrixMultiply(XMMatrixMultiply(OffsetRotate, OffsetTranslation), ColliderWorld);

	XMVECTOR colliderPosition = XMVectorSet(0, 0, 0, 1);
	colliderPosition = XMVector3TransformCoord(colliderPosition, ColliderWorld);

	XMStoreFloat3(&m_xmf3ColliderPosition, colliderPosition);

	if (m_Mass < 1000)
	{
		m_bPhysics = true;
		m_bInGravity = true;
	}

}

RigidBody::~RigidBody()
{
}

void RigidBody::Update(float elapsedTime)
{
	m_ElapsedTimeAfterCreated += elapsedTime;

	if (!m_bIsAwake)
		return;

	if (m_ElapsedTimeAfterCreated > 0.0f && !m_bIsCharacter && !m_bIsPlatform)
		m_bCanSleep = true;

	if (m_bIsCharacter)
	{
		m_xmf4Orientation = XMFLOAT4(0, 0, 0, 1);
		m_xmf3AngularVelocity = XMFLOAT3(0, 0, 0);
		m_xmf3Rotate = XMFLOAT3(0, m_xmf3Rotate.y, 0);
	}

	m_xmf3LastFrameAcceleration = m_xmf3Acceleration;

	if (m_bInGravity)
	{
		// 중력가속도 적용 (중력 * 물체 질량 / 물체 질량(선성분만 분리하기 위해) )
		m_xmf3LastFrameAcceleration.y += Physics::xmf3Gravity.y * m_Mass;
	}

	XMVECTOR linearAccel = XMLoadFloat3(&m_xmf3LastFrameAcceleration);

	XMVECTOR velocity = XMLoadFloat3(&m_xmf3Velocity);
	velocity += linearAccel * elapsedTime;
	velocity *= pow(m_LinearDamping, elapsedTime);
	XMStoreFloat3(&m_xmf3Velocity, velocity);
	XMVECTOR position = XMLoadFloat3(&m_xmf3Position);
	position += velocity * elapsedTime;
	XMStoreFloat3(&m_xmf3Position, position);

	XMVECTOR angularVelocity = XMLoadFloat3(&m_xmf3AngularVelocity);

	if (!m_bIsCharacter)
	{
		// damping 적용
		angularVelocity *= pow(m_AngularDamping, elapsedTime);
		XMStoreFloat3(&m_xmf3AngularVelocity, angularVelocity);
		XMVECTOR prevOrientation = XMLoadFloat4(&m_xmf4Orientation);
		XMVECTOR dq = angularVelocity * elapsedTime;
		dq = XMQuaternionMultiply(prevOrientation, dq) * 0.5f;
		XMStoreFloat4(&m_xmf4Orientation, prevOrientation + dq);
	}
	else
		m_xmf4Orientation = XMFLOAT4(0, 0, 0, 1);

	if (XMVector3IsInfinite(XMLoadFloat3(&m_xmf3Position)) || XMVector3IsNaN(XMLoadFloat3(&m_xmf3Position)))
	{
		SetInvalid(true);
		return;
	}

	// 파생 데이터 갱신
	CalcDerivedData();

	// Sleep 체크를 위해 motion 수치를 조정한다.
	if (m_bCanSleep)
	{
		float currentMotion = XMVectorGetX(XMVectorSum(velocity * velocity)) +
			XMVectorGetX(XMVectorSum(angularVelocity * angularVelocity));

		float bias = powf(0.5f, elapsedTime);
		m_Motion = bias * m_Motion + (1 - bias) * currentMotion;

		if (m_Motion < Physics::sleepEpsilon)
			SetIsAwake(false);
		else if (m_Motion > 10 * Physics::sleepEpsilon) m_Motion = 10 * Physics::sleepEpsilon;

		m_lastMotion = m_Motion;
	}
}

void RigidBody::CalcDerivedData()
{
	// 쿼터니언 정규화
	XMStoreFloat4(&m_xmf4Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmf4Orientation)));

	XMMATRIX World = XMMatrixIdentity();
	XMMATRIX Translation = XMMatrixTranslation(m_xmf3Position.x, m_xmf3Position.y, m_xmf3Position.z);
	XMMATRIX Rotate = XMMatrixRotationQuaternion(XMLoadFloat4(&m_xmf4Orientation));
	if (m_bIsCharacter)
	{
		Rotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_xmf3Rotate.x),
			XMConvertToRadians(m_xmf3Rotate.y),
			XMConvertToRadians(m_xmf3Rotate.z));
	}

	XMMATRIX Scale = XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z);
	World = XMMatrixMultiply(Scale, XMMatrixMultiply(Rotate, Translation));

	XMStoreFloat4x4(&m_xmf4x4World, World);

	// Collider Position 갱신
	XMMATRIX ColliderWorld = World;
	XMMATRIX OffsetTranslation = XMMatrixTranslation(m_xmf3ColliderOffsetPosition.x, m_xmf3ColliderOffsetPosition.y, m_xmf3ColliderOffsetPosition.z);
	XMMATRIX OffsetRotate = XMMatrixRotationRollPitchYaw(m_xmf3ColliderOffsetRotation.x, m_xmf3ColliderOffsetRotation.y, m_xmf3ColliderOffsetRotation.z);
	ColliderWorld = XMMatrixMultiply(XMMatrixMultiply(OffsetRotate, OffsetTranslation), ColliderWorld);

	XMVECTOR colliderPosition = XMVectorSet(0, 0, 0, 1);
	colliderPosition = XMVector3TransformCoord(colliderPosition, ColliderWorld);

	XMStoreFloat3(&m_xmf3ColliderPosition, colliderPosition);

	XMMATRIX rotateInertiaForWorld = XMLoadFloat4x4(&m_xmf4x4InverseRotateInertia);

	XMStoreFloat4x4(&m_xmf4x4InverseRotateInertiaForWorld, rotateInertiaForWorld);
}

void RigidBody::AddContact(std::shared_ptr<Contact> pContact)
{
	m_vContacts.push_back(pContact);
}

void RigidBody::ClearContact()
{
	for (int i = 0; i < m_vContacts.size(); ++i)
		m_vContacts[i].reset();

	m_vContacts.clear();
}

void RigidBody::Destroy()
{
	ClearContact();
}

void RigidBody::SetRotateInertia(XMFLOAT4X4 xmf4x4RotateInertia)
{
	m_xmf4x4RotateInertia = xmf4x4RotateInertia;

	XMMATRIX inverseinertia = XMMatrixInverse(nullptr, XMLoadFloat4x4(&xmf4x4RotateInertia));

	if (XMMatrixIsNaN(inverseinertia))
		m_xmf4x4InverseRotateInertia = XMFLOAT4X4();
	else
		XMStoreFloat4x4(&m_xmf4x4InverseRotateInertia, inverseinertia);

}

void RigidBody::AddVelocity(float x, float y, float z)
{
	m_xmf3Velocity.x += x;
	m_xmf3Velocity.y += y;
	m_xmf3Velocity.z += z;
}
void RigidBody::AddVelocity(XMFLOAT3 addVelocity)
{
	if (m_bIsPlatform) return;
	AddVelocity(addVelocity.x, addVelocity.y, addVelocity.z);
}
void RigidBody::AddAngleVelocity(float x, float y, float z)
{
	m_xmf3AngularVelocity.x += x;
	m_xmf3AngularVelocity.y += y;
	m_xmf3AngularVelocity.z += z;
}
void RigidBody::AddAngleVelocity(XMFLOAT3 addAngleVelocity)
{
	if (m_bIsPlatform) return;
	if (m_bIsCharacter) return;
	AddAngleVelocity(addAngleVelocity.x, addAngleVelocity.y, addAngleVelocity.z);
}