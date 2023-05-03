#pragma once
#include "../../Common/Header/D3DUtil.h"
#include "../../Common/Header/MathHelper.h"
#include "Physics.h"

using namespace DirectX;

class RigidBody
{
public:
	RigidBody();
	RigidBody(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Rotate, float mass);
	RigidBody(const RigidBody& rhs) = delete;
	RigidBody& operator=(const RigidBody& rhs) = delete;
	virtual ~RigidBody();

protected:
	XMFLOAT3 m_xmf3Position = XMFLOAT3(0, 0, 0);
	XMFLOAT3 m_xmf3Rotate = XMFLOAT3(0, 0, 0);
	XMFLOAT4X4 m_xmf4x4World = MathHelper::identity4x4();

	float m_Mass = 1.0f;

	XMFLOAT3 m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3AngularVelocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	XMFLOAT4X4 m_xmf4x4RotateInertia = XMFLOAT4X4();
	XMFLOAT4X4 m_xmf4x4InverseRotateInertiaForWorld = XMFLOAT4X4();
	float m_LinearDamping = 0.9f;
	float m_AngularDamping = 0.8f;

	XMFLOAT3 m_xmf3Acceleration = XMFLOAT3(0, 0, 0);
	XMFLOAT3 m_xmf3LastFrameAcceleration = XMFLOAT3(0, 0, 0);

	float m_Motion = 0;

	bool m_bIsAwake = true;
	bool m_bCanSleep = false;
	bool m_bPhysics = true;

public:
	void Update(float elapsedTime);
	void UpdateWorldTransform();
	void UpdateInverseRotateInertiaForWorld();

public:
	void AddVelocity(float x, float y, float z)
	{
		m_xmf3Velocity.x += x;
		m_xmf3Velocity.y += y;
		m_xmf3Velocity.z += z;
	}
	void AddVelocity(XMFLOAT3 addVelocity)
	{
		AddVelocity(addVelocity.x, addVelocity.y, addVelocity.z);
	}
	void AddAngleVelocity(float x, float y, float z)
	{
		m_xmf3AngularVelocity.x += x;
		m_xmf3AngularVelocity.y += y;
		m_xmf3AngularVelocity.z += z;
	}
	void AddAngleVelocity(XMFLOAT3 addAngleVelocity)
	{
		AddAngleVelocity(addAngleVelocity.x, addAngleVelocity.y, addAngleVelocity.z);
	}

	// Set =============================================================

	void SetPosition(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	void SetRotate(XMFLOAT3 xmf3Rotate) { m_xmf3Rotate = xmf3Rotate; }
	void SetWorld(XMFLOAT4X4 xmf4x4World) { m_xmf4x4World = xmf4x4World; }

	void SetMass(float Mass) { m_Mass = Mass; }
	
	void SetVelocity(const XMFLOAT3& Velocity) { m_xmf3Velocity = Velocity; }
	void SetAngularVelocity(const XMFLOAT3& AngularVelocity) { m_xmf3AngularVelocity = AngularVelocity; }

	void SetAcceleration(XMFLOAT3 xmf3Acceleration) { m_xmf3Acceleration = xmf3Acceleration; }

	void SetRotateInertia(XMFLOAT4X4 xmf4x4RotateInertia) { m_xmf4x4RotateInertia = xmf4x4RotateInertia; }
	void SetLinearDamping(float LinearDamping) { m_LinearDamping = LinearDamping; }
	void SetAngleDamping(float AngularDamping) { m_AngularDamping = AngularDamping; }

	void SetIsAwake(bool bIsAwake) { m_bIsAwake = bIsAwake; }
	void SetCanSleep(bool bCanSleep) {
		m_bCanSleep = bCanSleep; if (!m_bCanSleep && !m_bIsAwake) { SetIsAwake(true); }
	}
	void SetPhysics(bool bPhysics) { m_bPhysics = bPhysics; }

	// Get =============================================================

	const XMFLOAT3& GetPosition() { return m_xmf3Position; }
	const XMFLOAT3& GetRotate() { return m_xmf3Rotate; }
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

};

