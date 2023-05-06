#include "../Header/RigidBody.h"

RigidBody::RigidBody()
{
}

RigidBody::RigidBody(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Orientation, XMFLOAT3 xmf3Scale, float mass)
{
	m_xmf3Position = xmf3Position;
	m_xmf4Orientation = xmf4Orientation;
	m_xmf3Scale = xmf3Scale;
	m_Mass = mass;
}

RigidBody::~RigidBody()
{
}

void RigidBody::Update(float elapsedTime)
{
	if (!m_bIsAwake) 
		return;

	m_xmf3LastFrameAcceleration = m_xmf3Acceleration;

	if (m_bInGravity)
	{
		// 중력가속도 적용 (중력 * 물체 질량 / 물체 질량(선성분만 분리하기 위해) )
		m_xmf3LastFrameAcceleration.y += Physics::xmf3Gravity.y;
	}

	XMVECTOR linearAccel = XMLoadFloat3(&m_xmf3LastFrameAcceleration);

	XMVECTOR velocity = XMLoadFloat3(&m_xmf3Velocity);
	XMVECTOR angularVelocity = XMLoadFloat3(&m_xmf3AngularVelocity);

	velocity += linearAccel * elapsedTime;

	// damping 적용
	velocity *= pow(m_LinearDamping, elapsedTime);
	angularVelocity *= pow(m_AngularDamping, elapsedTime);

	XMStoreFloat3(&m_xmf3Velocity, velocity);
	XMStoreFloat3(&m_xmf3AngularVelocity, angularVelocity);

	XMVECTOR position = XMLoadFloat3(&m_xmf3Position);
	position += velocity * elapsedTime;
	XMStoreFloat3(&m_xmf3Position, position);


	if (XMVector3IsInfinite(XMLoadFloat3(&m_xmf3Position)) || XMVector3IsNaN(XMLoadFloat3(&m_xmf3Position)))
	{
		SetInvalid(true);
		return;
	}

	XMVECTOR prevOrientation = XMLoadFloat4(&m_xmf4Orientation);
	XMVECTOR dq = angularVelocity * elapsedTime;
	dq = XMQuaternionMultiply(prevOrientation, dq) * 0.5f;
	XMStoreFloat4(&m_xmf4Orientation, prevOrientation + dq);

	// 파생 데이터 갱신
	CalcDerivedData();

	// Sleep 체크를 위해 motion 수치를 조정한다.
	if (m_bCanSleep)
	{
		float currentMotion =  XMVectorGetX(XMVectorSum(velocity * velocity)) + 
			XMVectorGetX(XMVectorSum(angularVelocity * angularVelocity));

		float bias = powf(0.5f, elapsedTime);
		m_Motion = bias * m_Motion + (1 - bias) * currentMotion;

		if (m_Motion < Physics::sleepEpsilon ) 
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
	XMMATRIX Scale = XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z);
	World = XMMatrixMultiply(Scale, XMMatrixMultiply(Rotate, Translation));

	XMStoreFloat4x4(&m_xmf4x4World, World);

	// World(-1) * Inertia(-1) * World 순서로 행렬곱
	/*XMMATRIX inverseRotateInertia = XMLoadFloat4x4(&m_xmf4x4InverseRotateInertia);
	XMMATRIX rotateInertiaForWorld = XMMatrixMultiply(XMMatrixTranspose(World), inverseRotateInertia);
	rotateInertiaForWorld = XMMatrixMultiply(rotateInertiaForWorld, World);*/

	XMMATRIX rotateInertiaForWorld = XMLoadFloat4x4(&m_xmf4x4InverseRotateInertia);

	XMStoreFloat4x4(&m_xmf4x4InverseRotateInertiaForWorld, rotateInertiaForWorld);
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