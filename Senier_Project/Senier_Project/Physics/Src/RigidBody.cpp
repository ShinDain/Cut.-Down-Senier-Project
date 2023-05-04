#include "../Header/RigidBody.h"

RigidBody::RigidBody()
{
}

RigidBody::RigidBody(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Rotate, XMFLOAT3 xmf3Scale, float mass)
{
	m_xmf3Position = xmf3Position;
	m_xmf3Rotate = xmf3Rotate;
	m_xmf3Scale = xmf3Scale;
	m_Mass = mass;
}

RigidBody::~RigidBody()
{
}

void RigidBody::Update(float elapsedTime)
{
	if (!m_bIsAwake) return;

	m_xmf3LastFrameAcceleration = m_xmf3Acceleration;
	// 중력가속도 적용 (중력 * 물체 질량 / 물체 질량(선성분만 분리하기 위해) )
	m_xmf3LastFrameAcceleration.y += Physics::xmf3Gravity.y;
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
	XMVECTOR rotation = XMLoadFloat3(&m_xmf3Rotate);
	position += velocity * elapsedTime;
	rotation += angularVelocity * elapsedTime;
	XMStoreFloat3(&m_xmf3Position, position);
	XMStoreFloat3(&m_xmf3Rotate, rotation);

	UpdateWorldTransform();
	UpdateInverseRotateInertiaForWorld();

	// Sleep 체크를 위해 motion 수치를 조정한다.
	if (m_bCanSleep)
	{
		float currentMotion =  XMVectorGetX(XMVectorSum(velocity * velocity)) + 
			XMVectorGetX(XMVectorSum(angularVelocity * angularVelocity));

		float bias = pow(0.5, elapsedTime);
		m_Motion = bias * m_Motion + (1 - bias) * currentMotion;

		if (m_Motion > Physics::sleepEpsilon) SetIsAwake(false);
		else if (m_Motion > 10 * Physics::sleepEpsilon) m_Motion = 10 * Physics::sleepEpsilon;
	}
}

void RigidBody::UpdateWorldTransform()
{
	XMMATRIX World = XMMatrixIdentity();
	XMMATRIX Translation = XMMatrixTranslation(m_xmf3Position.x, m_xmf3Position.y, m_xmf3Position.z);
	XMMATRIX Rotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_xmf3Rotate.x),
												   XMConvertToRadians(m_xmf3Rotate.y),
												   XMConvertToRadians(m_xmf3Rotate.z));
	XMMATRIX Scale = XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z);
	World = XMMatrixMultiply(Scale, XMMatrixMultiply(Rotate, Translation));

	XMStoreFloat4x4(&m_xmf4x4World, World);
}

void RigidBody::UpdateInverseRotateInertiaForWorld()
{
	// World(-1) * Inertia(-1) * World 순서로 행렬 갱신
	// World 좌표계의 데이터에 곱해질 Inertia 행렬

	XMMATRIX inverseRotateInertia = XMLoadFloat4x4(&m_xmf4x4RotateInertia);
	inverseRotateInertia = XMMatrixInverse(nullptr, inverseRotateInertia);

	XMMATRIX world = XMLoadFloat4x4(&m_xmf4x4World);
	XMMATRIX inverseWorld = XMMatrixInverse(nullptr, world);
	XMMATRIX rotateInertiaForWorld = XMMatrixIdentity();

	rotateInertiaForWorld = XMMatrixMultiply(inverseWorld, inverseRotateInertia);
	rotateInertiaForWorld = XMMatrixMultiply(rotateInertiaForWorld, world);

	XMStoreFloat4x4(&m_xmf4x4InverseRotateInertiaForWorld, rotateInertiaForWorld);
}
