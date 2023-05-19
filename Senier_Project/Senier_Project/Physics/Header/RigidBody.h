#ifndef RIGIDBODY_H

#define RIGIDBODY_H

#include "../../Common/Header/D3DUtil.h"
#include "../../Common/Header/MathHelper.h"
#include "Physics.h"

using namespace DirectX;

#define MIN_VELOCITY 2.0f

class Contact;


class RigidBody
{
public:
	RigidBody();
	RigidBody(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Orientation, XMFLOAT3 xmf3Rotate,  XMFLOAT3 xmf3Scale, float mass,
		XMFLOAT3 xmf3ColliderOffsetPosition, XMFLOAT3 xmf3ColliderOffsetRotation);
	RigidBody(const RigidBody& rhs) = delete;
	RigidBody& operator=(const RigidBody& rhs) = delete;
	virtual ~RigidBody();

protected:
	XMFLOAT3 m_xmf3Position = XMFLOAT3(0, 0, 0);
	XMFLOAT3 m_xmf3ColliderPosition = XMFLOAT3(0, 0, 0);
	XMFLOAT4 m_xmf4Orientation = XMFLOAT4(0, 0, 0, 1);
	XMFLOAT3 m_xmf3Rotate = XMFLOAT3(0, 0, 0);
	XMFLOAT3 m_xmf3Scale = XMFLOAT3(1,1,1);
	//XMFLOAT3 m_xmf3Scale = XMFLOAT3(0, 0, 0);
	XMFLOAT4X4 m_xmf4x4World = MathHelper::identity4x4();

	XMFLOAT3 m_xmf3ColliderOffsetPosition = XMFLOAT3(0, 0, 0);
	XMFLOAT3 m_xmf3ColliderOffsetRotation = XMFLOAT3(0, 0, 0);

	float m_Mass = 1.0f;

	XMFLOAT3 m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3AngularVelocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	XMFLOAT4X4 m_xmf4x4RotateInertia = XMFLOAT4X4();
	XMFLOAT4X4 m_xmf4x4InverseRotateInertia = XMFLOAT4X4();
	XMFLOAT4X4 m_xmf4x4InverseRotateInertiaForWorld = XMFLOAT4X4();
	float m_LinearDamping = 0.9f;
	float m_AngularDamping = 0.8f;

	XMFLOAT3 m_xmf3Acceleration = XMFLOAT3(0, 0, 0);
	XMFLOAT3 m_xmf3LastFrameAcceleration = XMFLOAT3(0, 0, 0);

	float m_Motion = 0;
	float m_lastMotion = 0;

	float m_ElapsedTimeAfterCreated = 0;

	bool m_bIsAwake = true;
	bool m_bCanSleep = false;
	bool m_bPhysics = false;
	bool m_bInGravity = false;

	bool m_bIsCharacter = false;
	bool m_bIsPlatform = false;

	bool m_bInvalid = false;

	// 자신(Body)을 가지는 Contact의 vector,
	std::vector<std::shared_ptr<Contact>> m_vContacts;

public:
	void Update(float elapsedTime);
	void CalcDerivedData();

	void AddContact(std::shared_ptr<Contact> pContact);
	void ClearContact();
	// 여기 구현 아직 안됨,=====================================
	// 여기 구현 아직 안됨,=====================================
	// 여기 구현 아직 안됨,=====================================
	// 여기 구현 아직 안됨,=====================================
	void Destroy();

	const std::vector<std::shared_ptr<Contact>>& GetRelativeContacts() { return m_vContacts; }

public:
	void AddVelocity(float x, float y, float z);
	void AddVelocity(XMFLOAT3 addVelocity);
	void AddAngleVelocity(float x, float y, float z);
	void AddAngleVelocity(XMFLOAT3 addAngleVelocity);

	// Set =============================================================

	void SetPosition(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	void SetOrientation(XMFLOAT4 xmf4Orientation) { m_xmf4Orientation = xmf4Orientation; 
													XMStoreFloat4(&m_xmf4Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmf4Orientation))); }
	void SetRotate(XMFLOAT3 xmf3Rotate) { m_xmf3Rotate = xmf3Rotate; }
	void SetScale(XMFLOAT3 xmf3Scale) { m_xmf3Scale = xmf3Scale; }
	void SetWorld(XMFLOAT4X4 xmf4x4World) { m_xmf4x4World = xmf4x4World; }

	void SetMass(float Mass) { m_Mass = Mass; }
	
	void SetVelocity(const XMFLOAT3& Velocity) { m_xmf3Velocity = Velocity; }
	void SetAngularVelocity(const XMFLOAT3& AngularVelocity) { m_xmf3AngularVelocity = AngularVelocity; }

	void SetAcceleration(XMFLOAT3 xmf3Acceleration) { m_xmf3Acceleration = xmf3Acceleration; }

	void SetRotateInertia(XMFLOAT4X4 xmf4x4RotateInertia);
	void SetLinearDamping(float LinearDamping) { m_LinearDamping = LinearDamping; }
	void SetAngleDamping(float AngularDamping) { m_AngularDamping = AngularDamping; }

	void SetIsAwake(bool bIsAwake) { m_bIsAwake = bIsAwake; 
	if (bIsAwake) m_Motion = Physics::sleepEpsilon * 2.0f; 
	else {
		m_xmf3Velocity = XMFLOAT3(0, 0, 0);
		m_xmf3AngularVelocity = XMFLOAT3(0, 0, 0);
	}
	}
	void SetCanSleep(bool bCanSleep) {
		m_bCanSleep = bCanSleep; if (!m_bCanSleep && !m_bIsAwake) { SetIsAwake(true); }
	}
	void SetPhysics(bool bPhysics) { m_bPhysics = bPhysics; }
	void SetInGravity(bool bInGravity) { m_bInGravity = bInGravity; }
	void SetIsCharacter(bool bIsCharacter) { m_bIsCharacter = bIsCharacter; }
	void SetIsPlatform(bool bIsPlatform) { m_bIsPlatform = bIsPlatform; }

	void SetInvalid(bool bInvalid) { m_bInvalid = bInvalid; }

	// Get =============================================================

	const XMFLOAT3& GetPosition() { return m_xmf3Position; }
	const XMFLOAT3& GetColliderPosition() { return m_xmf3ColliderPosition; }
	const XMFLOAT4& GetOrientation() { return m_xmf4Orientation; }
	const XMFLOAT3& GetRotate() { return m_xmf3Rotate; }
	const XMFLOAT3& GetScale() { return m_xmf3Scale; }
	const XMFLOAT4X4& GetWorld() { return m_xmf4x4World; }
		 
	float GetMass() { return m_Mass; }
		 
	const XMFLOAT3& GetVelocity() { return m_xmf3Velocity; }
	const XMFLOAT3& GetAngularVelocity() { return m_xmf3AngularVelocity; }
		 
	const XMFLOAT3& GetAcceleration() { return m_xmf3Acceleration; }
	const XMFLOAT3& GetLastFrameAcceleration() { return m_xmf3LastFrameAcceleration; }
		 
	const XMFLOAT4X4& GetRotateInertia() { return m_xmf4x4RotateInertia; }
	const XMFLOAT4X4& GetInverseRotateInertiaForWorld() { return m_xmf4x4InverseRotateInertiaForWorld; }
	float GetLinearDamping() { return m_LinearDamping; }
	float GetAngleDamping() { return m_AngularDamping; }
		
	bool GetIsAwake() { return m_bIsAwake; }
	bool GetCanSleep() { return m_bCanSleep; }
	bool GetPhysics() { return m_bPhysics; }
	bool GetInGravity() { return m_bInGravity; }
	bool GetIsCharacter() { return m_bIsCharacter; }
	bool GetIsPlatform() { return m_bIsPlatform; }

	bool GetInvalid() { return m_bInvalid; }

};

#endif // !RIGIDBODY_H