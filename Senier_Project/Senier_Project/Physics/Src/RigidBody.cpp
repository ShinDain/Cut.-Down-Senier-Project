#include "../Header/RigidBody.h"



void RigidBody::AddVelocity(float x, float y, float z)
{
	m_xmf3Velocity.x += x;
	m_xmf3Velocity.y += y;
	m_xmf3Velocity.z += z;
}
void RigidBody::AddVelocity(XMFLOAT3 addVelocity)
{
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
	AddAngleVelocity(addAngleVelocity.x, addAngleVelocity.y, addAngleVelocity.z);
}