#include "../Header/RigidBody.h"

RigidBody::RigidBody()
{
}

RigidBody::RigidBody(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Orientation, float mass)
{
	m_xmf3Position = xmf3Position;
	m_xmf4Orientation = xmf4Orientation;
	//m_xmf3Scale = xmf3Scale;
	m_Mass = mass;
}

RigidBody::~RigidBody()
{
}

void RigidBody::Update(float elapsedTime)
{
	if (!m_bIsAwake) return;

	m_xmf3LastFrameAcceleration = m_xmf3Acceleration;
	// �߷°��ӵ� ���� (�߷� * ��ü ���� / ��ü ����(�����и� �и��ϱ� ����) )
	m_xmf3LastFrameAcceleration.y += Physics::xmf3Gravity.y;
	XMVECTOR linearAccel = XMLoadFloat3(&m_xmf3LastFrameAcceleration);

	XMVECTOR velocity = XMLoadFloat3(&m_xmf3Velocity);
	XMVECTOR angularVelocity = XMLoadFloat3(&m_xmf3AngularVelocity);

	velocity += linearAccel * elapsedTime;
	
	// damping ����
	velocity *= pow(m_LinearDamping, elapsedTime);
	angularVelocity *= pow(m_AngularDamping, elapsedTime);

	XMStoreFloat3(&m_xmf3Velocity, velocity);
	XMStoreFloat3(&m_xmf3AngularVelocity, angularVelocity);

	XMVECTOR position = XMLoadFloat3(&m_xmf3Position);
	position += velocity * elapsedTime;
	XMStoreFloat3(&m_xmf3Position, position);

	XMVECTOR prevOrientation = XMLoadFloat4(&m_xmf4Orientation);
	XMVECTOR dq = angularVelocity * elapsedTime;
	dq = XMQuaternionMultiply(prevOrientation, dq) * 0.5f;
	XMStoreFloat4(&m_xmf4Orientation, prevOrientation + dq);

	// �Ļ� ������ ����
	CalcDerivedData();

	// Sleep üũ�� ���� motion ��ġ�� �����Ѵ�.
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

void RigidBody::CalcDerivedData()
{
	// ���ʹϾ� ����ȭ
	XMStoreFloat4(&m_xmf4Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmf4Orientation)));
	UpdateWorldTransform();
	UpdateInverseRotateInertiaForWorld();
}

void RigidBody::UpdateWorldTransform()
{
	XMMATRIX World = XMMatrixIdentity();
	XMMATRIX Translation = XMMatrixTranslation(m_xmf3Position.x, m_xmf3Position.y, m_xmf3Position.z);
	XMMATRIX Rotate = XMMatrixRotationQuaternion(XMLoadFloat4(&m_xmf4Orientation));
	//XMMATRIX Scale = XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z);
	World = XMMatrixMultiply(Rotate, Translation);

	XMStoreFloat4x4(&m_xmf4x4World, World);
}

void RigidBody::UpdateInverseRotateInertiaForWorld()
{
	// World(-1) * Inertia(-1) * World ������ ��� ����
	// World ��ǥ���� �����Ϳ� ������ Inertia ���

	XMMATRIX inverseRotateInertia = XMLoadFloat4x4(&m_xmf4x4InverseRotateInertia);

	XMMATRIX world = XMLoadFloat4x4(&m_xmf4x4World);
	XMMATRIX transposeWorld = XMMatrixTranspose(world);
	XMMATRIX rotateInertiaForWorld = XMMatrixMultiply(transposeWorld, inverseRotateInertia);
	rotateInertiaForWorld = XMMatrixMultiply(rotateInertiaForWorld, world);

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
