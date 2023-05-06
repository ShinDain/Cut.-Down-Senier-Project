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
	// Contact Resolve를 위한 데이터 준비
	if (!m_pBody[0]) SwapBodies();

	// 잘못된 Contact 데이터 assert
	assert(m_pBody[0]);

	CalcContactToWorld();

	// 충돌점의 상대 좌표 계산, RelativePoint는 World 좌표계
	XMVECTOR contactPoint = XMLoadFloat3(&m_xmf3ContactPoint);
	XMVECTOR bodyPos_0 = XMLoadFloat3(&m_pBody[0]->GetPosition());
	XMStoreFloat3(&m_pxmf3RelativePosition[0], contactPoint - bodyPos_0);

	if (m_pBody[1])
	{
		XMVECTOR bodyPos_1 = XMLoadFloat3(&m_pBody[1]->GetPosition());
		XMStoreFloat3(&m_pxmf3RelativePosition[1], contactPoint - bodyPos_1);
	}

	// 물체의 접근 속도 계산
	XMVECTOR contactVelocity = CalcLocalVelocity(0, elapsedTime);

	// 물체 1이 존재 시, 유사 과정 진행
	if (m_pBody[1])
	{
		// 기존 접근 속도의 반대 방향이므로 빼준다.
		// ContactVelocity = (분리 속도)
		contactVelocity -= CalcLocalVelocity(1, elapsedTime);
	}
	XMStoreFloat3(&m_xmf3ContactVelocity, contactVelocity);

	CalcDesiredDeltaVelocity(elapsedTime);
}

void Contact::ApplyVelocityChange(XMVECTOR& deltaLinearVel_0, XMVECTOR& deltaLinearVel_1, 
								  XMVECTOR& deltaAngularVel_0, XMVECTOR& deltaAngularVel_1)
{
	// World 좌표계에 적용될 InverseInertia 행렬
	XMMATRIX inverseInertia_0 = XMLoadFloat4x4(&m_pBody[0]->GetInverseRotateInertiaForWorld());
	XMMATRIX inverseInertia_1 = XMMatrixIdentity();
	if (m_pBody[1])
		inverseInertia_1 = XMLoadFloat4x4(&m_pBody[1]->GetInverseRotateInertiaForWorld());

	// Contact 좌표계의 Impulse
	XMVECTOR impulseContact;

	if (m_Friction == 0)
	{
		impulseContact = CalcFrictionlessImpulse(inverseInertia_0, inverseInertia_1);
	}
	else
	{
		impulseContact = CalcFrictionImpulse(inverseInertia_0, inverseInertia_1);
	}

	// World 좌표계로 변환
	XMMATRIX contactToWorld = XMLoadFloat4x4(&m_xmf4x4ContactToWorld);
	XMVECTOR impulse = XMVector3TransformCoord(impulseContact, contactToWorld);

	// 충격량을 이용해 선성분과 각성분 변화량 계산
	XMVECTOR angularResult = XMLoadFloat3(&m_pxmf3RelativePosition[0]);
	angularResult = XMVector3Cross(angularResult, impulse);
	angularResult = XMVector3TransformCoord(angularResult, inverseInertia_0);
	deltaAngularVel_0 = angularResult;

	float mass = m_pBody[0]->GetMass();
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
		// Body1의 경우 반대 방향 작용이므로,
		// pos X (-impulse) => impulse X pos
		angularResult = XMVector3Cross(impulse, angularResult);
		angularResult = XMVector3TransformCoord(angularResult, inverseInertia_1);
		deltaAngularVel_1 = angularResult;

		mass = m_pBody[1]->GetMass();

		// 충격량 방향을 반대로
		linearResult = -impulse / mass;
		deltaLinearVel_1 = linearResult;
		XMStoreFloat3(&xmf3LinearResult, linearResult);
		XMStoreFloat3(&xmf3AngularResult, angularResult);

		m_pBody[1]->AddVelocity(xmf3LinearResult);
		m_pBody[1]->AddAngleVelocity(xmf3AngularResult);
	}
}

void Contact::ApplyPositionChange(XMVECTOR& deltaLinearPos_0, XMVECTOR& deltaLinearPos_1,
								  XMVECTOR& deltaAngularPos_0, XMVECTOR& deltaAngularPos_1, float depth)
{
	const float angularLimit = 0.2f;

	float angularMove[2];
	float linearMove[2];

	float totalInertia = 0;
	float linearInertia[2];
	float angularInertia[2];

	XMVECTOR contactNormal = XMLoadFloat3(&m_xmf3ContactNormal);

	// 교차 해결을 위한 Position 변화를 시키기 전,
	// 교차로 인해 발생할 Position의 변화를 두 물체가 서로 나누어 적용받게 된다.
	// 해당 비율 계산을 위해 충돌로 인해 발생할 충격량의 크기를 이용한다.	(책에서는 이를 관성으로 표현)
	// 이하 과정은 FrictionLessImpulse와 유사하다.
	for (int i = 0; i < 2; ++i)
	{
		if (m_pBody[i])
		{
			XMMATRIX inverseInertiaForWorld = XMLoadFloat4x4(&m_pBody[i]->GetInverseRotateInertiaForWorld());
			XMVECTOR relativePosition = XMLoadFloat3(&m_pxmf3RelativePosition[i]);

			// 각성분
			XMVECTOR angularInertiaWorld = XMVector3Cross(relativePosition, contactNormal);
			angularInertiaWorld = XMVector3TransformNormal(angularInertiaWorld, inverseInertiaForWorld);
			angularInertiaWorld = XMVector3Cross(angularInertiaWorld, relativePosition);
			angularInertia[i] = XMVectorGetX(XMVector3Dot(angularInertiaWorld, contactNormal));

			// 선성분
			linearInertia[i] = ((float)1) / m_pBody[i]->GetMass();

			// 모든 각/선성분 관성의 합
			totalInertia += linearInertia[i] + angularInertia[i];
		}
	}

	for (int i = 0; i < 2; ++i)
	{
		if (m_pBody[i])
		{
			// 관성에 따라 취할 Position 변화의 비율
			float sign = (i == 0) ? 1 : -1;
			angularMove[i] = sign * depth * (angularInertia[i] / totalInertia);
			linearMove[i] = sign * depth * (linearInertia[i] / totalInertia);

			// 물체의 무게중심을 넘어 교차된 경우, 과하게 회전하는 경우를 방지하기 위해서
			// Contact Normal을 법선으로 하는 평면에 사영시킨 
			// 크기를 참조하여 회전을 조절한다. => Relative Point에 따라 값이 조절
			XMVECTOR relativePosition = XMLoadFloat3(&m_pxmf3RelativePosition[i]);

			XMVECTOR projection = contactNormal * -XMVectorGetX(XMVector3Dot(relativePosition, contactNormal)) + relativePosition;

			float maxMagnitude = angularLimit * XMVectorGetX(XMVector3Length(projection));

			float totalMove = angularMove[i] + linearMove[i];
			if (angularMove[i] < -maxMagnitude)
			{
				angularMove[i] = -maxMagnitude;
				linearMove[i] = totalMove - angularMove[i];
			}
			else if (angularMove[i] > maxMagnitude)
			{
				angularMove[i] = maxMagnitude;
				linearMove[i] = totalMove - angularMove[i];
			}
			
			if (angularMove[i] == 0)
			{
				if (i == 0)
					deltaAngularPos_0 = XMVectorZero();
				else if(i == 1)
					deltaAngularPos_1 = XMVectorZero();
			}
			else
			{
				XMVECTOR targetAngularDirection = XMVector3Cross(relativePosition, contactNormal);
				XMMATRIX inverseInertia = XMLoadFloat4x4(&m_pBody[i]->GetInverseRotateInertiaForWorld());

				if (i == 0)
					deltaAngularPos_0 = XMVector3TransformNormal(targetAngularDirection, inverseInertia) * (angularMove[i] / angularInertia[i]);
				else if (i == 1)
					deltaAngularPos_1 = XMVector3TransformNormal(targetAngularDirection, inverseInertia) * (angularMove[i] / angularInertia[i]);
			}

			if (i == 0)
				deltaLinearPos_0 = contactNormal * linearMove[i];
			else if (i == 1)
				deltaLinearPos_1 = contactNormal * linearMove[i];

			XMVECTOR position = XMLoadFloat3(&m_pBody[i]->GetPosition());
			position += contactNormal * linearMove[i];
			XMFLOAT3 xmf3Position;
			XMStoreFloat3(&xmf3Position, position);
			m_pBody[i]->SetPosition(xmf3Position);

			XMVECTOR dq = XMVectorZero();
			XMVECTOR q = XMLoadFloat4(&m_pBody[i]->GetOrientation());
			if (i == 0)
				dq = deltaAngularPos_0;
			else if (i == 1)
				dq = deltaAngularPos_1;
			dq = XMQuaternionMultiply(q, dq) * 0.5f;
			q += dq;

			XMFLOAT4 xmf4Orientation;
			XMStoreFloat4(&xmf4Orientation, q);
			m_pBody[i]->SetOrientation(xmf4Orientation);


			if (!m_pBody[i]->GetIsAwake())
			{
				m_pBody[i]->CalcDerivedData();
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

	// 이번 프레임에 누적된 가속도의 크기 계산
	float velocityFromAcc = 0;
	XMVECTOR contactNormal = XMLoadFloat3(&m_xmf3ContactNormal);

	// ContactNormal 방향으로 작용한 힘에 의해 가속된 속도의 크기를 계산하여 제한다.
	// 중력에 의해 가라앉아 진동하는 현상을 방지
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

		velocityFromAcc -= XMVectorGetX(lastFrameAccel);
	}

	// 속도가 너무 작다면 탄성 계수를 0으로,
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

	// 물체가 가진 각속도를 외적하여 회전 유도 속도로 변형
	velocity = XMVector3Cross(velocity, relativePosition);			
	velocity += XMLoadFloat3(&thisBody->GetVelocity());

	XMMATRIX contactToWorld = XMLoadFloat4x4(&m_xmf4x4ContactToWorld);
	XMVECTOR contactVelocity = XMVector3TransformNormal(velocity, XMMatrixTranspose(contactToWorld));

	// 가속도로 인해 변화한 속도량 측정
	XMVECTOR accVelocity = XMLoadFloat3(&thisBody->GetLastFrameAcceleration()) * elapsedTime;
	accVelocity = XMVector3TransformNormal(accVelocity, XMMatrixTranspose(contactToWorld));

	// 경사면에서 물체가 가라앉는 현상 방지 (가라앉을 시 마찰력이 작용함에도 미끄러지는 현상 발생)
	// 경사면의 평면 방향 속도만 적용하기 위해서 ContactNormal 방향의 속도는 제거
	XMFLOAT3 xmf3AccelVelocity = XMFLOAT3(0, 0, 0);
	XMStoreFloat3(&xmf3AccelVelocity, accVelocity);
	xmf3AccelVelocity.x = 0;
	accVelocity = XMLoadFloat3(&xmf3AccelVelocity);

	contactVelocity += accVelocity;
	
	return contactVelocity;
}

void Contact::CalcContactToWorld()
{
	// Contact Normal을 X축으로 하는 좌표계를 Contact 좌표계라 정의,
	// 책에 나온 내용과 조금 다름

	/*XMVECTOR contactNormal = XMLoadFloat3(&m_xmf3ContactNormal);
	XMVECTOR contactTangentY = XMVectorZero();
	XMVECTOR contactTangentZ = XMVectorZero();
	XMFLOAT3 xmf3WorldY = XMFLOAT3(0, 1, 0);
	XMVECTOR worldY = XMLoadFloat3(&xmf3WorldY);

	contactTangentZ = XMVector3Cross(worldY, contactNormal);
	contactTangentZ = XMVector3Normalize(contactTangentZ);

	contactTangentY = XMVector3Cross(contactTangentZ, contactNormal);
	contactTangentY = XMVector3Normalize(contactTangentY);

	XMFLOAT3 xmf3ContactNormal;
	XMFLOAT3 xmf3ContactTangentY;
	XMFLOAT3 xmf3ContactTangentZ;
	XMStoreFloat3(&xmf3ContactNormal, contactNormal);
	XMStoreFloat3(&xmf3ContactTangentY, contactTangentY);
	XMStoreFloat3(&xmf3ContactTangentZ, contactTangentZ);

	m_xmf4x4ContactToWorld = XMFLOAT4X4(xmf3ContactNormal.x, xmf3ContactNormal.y, xmf3ContactNormal.z, 0,
										xmf3ContactTangentY.x, xmf3ContactTangentY.y, xmf3ContactTangentY.z, 0,
										xmf3ContactTangentZ.x, xmf3ContactTangentZ.y, xmf3ContactTangentZ.z, 0,
										0, 0, 0, 1);*/

	XMFLOAT3 contactTangent[2];

	if (fabsf(m_xmf3ContactNormal.x) > fabsf(m_xmf3ContactNormal.y))
	{
		float s = 1.f / sqrtf(m_xmf3ContactNormal.z * m_xmf3ContactNormal.z + m_xmf3ContactNormal.x * m_xmf3ContactNormal.x);

		contactTangent[0].x = m_xmf3ContactNormal.z * s;
		contactTangent[0].y = 0.f;
		contactTangent[0].z = -m_xmf3ContactNormal.x * s;

		contactTangent[1].x = m_xmf3ContactNormal.y * contactTangent[0].z;
		contactTangent[1].y = m_xmf3ContactNormal.z * contactTangent[0].x - m_xmf3ContactNormal.x * contactTangent[0].z;
		contactTangent[1].z = -m_xmf3ContactNormal.y * contactTangent[0].x;
	}
	else
	{
		float s = 1.f / sqrtf(m_xmf3ContactNormal.y * m_xmf3ContactNormal.y + m_xmf3ContactNormal.z * m_xmf3ContactNormal.z);

		contactTangent[0].x = 0.f;
		contactTangent[0].y = -m_xmf3ContactNormal.z * s;
		contactTangent[0].z = m_xmf3ContactNormal.y * s;

		contactTangent[1].x = m_xmf3ContactNormal.y * contactTangent[0].z - m_xmf3ContactNormal.z * contactTangent[0].y;
		contactTangent[1].y = -m_xmf3ContactNormal.x * contactTangent[0].z;
		contactTangent[1].z = m_xmf3ContactNormal.x * contactTangent[0].y;
	}

	m_xmf4x4ContactToWorld = XMFLOAT4X4(m_xmf3ContactNormal.x, m_xmf3ContactNormal.y, m_xmf3ContactNormal.z, 0,
										contactTangent[0].x, contactTangent[0].y, contactTangent[0].z, 0,
										contactTangent[1].x, contactTangent[1].y, contactTangent[1].z, 0,
										0,0,0,1);
}

XMVECTOR Contact::CalcFrictionlessImpulse(FXMMATRIX InverseInertia_0, CXMMATRIX InverseInertia_1)
{
	XMVECTOR impulseContact = XMVectorZero();
	XMVECTOR contactNormal = XMLoadFloat3(&m_xmf3ContactNormal);

	// 단위 충격량 당 속도변화를 계산
	// 각속도
	XMVECTOR relativePosition_0 = XMLoadFloat3(&m_pxmf3RelativePosition[0]);			// RelativePosition은 World 좌표계
	XMVECTOR deltaVelWorld = XMVector3Cross(relativePosition_0, contactNormal);
	deltaVelWorld = XMVector3TransformNormal(deltaVelWorld, InverseInertia_0);
	deltaVelWorld = XMVector3Cross(deltaVelWorld, relativePosition_0);

	// Contact Normal로 사영하여 Normal 방향 크기만 추출
	float deltaVelocity = XMVectorGetX(XMVector3Dot(deltaVelWorld, contactNormal));
	// 선속도
	deltaVelocity += ((float)1) / m_pBody[0]->GetMass();

	if (m_pBody[1])
	{
		// 각속도
		XMVECTOR relativePosition_1 = XMLoadFloat3(&m_pxmf3RelativePosition[1]);		// RelativePosition은 World 좌표계
		deltaVelWorld = XMVector3Cross(relativePosition_1, contactNormal);
		deltaVelWorld = XMVector3TransformNormal(deltaVelWorld, InverseInertia_1);
		deltaVelWorld = XMVector3Cross(deltaVelWorld, relativePosition_1);

		// Contact Normal로 사영하여 Normal 방향 크기만 추출
		deltaVelocity += XMVectorGetX(XMVector3Dot(deltaVelWorld, contactNormal));
		// 선속도
		deltaVelocity += ((float)1) / m_pBody[1]->GetMass();
	}

	// 기대 속도변화량을 단위 충격량당 속도변화량으로 나눠 Impulse의 크기 획득, Contact 좌표계
	XMFLOAT3 xmf3Result = XMFLOAT3(m_DesiredDeltaVelocity / deltaVelocity, 0, 0);
	XMVECTOR result = XMLoadFloat3(&xmf3Result);

	return result;
}

XMVECTOR Contact::CalcFrictionImpulse(FXMMATRIX InverseInertia_0, CXMMATRIX InverseInertia_1)
{
	XMVECTOR resultImpulse = XMVectorZero();
	float inverseMass = ((float)1) / m_pBody[0]->GetMass();

	// 마찰력은 진행 방향의 반대로 작용한다. 
	// 따라서 기존 Contact Normal 방향의 힘만이 아닌 Contact 좌표계의 
	// x, y, z 축 모두를 고려하여 Impulse를 계산할 필요가 있다.
	// 때문에 행렬을 활용할 것이다.
	
	// 단위 충격량당 Toque의 크기를 반환하는 행렬
	XMMATRIX impulseToToque = MathHelper::MakeSkewSymmetric(m_pxmf3RelativePosition[0]);

	// 단위 충격량당 각속도 변화량을 반환하는 행렬, World 좌표계
	XMMATRIX deltaVelPerImpulseWorld = XMMatrixSet(-1,  0,  0, 0,
												    0, -1,  0, 0,
												    0,  0, -1, 0,
												    0,  0,  0, 1);
	deltaVelPerImpulseWorld = XMMatrixMultiply(deltaVelPerImpulseWorld, impulseToToque);
	deltaVelPerImpulseWorld = XMMatrixMultiply(deltaVelPerImpulseWorld, InverseInertia_0);
	deltaVelPerImpulseWorld = XMMatrixMultiply(deltaVelPerImpulseWorld, impulseToToque);


	if (m_pBody[1])
	{
		impulseToToque = MathHelper::MakeSkewSymmetric(m_pxmf3RelativePosition[1]);

		XMMATRIX deltaVelPerImpulseWorld2 = XMMatrixSet(-1, 0, 0, 0,
														0, -1, 0, 0,
														0, 0, -1, 0,
														0, 0, 0, 1);
		deltaVelPerImpulseWorld2 = XMMatrixMultiply(deltaVelPerImpulseWorld2, impulseToToque);
		deltaVelPerImpulseWorld2 = XMMatrixMultiply(deltaVelPerImpulseWorld2, InverseInertia_1);
		deltaVelPerImpulseWorld2 = XMMatrixMultiply(deltaVelPerImpulseWorld2, impulseToToque);

		// 속도의 총량에 추가
		deltaVelPerImpulseWorld += deltaVelPerImpulseWorld2;

		inverseMass += ((float)1) / m_pBody[1]->GetMass();
	}

	XMMATRIX contactToWorld = XMLoadFloat4x4(&m_xmf4x4ContactToWorld);

	XMMATRIX deltaVelPerImpulse = XMMatrixMultiply(contactToWorld, deltaVelPerImpulseWorld);
	deltaVelPerImpulse = XMMatrixMultiply(deltaVelPerImpulse, XMMatrixTranspose(contactToWorld));

	XMFLOAT4X4 xmf4x4DeltaVelPerImpulse;
	XMStoreFloat4x4(&xmf4x4DeltaVelPerImpulse, deltaVelPerImpulse);
	// 선성분 추가
	xmf4x4DeltaVelPerImpulse._11 += inverseMass;
	xmf4x4DeltaVelPerImpulse._22 += inverseMass;
	xmf4x4DeltaVelPerImpulse._33 += inverseMass;
	deltaVelPerImpulse = XMLoadFloat4x4(&xmf4x4DeltaVelPerImpulse);

	// 단위 속도 변화량당 충격량
	XMVECTOR determinant;
	XMMATRIX impulsePerDeltaVel = XMMatrixInverse(&determinant, deltaVelPerImpulse);
	
	assert(XMVectorGetX(determinant) != 0);

	// 처리를 위한 속도
	XMFLOAT3 xmf3VelKill = XMFLOAT3(m_DesiredDeltaVelocity, -m_xmf3ContactVelocity.y, -m_xmf3ContactVelocity.z);
	resultImpulse = XMLoadFloat3(&xmf3VelKill);
	resultImpulse = XMVector3TransformNormal(resultImpulse, impulsePerDeltaVel);

	XMFLOAT3 xmf3ImpulseContact;
	XMStoreFloat3(&xmf3ImpulseContact, resultImpulse);

	float planarImpulse = sqrtf(xmf3ImpulseContact.y * xmf3ImpulseContact.y + xmf3ImpulseContact.z * xmf3ImpulseContact.z);

	// YZ평면 속도가 정지 마찰보다 큰 경우
	if (planarImpulse > xmf3ImpulseContact.x * m_Friction)
	{
		// 단위 크기로 변경 (방향 유지)
		xmf3ImpulseContact.y /= planarImpulse;
		xmf3ImpulseContact.z /= planarImpulse;

		// 평면에 대한 마찰을 고려하여 계산
		// Impulse의 Y,Z 성분을 Contact Normal에 사영 시켜 단위 충격량에 추가한다.
		xmf3ImpulseContact.x = xmf4x4DeltaVelPerImpulse._11 + 
			xmf4x4DeltaVelPerImpulse._21 * m_Friction * xmf3ImpulseContact.y +
			xmf4x4DeltaVelPerImpulse._31 * m_Friction * xmf3ImpulseContact.z;
		xmf3ImpulseContact.x = m_DesiredDeltaVelocity / xmf3ImpulseContact.x;

		xmf3ImpulseContact.y *= m_Friction * xmf3ImpulseContact.x;
		xmf3ImpulseContact.z *= m_Friction * xmf3ImpulseContact.x;

		resultImpulse = XMLoadFloat3(&xmf3ImpulseContact);
	}
	
	return resultImpulse;
}
