#include "../Header/Contact.h"

Contact::Contact(RigidBody* pBody1, RigidBody* pBody2, float friction, float restitution, XMFLOAT3 xmf3ContactPoint, XMFLOAT3 xmf3ContactNormal, float depth)
	: m_Friction(friction), m_Restitution(restitution), m_xmf3ContactPoint(xmf3ContactPoint), m_xmf3ContactNormal(xmf3ContactNormal), m_Depth(depth)
{
	m_pBody[0] = pBody1;
	m_pBody[1] = pBody2;
}

Contact::~Contact()
{
}

void Contact::CalcInternals(float elapsedTime)
{
	// Contact Resolve�� ���� ������ �غ�
	if (!m_pBody[0]) SwapBodies();

	// �߸��� Contact ������ assert
	assert(m_pBody[0]);

	CalcContactToWorld();

	// �浹���� ��� ��ǥ ���, RelativePoint�� World ��ǥ��
	XMVECTOR contactPoint = XMLoadFloat3(&m_xmf3ContactPoint);
	XMVECTOR bodyPos_0 = XMLoadFloat3(&m_pBody[0]->GetPosition());
	XMStoreFloat3(&m_pxmf3RelativePosition[0], contactPoint - bodyPos_0);

	// ��ü�� ���� �ӵ� ���
	XMVECTOR contactVelocity = CalcLocalVelocity(0, elapsedTime);

	// ��ü 1�� ���� ��, ���� ���� ����
	if (m_pBody[1])
	{
		XMVECTOR bodyPos_1 = XMLoadFloat3(&m_pBody[1]->GetPosition());
		XMStoreFloat3(&m_pxmf3RelativePosition[1], contactPoint - bodyPos_1);
	
		// ���� ���� �ӵ��� �ݴ� �����̹Ƿ� ���ش�.
		// ContactVelocity = (�и� �ӵ�)
		contactVelocity -= CalcLocalVelocity(1, elapsedTime);
	}
	XMStoreFloat3(&m_xmf3ContactVelocity, contactVelocity);

	CalcDesiredDeltaVelocity(elapsedTime);
}

void Contact::ApplyVelocityChange(XMVECTOR& deltaLinearVel_0, XMVECTOR& deltaLinearVel_1, 
								  XMVECTOR& deltaAngularVel_0, XMVECTOR& deltaAngularVel_1)
{
	// World ��ǥ�迡 ����� InverseInertia ���
	XMMATRIX inverseInertia_0 = XMLoadFloat4x4(&m_pBody[0]->GetInverseRotateInertiaForWorld());
	XMMATRIX inverseInertia_1 = XMLoadFloat4x4(&m_pBody[1]->GetInverseRotateInertiaForWorld());

	// Contact ��ǥ���� Impulse
	XMVECTOR impulseContact;

	if (m_Friction == 0)
	{
		impulseContact = CalcFrictionlessImpulse(inverseInertia_0, inverseInertia_1);
	}
	else
	{
		impulseContact = CalcFrictionImpulse(inverseInertia_0, inverseInertia_1);
	}

	// World ��ǥ��� ��ȯ
	XMMATRIX contactToWorld = XMLoadFloat4x4(&m_xmf4x4ContactToWorld);
	XMVECTOR impulse = XMVector3TransformNormal(impulseContact, contactToWorld);

	// ��ݷ��� �̿��� �����а� ������ ��ȭ�� ���
	XMVECTOR angularResult = XMLoadFloat3(&m_pxmf3RelativePosition[0]);
	angularResult = XMVector3Cross(angularResult, impulse);
	angularResult = XMVector3TransformNormal(angularResult, inverseInertia_0);
	deltaAngularVel_0 = angularResult;
	XMVECTOR mass = XMVectorReplicate(m_pBody[0]->GetMass());
	XMVECTOR linearResult = impulse / mass;
	deltaLinearVel_0 = linearResult;
	XMFLOAT3 xmf3LinearResult;
	XMFLOAT3 xmf3AngularResult;
	XMStoreFloat3(&xmf3LinearResult, linearResult);
	XMStoreFloat3(&xmf3AngularResult, angularResult);

	m_pBody[0]->AddVelocity(xmf3LinearResult);
	m_pBody[0]->AddAngleVelocity(xmf3AngularResult);

	if (m_pBody[1])
	{
		angularResult = XMLoadFloat3(&m_pxmf3RelativePosition[1]);
		// Body1�� ��� �ݴ� ���� �ۿ��̹Ƿ�,
		// pos X (-impulse) => impulse X pos
		angularResult = XMVector3Cross(impulse, angularResult);
		angularResult = XMVector3TransformNormal(angularResult, inverseInertia_1);
		deltaAngularVel_1 = angularResult;

		mass = XMVectorReplicate(m_pBody[1]->GetMass());

		// ��ݷ� ������ �ݴ��
		linearResult = -impulse / mass;
		deltaLinearVel_1 = linearResult;
		XMStoreFloat3(&xmf3LinearResult, linearResult);
		XMStoreFloat3(&xmf3AngularResult, angularResult);

		m_pBody[1]->AddVelocity(xmf3LinearResult);
		m_pBody[1]->AddAngleVelocity(xmf3AngularResult);
	}
}

void Contact::ApplyPositionChange(XMVECTOR& deltaLinearVel_0, XMVECTOR& deltaLinearVel_1, 
								  XMVECTOR& deltaAngularVel_0, XMVECTOR& deltaAngularVel_1, float depth)
{
	const float angularLimit = 0.2f;

	float angularMove[2];
	float linearMove[2];

	float totalInertia = 0;
	float linearInertia[2];
	float angularInertia[2];

	XMVECTOR contactNormal = XMLoadFloat3(&m_xmf3ContactNormal);

	// ���� �ذ��� ���� Position ��ȭ�� ��Ű�� ��,
	// ������ ���� �߻��� Position�� ��ȭ�� �� ��ü�� ���� ������ ����ް� �ȴ�.
	// �ش� ���� ����� ���� �浹�� ���� �߻��� ��ݷ��� ũ�⸦ �̿��Ѵ�.	(å������ �̸� �������� ǥ��)
	// ���� ������ FrictionLessImpulse�� �����ϴ�.
	for (int i = 0; i < 2; ++i)
	{
		if (m_pBody[i])
		{
			XMMATRIX inverseInertiaForWorld = XMLoadFloat4x4(&m_pBody[i]->GetInverseRotateInertiaForWorld());
			XMVECTOR relativePosition = XMLoadFloat3(&m_pxmf3RelativePosition[i]);

			// ������
			XMVECTOR angularInertiaWorld = relativePosition;
			angularInertiaWorld = XMVector3Cross(angularInertiaWorld, contactNormal);
			angularInertiaWorld = XMVector3TransformNormal(angularInertiaWorld, inverseInertiaForWorld);
			angularInertiaWorld = XMVector3Cross(angularInertiaWorld, relativePosition);
			angularInertia[i] = XMVectorGetX(XMVector3Dot(angularInertiaWorld, contactNormal));

			// ������
			linearInertia[i] = ((float)1) / m_pBody[i]->GetMass();

			// ��� ��/������ ������ ��
			totalInertia += angularInertia[i] + linearInertia[i];
		}
	}

	for (int i = 0; i < 2; ++i)
	{
		if (m_pBody[i])
		{
			// ������ ���� ���� Position ��ȭ�� ����
			float sign = (i == 0) ? 1 : -1;
			angularMove[i] = sign * m_Depth * (angularInertia[i] / totalInertia);
			linearMove[i] = sign * m_Depth * (linearInertia[i] / totalInertia);

			// ��ü�� �����߽��� �Ѿ� ������ ���, ���ϰ� ȸ���ϴ� ��츦 �����ϱ� ���ؼ�
			// Contact Normal�� �������� �ϴ� ��鿡 �翵��Ų 
			// ũ�⸦ �����Ͽ� ȸ���� �����Ѵ�. => Relative Point�� ���� ���� ����
			XMVECTOR relativePosition = XMLoadFloat3(&m_pxmf3RelativePosition[i]);

			XMVECTOR projection = relativePosition;
			XMVECTOR amountContact = XMVector3Dot(projection, contactNormal);
			amountContact *= contactNormal;
			projection -= amountContact;

			float maxMagnitude = angularLimit * XMVectorGetX(XMVector3Length(projection));

			if (angularMove[i] < -maxMagnitude)
			{
				float totalMove = angularMove[i] + linearMove[i];
				angularMove[i] = -maxMagnitude;
				linearMove[i] = totalMove - angularMove[i];
			}
			else if (angularMove[i] > maxMagnitude)
			{
				float totalMove = angularMove[i] + linearMove[i];
				angularMove[i] = maxMagnitude;
				linearMove[i] = totalMove - angularMove[i];
			}

			if (angularMove[i] == 0)
			{
				if (i == 0)
					deltaAngularVel_0 = XMVectorZero();
				else if(i == 1)
					deltaAngularVel_1 = XMVectorZero();
			}
			else
			{
				XMVECTOR rotatePerMove = relativePosition;
				rotatePerMove = XMVector3Cross(rotatePerMove, contactNormal);
				XMMATRIX inverseInertia = XMLoadFloat4x4(&m_pBody[i]->GetInverseRotateInertiaForWorld());

				rotatePerMove = XMVector3TransformNormal(rotatePerMove, inverseInertia);
				rotatePerMove /= angularInertia[i];
				
				if (i == 0)
					deltaAngularVel_0 = angularMove[i] * rotatePerMove;
				else if (i == 1)
					deltaAngularVel_1 = angularMove[i] * rotatePerMove;
			}

			if (i == 0)
				deltaLinearVel_0 = contactNormal * linearMove[i];
			else if (i == 1)
				deltaLinearVel_1 = contactNormal * linearMove[i];

			XMVECTOR position = XMLoadFloat3(&m_pBody[i]->GetPosition());
			position += contactNormal * linearMove[i];
			XMFLOAT3 xmf3Position;
			XMStoreFloat3(&xmf3Position, position);
			m_pBody[i]->SetPosition(xmf3Position);

			XMVECTOR rotate = XMLoadFloat3(&m_pBody[i]->GetRotate());
			if (i == 0)
				rotate += deltaLinearVel_0;
			else if (i == 1)
				rotate += deltaLinearVel_1;
			XMFLOAT3 xmf3Rotate;
			XMStoreFloat3(&xmf3Rotate, rotate);
			m_pBody[i]->SetRotate(xmf3Rotate);


			if (!m_pBody[i]->GetIsAwake())
			{
				m_pBody[i]->UpdateWorldTransform();
				m_pBody[i]->UpdateInverseRotateInertiaForWorld();
			}
		}
	}
}

void Contact::SwapBodies()
{
	m_xmf3ContactNormal.x *= -1;
	m_xmf3ContactNormal.y *= -1;
	m_xmf3ContactNormal.z *= -1;

	RigidBody* tmp = m_pBody[0];
	m_pBody[0] = m_pBody[1];
	m_pBody[1] = tmp;
}

void Contact::MatchAwakeState()
{
	if (!m_pBody[1]) return;

	bool bWake0 = m_pBody[0]->GetIsAwake();
	bool bWake1 = m_pBody[1]->GetIsAwake();

	if (bWake0 ^ bWake1)
	{
		if (bWake0) m_pBody[1]->SetIsAwake(true);
		else m_pBody[0]->SetIsAwake(true);
	}
}

void Contact::CalcDesiredDeltaVelocity(float elapsedTime)
{
	const static float velocityLimit = 0.25f;

	// �̹� �����ӿ� ������ ���ӵ��� ũ�� ���
	float velocityFromAcc = 0;
	XMVECTOR contactNormal = XMLoadFloat3(&m_xmf3ContactNormal);

	// ContactNormal �������� �ۿ��� ���� ���� ���ӵ� �ӵ��� ũ�⸦ ����Ͽ� ���Ѵ�.
	// �߷¿� ���� ����ɾ� �����ϴ� ������ ����
	if (m_pBody[0]->GetIsAwake())
	{
		XMVECTOR lastFrameAccel = XMLoadFloat3(&m_pBody[0]->GetLastFrameAcceleration());
		
		lastFrameAccel = lastFrameAccel * elapsedTime;
		lastFrameAccel = XMVector3Dot(lastFrameAccel, contactNormal);
		
		velocityFromAcc += XMVectorGetX(lastFrameAccel);
	}

	if (m_pBody[1] && m_pBody[1]->GetIsAwake())
	{
		XMVECTOR lastFrameAccel = XMLoadFloat3(&m_pBody[1]->GetLastFrameAcceleration());

		lastFrameAccel = lastFrameAccel * elapsedTime;
		lastFrameAccel = XMVector3Dot(lastFrameAccel, contactNormal);

		velocityFromAcc += XMVectorGetX(lastFrameAccel);
	}

	// �ӵ��� �ʹ� �۴ٸ� ź�� ����� 0����,
	float thisRestitution = m_Restitution;
	if (fabs(m_xmf3ContactVelocity.x) < velocityLimit)
	{
		thisRestitution = 0;
	}


	m_DesiredDeltaVelocity = -m_xmf3ContactVelocity.x - thisRestitution * (m_xmf3ContactVelocity.x - velocityFromAcc);
}

XMVECTOR Contact::CalcLocalVelocity(int nbodyidx, float elapsedTime)
{
	RigidBody* thisBody = m_pBody[nbodyidx];

	XMVECTOR relativePosition = XMLoadFloat3(&m_pxmf3RelativePosition[nbodyidx]);
	XMVECTOR velocity = XMLoadFloat3(&thisBody->GetAngularVelocity());

	// ��ü�� ���� ���ӵ��� �����Ͽ� ȸ�� ���� �ӵ��� ����
	velocity = XMVector3Cross(velocity, relativePosition);			
	velocity += XMLoadFloat3(&thisBody->GetVelocity());

	XMMATRIX contactToWorld = XMLoadFloat4x4(&m_xmf4x4ContactToWorld);
	XMVECTOR contactVelocity = XMVector3TransformNormal(velocity, XMMatrixTranspose(contactToWorld));

	// ���ӵ��� ���� ��ȭ�� �ӵ��� ����
	XMVECTOR accVelocity = XMLoadFloat3(&thisBody->GetLastFrameAcceleration()) * elapsedTime;
	accVelocity = XMVector3TransformNormal(accVelocity, XMMatrixTranspose(contactToWorld));

	// ���鿡�� ��ü�� ����ɴ� ���� ���� (������� �� �������� �ۿ��Կ��� �̲������� ���� �߻�)
	// ������ ��� ���� �ӵ��� �����ϱ� ���ؼ� ContactNormal ������ �ӵ��� ����
	XMFLOAT3 xmf3AccelVelocity = XMFLOAT3(0, 0, 0);
	XMStoreFloat3(&xmf3AccelVelocity, accVelocity);
	xmf3AccelVelocity.x = 0;
	accVelocity = XMLoadFloat3(&xmf3AccelVelocity);

	contactVelocity += accVelocity;
	
	return contactVelocity;
}

void Contact::CalcContactToWorld()
{
	// Contact Normal�� X������ �ϴ� ��ǥ�踦 Contact ��ǥ��� ����,
	// å�� ���� ����� ���� �ٸ�

	XMVECTOR contactNormal = XMLoadFloat3(&m_xmf3ContactNormal);
	XMVECTOR contactTangentY = XMVectorZero();
	XMVECTOR contactTangentZ = XMVectorZero();
	XMFLOAT3 xmf3WorldY = XMFLOAT3(0, 1, 0);
	XMVECTOR worldY = XMLoadFloat3(&xmf3WorldY);

	contactTangentZ = XMVector3Cross(worldY, contactNormal);
	contactTangentZ = XMVector3Normalize(contactTangentZ);

	contactTangentY = XMVector3Cross(contactTangentZ, contactNormal);
	contactTangentY = XMVector3Normalize(contactTangentY);

	XMMATRIX contactToWorld = XMMatrixIdentity();
	contactToWorld.r[0] = contactNormal;
	contactToWorld.r[1] = contactTangentY;
	contactToWorld.r[2] = contactTangentZ;
}

XMVECTOR Contact::CalcFrictionlessImpulse(FXMMATRIX InverseInertia_0, CXMMATRIX InverseInertia_1)
{
	XMVECTOR impulseContact = XMVectorZero();
	XMVECTOR contactNormal = XMLoadFloat3(&m_xmf3ContactNormal);

	// ���� ��ݷ� �� �ӵ���ȭ�� ���
	// ���ӵ�
	XMVECTOR relativePosition_0 = XMLoadFloat3(&m_pxmf3RelativePosition[0]);			// RelativePosition�� World ��ǥ��
	XMVECTOR deltaVelWorld = XMVector3Cross(relativePosition_0, contactNormal);
	deltaVelWorld = XMVector3TransformNormal(deltaVelWorld, InverseInertia_0);
	deltaVelWorld = XMVector3Cross(deltaVelWorld, relativePosition_0);

	// Contact Normal�� �翵�Ͽ� Normal ���� ũ�⸸ ����
	float deltaVelocity = XMVectorGetX(XMVector3Dot(deltaVelWorld, contactNormal));
	// ���ӵ�
	deltaVelocity += ((float)1) / m_pBody[0]->GetMass();

	if (m_pBody[1])
	{
		// ���ӵ�
		XMVECTOR relativePosition_1 = XMLoadFloat3(&m_pxmf3RelativePosition[1]);		// RelativePosition�� World ��ǥ��
		deltaVelWorld = XMVector3Cross(relativePosition_1, contactNormal);
		deltaVelWorld = XMVector3TransformNormal(deltaVelWorld, InverseInertia_1);
		deltaVelWorld = XMVector3Cross(deltaVelWorld, relativePosition_1);

		// Contact Normal�� �翵�Ͽ� Normal ���� ũ�⸸ ����
		deltaVelocity += XMVectorGetX(XMVector3Dot(deltaVelWorld, contactNormal));
		// ���ӵ�
		deltaVelocity += ((float)1) / m_pBody[1]->GetMass();
	}

	// ��� �ӵ���ȭ���� ���� ��ݷ��� �ӵ���ȭ������ ���� Impulse�� ũ�� ȹ��, Contact ��ǥ��
	XMFLOAT3 xmf3Result = XMFLOAT3(m_DesiredDeltaVelocity / deltaVelocity, 0, 0);
	XMVECTOR result = XMLoadFloat3(&xmf3Result);

	return result;
}

XMVECTOR Contact::CalcFrictionImpulse(FXMMATRIX InverseInertia_0, CXMMATRIX InverseInertia_1)
{
	XMVECTOR resultImpulse = XMVectorZero();
	float inverseMass = ((float)1) / m_pBody[0]->GetMass();

	// �������� ���� ������ �ݴ�� �ۿ��Ѵ�. 
	// ���� ���� Contact Normal ������ ������ �ƴ� Contact ��ǥ���� 
	// x, y, z �� ��θ� �����Ͽ� Impulse�� ����� �ʿ䰡 �ִ�.
	// ������ ����� Ȱ���� ���̴�.
	
	// ���� ��ݷ��� Toque�� ũ�⸦ ��ȯ�ϴ� ���
	XMMATRIX impulseToToque = MathHelper::MakeSkewSymmetric(m_pxmf3RelativePosition[0]);

	// ���� ��ݷ��� ���ӵ� ��ȭ���� ��ȯ�ϴ� ���, World ��ǥ��
	XMMATRIX deltaVelPerImpulseWorld = impulseToToque;
	deltaVelPerImpulseWorld = XMMatrixMultiply(deltaVelPerImpulseWorld, InverseInertia_0);
	deltaVelPerImpulseWorld = XMMatrixMultiply(deltaVelPerImpulseWorld, impulseToToque);
	deltaVelPerImpulseWorld *= -1;

	if (m_pBody[1])
	{
		impulseToToque = MathHelper::MakeSkewSymmetric(m_pxmf3RelativePosition[1]);

		XMMATRIX deltaVelPerImpulseWorld2 = impulseToToque;
		deltaVelPerImpulseWorld2 = XMMatrixMultiply(deltaVelPerImpulseWorld2, InverseInertia_1);
		deltaVelPerImpulseWorld2 = XMMatrixMultiply(deltaVelPerImpulseWorld2, impulseToToque);
		deltaVelPerImpulseWorld2 *= -1;

		// �ӵ��� �ѷ��� �߰�
		deltaVelPerImpulseWorld += deltaVelPerImpulseWorld2;

		inverseMass += ((float)1) / m_pBody[1]->GetMass();
	}

	XMMATRIX contactToWorld = XMLoadFloat4x4(&m_xmf4x4ContactToWorld);

	XMMATRIX deltaVelPerImpulse = contactToWorld;
	deltaVelPerImpulse = XMMatrixMultiply(deltaVelPerImpulse, deltaVelPerImpulseWorld);
	deltaVelPerImpulse = XMMatrixMultiply(deltaVelPerImpulseWorld, XMMatrixTranspose(contactToWorld));

	XMFLOAT4X4 xmf4x4DeltaVelPerImpulse;
	XMStoreFloat4x4(&xmf4x4DeltaVelPerImpulse, deltaVelPerImpulse);
	// ������ �߰�
	xmf4x4DeltaVelPerImpulse._11 += inverseMass;
	xmf4x4DeltaVelPerImpulse._22 += inverseMass;
	xmf4x4DeltaVelPerImpulse._33 += inverseMass;
	deltaVelPerImpulse = XMLoadFloat4x4(&xmf4x4DeltaVelPerImpulse);

	// ���� �ӵ� ��ȭ���� ��ݷ�
	XMMATRIX impulsePerDeltaVel = XMMatrixInverse(nullptr, deltaVelPerImpulse);

	// ó���� ���� �ӵ�
	XMFLOAT3 xmf3VelKill = XMFLOAT3(m_DesiredDeltaVelocity, -m_xmf3ContactVelocity.y, -m_xmf3ContactVelocity.z);
	resultImpulse = XMLoadFloat3(&xmf3VelKill);
	resultImpulse = XMVector3TransformNormal(resultImpulse, impulsePerDeltaVel);

	XMFLOAT3 xmf3ImpulseContact;
	XMStoreFloat3(&xmf3ImpulseContact, resultImpulse);

	float planarImpulse = sqrtf(xmf3ImpulseContact.y * xmf3ImpulseContact.y + xmf3ImpulseContact.z * xmf3ImpulseContact.z);

	// YZ��� �ӵ��� ���� �������� ū ���
	if (planarImpulse > xmf3ImpulseContact.x * m_Friction)
	{
		// ���� ũ��� ���� (���� ����)
		xmf3ImpulseContact.y /= planarImpulse;
		xmf3ImpulseContact.z /= planarImpulse;

		// ��鿡 ���� ������ �����Ͽ� ���
		// Impulse�� Y,Z ������ Contact Normal�� �翵 ���� ���� ��ݷ��� �߰��Ѵ�.
		xmf3ImpulseContact.x = xmf4x4DeltaVelPerImpulse._11 + 
			xmf4x4DeltaVelPerImpulse._12 * m_Friction * xmf3ImpulseContact.y +
			xmf4x4DeltaVelPerImpulse._13 * m_Friction * xmf3ImpulseContact.z;
		xmf3ImpulseContact.x = m_DesiredDeltaVelocity / xmf3ImpulseContact.x;

		xmf3ImpulseContact.y *= m_Friction * xmf3ImpulseContact.x;
		xmf3ImpulseContact.z *= m_Friction * xmf3ImpulseContact.x;

		resultImpulse = XMLoadFloat3(&xmf3ImpulseContact);
	}
	
	return resultImpulse;
}